import copy
import os
import re
from textwrap import dedent
from typing import Any, Dict, List, Optional, Set, Tuple
from unittest import mock

import orjson
from django.conf import settings
from django.test import override_settings
from markdown import Markdown

from zerver.lib.actions import (
    change_user_is_active,
    do_add_alert_words,
    do_create_realm,
    do_remove_realm_emoji,
    do_set_realm_property,
    do_set_user_display_setting,
)
from zerver.lib.alert_words import get_alert_word_automaton
from zerver.lib.camo import get_camo_url
from zerver.lib.create_user import create_user
from zerver.lib.emoji import get_emoji_url
from zerver.lib.exceptions import MarkdownRenderingException
from zerver.lib.markdown import (
    MarkdownListPreprocessor,
    MentionData,
    clear_state_for_testing,
    content_has_emoji_syntax,
    fetch_tweet_data,
    get_possible_mentions_info,
    get_tweet_id,
    image_preview_enabled,
    markdown_convert,
    maybe_update_markdown_engines,
    possible_linked_stream_names,
    topic_links,
    url_embed_preview_enabled,
    url_to_a,
)
from zerver.lib.markdown.fenced_code import FencedBlockPreprocessor
from zerver.lib.mdiff import diff_strings
from zerver.lib.mention import possible_mentions, possible_user_group_mentions
from zerver.lib.message import render_markdown
from zerver.lib.request import JsonableError
from zerver.lib.test_classes import ZulipTestCase
from zerver.lib.tex import render_tex
from zerver.lib.user_groups import create_user_group
from zerver.models import (
    MAX_MESSAGE_LENGTH,
    Message,
    RealmEmoji,
    RealmFilter,
    Stream,
    UserGroup,
    UserMessage,
    UserProfile,
    flush_linkifiers,
    flush_per_request_caches,
    get_client,
    get_realm,
    get_stream,
    linkifiers_for_realm,
    realm_in_local_linkifiers_cache,
)


class SimulatedFencedBlockPreprocessor(FencedBlockPreprocessor):
    # Simulate code formatting.

    def format_code(self, lang: Optional[str], code: str) -> str:
        return (lang or "") + ":" + code

    def placeholder(self, s: str) -> str:
        return "**" + s.strip("\n") + "**"


class FencedBlockPreprocessorTest(ZulipTestCase):
    def test_simple_quoting(self) -> None:
        processor = FencedBlockPreprocessor(Markdown())
        markdown_input = [
            "~~~ quote",
            "hi",
            "bye",
            "",
            "",
        ]
        expected = [
            "",
            "> hi",
            "> bye",
            "> ",
            "> ",
            "",
            "",
        ]
        lines = processor.run(markdown_input)
        self.assertEqual(lines, expected)

    def test_serial_quoting(self) -> None:
        processor = FencedBlockPreprocessor(Markdown())
        markdown_input = [
            "~~~ quote",
            "hi",
            "~~~",
            "",
            "~~~ quote",
            "bye",
            "",
            "",
        ]
        expected = [
            "",
            "> hi",
            "",
            "",
            "",
            "> bye",
            "> ",
            "> ",
            "",
            "",
        ]
        lines = processor.run(markdown_input)
        self.assertEqual(lines, expected)

    def test_serial_code(self) -> None:
        processor = SimulatedFencedBlockPreprocessor(Markdown())

        markdown_input = [
            "``` .py",
            "hello()",
            "```",
            "",
            "```vb.net",
            "goodbye()",
            "```",
            "",
            "```c#",
            "weirdchar()",
            "```",
            "",
            "```",
            "no-highlight()",
            "```",
            "",
        ]
        expected = [
            "",
            "**py:hello()**",
            "",
            "",
            "",
            "**vb.net:goodbye()**",
            "",
            "",
            "",
            "**c#:weirdchar()**",
            "",
            "",
            "",
            "**:no-highlight()**",
            "",
            "",
        ]
        lines = processor.run(markdown_input)
        self.assertEqual(lines, expected)

    def test_nested_code(self) -> None:
        processor = SimulatedFencedBlockPreprocessor(Markdown())

        markdown_input = [
            "~~~ quote",
            "hi",
            "``` .py",
            "hello()",
            "```",
            "",
            "",
        ]
        expected = [
            "",
            "> hi",
            "> ",
            "> **py:hello()**",
            "> ",
            "> ",
            "> ",
            "",
            "",
        ]
        lines = processor.run(markdown_input)
        self.assertEqual(lines, expected)


def markdown_convert_wrapper(content: str) -> str:
    return markdown_convert(
        content=content,
        message_realm=get_realm("zulip"),
    )


class MarkdownMiscTest(ZulipTestCase):
    def test_diffs_work_as_expected(self) -> None:
        str1 = "<p>The quick brown fox jumps over the lazy dog.  Animal stories are fun, yeah</p>"
        str2 = "<p>The fast fox jumps over the lazy dogs and cats.  Animal stories are fun</p>"
        expected_diff = "\u001b[34m-\u001b[0m <p>The \u001b[33mquick brown\u001b[0m fox jumps over the lazy dog.  Animal stories are fun\u001b[31m, yeah\u001b[0m</p>\n\u001b[34m+\u001b[0m <p>The \u001b[33mfast\u001b[0m fox jumps over the lazy dog\u001b[32ms and cats\u001b[0m.  Animal stories are fun</p>\n"
        self.assertEqual(diff_strings(str1, str2), expected_diff)

    def test_get_possible_mentions_info(self) -> None:
        realm = get_realm("zulip")

        def make_user(email: str, full_name: str) -> UserProfile:
            return create_user(
                email=email,
                password="whatever",
                realm=realm,
                full_name=full_name,
            )

        fred1 = make_user("fred1@example.com", "Fred Flintstone")
        change_user_is_active(fred1, False)

        fred2 = make_user("fred2@example.com", "Fred Flintstone")

        fred3 = make_user("fred3@example.com", "Fred Flintstone")
        change_user_is_active(fred3, False)

        fred4 = make_user("fred4@example.com", "Fred Flintstone")

        lst = get_possible_mentions_info(
            realm.id, {"Fred Flintstone", "Cordelia, LEAR's daughter", "Not A User"}
        )
        set_of_names = set(map(lambda x: x["full_name"].lower(), lst))
        self.assertEqual(set_of_names, {"fred flintstone", "cordelia, lear's daughter"})

        by_id = {row["id"]: row for row in lst}
        self.assertEqual(
            by_id.get(fred2.id),
            dict(
                email=fred2.email,
                full_name="Fred Flintstone",
                id=fred2.id,
            ),
        )
        self.assertEqual(
            by_id.get(fred4.id),
            dict(
                email=fred4.email,
                full_name="Fred Flintstone",
                id=fred4.id,
            ),
        )

    def test_mention_data(self) -> None:
        realm = get_realm("zulip")
        hamlet = self.example_user("hamlet")
        cordelia = self.example_user("cordelia")
        content = "@**King Hamlet** @**Cordelia, lear's daughter**"
        mention_data = MentionData(realm.id, content)
        self.assertEqual(mention_data.get_user_ids(), {hamlet.id, cordelia.id})
        self.assertEqual(
            mention_data.get_user_by_id(hamlet.id),
            dict(
                email=hamlet.email,
                full_name=hamlet.full_name,
                id=hamlet.id,
            ),
        )

        user = mention_data.get_user_by_name("king hamLET")
        assert user is not None
        self.assertEqual(user["email"], hamlet.email)

        self.assertFalse(mention_data.message_has_wildcards())
        content = "@**King Hamlet** @**Cordelia, lear's daughter** @**all**"
        mention_data = MentionData(realm.id, content)
        self.assertTrue(mention_data.message_has_wildcards())

    def test_invalid_katex_path(self) -> None:
        with self.settings(DEPLOY_ROOT="/nonexistent"):
            with self.assertLogs(level="ERROR") as m:
                render_tex("random text")
            self.assertEqual(m.output, ["ERROR:root:Cannot find KaTeX for latex rendering!"])


class MarkdownListPreprocessorTest(ZulipTestCase):
    # We test that the preprocessor inserts blank lines at correct places.
    # We use <> to indicate that we need to insert a blank line here.
    def split_message(self, msg: str) -> Tuple[List[str], List[str]]:
        original = msg.replace("<>", "").split("\n")
        expected = re.split(r"\n|<>", msg)
        return original, expected

    def test_basic_list(self) -> None:
        preprocessor = MarkdownListPreprocessor()
        original, expected = self.split_message("List without a gap\n<>* One\n* Two")
        self.assertEqual(preprocessor.run(original), expected)

    def test_list_after_quotes(self) -> None:
        preprocessor = MarkdownListPreprocessor()
        original, expected = self.split_message(
            "```quote\nSomething\n```\n\nList without a gap\n<>* One\n* Two"
        )
        self.assertEqual(preprocessor.run(original), expected)

    def test_list_in_code(self) -> None:
        preprocessor = MarkdownListPreprocessor()
        original, expected = self.split_message("```\nList without a gap\n* One\n* Two\n```")
        self.assertEqual(preprocessor.run(original), expected)

    def test_complex_nesting_with_different_fences(self) -> None:
        preprocessor = MarkdownListPreprocessor()
        msg = """```quote
In quote. We should convert a list here:<>
* one
* two

~~~
This is a nested code fence, do not make changes here:
* one
* two

````quote
Quote in code fence. Should not convert:
* one
* two
````

~~~

Back in the quote. We should convert:<>
* one
* two
```

Outside. Should convert:<>
* one
* two
        """
        original, expected = self.split_message(msg)
        self.assertEqual(preprocessor.run(original), expected)

    def test_complex_nesting_with_same_fence(self) -> None:
        preprocessor = MarkdownListPreprocessor()
        msg = """```quote
In quote. We should convert a list here:<>
* one
* two

```python
This is a nested code fence, do not make changes here:
* one
* two

```quote
Quote in code fence. Should not convert:
* one
* two
```

```

Back in the quote. We should convert:<>
* one
* two
```

Outside. Should convert:<>
* one
* two
        """
        original, expected = self.split_message(msg)
        self.assertEqual(preprocessor.run(original), expected)


class MarkdownTest(ZulipTestCase):
    def setUp(self) -> None:
        super().setUp()
        clear_state_for_testing()

    def assertEqual(self, first: Any, second: Any, msg: str = "") -> None:
        if isinstance(first, str) and isinstance(second, str):
            if first != second:
                raise AssertionError(
                    "Actual and expected outputs do not match; showing diff.\n"
                    + diff_strings(first, second)
                    + msg
                )
        else:
            super().assertEqual(first, second)

    def load_markdown_tests(self) -> Tuple[Dict[str, Any], List[List[str]]]:
        test_fixtures = {}
        with open(
            os.path.join(os.path.dirname(__file__), "fixtures/markdown_test_cases.json"), "rb"
        ) as f:
            data = orjson.loads(f.read())
        for test in data["regular_tests"]:
            test_fixtures[test["name"]] = test

        return test_fixtures, data["linkify_tests"]

    def test_markdown_no_ignores(self) -> None:
        # We do not want any ignored tests to be committed and merged.
        format_tests, linkify_tests = self.load_markdown_tests()
        for name, test in format_tests.items():
            message = f'Test "{name}" shouldn\'t be ignored.'
            is_ignored = test.get("ignore", False)
            self.assertFalse(is_ignored, message)

    def test_markdown_fixtures(self) -> None:
        format_tests, linkify_tests = self.load_markdown_tests()
        valid_keys = {
            "name",
            "input",
            "expected_output",
            "backend_only_rendering",
            "marked_expected_output",
            "text_content",
            "translate_emoticons",
            "ignore",
        }

        for name, test in format_tests.items():
            with self.subTest(markdown_test_case=name):
                # Check that there aren't any unexpected keys as those are often typos
                self.assertEqual(len(set(test.keys()) - valid_keys), 0)
                # Ignore tests if specified
                if test.get("ignore", False):
                    continue  # nocoverage

                if test.get("translate_emoticons", False):
                    # Create a userprofile and send message with it.
                    user_profile = self.example_user("othello")
                    do_set_user_display_setting(user_profile, "translate_emoticons", True)
                    msg = Message(sender=user_profile, sending_client=get_client("test"))
                    converted = render_markdown(msg, test["input"])
                else:
                    converted = markdown_convert_wrapper(test["input"])

                self.assertEqual(converted, test["expected_output"])

        def replaced(payload: str, url: str, phrase: str = "") -> str:
            if url[:4] == "http":
                href = url
            elif "@" in url:
                href = "mailto:" + url
            else:
                href = "http://" + url
            return payload % (f'<a href="{href}">{url}</a>',)

        with mock.patch(
            "zerver.lib.url_preview.preview.link_embed_data_from_cache", return_value=None
        ):
            for inline_url, reference, url in linkify_tests:
                try:
                    match = replaced(reference, url, phrase=inline_url)
                except TypeError:
                    match = reference
                converted = markdown_convert_wrapper(inline_url)
                self.assertEqual(match, converted)

    def test_inline_file(self) -> None:
        msg = "Check out this file file:///Volumes/myserver/Users/Shared/pi.py"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            '<p>Check out this file <a href="file:///Volumes/myserver/Users/Shared/pi.py">file:///Volumes/myserver/Users/Shared/pi.py</a></p>',
        )

        clear_state_for_testing()
        with self.settings(ENABLE_FILE_LINKS=False):
            realm = do_create_realm(string_id="file_links_test", name="file_links_test")
            maybe_update_markdown_engines(realm.id, False)
            converted = markdown_convert(msg, message_realm=realm)
            self.assertEqual(
                converted, "<p>Check out this file file:///Volumes/myserver/Users/Shared/pi.py</p>"
            )

    def test_inline_bitcoin(self) -> None:
        msg = "To bitcoin:1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa or not to bitcoin"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            '<p>To <a href="bitcoin:1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa">bitcoin:1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa</a> or not to bitcoin</p>',
        )

    def test_inline_youtube(self) -> None:
        msg = "Check out the debate: http://www.youtube.com/watch?v=hx1mjT73xYE"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p>Check out the debate: <a href="http://www.youtube.com/watch?v=hx1mjT73xYE">http://www.youtube.com/watch?v=hx1mjT73xYE</a></p>\n<div class="youtube-video message_inline_image"><a data-id="hx1mjT73xYE" href="http://www.youtube.com/watch?v=hx1mjT73xYE"><img src="{get_camo_url("https://i.ytimg.com/vi/hx1mjT73xYE/default.jpg")}"></a></div>""",
        )

        msg = "http://www.youtube.com/watch?v=hx1mjT73xYE"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p><a href="http://www.youtube.com/watch?v=hx1mjT73xYE">http://www.youtube.com/watch?v=hx1mjT73xYE</a></p>\n<div class="youtube-video message_inline_image"><a data-id="hx1mjT73xYE" href="http://www.youtube.com/watch?v=hx1mjT73xYE"><img src="{get_camo_url("https://i.ytimg.com/vi/hx1mjT73xYE/default.jpg")}"></a></div>""",
        )

        msg = "https://youtu.be/hx1mjT73xYE"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p><a href="https://youtu.be/hx1mjT73xYE">https://youtu.be/hx1mjT73xYE</a></p>\n<div class="youtube-video message_inline_image"><a data-id="hx1mjT73xYE" href="https://youtu.be/hx1mjT73xYE"><img src="{get_camo_url("https://i.ytimg.com/vi/hx1mjT73xYE/default.jpg")}"></a></div>""",
        )

        msg = "https://www.youtube.com/playlist?list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo"
        not_converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            not_converted,
            '<p><a href="https://www.youtube.com/playlist?list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo">https://www.youtube.com/playlist?list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo</a></p>',
        )

        msg = (
            "https://www.youtube.com/playlist?v=O5nskjZ_GoI&list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo"
        )
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p><a href="https://www.youtube.com/playlist?v=O5nskjZ_GoI&amp;list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo">https://www.youtube.com/playlist?v=O5nskjZ_GoI&amp;list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo</a></p>\n<div class="youtube-video message_inline_image"><a data-id="O5nskjZ_GoI" href="https://www.youtube.com/playlist?v=O5nskjZ_GoI&amp;list=PL8dPuuaLjXtNlUrzyH5r6jN9ulIgZBpdo"><img src="{get_camo_url("https://i.ytimg.com/vi/O5nskjZ_GoI/default.jpg")}"></a></div>""",
        )

        msg = "http://www.youtube.com/watch_videos?video_ids=nOJgD4fcZhI,i96UO8-GFvw"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p><a href="http://www.youtube.com/watch_videos?video_ids=nOJgD4fcZhI,i96UO8-GFvw">http://www.youtube.com/watch_videos?video_ids=nOJgD4fcZhI,i96UO8-GFvw</a></p>\n<div class="youtube-video message_inline_image"><a data-id="nOJgD4fcZhI" href="http://www.youtube.com/watch_videos?video_ids=nOJgD4fcZhI,i96UO8-GFvw"><img src="{get_camo_url("https://i.ytimg.com/vi/nOJgD4fcZhI/default.jpg")}"></a></div>""",
        )

    @override_settings(INLINE_URL_EMBED_PREVIEW=False)
    def test_inline_vimeo(self) -> None:
        msg = "Check out the debate: https://vimeo.com/246979354"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            '<p>Check out the debate: <a href="https://vimeo.com/246979354">https://vimeo.com/246979354</a></p>',
        )

        msg = "https://vimeo.com/246979354"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            '<p><a href="https://vimeo.com/246979354">https://vimeo.com/246979354</a></p>',
        )

    @override_settings(INLINE_IMAGE_PREVIEW=True)
    def test_inline_image_thumbnail_url(self) -> None:
        realm = get_realm("zephyr")
        msg = "[foobar](/user_uploads/{realm_id}/50/w2G6ok9kr8AMCQCTNAUOFMln/IMG_0677.JPG)"
        msg = msg.format(realm_id=realm.id)
        thumbnail_img = '<img data-src-fullsize="/thumbnail?url=user_uploads%2F{realm_id}%2F50%2Fw2G6ok9kr8AMCQCTNAUOFMln%2FIMG_0677.JPG&amp;size=full" src="/thumbnail?url=user_uploads%2F{realm_id}%2F50%2Fw2G6ok9kr8AMCQCTNAUOFMln%2FIMG_0677.JPG&amp;size=thumbnail"><'
        thumbnail_img = thumbnail_img.format(realm_id=realm.id)
        converted = markdown_convert_wrapper(msg)
        self.assertIn(thumbnail_img, converted)

        msg = "https://www.google.com/images/srpr/logo4w.png"
        thumbnail_img = '<img data-src-fullsize="/thumbnail?url=https%3A%2F%2Fwww.google.com%2Fimages%2Fsrpr%2Flogo4w.png&amp;size=full" src="/thumbnail?url=https%3A%2F%2Fwww.google.com%2Fimages%2Fsrpr%2Flogo4w.png&amp;size=thumbnail">'
        converted = markdown_convert_wrapper(msg)
        self.assertIn(thumbnail_img, converted)

        msg = "www.google.com/images/srpr/logo4w.png"
        thumbnail_img = '<img data-src-fullsize="/thumbnail?url=http%3A%2F%2Fwww.google.com%2Fimages%2Fsrpr%2Flogo4w.png&amp;size=full" src="/thumbnail?url=http%3A%2F%2Fwww.google.com%2Fimages%2Fsrpr%2Flogo4w.png&amp;size=thumbnail">'
        converted = markdown_convert_wrapper(msg)
        self.assertIn(thumbnail_img, converted)

        msg = "https://www.google.com/images/srpr/logo4w.png"
        thumbnail_img = f"""<div class="message_inline_image"><a href="https://www.google.com/images/srpr/logo4w.png"><img src="{get_camo_url("https://www.google.com/images/srpr/logo4w.png")}"></a></div>"""
        with self.settings(THUMBNAIL_IMAGES=False):
            converted = markdown_convert_wrapper(msg)
        self.assertIn(thumbnail_img, converted)

        # Any URL which is not an external link and doesn't start with
        # /user_uploads/ is not thumbnailed
        msg = "[foobar](/static/images/cute/turtle.png)"
        thumbnail_img = '<div class="message_inline_image"><a href="/static/images/cute/turtle.png" title="foobar"><img src="/static/images/cute/turtle.png"></a></div>'
        converted = markdown_convert_wrapper(msg)
        self.assertIn(thumbnail_img, converted)

        msg = "[foobar](/user_avatars/{realm_id}/emoji/images/50.png)"
        msg = msg.format(realm_id=realm.id)
        thumbnail_img = '<div class="message_inline_image"><a href="/user_avatars/{realm_id}/emoji/images/50.png" title="foobar"><img src="/user_avatars/{realm_id}/emoji/images/50.png"></a></div>'
        thumbnail_img = thumbnail_img.format(realm_id=realm.id)
        converted = markdown_convert_wrapper(msg)
        self.assertIn(thumbnail_img, converted)

    @override_settings(INLINE_IMAGE_PREVIEW=True)
    def test_inline_image_preview(self) -> None:
        with_preview = '<div class="message_inline_image"><a href="http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg"><img data-src-fullsize="/thumbnail?url=http%3A%2F%2Fcdn.wallpapersafari.com%2F13%2F6%2F16eVjx.jpeg&amp;size=full" src="/thumbnail?url=http%3A%2F%2Fcdn.wallpapersafari.com%2F13%2F6%2F16eVjx.jpeg&amp;size=thumbnail"></a></div>'
        without_preview = '<p><a href="http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg">http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg</a></p>'
        content = "http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg"

        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, with_preview)

        realm = msg.get_realm()
        setattr(realm, "inline_image_preview", False)
        realm.save()

        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, without_preview)

    @override_settings(THUMBNAIL_IMAGES=False, EXTERNAL_URI_SCHEME="https://")
    def test_external_image_preview_use_camo(self) -> None:
        content = "https://example.com/thing.jpeg"

        thumbnail_img = f"""<div class="message_inline_image"><a href="{content}"><img src="{get_camo_url(content)}"></a></div>"""
        converted = markdown_convert_wrapper(content)
        self.assertIn(converted, thumbnail_img)

    @override_settings(THUMBNAIL_IMAGES=False, EXTERNAL_URI_SCHEME="https://")
    def test_static_image_preview_skip_camo(self) -> None:
        content = f"{ settings.STATIC_URL }/thing.jpeg"

        thumbnail_img = f"""<div class="message_inline_image"><a href="{content}"><img src="{content}"></a></div>"""
        converted = markdown_convert_wrapper(content)
        self.assertIn(converted, thumbnail_img)

    @override_settings(THUMBNAIL_IMAGES=False, EXTERNAL_URI_SCHEME="https://")
    def test_realm_image_preview_skip_camo(self) -> None:
        content = f"https://zulip.{ settings.EXTERNAL_HOST }/thing.jpeg"
        converted = markdown_convert_wrapper(content)
        self.assertNotIn(converted, get_camo_url(content))

    @override_settings(THUMBNAIL_IMAGES=False, EXTERNAL_URI_SCHEME="https://")
    def test_cross_realm_image_preview_use_camo(self) -> None:
        content = f"https://otherrealm.{ settings.EXTERNAL_HOST }/thing.jpeg"

        thumbnail_img = f"""<div class="message_inline_image"><a href="{ content }"><img src="{ get_camo_url(content) }"></a></div>"""
        converted = markdown_convert_wrapper(content)
        self.assertIn(converted, thumbnail_img)

    @override_settings(INLINE_IMAGE_PREVIEW=True)
    def test_inline_image_quoted_blocks(self) -> None:
        content = "http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg"
        expected = '<div class="message_inline_image"><a href="http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg"><img data-src-fullsize="/thumbnail?url=http%3A%2F%2Fcdn.wallpapersafari.com%2F13%2F6%2F16eVjx.jpeg&amp;size=full" src="/thumbnail?url=http%3A%2F%2Fcdn.wallpapersafari.com%2F13%2F6%2F16eVjx.jpeg&amp;size=thumbnail"></a></div>'
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, expected)

        content = ">http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg\n\nAwesome!"
        expected = '<blockquote>\n<p><a href="http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg">http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg</a></p>\n</blockquote>\n<p>Awesome!</p>'
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, expected)

        content = ">* http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg\n\nAwesome!"
        expected = '<blockquote>\n<ul>\n<li><a href="http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg">http://cdn.wallpapersafari.com/13/6/16eVjx.jpeg</a></li>\n</ul>\n</blockquote>\n<p>Awesome!</p>'
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, expected)

    @override_settings(INLINE_IMAGE_PREVIEW=True)
    def test_inline_image_preview_order(self) -> None:
        realm = get_realm("zulip")
        content = "http://imaging.nikon.com/lineup/dslr/df/img/sample/img_01.jpg\nhttp://imaging.nikon.com/lineup/dslr/df/img/sample/img_02.jpg\nhttp://imaging.nikon.com/lineup/dslr/df/img/sample/img_03.jpg"
        expected = '<p><a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_01.jpg">http://imaging.nikon.com/lineup/dslr/df/img/sample/img_01.jpg</a><br>\n<a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_02.jpg">http://imaging.nikon.com/lineup/dslr/df/img/sample/img_02.jpg</a><br>\n<a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_03.jpg">http://imaging.nikon.com/lineup/dslr/df/img/sample/img_03.jpg</a></p>\n<div class="message_inline_image"><a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_01.jpg"><img data-src-fullsize="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_01.jpg&amp;size=full" src="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_01.jpg&amp;size=thumbnail"></a></div><div class="message_inline_image"><a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_02.jpg"><img data-src-fullsize="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_02.jpg&amp;size=full" src="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_02.jpg&amp;size=thumbnail"></a></div><div class="message_inline_image"><a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_03.jpg"><img data-src-fullsize="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_03.jpg&amp;size=full" src="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_03.jpg&amp;size=thumbnail"></a></div>'

        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, expected)

        content = "http://imaging.nikon.com/lineup/dslr/df/img/sample/img_01.jpg\n\n>http://imaging.nikon.com/lineup/dslr/df/img/sample/img_02.jpg\n\n* http://imaging.nikon.com/lineup/dslr/df/img/sample/img_03.jpg\n* https://www.google.com/images/srpr/logo4w.png"
        expected = '<div class="message_inline_image"><a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_01.jpg"><img data-src-fullsize="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_01.jpg&amp;size=full" src="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_01.jpg&amp;size=thumbnail"></a></div><blockquote>\n<p><a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_02.jpg">http://imaging.nikon.com/lineup/dslr/df/img/sample/img_02.jpg</a></p>\n</blockquote>\n<ul>\n<li><div class="message_inline_image"><a href="http://imaging.nikon.com/lineup/dslr/df/img/sample/img_03.jpg"><img data-src-fullsize="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_03.jpg&amp;size=full" src="/thumbnail?url=http%3A%2F%2Fimaging.nikon.com%2Flineup%2Fdslr%2Fdf%2Fimg%2Fsample%2Fimg_03.jpg&amp;size=thumbnail"></a></div></li>\n<li><div class="message_inline_image"><a href="https://www.google.com/images/srpr/logo4w.png"><img data-src-fullsize="/thumbnail?url=https%3A%2F%2Fwww.google.com%2Fimages%2Fsrpr%2Flogo4w.png&amp;size=full" src="/thumbnail?url=https%3A%2F%2Fwww.google.com%2Fimages%2Fsrpr%2Flogo4w.png&amp;size=thumbnail"></a></div></li>\n</ul>'

        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, expected)

        content = "Test 1\n[21136101110_1dde1c1a7e_o.jpg](/user_uploads/{realm_id}/6d/F1PX6u16JA2P-nK45PyxHIYZ/21136101110_1dde1c1a7e_o.jpg) \n\nNext image\n[IMG_20161116_023910.jpg](/user_uploads/{realm_id}/69/sh7L06e7uH7NaX6d5WFfVYQp/IMG_20161116_023910.jpg) \n\nAnother screenshot\n[Screenshot-from-2016-06-01-16-22-42.png](/user_uploads/{realm_id}/70/_aZmIEWaN1iUaxwkDjkO7bpj/Screenshot-from-2016-06-01-16-22-42.png)"
        content = content.format(realm_id=realm.id)
        expected = '<p>Test 1<br>\n<a href="/user_uploads/{realm_id}/6d/F1PX6u16JA2P-nK45PyxHIYZ/21136101110_1dde1c1a7e_o.jpg">21136101110_1dde1c1a7e_o.jpg</a> </p>\n<div class="message_inline_image"><a href="/user_uploads/{realm_id}/6d/F1PX6u16JA2P-nK45PyxHIYZ/21136101110_1dde1c1a7e_o.jpg" title="21136101110_1dde1c1a7e_o.jpg"><img data-src-fullsize="/thumbnail?url=user_uploads%2F{realm_id}%2F6d%2FF1PX6u16JA2P-nK45PyxHIYZ%2F21136101110_1dde1c1a7e_o.jpg&amp;size=full" src="/thumbnail?url=user_uploads%2F{realm_id}%2F6d%2FF1PX6u16JA2P-nK45PyxHIYZ%2F21136101110_1dde1c1a7e_o.jpg&amp;size=thumbnail"></a></div><p>Next image<br>\n<a href="/user_uploads/{realm_id}/69/sh7L06e7uH7NaX6d5WFfVYQp/IMG_20161116_023910.jpg">IMG_20161116_023910.jpg</a> </p>\n<div class="message_inline_image"><a href="/user_uploads/{realm_id}/69/sh7L06e7uH7NaX6d5WFfVYQp/IMG_20161116_023910.jpg" title="IMG_20161116_023910.jpg"><img data-src-fullsize="/thumbnail?url=user_uploads%2F{realm_id}%2F69%2Fsh7L06e7uH7NaX6d5WFfVYQp%2FIMG_20161116_023910.jpg&amp;size=full" src="/thumbnail?url=user_uploads%2F{realm_id}%2F69%2Fsh7L06e7uH7NaX6d5WFfVYQp%2FIMG_20161116_023910.jpg&amp;size=thumbnail"></a></div><p>Another screenshot<br>\n<a href="/user_uploads/{realm_id}/70/_aZmIEWaN1iUaxwkDjkO7bpj/Screenshot-from-2016-06-01-16-22-42.png">Screenshot-from-2016-06-01-16-22-42.png</a></p>\n<div class="message_inline_image"><a href="/user_uploads/{realm_id}/70/_aZmIEWaN1iUaxwkDjkO7bpj/Screenshot-from-2016-06-01-16-22-42.png" title="Screenshot-from-2016-06-01-16-22-42.png"><img data-src-fullsize="/thumbnail?url=user_uploads%2F{realm_id}%2F70%2F_aZmIEWaN1iUaxwkDjkO7bpj%2FScreenshot-from-2016-06-01-16-22-42.png&amp;size=full" src="/thumbnail?url=user_uploads%2F{realm_id}%2F70%2F_aZmIEWaN1iUaxwkDjkO7bpj%2FScreenshot-from-2016-06-01-16-22-42.png&amp;size=thumbnail"></a></div>'
        expected = expected.format(realm_id=realm.id)

        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, expected)

    @override_settings(INLINE_IMAGE_PREVIEW=True)
    def test_corrected_image_source(self) -> None:
        # testing only Wikipedia because linx.li URLs can be expected to expire
        content = "https://en.wikipedia.org/wiki/File:Wright_of_Derby,_The_Orrery.jpg"
        expected = '<div class="message_inline_image"><a href="https://en.wikipedia.org/wiki/Special:FilePath/File:Wright_of_Derby,_The_Orrery.jpg"><img data-src-fullsize="/thumbnail?url=https%3A%2F%2Fen.wikipedia.org%2Fwiki%2FSpecial%3AFilePath%2FFile%3AWright_of_Derby%2C_The_Orrery.jpg&amp;size=full" src="/thumbnail?url=https%3A%2F%2Fen.wikipedia.org%2Fwiki%2FSpecial%3AFilePath%2FFile%3AWright_of_Derby%2C_The_Orrery.jpg&amp;size=thumbnail"></a></div>'

        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        converted = render_markdown(msg, content)
        self.assertEqual(converted, expected)

    @override_settings(INLINE_IMAGE_PREVIEW=False)
    def test_image_preview_enabled(self) -> None:
        ret = image_preview_enabled()
        self.assertEqual(ret, False)

        settings.INLINE_IMAGE_PREVIEW = True

        sender_user_profile = self.example_user("othello")
        message = Message(sender=sender_user_profile, sending_client=get_client("test"))
        realm = message.get_realm()

        ret = image_preview_enabled()
        self.assertEqual(ret, True)

        ret = image_preview_enabled(no_previews=True)
        self.assertEqual(ret, False)

        ret = image_preview_enabled(message, realm)
        self.assertEqual(ret, True)

        ret = image_preview_enabled(message)
        self.assertEqual(ret, True)

        ret = image_preview_enabled(message, realm, no_previews=True)
        self.assertEqual(ret, False)

        ret = image_preview_enabled(message, no_previews=True)
        self.assertEqual(ret, False)

    @override_settings(INLINE_URL_EMBED_PREVIEW=False)
    def test_url_embed_preview_enabled(self) -> None:
        sender_user_profile = self.example_user("othello")
        message = copy.deepcopy(
            Message(sender=sender_user_profile, sending_client=get_client("test"))
        )
        realm = message.get_realm()
        realm.inline_url_embed_preview = True  # off by default
        realm.save(update_fields=["inline_url_embed_preview"])

        ret = url_embed_preview_enabled()
        self.assertEqual(ret, False)

        settings.INLINE_URL_EMBED_PREVIEW = True

        ret = url_embed_preview_enabled()
        self.assertEqual(ret, True)

        ret = image_preview_enabled(no_previews=True)
        self.assertEqual(ret, False)

        ret = url_embed_preview_enabled(message, realm)
        self.assertEqual(ret, True)
        ret = url_embed_preview_enabled(message)
        self.assertEqual(ret, True)

        ret = url_embed_preview_enabled(message, no_previews=True)
        self.assertEqual(ret, False)

    def test_inline_dropbox(self) -> None:
        msg = "Look at how hilarious our old office was: https://www.dropbox.com/s/ymdijjcg67hv2ta/IMG_0923.JPG"
        image_info = {
            "image": "https://photos-4.dropbox.com/t/2/AABIre1oReJgPYuc_53iv0IHq1vUzRaDg2rrCfTpiWMccQ/12/129/jpeg/1024x1024/2/_/0/4/IMG_0923.JPG/CIEBIAEgAiAHKAIoBw/ymdijjcg67hv2ta/AABz2uuED1ox3vpWWvMpBxu6a/IMG_0923.JPG",
            "desc": "Shared with Dropbox",
            "title": "IMG_0923.JPG",
        }
        with mock.patch("zerver.lib.markdown.fetch_open_graph_image", return_value=image_info):
            converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p>Look at how hilarious our old office was: <a href="https://www.dropbox.com/s/ymdijjcg67hv2ta/IMG_0923.JPG">https://www.dropbox.com/s/ymdijjcg67hv2ta/IMG_0923.JPG</a></p>\n<div class="message_inline_image"><a href="https://www.dropbox.com/s/ymdijjcg67hv2ta/IMG_0923.JPG" title="IMG_0923.JPG"><img src="{get_camo_url("https://www.dropbox.com/s/ymdijjcg67hv2ta/IMG_0923.JPG?raw=1")}"></a></div>""",
        )

        msg = "Look at my hilarious drawing folder: https://www.dropbox.com/sh/cm39k9e04z7fhim/AAAII5NK-9daee3FcF41anEua?dl="
        image_info = {
            "image": "https://cf.dropboxstatic.com/static/images/icons128/folder_dropbox.png",
            "desc": "Shared with Dropbox",
            "title": "Saves",
        }
        with mock.patch("zerver.lib.markdown.fetch_open_graph_image", return_value=image_info):
            converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p>Look at my hilarious drawing folder: <a href="https://www.dropbox.com/sh/cm39k9e04z7fhim/AAAII5NK-9daee3FcF41anEua?dl=">https://www.dropbox.com/sh/cm39k9e04z7fhim/AAAII5NK-9daee3FcF41anEua?dl=</a></p>\n<div class="message_inline_ref"><a href="https://www.dropbox.com/sh/cm39k9e04z7fhim/AAAII5NK-9daee3FcF41anEua?dl=" title="Saves"><img src="{get_camo_url("https://cf.dropboxstatic.com/static/images/icons128/folder_dropbox.png")}"></a><div><div class="message_inline_image_title">Saves</div><desc class="message_inline_image_desc"></desc></div></div>""",
        )

    def test_inline_dropbox_preview(self) -> None:
        # Test photo album previews
        msg = "https://www.dropbox.com/sc/tditp9nitko60n5/03rEiZldy5"
        image_info = {
            "image": "https://photos-6.dropbox.com/t/2/AAAlawaeD61TyNewO5vVi-DGf2ZeuayfyHFdNTNzpGq-QA/12/271544745/jpeg/1024x1024/2/_/0/5/baby-piglet.jpg/CKnjvYEBIAIgBygCKAc/tditp9nitko60n5/AADX03VAIrQlTl28CtujDcMla/0",
            "desc": "Shared with Dropbox",
            "title": "1 photo",
        }
        with mock.patch("zerver.lib.markdown.fetch_open_graph_image", return_value=image_info):
            converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p><a href="https://www.dropbox.com/sc/tditp9nitko60n5/03rEiZldy5">https://www.dropbox.com/sc/tditp9nitko60n5/03rEiZldy5</a></p>\n<div class="message_inline_image"><a href="https://www.dropbox.com/sc/tditp9nitko60n5/03rEiZldy5" title="1 photo"><img src="{get_camo_url("https://photos-6.dropbox.com/t/2/AAAlawaeD61TyNewO5vVi-DGf2ZeuayfyHFdNTNzpGq-QA/12/271544745/jpeg/1024x1024/2/_/0/5/baby-piglet.jpg/CKnjvYEBIAIgBygCKAc/tditp9nitko60n5/AADX03VAIrQlTl28CtujDcMla/0")}"></a></div>""",
        )

    def test_inline_dropbox_negative(self) -> None:
        # Make sure we're not overzealous in our conversion:
        msg = "Look at the new dropbox logo: https://www.dropbox.com/static/images/home_logo.png"
        with mock.patch("zerver.lib.markdown.fetch_open_graph_image", return_value=None):
            converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            '<p>Look at the new dropbox logo: <a href="https://www.dropbox.com/static/images/home_logo.png">https://www.dropbox.com/static/images/home_logo.png</a></p>\n<div class="message_inline_image"><a href="https://www.dropbox.com/static/images/home_logo.png"><img data-src-fullsize="/thumbnail?url=https%3A%2F%2Fwww.dropbox.com%2Fstatic%2Fimages%2Fhome_logo.png&amp;size=full" src="/thumbnail?url=https%3A%2F%2Fwww.dropbox.com%2Fstatic%2Fimages%2Fhome_logo.png&amp;size=thumbnail"></a></div>',
        )

    def test_inline_dropbox_bad(self) -> None:
        # Don't fail on bad dropbox links
        msg = "https://zulip-test.dropbox.com/photos/cl/ROmr9K1XYtmpneM"
        with mock.patch("zerver.lib.markdown.fetch_open_graph_image", return_value=None):
            converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            '<p><a href="https://zulip-test.dropbox.com/photos/cl/ROmr9K1XYtmpneM">https://zulip-test.dropbox.com/photos/cl/ROmr9K1XYtmpneM</a></p>',
        )

    def test_inline_github_preview(self) -> None:
        # Test photo album previews
        msg = "Test: https://github.com/zulip/zulip/blob/main/static/images/logo/zulip-icon-128x128.png"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            '<p>Test: <a href="https://github.com/zulip/zulip/blob/main/static/images/logo/zulip-icon-128x128.png">https://github.com/zulip/zulip/blob/main/static/images/logo/zulip-icon-128x128.png</a></p>\n<div class="message_inline_image"><a href="https://github.com/zulip/zulip/blob/main/static/images/logo/zulip-icon-128x128.png"><img data-src-fullsize="/thumbnail?url=https%3A%2F%2Fraw.githubusercontent.com%2Fzulip%2Fzulip%2Fmain%2Fstatic%2Fimages%2Flogo%2Fzulip-icon-128x128.png&amp;size=full" src="/thumbnail?url=https%3A%2F%2Fraw.githubusercontent.com%2Fzulip%2Fzulip%2Fmain%2Fstatic%2Fimages%2Flogo%2Fzulip-icon-128x128.png&amp;size=thumbnail"></a></div>',
        )

        msg = "Test: https://developer.github.com/assets/images/hero-circuit-bg.png"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            '<p>Test: <a href="https://developer.github.com/assets/images/hero-circuit-bg.png">https://developer.github.com/assets/images/hero-circuit-bg.png</a></p>\n<div class="message_inline_image"><a href="https://developer.github.com/assets/images/hero-circuit-bg.png"><img data-src-fullsize="/thumbnail?url=https%3A%2F%2Fdeveloper.github.com%2Fassets%2Fimages%2Fhero-circuit-bg.png&amp;size=full" src="/thumbnail?url=https%3A%2F%2Fdeveloper.github.com%2Fassets%2Fimages%2Fhero-circuit-bg.png&amp;size=thumbnail"></a></div>',
        )

    def test_inline_youtube_preview(self) -> None:
        # Test YouTube URLs in spoilers
        msg = """\n```spoiler Check out this PyCon video\nhttps://www.youtube.com/watch?v=0c46YHS3RY8\n```"""
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<div class="spoiler-block"><div class="spoiler-header">\n<p>Check out this PyCon video</p>\n</div><div class="spoiler-content" aria-hidden="true">\n<p><a href="https://www.youtube.com/watch?v=0c46YHS3RY8">https://www.youtube.com/watch?v=0c46YHS3RY8</a></p>\n<div class="youtube-video message_inline_image"><a data-id="0c46YHS3RY8" href="https://www.youtube.com/watch?v=0c46YHS3RY8"><img src="{get_camo_url("https://i.ytimg.com/vi/0c46YHS3RY8/default.jpg")}"></a></div></div></div>""",
        )

        # Test YouTube URLs in normal messages.
        msg = "[YouTube link](https://www.youtube.com/watch?v=0c46YHS3RY8)"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p><a href="https://www.youtube.com/watch?v=0c46YHS3RY8">YouTube link</a></p>\n<div class="youtube-video message_inline_image"><a data-id="0c46YHS3RY8" href="https://www.youtube.com/watch?v=0c46YHS3RY8"><img src="{get_camo_url("https://i.ytimg.com/vi/0c46YHS3RY8/default.jpg")}"></a></div>""",
        )

        msg = "https://www.youtube.com/watch?v=0c46YHS3RY8\n\nSample text\n\nhttps://www.youtube.com/watch?v=lXFO2ULktEI"
        converted = markdown_convert_wrapper(msg)

        self.assertEqual(
            converted,
            f"""<p><a href="https://www.youtube.com/watch?v=0c46YHS3RY8">https://www.youtube.com/watch?v=0c46YHS3RY8</a></p>\n<div class="youtube-video message_inline_image"><a data-id="0c46YHS3RY8" href="https://www.youtube.com/watch?v=0c46YHS3RY8"><img src="{get_camo_url("https://i.ytimg.com/vi/0c46YHS3RY8/default.jpg")}"></a></div><p>Sample text</p>\n<p><a href="https://www.youtube.com/watch?v=lXFO2ULktEI">https://www.youtube.com/watch?v=lXFO2ULktEI</a></p>\n<div class="youtube-video message_inline_image"><a data-id="lXFO2ULktEI" href="https://www.youtube.com/watch?v=lXFO2ULktEI"><img src="{get_camo_url("https://i.ytimg.com/vi/lXFO2ULktEI/default.jpg")}"></a></div>""",
        )

    def test_twitter_id_extraction(self) -> None:
        self.assertEqual(
            get_tweet_id("http://twitter.com/#!/VizzQuotes/status/409030735191097344"),
            "409030735191097344",
        )
        self.assertEqual(
            get_tweet_id("http://twitter.com/VizzQuotes/status/409030735191097344"),
            "409030735191097344",
        )
        self.assertEqual(
            get_tweet_id("http://twitter.com/VizzQuotes/statuses/409030735191097344"),
            "409030735191097344",
        )
        self.assertEqual(get_tweet_id("https://twitter.com/wdaher/status/1017581858"), "1017581858")
        self.assertEqual(
            get_tweet_id("https://twitter.com/wdaher/status/1017581858/"), "1017581858"
        )
        self.assertEqual(
            get_tweet_id("https://twitter.com/windyoona/status/410766290349879296/photo/1"),
            "410766290349879296",
        )
        self.assertEqual(
            get_tweet_id("https://twitter.com/windyoona/status/410766290349879296/"),
            "410766290349879296",
        )

    def test_inline_interesting_links(self) -> None:
        def make_link(url: str) -> str:
            return f'<a href="{url}">{url}</a>'

        normal_tweet_html = (
            '<a href="https://twitter.com/Twitter"'
            ">@Twitter</a> "
            "meets @seepicturely at #tcdisrupt cc."
            '<a href="https://twitter.com/boscomonkey"'
            ">@boscomonkey</a> "
            '<a href="https://twitter.com/episod"'
            ">@episod</a> "
            '<a href="http://t.co/6J2EgYM"'
            ">http://instagr.am/p/MuW67/</a>"
        )

        mention_in_link_tweet_html = """<a href="http://t.co/@foo">http://foo.com</a>"""

        media_tweet_html = (
            '<a href="http://t.co/xo7pAhK6n3">'
            "http://twitter.com/NEVNBoston/status/421654515616849920/photo/1</a>"
        )

        emoji_in_tweet_html = """Zulip is <span aria-label=\"100\" class="emoji emoji-1f4af" role=\"img\" title="100">:100:</span>% open-source!"""

        def make_inline_twitter_preview(url: str, tweet_html: str, image_html: str = "") -> str:
            ## As of right now, all previews are mocked to be the exact same tweet
            return (
                '<div class="inline-preview-twitter">'
                '<div class="twitter-tweet">'
                f'<a href="{url}">'
                '<img class="twitter-avatar"'
                ' src="https://external-content.zulipcdn.net/external_content/1f7cd2436976d410eab8189ebceda87ae0b34ead/687474703a2f2f7062732e7477696d672e63'
                "6f6d2f70726f66696c655f696d616765732f313338303931323137332f53637265656e5f73686f745f323031312d30362d30335f61745f372e33352e33"
                '365f504d5f6e6f726d616c2e706e67">'
                "</a>"
                f"<p>{tweet_html}</p>"
                "<span>- Eoin McMillan (@imeoin)</span>"
                f"{image_html}"
                "</div>"
                "</div>"
            )

        msg = "http://www.twitter.com"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(converted, "<p>{}</p>".format(make_link("http://www.twitter.com")))

        msg = "http://www.twitter.com/wdaher/"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(converted, "<p>{}</p>".format(make_link("http://www.twitter.com/wdaher/")))

        msg = "http://www.twitter.com/wdaher/status/3"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted, "<p>{}</p>".format(make_link("http://www.twitter.com/wdaher/status/3"))
        )

        # id too long
        msg = "http://www.twitter.com/wdaher/status/2879779692873154569"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{}</p>".format(
                make_link("http://www.twitter.com/wdaher/status/2879779692873154569")
            ),
        )

        # id too large (i.e. tweet doesn't exist)
        msg = "http://www.twitter.com/wdaher/status/999999999999999999"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{}</p>".format(
                make_link("http://www.twitter.com/wdaher/status/999999999999999999")
            ),
        )

        msg = "http://www.twitter.com/wdaher/status/287977969287315456"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{}</p>\n{}".format(
                make_link("http://www.twitter.com/wdaher/status/287977969287315456"),
                make_inline_twitter_preview(
                    "http://www.twitter.com/wdaher/status/287977969287315456", normal_tweet_html
                ),
            ),
        )

        msg = "https://www.twitter.com/wdaher/status/287977969287315456"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{}</p>\n{}".format(
                make_link("https://www.twitter.com/wdaher/status/287977969287315456"),
                make_inline_twitter_preview(
                    "https://www.twitter.com/wdaher/status/287977969287315456", normal_tweet_html
                ),
            ),
        )

        msg = "http://twitter.com/wdaher/status/287977969287315456"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{}</p>\n{}".format(
                make_link("http://twitter.com/wdaher/status/287977969287315456"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315456", normal_tweet_html
                ),
            ),
        )

        # Repeated links will only be converted once
        msg = (
            "http://twitter.com/wdaher/status/287977969287315456 "
            "http://twitter.com/wdaher/status/287977969287315457 "
            "http://twitter.com/wdaher/status/287977969287315457 "
            "http://twitter.com/wdaher/status/287977969287315457"
        )
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{} {} {} {}</p>\n{}{}".format(
                make_link("http://twitter.com/wdaher/status/287977969287315456"),
                make_link("http://twitter.com/wdaher/status/287977969287315457"),
                make_link("http://twitter.com/wdaher/status/287977969287315457"),
                make_link("http://twitter.com/wdaher/status/287977969287315457"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315456", normal_tweet_html
                ),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315457", normal_tweet_html
                ),
            ),
        )

        # A max of 3 will be converted
        msg = (
            "http://twitter.com/wdaher/status/287977969287315456 "
            "http://twitter.com/wdaher/status/287977969287315457 "
            "https://twitter.com/wdaher/status/287977969287315456 "
            "http://twitter.com/wdaher/status/287977969287315460"
        )
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{} {} {} {}</p>\n{}{}{}".format(
                make_link("http://twitter.com/wdaher/status/287977969287315456"),
                make_link("http://twitter.com/wdaher/status/287977969287315457"),
                make_link("https://twitter.com/wdaher/status/287977969287315456"),
                make_link("http://twitter.com/wdaher/status/287977969287315460"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315456", normal_tweet_html
                ),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315457", normal_tweet_html
                ),
                make_inline_twitter_preview(
                    "https://twitter.com/wdaher/status/287977969287315456", normal_tweet_html
                ),
            ),
        )

        # Test smart in-place inlining behavior:
        msg = (
            "Paragraph 1: http://twitter.com/wdaher/status/287977969287315456\n\n"
            "Paragraph 2\n\n"
            "Paragraph 3: http://twitter.com/wdaher/status/287977969287315457"
        )
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>Paragraph 1: {}</p>\n{}<p>Paragraph 2</p>\n<p>Paragraph 3: {}</p>\n{}".format(
                make_link("http://twitter.com/wdaher/status/287977969287315456"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315456", normal_tweet_html
                ),
                make_link("http://twitter.com/wdaher/status/287977969287315457"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315457", normal_tweet_html
                ),
            ),
        )

        # Tweet has a mention in a URL, only the URL is linked
        msg = "http://twitter.com/wdaher/status/287977969287315458"

        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{}</p>\n{}".format(
                make_link("http://twitter.com/wdaher/status/287977969287315458"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315458",
                    mention_in_link_tweet_html,
                ),
            ),
        )

        # Tweet with an image
        msg = "http://twitter.com/wdaher/status/287977969287315459"

        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{}</p>\n{}".format(
                make_link("http://twitter.com/wdaher/status/287977969287315459"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315459",
                    media_tweet_html,
                    (
                        '<div class="twitter-image">'
                        '<a href="http://t.co/xo7pAhK6n3">'
                        f"""<img src="{get_camo_url("https://pbs.twimg.com/media/BdoEjD4IEAIq86Z.jpg:small")}">"""
                        "</a>"
                        "</div>"
                    ),
                ),
            ),
        )

        msg = "http://twitter.com/wdaher/status/287977969287315460"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>{}</p>\n{}".format(
                make_link("http://twitter.com/wdaher/status/287977969287315460"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315460", emoji_in_tweet_html
                ),
            ),
        )

        # Test Twitter previews in spoiler tags.
        msg = "```spoiler secret tweet\nTweet: http://twitter.com/wdaher/status/287977969287315456\n```"
        converted = markdown_convert_wrapper(msg)

        rendered_spoiler = '<div class="spoiler-block"><div class="spoiler-header">\n<p>secret tweet</p>\n</div><div class="spoiler-content" aria-hidden="true">\n<p>Tweet: {}</p>\n{}</div></div>'
        self.assertEqual(
            converted,
            rendered_spoiler.format(
                make_link("http://twitter.com/wdaher/status/287977969287315456"),
                make_inline_twitter_preview(
                    "http://twitter.com/wdaher/status/287977969287315456", normal_tweet_html
                ),
            ),
        )

    def test_fetch_tweet_data_settings_validation(self) -> None:
        with self.settings(TEST_SUITE=False, TWITTER_CONSUMER_KEY=None):
            self.assertIs(None, fetch_tweet_data("287977969287315459"))

    def test_content_has_emoji(self) -> None:
        self.assertFalse(content_has_emoji_syntax("boring"))
        self.assertFalse(content_has_emoji_syntax("hello: world"))
        self.assertFalse(content_has_emoji_syntax(":foobar"))
        self.assertFalse(content_has_emoji_syntax("::: hello :::"))

        self.assertTrue(content_has_emoji_syntax("foo :whatever:"))
        self.assertTrue(content_has_emoji_syntax("\n:whatever:"))
        self.assertTrue(content_has_emoji_syntax(":smile: ::::::"))

    def test_realm_emoji(self) -> None:
        def emoji_img(name: str, file_name: str, realm_id: int) -> str:
            return '<img alt="{}" class="emoji" src="{}" title="{}">'.format(
                name, get_emoji_url(file_name, realm_id), name[1:-1].replace("_", " ")
            )

        realm = get_realm("zulip")

        # Needs to mock an actual message because that's how Markdown obtains the realm
        msg = Message(sender=self.example_user("hamlet"))
        converted = markdown_convert(":green_tick:", message_realm=realm, message=msg)
        realm_emoji = RealmEmoji.objects.filter(
            realm=realm, name="green_tick", deactivated=False
        ).get()
        self.assertEqual(
            converted,
            "<p>{}</p>".format(emoji_img(":green_tick:", realm_emoji.file_name, realm.id)),
        )

        # Deactivate realm emoji.
        do_remove_realm_emoji(realm, "green_tick")
        converted = markdown_convert(":green_tick:", message_realm=realm, message=msg)
        self.assertEqual(converted, "<p>:green_tick:</p>")

    def test_deactivated_realm_emoji(self) -> None:
        # Deactivate realm emoji.
        realm = get_realm("zulip")
        do_remove_realm_emoji(realm, "green_tick")

        msg = Message(sender=self.example_user("hamlet"))
        converted = markdown_convert(":green_tick:", message_realm=realm, message=msg)
        self.assertEqual(converted, "<p>:green_tick:</p>")

    def test_unicode_emoji(self) -> None:
        msg = "\u2615"  # ☕
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            '<p><span aria-label="coffee" class="emoji emoji-2615" role="img" title="coffee">:coffee:</span></p>',
        )

        msg = "\u2615\u2615"  # ☕☕
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            '<p><span aria-label="coffee" class="emoji emoji-2615" role="img" title="coffee">:coffee:</span><span aria-label="coffee" class="emoji emoji-2615" role="img" title="coffee">:coffee:</span></p>',
        )

    def test_no_translate_emoticons_if_off(self) -> None:
        user_profile = self.example_user("othello")
        do_set_user_display_setting(user_profile, "translate_emoticons", False)
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = ":)"
        expected = "<p>:)</p>"
        converted = render_markdown(msg, content)
        self.assertEqual(converted, expected)

    def test_same_markup(self) -> None:
        msg = "\u2615"  # ☕
        unicode_converted = markdown_convert_wrapper(msg)

        msg = ":coffee:"  # ☕☕
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(converted, unicode_converted)

    def test_links_in_topic_name(self) -> None:
        realm = get_realm("zulip")
        msg = Message(sender=self.example_user("othello"))

        msg.set_topic_name("https://google.com/hello-world")
        converted_topic = topic_links(realm.id, msg.topic_name())
        self.assertEqual(
            converted_topic,
            [{"url": "https://google.com/hello-world", "text": "https://google.com/hello-world"}],
        )

        msg.set_topic_name("http://google.com/hello-world")
        converted_topic = topic_links(realm.id, msg.topic_name())
        self.assertEqual(
            converted_topic,
            [{"url": "http://google.com/hello-world", "text": "http://google.com/hello-world"}],
        )

        msg.set_topic_name("Without scheme google.com/hello-world")
        converted_topic = topic_links(realm.id, msg.topic_name())
        self.assertEqual(
            converted_topic,
            [{"url": "https://google.com/hello-world", "text": "google.com/hello-world"}],
        )

        msg.set_topic_name("Without scheme random.words/hello-world")
        converted_topic = topic_links(realm.id, msg.topic_name())
        self.assertEqual(converted_topic, [])

        msg.set_topic_name(
            "Try out http://ftp.debian.org, https://google.com/ and https://google.in/."
        )
        converted_topic = topic_links(realm.id, msg.topic_name())
        self.assertEqual(
            converted_topic,
            [
                {"url": "http://ftp.debian.org", "text": "http://ftp.debian.org"},
                {"url": "https://google.com/", "text": "https://google.com/"},
                {"url": "https://google.in/", "text": "https://google.in/"},
            ],
        )

        # test order for links without scheme
        msg.set_topic_name("google.in google.com")
        converted_topic = topic_links(realm.id, msg.topic_name())
        self.assertEqual(
            converted_topic,
            [
                {"url": "https://google.in", "text": "google.in"},
                {"url": "https://google.com", "text": "google.com"},
            ],
        )

    def test_realm_patterns(self) -> None:
        realm = get_realm("zulip")
        url_format_string = r"https://trac.example.com/ticket/%(id)s"
        linkifier = RealmFilter(
            realm=realm, pattern=r"#(?P<id>[0-9]{2,8})", url_format_string=url_format_string
        )
        linkifier.save()
        self.assertEqual(
            linkifier.__str__(),
            "<RealmFilter(zulip): #(?P<id>[0-9]{2,8}) https://trac.example.com/ticket/%(id)s>",
        )

        msg = Message(sender=self.example_user("othello"))
        msg.set_topic_name("#444")

        flush_per_request_caches()

        content = "We should fix #224 and #115, but not issue#124 or #1124z or [trac #15](https://trac.example.com/ticket/16) today."
        converted = markdown_convert(content, message_realm=realm, message=msg)
        converted_topic = topic_links(realm.id, msg.topic_name())

        self.assertEqual(
            converted,
            '<p>We should fix <a href="https://trac.example.com/ticket/224">#224</a> and <a href="https://trac.example.com/ticket/115">#115</a>, but not issue#124 or #1124z or <a href="https://trac.example.com/ticket/16">trac #15</a> today.</p>',
        )
        self.assertEqual(
            converted_topic, [{"url": "https://trac.example.com/ticket/444", "text": "#444"}]
        )

        msg.set_topic_name("#444 https://google.com")
        converted_topic = topic_links(realm.id, msg.topic_name())
        self.assertEqual(
            converted_topic,
            [
                {"url": "https://trac.example.com/ticket/444", "text": "#444"},
                {"url": "https://google.com", "text": "https://google.com"},
            ],
        )

        RealmFilter(
            realm=realm,
            pattern=r"#(?P<id>[a-zA-Z]+-[0-9]+)",
            url_format_string=r"https://trac.example.com/ticket/%(id)s",
        ).save()
        msg = Message(sender=self.example_user("hamlet"))

        content = "#ZUL-123 was fixed and code was deployed to production, also #zul-321 was deployed to staging"
        converted = markdown_convert(content, message_realm=realm, message=msg)

        self.assertEqual(
            converted,
            '<p><a href="https://trac.example.com/ticket/ZUL-123">#ZUL-123</a> was fixed and code was deployed to production, also <a href="https://trac.example.com/ticket/zul-321">#zul-321</a> was deployed to staging</p>',
        )

        def assert_conversion(content: str, should_have_converted: bool = True) -> None:
            converted = markdown_convert(content, message_realm=realm, message=msg)
            converted_topic = topic_links(realm.id, content)
            if should_have_converted:
                self.assertTrue("https://trac.example.com" in converted)
                self.assertEqual(len(converted_topic), 1)
                self.assertEqual(
                    converted_topic[0],
                    {"url": "https://trac.example.com/ticket/123", "text": "#123"},
                )
            else:
                self.assertTrue("https://trac.example.com" not in converted)
                self.assertEqual(len(converted_topic), 0)

        assert_conversion("Hello #123 World")
        assert_conversion("Hello #123World", False)
        assert_conversion("Hello#123 World", False)
        assert_conversion("Hello#123World", False)
        # Ideally, these should be converted, but Markdown doesn't
        # handle word boundary detection in languages that don't use
        # whitespace for that correctly yet.
        assert_conversion("チケットは#123です", False)
        assert_conversion("チケットは #123です", False)
        assert_conversion("チケットは#123 です", False)
        assert_conversion("チケットは #123 です")
        assert_conversion("(#123)")
        assert_conversion("#123>")
        assert_conversion('"#123"')
        assert_conversion("#123@")
        assert_conversion(")#123(", False)
        assert_conversion("##123", False)

        # test nested realm patterns should avoid double matching
        RealmFilter(
            realm=realm,
            pattern=r"hello#(?P<id>[0-9]+)",
            url_format_string=r"https://trac.example.com/hello/%(id)s",
        ).save()
        converted_topic = topic_links(realm.id, "hello#123 #234")
        self.assertEqual(
            converted_topic,
            [
                {"url": "https://trac.example.com/hello/123", "text": "hello#123"},
                {"url": "https://trac.example.com/ticket/234", "text": "#234"},
            ],
        )

        # test correct order when realm pattern and normal links are both present.
        converted_topic = topic_links(realm.id, "#234 https://google.com")
        self.assertEqual(
            converted_topic,
            [
                {"url": "https://trac.example.com/ticket/234", "text": "#234"},
                {"url": "https://google.com", "text": "https://google.com"},
            ],
        )

    def test_multiple_matching_realm_patterns(self) -> None:
        realm = get_realm("zulip")
        url_format_string = r"https://trac.example.com/ticket/%(id)s"
        linkifier_1 = RealmFilter(
            realm=realm,
            pattern=r"(?P<id>ABC\-[0-9]+)",
            url_format_string=url_format_string,
        )
        linkifier_1.save()
        self.assertEqual(
            linkifier_1.__str__(),
            r"<RealmFilter(zulip): (?P<id>ABC\-[0-9]+) https://trac.example.com/ticket/%(id)s>",
        )

        url_format_string = r"https://other-trac.example.com/ticket/%(id)s"
        linkifier_2 = RealmFilter(
            realm=realm,
            pattern=r"(?P<id>[A-Z][A-Z0-9]*\-[0-9]+)",
            url_format_string=url_format_string,
        )
        linkifier_2.save()
        self.assertEqual(
            linkifier_2.__str__(),
            r"<RealmFilter(zulip): (?P<id>[A-Z][A-Z0-9]*\-[0-9]+)"
            " https://other-trac.example.com/ticket/%(id)s>",
        )

        msg = Message(sender=self.example_user("othello"))
        msg.set_topic_name("ABC-123")

        flush_per_request_caches()

        content = (
            "We should fix ABC-123 or [trac ABC-123](https://trac.example.com/ticket/16) today."
        )
        converted = markdown_convert(content, message_realm=realm, message=msg)
        converted_topic = topic_links(realm.id, msg.topic_name())

        # The second linkifier (which was saved later) was ignored as the content was marked AtomicString after first conversion.
        # There was no easy way to support parsing both linkifiers and not run into an infinite loop, hence the second linkifier is ignored.
        self.assertEqual(
            converted,
            '<p>We should fix <a href="https://trac.example.com/ticket/ABC-123">ABC-123</a> or <a href="https://trac.example.com/ticket/16">trac ABC-123</a> today.</p>',
        )
        # Both the links should be generated in topics.
        self.assertEqual(
            converted_topic,
            [
                {"url": "https://trac.example.com/ticket/ABC-123", "text": "ABC-123"},
                {"url": "https://other-trac.example.com/ticket/ABC-123", "text": "ABC-123"},
            ],
        )

    def test_flush_linkifier(self) -> None:
        realm = get_realm("zulip")

        def flush() -> None:
            """
            flush_linkifiers is a post-save hook, so calling it
            directly for testing is kind of awkward
            """

            class Instance:
                realm_id: Optional[int] = None

            instance = Instance()
            instance.realm_id = realm.id
            flush_linkifiers(sender=None, instance=instance)

        def save_new_linkifier() -> None:
            linkifier = RealmFilter(realm=realm, pattern=r"whatever", url_format_string="whatever")
            linkifier.save()

        # start fresh for our realm
        flush()
        self.assertFalse(realm_in_local_linkifiers_cache(realm.id))

        # call this just for side effects of populating the cache
        linkifiers_for_realm(realm.id)
        self.assertTrue(realm_in_local_linkifiers_cache(realm.id))

        # Saving a new RealmFilter should have the side effect of
        # flushing the cache.
        save_new_linkifier()
        self.assertFalse(realm_in_local_linkifiers_cache(realm.id))

        # and flush it one more time, to make sure we don't get a KeyError
        flush()
        self.assertFalse(realm_in_local_linkifiers_cache(realm.id))

    def test_realm_patterns_negative(self) -> None:
        realm = get_realm("zulip")
        RealmFilter(
            realm=realm,
            pattern=r"#(?P<id>[0-9]{2,8})",
            url_format_string=r"https://trac.example.com/ticket/%(id)s",
        ).save()
        boring_msg = Message(sender=self.example_user("othello"))
        boring_msg.set_topic_name("no match here")
        converted_boring_topic = topic_links(realm.id, boring_msg.topic_name())
        self.assertEqual(converted_boring_topic, [])

    def test_is_status_message(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = "/me makes a list\n* one\n* two"
        rendered_content = render_markdown(msg, content)
        self.assertEqual(
            rendered_content,
            "<p>/me makes a list</p>\n<ul>\n<li>one</li>\n<li>two</li>\n</ul>",
        )
        self.assertTrue(Message.is_status_message(content, rendered_content))

        content = "/me takes a walk"
        rendered_content = render_markdown(msg, content)
        self.assertEqual(
            rendered_content,
            "<p>/me takes a walk</p>",
        )
        self.assertTrue(Message.is_status_message(content, rendered_content))

        content = "/me writes a second line\nline"
        rendered_content = render_markdown(msg, content)
        self.assertEqual(
            rendered_content,
            "<p>/me writes a second line<br>\nline</p>",
        )
        self.assertTrue(Message.is_status_message(content, rendered_content))

    def test_alert_words(self) -> None:
        user_profile = self.example_user("othello")
        do_add_alert_words(user_profile, ["ALERTWORD", "scaryword"])
        msg = Message(sender=user_profile, sending_client=get_client("test"))
        realm_alert_words_automaton = get_alert_word_automaton(user_profile.realm)

        def render(msg: Message, content: str) -> str:
            return render_markdown(
                msg, content, realm_alert_words_automaton=realm_alert_words_automaton
            )

        content = "We have an ALERTWORD day today!"
        self.assertEqual(render(msg, content), "<p>We have an ALERTWORD day today!</p>")
        self.assertEqual(msg.user_ids_with_alert_words, {user_profile.id})

        msg = Message(sender=user_profile, sending_client=get_client("test"))
        content = "We have a NOTHINGWORD day today!"
        self.assertEqual(render(msg, content), "<p>We have a NOTHINGWORD day today!</p>")
        self.assertEqual(msg.user_ids_with_alert_words, set())

    def test_alert_words_returns_user_ids_with_alert_words(self) -> None:
        alert_words_for_users: Dict[str, List[str]] = {
            "hamlet": ["how"],
            "cordelia": ["this possible"],
            "iago": ["hello"],
            "prospero": ["hello"],
            "othello": ["how are you"],
            "aaron": ["hey"],
        }
        user_profiles: Dict[str, UserProfile] = {}
        for (username, alert_words) in alert_words_for_users.items():
            user_profile = self.example_user(username)
            user_profiles.update({username: user_profile})
            do_add_alert_words(user_profile, alert_words)
        sender_user_profile = self.example_user("polonius")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        realm_alert_words_automaton = get_alert_word_automaton(sender_user_profile.realm)

        def render(msg: Message, content: str) -> str:
            return render_markdown(
                msg, content, realm_alert_words_automaton=realm_alert_words_automaton
            )

        content = "hello how is this possible how are you doing today"
        render(msg, content)
        expected_user_ids: Set[int] = {
            user_profiles["hamlet"].id,
            user_profiles["cordelia"].id,
            user_profiles["iago"].id,
            user_profiles["prospero"].id,
            user_profiles["othello"].id,
        }
        # All users except aaron have their alert word appear in the message content
        self.assertEqual(msg.user_ids_with_alert_words, expected_user_ids)

    def test_alert_words_returns_user_ids_with_alert_words_1(self) -> None:
        alert_words_for_users: Dict[str, List[str]] = {
            "hamlet": ["provisioning", "Prod deployment"],
            "cordelia": ["test", "Prod"],
            "iago": ["prod"],
            "prospero": ["deployment"],
            "othello": ["last"],
        }
        user_profiles: Dict[str, UserProfile] = {}
        for (username, alert_words) in alert_words_for_users.items():
            user_profile = self.example_user(username)
            user_profiles.update({username: user_profile})
            do_add_alert_words(user_profile, alert_words)
        sender_user_profile = self.example_user("polonius")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        realm_alert_words_automaton = get_alert_word_automaton(sender_user_profile.realm)

        def render(msg: Message, content: str) -> str:
            return render_markdown(
                msg, content, realm_alert_words_automaton=realm_alert_words_automaton
            )

        content = """Hello, everyone. Prod deployment has been completed
        And this is a new line
        to test out how Markdown convert this into something line ending split array
        and this is a new line
        last"""
        render(msg, content)
        expected_user_ids: Set[int] = {
            user_profiles["hamlet"].id,
            user_profiles["cordelia"].id,
            user_profiles["iago"].id,
            user_profiles["prospero"].id,
            user_profiles["othello"].id,
        }
        # All users have their alert word appear in the message content
        self.assertEqual(msg.user_ids_with_alert_words, expected_user_ids)

    def test_alert_words_returns_user_ids_with_alert_words_in_french(self) -> None:
        alert_words_for_users: Dict[str, List[str]] = {
            "hamlet": ["réglementaire", "une politique", "une merveille"],
            "cordelia": ["énormément", "Prod"],
            "iago": ["prod"],
            "prospero": ["deployment"],
            "othello": ["last"],
        }
        user_profiles: Dict[str, UserProfile] = {}
        for (username, alert_words) in alert_words_for_users.items():
            user_profile = self.example_user(username)
            user_profiles.update({username: user_profile})
            do_add_alert_words(user_profile, alert_words)
        sender_user_profile = self.example_user("polonius")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        realm_alert_words_automaton = get_alert_word_automaton(sender_user_profile.realm)

        def render(msg: Message, content: str) -> str:
            return render_markdown(
                msg, content, realm_alert_words_automaton=realm_alert_words_automaton
            )

        content = """This is to test out alert words work in languages with accented characters too
        bonjour est (énormément) ce a quoi ressemble le français
        et j'espère qu'il n'y n' réglementaire a pas de mots d'alerte dans ce texte français
        """
        render(msg, content)
        expected_user_ids: Set[int] = {user_profiles["hamlet"].id, user_profiles["cordelia"].id}
        # Only hamlet and cordelia have their alert-words appear in the message content
        self.assertEqual(msg.user_ids_with_alert_words, expected_user_ids)

    def test_alert_words_returns_empty_user_ids_with_alert_words(self) -> None:
        alert_words_for_users: Dict[str, List[str]] = {
            "hamlet": [],
            "cordelia": [],
            "iago": [],
            "prospero": [],
            "othello": [],
            "aaron": [],
        }
        user_profiles: Dict[str, UserProfile] = {}
        for (username, alert_words) in alert_words_for_users.items():
            user_profile = self.example_user(username)
            user_profiles.update({username: user_profile})
            do_add_alert_words(user_profile, alert_words)
        sender_user_profile = self.example_user("polonius")
        msg = Message(sender=user_profile, sending_client=get_client("test"))
        realm_alert_words_automaton = get_alert_word_automaton(sender_user_profile.realm)

        def render(msg: Message, content: str) -> str:
            return render_markdown(
                msg, content, realm_alert_words_automaton=realm_alert_words_automaton
            )

        content = """hello how is this possible how are you doing today
        This is to test that the no user_ids who have alrert wourldword is participating
        in sending of the message
        """
        render(msg, content)
        expected_user_ids: Set[int] = set()
        # None of the users have their alert-words appear in the message content
        self.assertEqual(msg.user_ids_with_alert_words, expected_user_ids)

    def get_mock_alert_words(self, num_words: int, word_length: int) -> List[str]:
        alert_words = ["x" * word_length] * num_words  # type List[str]
        return alert_words

    def test_alert_words_with_empty_alert_words(self) -> None:
        alert_words_for_users: Dict[str, List[str]] = {
            "hamlet": [],
            "cordelia": [],
            "iago": [],
            "othello": [],
        }
        user_profiles: Dict[str, UserProfile] = {}
        for (username, alert_words) in alert_words_for_users.items():
            user_profile = self.example_user(username)
            user_profiles.update({username: user_profile})
            do_add_alert_words(user_profile, alert_words)
        sender_user_profile = self.example_user("polonius")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        realm_alert_words_automaton = get_alert_word_automaton(sender_user_profile.realm)

        def render(msg: Message, content: str) -> str:
            return render_markdown(
                msg, content, realm_alert_words_automaton=realm_alert_words_automaton
            )

        content = """This is to test a empty alert words i.e. no user has any alert-words set"""
        render(msg, content)
        expected_user_ids: Set[int] = set()
        self.assertEqual(msg.user_ids_with_alert_words, expected_user_ids)

    def test_alert_words_retuns_user_ids_with_alert_words_with_huge_alert_words(self) -> None:

        alert_words_for_users: Dict[str, List[str]] = {
            "hamlet": ["issue124"],
            "cordelia": self.get_mock_alert_words(500, 10),
            "iago": self.get_mock_alert_words(500, 10),
            "othello": self.get_mock_alert_words(500, 10),
        }
        user_profiles: Dict[str, UserProfile] = {}
        for (username, alert_words) in alert_words_for_users.items():
            user_profile = self.example_user(username)
            user_profiles.update({username: user_profile})
            do_add_alert_words(user_profile, alert_words)
        sender_user_profile = self.example_user("polonius")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        realm_alert_words_automaton = get_alert_word_automaton(sender_user_profile.realm)

        def render(msg: Message, content: str) -> str:
            return render_markdown(
                msg, content, realm_alert_words_automaton=realm_alert_words_automaton
            )

        content = """The code above will print 10 random values of numbers between 1 and 100.
        The second line, for x in range(10), determines how many values will be printed (when you use
        range(x), the number that you use in place of x will be the amount of values that you'll have
        printed. if you want 20 values, use range(20). use range(5) if you only want 5 values returned,
        etc.). I was talking abou the issue124 on github. Then the third line: print random.randint(1,101) will automatically select a random integer
        between 1 and 100 for you. The process is fairly simple
        """
        render(msg, content)
        expected_user_ids: Set[int] = {user_profiles["hamlet"].id}
        # Only hamlet has alert-word 'issue124' present in the message content
        self.assertEqual(msg.user_ids_with_alert_words, expected_user_ids)

    def test_default_code_block_language(self) -> None:
        realm = get_realm("zulip")
        self.assertEqual(realm.default_code_block_language, None)
        text = "```{}\nconsole.log('Hello World');\n```\n"

        # Render without default language
        msg_with_js = markdown_convert_wrapper(text.format("js"))
        msg_with_python = markdown_convert_wrapper(text.format("python"))
        msg_without_language = markdown_convert_wrapper(text.format(""))
        msg_with_quote = markdown_convert_wrapper(text.format("quote"))
        msg_with_math = markdown_convert_wrapper(text.format("math"))
        msg_with_none = markdown_convert_wrapper(text.format("none"))

        # Render with default=javascript
        do_set_realm_property(realm, "default_code_block_language", "javascript", acting_user=None)
        msg_without_language_default_js = markdown_convert_wrapper(text.format(""))
        msg_with_python_default_js = markdown_convert_wrapper(text.format("python"))

        # Render with default=python
        do_set_realm_property(realm, "default_code_block_language", "python", acting_user=None)
        msg_without_language_default_py = markdown_convert_wrapper(text.format(""))
        msg_with_none_default_py = markdown_convert_wrapper(text.format("none"))

        # Render with default=quote
        do_set_realm_property(realm, "default_code_block_language", "quote", acting_user=None)
        msg_without_language_default_quote = markdown_convert_wrapper(text.format(""))

        # Render with default=math
        do_set_realm_property(realm, "default_code_block_language", "math", acting_user=None)
        msg_without_language_default_math = markdown_convert_wrapper(text.format(""))

        # Render without default language
        do_set_realm_property(realm, "default_code_block_language", None, acting_user=None)
        msg_without_language_final = markdown_convert_wrapper(text.format(""))

        self.assertTrue(msg_with_js == msg_without_language_default_js)
        self.assertTrue(
            msg_with_python == msg_with_python_default_js == msg_without_language_default_py
        )
        self.assertTrue(msg_with_quote == msg_without_language_default_quote)
        self.assertTrue(msg_with_math == msg_without_language_default_math)
        self.assertTrue(msg_without_language == msg_without_language_final)
        self.assertTrue(msg_with_none == msg_with_none_default_py)

        # Test checking inside nested quotes
        nested_text = "````quote\n\n{}\n\n{}````".format(text.format("js"), text.format(""))
        do_set_realm_property(realm, "default_code_block_language", "javascript", acting_user=None)
        rendered = markdown_convert_wrapper(nested_text)
        with_language, without_language = re.findall(r"<pre>(.*?)$", rendered, re.MULTILINE)
        self.assertTrue(with_language == without_language)

        do_set_realm_property(realm, "default_code_block_language", None, acting_user=None)
        rendered = markdown_convert_wrapper(nested_text)
        with_language, without_language = re.findall(r"<pre>(.*?)$", rendered, re.MULTILINE)
        self.assertFalse(with_language == without_language)

    def test_mention_wildcard(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = "@**all** test"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" data-user-id="*">' "@all" "</span> test</p>",
        )
        self.assertTrue(msg.mentions_wildcard)

    def test_mention_everyone(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = "@**everyone** test"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" data-user-id="*">' "@everyone" "</span> test</p>",
        )
        self.assertTrue(msg.mentions_wildcard)

    def test_mention_stream(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = "@**stream** test"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" data-user-id="*">' "@stream" "</span> test</p>",
        )
        self.assertTrue(msg.mentions_wildcard)

    def test_mention_at_wildcard(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = "@all test"
        self.assertEqual(render_markdown(msg, content), "<p>@all test</p>")
        self.assertFalse(msg.mentions_wildcard)
        self.assertEqual(msg.mentions_user_ids, set())

    def test_mention_at_everyone(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = "@everyone test"
        self.assertEqual(render_markdown(msg, content), "<p>@everyone test</p>")
        self.assertFalse(msg.mentions_wildcard)
        self.assertEqual(msg.mentions_user_ids, set())

    def test_mention_word_starting_with_at_wildcard(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = "test @alleycat.com test"
        self.assertEqual(render_markdown(msg, content), "<p>test @alleycat.com test</p>")
        self.assertFalse(msg.mentions_wildcard)
        self.assertEqual(msg.mentions_user_ids, set())

    def test_mention_at_normal_user(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        content = "@aaron test"
        self.assertEqual(render_markdown(msg, content), "<p>@aaron test</p>")
        self.assertFalse(msg.mentions_wildcard)
        self.assertEqual(msg.mentions_user_ids, set())

    def test_mention_single(self) -> None:
        sender_user_profile = self.example_user("othello")
        user_profile = self.example_user("hamlet")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        user_id = user_profile.id

        content = "@**King Hamlet**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" ' f'data-user-id="{user_id}">' "@King Hamlet</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, {user_profile.id})

        content = f"@**|{user_id}**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" ' f'data-user-id="{user_id}">' "@King Hamlet</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, {user_profile.id})

    def test_mention_silent(self) -> None:
        sender_user_profile = self.example_user("othello")
        user_profile = self.example_user("hamlet")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        user_id = user_profile.id

        content = "@_**King Hamlet**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention silent" '
            f'data-user-id="{user_id}">'
            "King Hamlet</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, set())

    def test_silent_wildcard_mention(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        wildcards = ["all", "everyone", "stream"]
        for wildcard in wildcards:
            content = f"@_**{wildcard}**"
            self.assertEqual(
                render_markdown(msg, content),
                f'<p><span class="user-mention silent" data-user-id="*">{wildcard}</span></p>',
            )
            self.assertFalse(msg.mentions_wildcard)

    def test_mention_invalid_followed_by_valid(self) -> None:
        sender_user_profile = self.example_user("othello")
        user_profile = self.example_user("hamlet")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        user_id = user_profile.id

        content = "@**Invalid user** and @**King Hamlet**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p>@<strong>Invalid user</strong> and <span class="user-mention" '
            f'data-user-id="{user_id}">'
            "@King Hamlet</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, {user_profile.id})

    def test_invalid_mention_not_uses_valid_mention_data(self) -> None:
        sender_user_profile = self.example_user("othello")
        hamlet = self.example_user("hamlet")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))

        # Even though King Hamlet will be present in mention data as
        # it was was fetched for first mention but second mention is
        # incorrect(as it uses hamlet's id) so it should not be able
        # to use that data for creating a valid mention.

        content = f"@**King Hamlet|10** and @**aaron|{hamlet.id}**"
        self.assertEqual(
            render_markdown(msg, content),
            f'<p><span class="user-mention" data-user-id="{hamlet.id}">'
            f"@King Hamlet</span> and @<strong>aaron|{hamlet.id}</strong></p>",
        )
        self.assertEqual(msg.mentions_user_ids, {hamlet.id})

    def test_silent_mention_invalid_followed_by_valid(self) -> None:
        sender_user_profile = self.example_user("othello")
        user_profile = self.example_user("hamlet")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        user_id = user_profile.id

        content = "@_**Invalid user** and @_**King Hamlet**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p>@_<strong>Invalid user</strong> and <span class="user-mention silent" '
            f'data-user-id="{user_id}">'
            "King Hamlet</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, set())

        content = f"@_**|123456789** and @_**|{user_id}**"
        self.assertEqual(
            render_markdown(msg, content),
            "<p>@_<strong>|123456789</strong> and "
            '<span class="user-mention silent" '
            f'data-user-id="{user_id}">'
            "King Hamlet</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, set())

    def test_possible_mentions(self) -> None:
        def assert_mentions(content: str, names: Set[str], has_wildcards: bool = False) -> None:
            self.assertEqual(possible_mentions(content), (names, has_wildcards))

        aaron = self.example_user("aaron")

        assert_mentions("", set())
        assert_mentions("boring", set())
        assert_mentions("@**all**", set(), True)
        assert_mentions("smush@**steve**smush", set())

        assert_mentions(
            f"Hello @**King Hamlet**, @**|{aaron.id}** and @**Cordelia, Lear's daughter**\n@**Foo van Barson|1234** @**all**",
            {"King Hamlet", f"|{aaron.id}", "Cordelia, Lear's daughter", "Foo van Barson|1234"},
            True,
        )

    def test_mention_multiple(self) -> None:
        sender_user_profile = self.example_user("othello")
        hamlet = self.example_user("hamlet")
        cordelia = self.example_user("cordelia")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))

        content = "@**King Hamlet** and @**Cordelia, Lear's daughter**, check this out"

        self.assertEqual(
            render_markdown(msg, content),
            "<p>"
            '<span class="user-mention" '
            f'data-user-id="{hamlet.id}">@King Hamlet</span> and '
            '<span class="user-mention" '
            f'data-user-id="{cordelia.id}">@Cordelia, Lear\'s daughter</span>, '
            "check this out</p>",
        )
        self.assertEqual(msg.mentions_user_ids, {hamlet.id, cordelia.id})

    def test_mention_in_quotes(self) -> None:
        othello = self.example_user("othello")
        hamlet = self.example_user("hamlet")
        cordelia = self.example_user("cordelia")
        msg = Message(sender=othello, sending_client=get_client("test"))

        content = "> @**King Hamlet** and @**Othello, the Moor of Venice**\n\n @**King Hamlet** and @**Cordelia, Lear's daughter**"
        self.assertEqual(
            render_markdown(msg, content),
            "<blockquote>\n<p>"
            f'<span class="user-mention silent" data-user-id="{hamlet.id}">King Hamlet</span>'
            " and "
            f'<span class="user-mention silent" data-user-id="{othello.id}">Othello, the Moor of Venice</span>'
            "</p>\n</blockquote>\n"
            "<p>"
            f'<span class="user-mention" data-user-id="{hamlet.id}">@King Hamlet</span>'
            " and "
            f'<span class="user-mention" data-user-id="{cordelia.id}">@Cordelia, Lear\'s daughter</span>'
            "</p>",
        )
        self.assertEqual(msg.mentions_user_ids, {hamlet.id, cordelia.id})

        # Both fenced quote and > quote should be identical for both silent and regular syntax.
        expected = (
            "<blockquote>\n<p>"
            f'<span class="user-mention silent" data-user-id="{hamlet.id}">King Hamlet</span>'
            "</p>\n</blockquote>"
        )
        content = "```quote\n@**King Hamlet**\n```"
        self.assertEqual(render_markdown(msg, content), expected)
        self.assertEqual(msg.mentions_user_ids, set())
        content = "> @**King Hamlet**"
        self.assertEqual(render_markdown(msg, content), expected)
        self.assertEqual(msg.mentions_user_ids, set())
        content = "```quote\n@_**King Hamlet**\n```"
        self.assertEqual(render_markdown(msg, content), expected)
        self.assertEqual(msg.mentions_user_ids, set())
        content = "> @_**King Hamlet**"
        self.assertEqual(render_markdown(msg, content), expected)
        self.assertEqual(msg.mentions_user_ids, set())

    def test_wildcard_mention_in_quotes(self) -> None:
        user_profile = self.example_user("othello")
        msg = Message(sender=user_profile, sending_client=get_client("test"))

        def assert_silent_mention(content: str, wildcard: str) -> None:
            expected = (
                "<blockquote>\n<p>"
                f'<span class="user-mention silent" data-user-id="*">{wildcard}</span>'
                "</p>\n</blockquote>"
            )
            self.assertEqual(render_markdown(msg, content), expected)
            self.assertFalse(msg.mentions_wildcard)

        wildcards = ["all", "everyone", "stream"]
        for wildcard in wildcards:
            assert_silent_mention(f"> @**{wildcard}**", wildcard)
            assert_silent_mention(f"> @_**{wildcard}**", wildcard)
            assert_silent_mention(f"```quote\n@**{wildcard}**\n```", wildcard)
            assert_silent_mention(f"```quote\n@_**{wildcard}**\n```", wildcard)

    def test_mention_duplicate_full_name(self) -> None:
        realm = get_realm("zulip")

        def make_user(email: str, full_name: str) -> UserProfile:
            return create_user(
                email=email,
                password="whatever",
                realm=realm,
                full_name=full_name,
            )

        sender_user_profile = self.example_user("othello")
        twin1 = make_user("twin1@example.com", "Mark Twin")
        twin2 = make_user("twin2@example.com", "Mark Twin")
        cordelia = self.example_user("cordelia")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))

        content = f"@**Mark Twin|{twin1.id}**, @**Mark Twin|{twin2.id}** and @**Cordelia, Lear's daughter**, hi."

        self.assertEqual(
            render_markdown(msg, content),
            "<p>"
            '<span class="user-mention" '
            f'data-user-id="{twin1.id}">@Mark Twin</span>, '
            '<span class="user-mention" '
            f'data-user-id="{twin2.id}">@Mark Twin</span> and '
            '<span class="user-mention" '
            f'data-user-id="{cordelia.id}">@Cordelia, Lear\'s daughter</span>, '
            "hi.</p>",
        )
        self.assertEqual(msg.mentions_user_ids, {twin1.id, twin2.id, cordelia.id})

    def test_mention_invalid(self) -> None:
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))

        content = "Hey @**Nonexistent User**"
        self.assertEqual(
            render_markdown(msg, content), "<p>Hey @<strong>Nonexistent User</strong></p>"
        )
        self.assertEqual(msg.mentions_user_ids, set())

    def test_user_mention_atomic_string(self) -> None:
        sender_user_profile = self.example_user("othello")
        realm = get_realm("zulip")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        # Create a linkifier.
        url_format_string = r"https://trac.example.com/ticket/%(id)s"
        linkifier = RealmFilter(
            realm=realm, pattern=r"#(?P<id>[0-9]{2,8})", url_format_string=url_format_string
        )
        linkifier.save()
        self.assertEqual(
            linkifier.__str__(),
            "<RealmFilter(zulip): #(?P<id>[0-9]{2,8}) https://trac.example.com/ticket/%(id)s>",
        )
        # Create a user that potentially interferes with the pattern.
        test_user = create_user(
            email="atomic@example.com",
            password="whatever",
            realm=realm,
            full_name="Atomic #123",
        )
        content = "@**Atomic #123**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" '
            f'data-user-id="{test_user.id}">'
            "@Atomic #123</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, {test_user.id})
        content = "@_**Atomic #123**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention silent" '
            f'data-user-id="{test_user.id}">'
            "Atomic #123</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, set())

    def create_user_group_for_test(self, user_group_name: str) -> UserGroup:
        othello = self.example_user("othello")
        return create_user_group(user_group_name, [othello], get_realm("zulip"))

    def test_user_group_mention_single(self) -> None:
        sender_user_profile = self.example_user("othello")
        user_profile = self.example_user("hamlet")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        user_id = user_profile.id
        user_group = self.create_user_group_for_test("support")

        content = "@**King Hamlet** @*support*"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" '
            f'data-user-id="{user_id}">'
            "@King Hamlet</span> "
            '<span class="user-group-mention" '
            f'data-user-group-id="{user_group.id}">'
            "@support</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, {user_profile.id})
        self.assertEqual(msg.mentions_user_group_ids, {user_group.id})

    def test_invalid_user_group_followed_by_valid_mention_single(self) -> None:
        sender_user_profile = self.example_user("othello")
        user_profile = self.example_user("hamlet")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        user_id = user_profile.id
        user_group = self.create_user_group_for_test("support")

        content = "@**King Hamlet** @*Invalid user group* @*support*"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" '
            f'data-user-id="{user_id}">'
            "@King Hamlet</span> "
            "@<em>Invalid user group</em> "
            '<span class="user-group-mention" '
            f'data-user-group-id="{user_group.id}">'
            "@support</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, {user_profile.id})
        self.assertEqual(msg.mentions_user_group_ids, {user_group.id})

    def test_user_group_mention_atomic_string(self) -> None:
        sender_user_profile = self.example_user("othello")
        realm = get_realm("zulip")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        user_profile = self.example_user("hamlet")
        # Create a linkifier.
        url_format_string = r"https://trac.example.com/ticket/%(id)s"
        linkifier = RealmFilter(
            realm=realm, pattern=r"#(?P<id>[0-9]{2,8})", url_format_string=url_format_string
        )
        linkifier.save()
        self.assertEqual(
            linkifier.__str__(),
            "<RealmFilter(zulip): #(?P<id>[0-9]{2,8}) https://trac.example.com/ticket/%(id)s>",
        )
        # Create a user-group that potentially interferes with the pattern.
        user_id = user_profile.id
        user_group = self.create_user_group_for_test("support #123")

        content = "@**King Hamlet** @*support #123*"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><span class="user-mention" '
            f'data-user-id="{user_id}">'
            "@King Hamlet</span> "
            '<span class="user-group-mention" '
            f'data-user-group-id="{user_group.id}">'
            "@support #123</span></p>",
        )
        self.assertEqual(msg.mentions_user_ids, {user_profile.id})
        self.assertEqual(msg.mentions_user_group_ids, {user_group.id})

    def test_possible_user_group_mentions(self) -> None:
        def assert_mentions(content: str, names: Set[str]) -> None:
            self.assertEqual(possible_user_group_mentions(content), names)

        assert_mentions("", set())
        assert_mentions("boring", set())
        assert_mentions("@**all**", set())
        assert_mentions("smush@*steve*smush", set())

        assert_mentions(
            "@*support* Hello @**King Hamlet** and @**Cordelia, Lear's daughter**\n"
            "@**Foo van Barson** @**all**",
            {"support"},
        )

        assert_mentions(
            "Attention @*support*, @*frontend* and @*backend*\ngroups.",
            {"support", "frontend", "backend"},
        )

    def test_user_group_mention_multiple(self) -> None:
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        support = self.create_user_group_for_test("support")
        backend = self.create_user_group_for_test("backend")

        content = "@*support* and @*backend*, check this out"
        self.assertEqual(
            render_markdown(msg, content),
            "<p>"
            '<span class="user-group-mention" '
            f'data-user-group-id="{support.id}">'
            "@support</span> "
            "and "
            '<span class="user-group-mention" '
            f'data-user-group-id="{backend.id}">'
            "@backend</span>, "
            "check this out"
            "</p>",
        )

        self.assertEqual(msg.mentions_user_group_ids, {support.id, backend.id})

    def test_user_group_mention_edit(self) -> None:
        sender_user_profile = self.example_user("hamlet")
        user_profile = self.example_user("othello")
        self.create_user_group_for_test("support")
        self.login("hamlet")

        msg_id = self.send_stream_message(
            sender_user_profile, "Denmark", topic_name="editing", content="test"
        )

        def update_message_and_check_flag(content: str, mentioned: bool) -> None:
            result = self.client_patch(
                "/json/messages/" + str(msg_id),
                {
                    "message_id": msg_id,
                    "content": content,
                },
            )
            self.assert_json_success(result)
            um = UserMessage.objects.get(
                user_profile_id=user_profile.id,
                message_id=msg_id,
            )
            if mentioned:
                self.assertIn("mentioned", um.flags_list())
            else:
                self.assertNotIn("mentioned", um.flags_list())

        update_message_and_check_flag("@*support*", True)
        update_message_and_check_flag("@*support-invalid* edited", False)
        update_message_and_check_flag("@*support* edited", True)
        update_message_and_check_flag("edited", False)
        update_message_and_check_flag("@*support*", True)

    def test_user_group_mention_invalid(self) -> None:
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))

        content = "Hey @*Nonexistent group*"
        self.assertEqual(render_markdown(msg, content), "<p>Hey @<em>Nonexistent group</em></p>")
        self.assertEqual(msg.mentions_user_group_ids, set())

    def test_stream_single(self) -> None:
        denmark = get_stream("Denmark", get_realm("zulip"))
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "#**Denmark**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><a class="stream" data-stream-id="{d.id}" href="/#narrow/stream/{d.id}-Denmark">#{d.name}</a></p>'.format(
                d=denmark,
            ),
        )

    def test_invalid_stream_followed_by_valid_mention(self) -> None:
        denmark = get_stream("Denmark", get_realm("zulip"))
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "#**Invalid** and #**Denmark**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p>#<strong>Invalid</strong> and <a class="stream" data-stream-id="{d.id}" href="/#narrow/stream/{d.id}-Denmark">#{d.name}</a></p>'.format(
                d=denmark,
            ),
        )

    def test_stream_multiple(self) -> None:
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        realm = get_realm("zulip")
        denmark = get_stream("Denmark", realm)
        scotland = get_stream("Scotland", realm)
        content = "Look to #**Denmark** and #**Scotland**, there something"
        self.assertEqual(
            render_markdown(msg, content),
            "<p>Look to "
            '<a class="stream" '
            'data-stream-id="{denmark.id}" '
            'href="/#narrow/stream/{denmark.id}-Denmark">#{denmark.name}</a> and '
            '<a class="stream" '
            'data-stream-id="{scotland.id}" '
            'href="/#narrow/stream/{scotland.id}-Scotland">#{scotland.name}</a>, '
            "there something</p>".format(denmark=denmark, scotland=scotland),
        )

    def test_stream_case_sensitivity(self) -> None:
        realm = get_realm("zulip")
        case_sens = Stream.objects.create(name="CaseSens", realm=realm)
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "#**CaseSens**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><a class="stream" data-stream-id="{s.id}" href="/#narrow/stream/{s.id}-{s.name}">#{s.name}</a></p>'.format(
                s=case_sens,
            ),
        )

    def test_stream_case_sensitivity_nonmatching(self) -> None:
        """#StreamName requires the stream be spelled with the correct case
        currently.  If we change that in the future, we'll need to change this
        test."""
        realm = get_realm("zulip")
        Stream.objects.create(name="CaseSens", realm=realm)
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "#**casesens**"
        self.assertEqual(render_markdown(msg, content), "<p>#<strong>casesens</strong></p>")

    def test_topic_single(self) -> None:
        denmark = get_stream("Denmark", get_realm("zulip"))
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "#**Denmark>some topic**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><a class="stream-topic" data-stream-id="{d.id}" href="/#narrow/stream/{d.id}-Denmark/topic/some.20topic">#{d.name} &gt; some topic</a></p>'.format(
                d=denmark,
            ),
        )

    def test_topic_atomic_string(self) -> None:
        realm = get_realm("zulip")
        # Create a linkifier.
        sender_user_profile = self.example_user("othello")
        url_format_string = r"https://trac.example.com/ticket/%(id)s"
        linkifier = RealmFilter(
            realm=realm, pattern=r"#(?P<id>[0-9]{2,8})", url_format_string=url_format_string
        )
        linkifier.save()
        self.assertEqual(
            linkifier.__str__(),
            "<RealmFilter(zulip): #(?P<id>[0-9]{2,8}) https://trac.example.com/ticket/%(id)s>",
        )
        # Create a topic link that potentially interferes with the pattern.
        denmark = get_stream("Denmark", realm)
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "#**Denmark>#1234**"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><a class="stream-topic" data-stream-id="{d.id}" href="/#narrow/stream/{d.id}-Denmark/topic/.231234">#{d.name} &gt; #1234</a></p>'.format(
                d=denmark,
            ),
        )

    def test_topic_multiple(self) -> None:
        denmark = get_stream("Denmark", get_realm("zulip"))
        scotland = get_stream("Scotland", get_realm("zulip"))
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "This has two links: #**Denmark>some topic** and #**Scotland>other topic**."
        self.assertEqual(
            render_markdown(msg, content),
            "<p>This has two links: "
            '<a class="stream-topic" data-stream-id="{denmark.id}" '
            'href="/#narrow/stream/{denmark.id}-{denmark.name}/topic/some.20topic">'
            "#{denmark.name} &gt; some topic</a>"
            " and "
            '<a class="stream-topic" data-stream-id="{scotland.id}" '
            'href="/#narrow/stream/{scotland.id}-{scotland.name}/topic/other.20topic">'
            "#{scotland.name} &gt; other topic</a>"
            ".</p>".format(denmark=denmark, scotland=scotland),
        )

    def test_possible_stream_names(self) -> None:
        content = """#**test here**
            This mentions #**Denmark** too.
            #**garçon** #**천국** @**Ignore Person**
        """
        self.assertEqual(
            possible_linked_stream_names(content),
            {"test here", "Denmark", "garçon", "천국"},
        )

    def test_stream_unicode(self) -> None:
        realm = get_realm("zulip")
        uni = Stream.objects.create(name="привет", realm=realm)
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "#**привет**"
        quoted_name = ".D0.BF.D1.80.D0.B8.D0.B2.D0.B5.D1.82"
        href = f"/#narrow/stream/{uni.id}-{quoted_name}"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><a class="stream" data-stream-id="{s.id}" href="{href}">#{s.name}</a></p>'.format(
                s=uni,
                href=href,
            ),
        )

    def test_stream_atomic_string(self) -> None:
        realm = get_realm("zulip")
        # Create a linkifier.
        sender_user_profile = self.example_user("othello")
        url_format_string = r"https://trac.example.com/ticket/%(id)s"
        linkifier = RealmFilter(
            realm=realm, pattern=r"#(?P<id>[0-9]{2,8})", url_format_string=url_format_string
        )
        linkifier.save()
        self.assertEqual(
            linkifier.__str__(),
            "<RealmFilter(zulip): #(?P<id>[0-9]{2,8}) https://trac.example.com/ticket/%(id)s>",
        )
        # Create a stream that potentially interferes with the pattern.
        stream = Stream.objects.create(name="Stream #1234", realm=realm)
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))
        content = "#**Stream #1234**"
        href = f"/#narrow/stream/{stream.id}-Stream-.231234"
        self.assertEqual(
            render_markdown(msg, content),
            '<p><a class="stream" data-stream-id="{s.id}" href="{href}">#{s.name}</a></p>'.format(
                s=stream,
                href=href,
            ),
        )

    def test_stream_invalid(self) -> None:
        sender_user_profile = self.example_user("othello")
        msg = Message(sender=sender_user_profile, sending_client=get_client("test"))

        content = "There #**Nonexistentstream**"
        self.assertEqual(
            render_markdown(msg, content), "<p>There #<strong>Nonexistentstream</strong></p>"
        )
        self.assertEqual(msg.mentions_user_ids, set())

    def test_image_preview_title(self) -> None:
        msg = "[My favorite image](https://example.com/testimage.png)"
        converted = markdown_convert_wrapper(msg)
        self.assertEqual(
            converted,
            "<p>"
            '<a href="https://example.com/testimage.png">My favorite image</a>'
            "</p>\n"
            '<div class="message_inline_image">'
            '<a href="https://example.com/testimage.png" title="My favorite image">'
            '<img data-src-fullsize="/thumbnail?url=https%3A%2F%2Fexample.com%2Ftestimage.png&amp;size=full" src="/thumbnail?url=https%3A%2F%2Fexample.com%2Ftestimage.png&amp;size=thumbnail">'
            "</a>"
            "</div>",
        )

    def test_mit_rendering(self) -> None:
        """Test the Markdown configs for the MIT Zephyr mirroring system;
        verifies almost all inline patterns are disabled, but
        inline_interesting_links is still enabled"""
        msg = "**test**"
        realm = get_realm("zephyr")
        client = get_client("zephyr_mirror")
        message = Message(sending_client=client, sender=self.mit_user("sipbtest"))
        converted = markdown_convert(msg, message_realm=realm, message=message)
        self.assertEqual(
            converted,
            "<p>**test**</p>",
        )
        msg = "* test"
        converted = markdown_convert(msg, message_realm=realm, message=message)
        self.assertEqual(
            converted,
            "<p>* test</p>",
        )
        msg = "https://lists.debian.org/debian-ctte/2014/02/msg00173.html"
        converted = markdown_convert(msg, message_realm=realm, message=message)
        self.assertEqual(
            converted,
            '<p><a href="https://lists.debian.org/debian-ctte/2014/02/msg00173.html">https://lists.debian.org/debian-ctte/2014/02/msg00173.html</a></p>',
        )

    def test_url_to_a(self) -> None:
        url = "javascript://example.com/invalidURL"
        converted = url_to_a(db_data=None, url=url, text=url)
        self.assertEqual(
            converted,
            "javascript://example.com/invalidURL",
        )

    def test_disabled_code_block_processor(self) -> None:
        msg = (
            "Hello,\n\n"
            + "    I am writing this message to test something. I am writing this message to test something."
        )
        converted = markdown_convert_wrapper(msg)
        expected_output = (
            "<p>Hello,</p>\n"
            + '<div class="codehilite"><pre><span></span><code>I am writing this message to test something. I am writing this message to test something.\n'
            + "</code></pre></div>"
        )
        self.assertEqual(converted, expected_output)

        realm = do_create_realm(
            string_id="code_block_processor_test", name="code_block_processor_test"
        )
        maybe_update_markdown_engines(realm.id, True)
        converted = markdown_convert(msg, message_realm=realm, email_gateway=True)
        expected_output = (
            "<p>Hello,</p>\n"
            + "<p>I am writing this message to test something. I am writing this message to test something.</p>"
        )
        self.assertEqual(converted, expected_output)

    def test_normal_link(self) -> None:
        realm = get_realm("zulip")
        sender_user_profile = self.example_user("othello")
        message = Message(sender=sender_user_profile, sending_client=get_client("test"))
        msg = "http://example.com/#settings/"

        self.assertEqual(
            markdown_convert(msg, message_realm=realm, message=message),
            '<p><a href="http://example.com/#settings/">http://example.com/#settings/</a></p>',
        )

    def test_relative_link(self) -> None:
        realm = get_realm("zulip")
        sender_user_profile = self.example_user("othello")
        message = Message(sender=sender_user_profile, sending_client=get_client("test"))
        msg = "http://zulip.testserver/#narrow/stream/999-hello"

        self.assertEqual(
            markdown_convert(msg, message_realm=realm, message=message),
            '<p><a href="#narrow/stream/999-hello">http://zulip.testserver/#narrow/stream/999-hello</a></p>',
        )

    def test_relative_link_streams_page(self) -> None:
        realm = get_realm("zulip")
        sender_user_profile = self.example_user("othello")
        message = Message(sender=sender_user_profile, sending_client=get_client("test"))
        msg = "http://zulip.testserver/#streams/all"

        self.assertEqual(
            markdown_convert(msg, message_realm=realm, message=message),
            '<p><a href="#streams/all">http://zulip.testserver/#streams/all</a></p>',
        )

    def test_md_relative_link(self) -> None:
        realm = get_realm("zulip")
        sender_user_profile = self.example_user("othello")
        message = Message(sender=sender_user_profile, sending_client=get_client("test"))
        msg = "[hello](http://zulip.testserver/#narrow/stream/999-hello)"

        self.assertEqual(
            markdown_convert(msg, message_realm=realm, message=message),
            '<p><a href="#narrow/stream/999-hello">hello</a></p>',
        )

    def test_html_entity_conversion(self) -> None:
        msg = """\
            Test raw: Hello, &copy;
            Test inline code: `&copy;`

            Test fenced code:
            ```
            &copy;
            &copy;
            ```

            Test quote:
            ~~~quote
            &copy;
            ~~~

            Test a list:
            * &copy;
            * `&copy;`
            * ```&copy;```

            Test an indented block:

                &copy;"""

        expected_output = """\
            <p>Test raw: Hello, &copy;<br>
            Test inline code: <code>&amp;copy;</code></p>
            <p>Test fenced code:</p>
            <div class="codehilite"><pre><span></span><code>&amp;copy;
            &amp;copy;
            </code></pre></div>
            <p>Test quote:</p>
            <blockquote>
            <p>&copy;</p>
            </blockquote>
            <p>Test a list:</p>
            <ul>
            <li>&copy;</li>
            <li><code>&amp;copy;</code></li>
            <li><code>&amp;copy;</code></li>
            </ul>
            <p>Test an indented block:</p>
            <div class="codehilite"><pre><span></span><code>&amp;copy;
            </code></pre></div>"""

        converted = markdown_convert_wrapper(dedent(msg))
        self.assertEqual(converted, dedent(expected_output))


class MarkdownApiTests(ZulipTestCase):
    def test_render_message_api(self) -> None:
        content = "That is a **bold** statement"
        result = self.api_post(
            self.example_user("othello"),
            "/api/v1/messages/render",
            dict(content=content),
        )
        self.assert_json_success(result)
        self.assertEqual(
            result.json()["rendered"], "<p>That is a <strong>bold</strong> statement</p>"
        )

    def test_render_mention_stream_api(self) -> None:
        """Determines whether we're correctly passing the realm context"""
        content = "This mentions #**Denmark** and @**King Hamlet**."
        result = self.api_post(
            self.example_user("othello"),
            "/api/v1/messages/render",
            dict(content=content),
        )
        self.assert_json_success(result)
        user_id = self.example_user("hamlet").id
        stream_id = get_stream("Denmark", get_realm("zulip")).id
        self.assertEqual(
            result.json()["rendered"],
            f'<p>This mentions <a class="stream" data-stream-id="{stream_id}" href="/#narrow/stream/{stream_id}-Denmark">#Denmark</a> and <span class="user-mention" data-user-id="{user_id}">@King Hamlet</span>.</p>',
        )


class MarkdownErrorTests(ZulipTestCase):
    def test_markdown_error_handling(self) -> None:
        with self.simulated_markdown_failure():
            with self.assertRaises(MarkdownRenderingException):
                markdown_convert_wrapper("")

    def test_send_message_errors(self) -> None:

        message = "whatever"
        with self.simulated_markdown_failure():
            # We don't use assertRaisesRegex because it seems to not
            # handle i18n properly here on some systems.
            with self.assertRaises(JsonableError):
                self.send_stream_message(self.example_user("othello"), "Denmark", message)

    def test_ultra_long_rendering(self) -> None:
        """A rendered message with an ultra-long length (> 10 * MAX_MESSAGE_LENGTH)
        throws an exception"""
        msg = "mock rendered message\n" * MAX_MESSAGE_LENGTH

        with mock.patch("zerver.lib.markdown.timeout", return_value=msg), mock.patch(
            "zerver.lib.markdown.markdown_logger"
        ):
            with self.assertRaises(MarkdownRenderingException):
                markdown_convert_wrapper(msg)

    def test_curl_code_block_validation(self) -> None:
        processor = SimulatedFencedBlockPreprocessor(Markdown())
        processor.run_content_validators = True

        markdown_input = [
            "``` curl",
            "curl {{ api_url }}/v1/register",
            "    -u BOT_EMAIL_ADDRESS:BOT_API_KEY",
            '    -d "queue_id=1375801870:2942"',
            "```",
        ]

        with self.assertRaises(MarkdownRenderingException):
            processor.run(markdown_input)

    def test_curl_code_block_without_validation(self) -> None:
        processor = SimulatedFencedBlockPreprocessor(Markdown())

        markdown_input = [
            "``` curl",
            "curl {{ api_url }}/v1/register",
            "    -u BOT_EMAIL_ADDRESS:BOT_API_KEY",
            '    -d "queue_id=1375801870:2942"',
            "```",
        ]
        expected = [
            "",
            "**curl:curl {{ api_url }}/v1/register",
            "    -u BOT_EMAIL_ADDRESS:BOT_API_KEY",
            '    -d "queue_id=1375801870:2942"**',
            "",
            "",
        ]

        result = processor.run(markdown_input)
        self.assertEqual(result, expected)
