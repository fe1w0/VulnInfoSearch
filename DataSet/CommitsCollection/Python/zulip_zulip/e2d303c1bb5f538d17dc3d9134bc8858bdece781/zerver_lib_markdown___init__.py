# Zulip's main Markdown implementation.  See docs/subsystems/markdown.md for
# detailed documentation on our Markdown syntax.
import datetime
import functools
import html
import logging
import re
import time
import urllib
import urllib.parse
from collections import defaultdict, deque
from dataclasses import dataclass
from typing import (
    Any,
    Callable,
    Dict,
    Generic,
    List,
    Match,
    Optional,
    Pattern,
    Set,
    Tuple,
    TypeVar,
    Union,
)
from urllib.parse import urlencode, urlsplit
from xml.etree import ElementTree as etree
from xml.etree.ElementTree import Element, SubElement

import ahocorasick
import dateutil.parser
import dateutil.tz
import markdown
import markdown.blockprocessors
import markdown.inlinepatterns
import markdown.postprocessors
import markdown.treeprocessors
import markdown.util
import re2
import requests
from django.conf import settings
from django.db.models import Q
from markdown.blockparser import BlockParser
from markdown.extensions import codehilite, nl2br, sane_lists, tables
from tlds import tld_set
from typing_extensions import TypedDict

from zerver.lib import mention as mention
from zerver.lib.cache import NotFoundInCache, cache_with_key
from zerver.lib.camo import get_camo_url
from zerver.lib.emoji import (
    codepoint_to_name,
    emoticon_regex,
    name_to_codepoint,
    translate_emoticons,
)
from zerver.lib.exceptions import MarkdownRenderingException
from zerver.lib.markdown import fenced_code
from zerver.lib.markdown.fenced_code import FENCE_RE
from zerver.lib.mention import extract_user_group, possible_mentions, possible_user_group_mentions
from zerver.lib.subdomains import is_static_or_current_realm_url
from zerver.lib.tex import render_tex
from zerver.lib.thumbnail import user_uploads_or_external
from zerver.lib.timeout import TimeoutExpired, timeout
from zerver.lib.timezone import common_timezones
from zerver.lib.types import LinkifierDict
from zerver.lib.url_encoding import encode_stream, hash_util_encode
from zerver.lib.url_preview import preview as link_preview
from zerver.models import (
    MAX_MESSAGE_LENGTH,
    Message,
    Realm,
    UserGroup,
    UserGroupMembership,
    UserProfile,
    get_active_streams,
    linkifiers_for_realm,
)

ReturnT = TypeVar("ReturnT")


def one_time(method: Callable[[], ReturnT]) -> Callable[[], ReturnT]:
    """
    Use this decorator with extreme caution.
    The function you wrap should have no dependency
    on any arguments (no args, no kwargs) nor should
    it depend on any global state.
    """
    val = None

    def cache_wrapper() -> ReturnT:
        nonlocal val
        if val is None:
            val = method()
        return val

    return cache_wrapper


class FullNameInfo(TypedDict):
    id: int
    email: str
    full_name: str


class LinkInfo(TypedDict):
    parent: Element
    title: Optional[str]
    index: Optional[int]
    remove: Optional[Element]


DbData = Dict[str, Any]

# Format version of the Markdown rendering; stored along with rendered
# messages so that we can efficiently determine what needs to be re-rendered
version = 1

_T = TypeVar("_T")
ElementStringNone = Union[Element, Optional[str]]

EMOJI_REGEX = r"(?P<syntax>:[\w\-\+]+:)"


def verbose_compile(pattern: str) -> Pattern[str]:
    return re.compile(
        f"^(.*?){pattern}(.*?)$",
        re.DOTALL | re.UNICODE | re.VERBOSE,
    )


def normal_compile(pattern: str) -> Pattern[str]:
    return re.compile(
        fr"^(.*?){pattern}(.*)$",
        re.DOTALL | re.UNICODE,
    )


STREAM_LINK_REGEX = r"""
                     (?<![^\s'"\(,:<])            # Start after whitespace or specified chars
                     \#\*\*                       # and after hash sign followed by double asterisks
                         (?P<stream_name>[^\*]+)  # stream name can contain anything
                     \*\*                         # ends by double asterisks
                    """


@one_time
def get_compiled_stream_link_regex() -> Pattern[str]:
    # Not using verbose_compile as it adds ^(.*?) and
    # (.*?)$ which cause extra overhead of matching
    # pattern which is not required.
    # With new InlineProcessor these extra patterns
    # are not required.
    return re.compile(
        STREAM_LINK_REGEX,
        re.DOTALL | re.UNICODE | re.VERBOSE,
    )


STREAM_TOPIC_LINK_REGEX = r"""
                     (?<![^\s'"\(,:<])             # Start after whitespace or specified chars
                     \#\*\*                        # and after hash sign followed by double asterisks
                         (?P<stream_name>[^\*>]+)  # stream name can contain anything except >
                         >                         # > acts as separator
                         (?P<topic_name>[^\*]+)     # topic name can contain anything
                     \*\*                          # ends by double asterisks
                   """


@one_time
def get_compiled_stream_topic_link_regex() -> Pattern[str]:
    # Not using verbose_compile as it adds ^(.*?) and
    # (.*?)$ which cause extra overhead of matching
    # pattern which is not required.
    # With new InlineProcessor these extra patterns
    # are not required.
    return re.compile(
        STREAM_TOPIC_LINK_REGEX,
        re.DOTALL | re.UNICODE | re.VERBOSE,
    )


LINK_REGEX: Optional[Pattern[str]] = None


def get_web_link_regex() -> Pattern[str]:
    # We create this one time, but not at startup.  So the
    # first message rendered in any process will have some
    # extra costs.  It's roughly 75ms to run this code, so
    # caching the value in LINK_REGEX is super important here.
    global LINK_REGEX
    if LINK_REGEX is not None:
        return LINK_REGEX

    tlds = "|".join(list_of_tlds())

    # A link starts at a word boundary, and ends at space, punctuation, or end-of-input.
    #
    # We detect a URL either by the `https?://` or by building around the TLD.

    # In lieu of having a recursive regex (which python doesn't support) to match
    # arbitrary numbers of nested matching parenthesis, we manually build a regexp that
    # can match up to six
    # The inner_paren_contents chunk matches the innermore non-parenthesis-holding text,
    # and the paren_group matches text with, optionally, a matching set of parens
    inner_paren_contents = r"[^\s()\"]*"
    paren_group = r"""
                    [^\s()\"]*?            # Containing characters that won't end the URL
                    (?: \( %s \)           # and more characters in matched parens
                        [^\s()\"]*?        # followed by more characters
                    )*                     # zero-or-more sets of paired parens
                   """
    nested_paren_chunk = paren_group
    for i in range(6):
        nested_paren_chunk = nested_paren_chunk % (paren_group,)
    nested_paren_chunk = nested_paren_chunk % (inner_paren_contents,)

    file_links = r"| (?:file://(/[^/ ]*)+/?)" if settings.ENABLE_FILE_LINKS else r""
    REGEX = fr"""
        (?<![^\s'"\(,:<])    # Start after whitespace or specified chars
                             # (Double-negative lookbehind to allow start-of-string)
        (?P<url>             # Main group
            (?:(?:           # Domain part
                https?://[\w.:@-]+?   # If it has a protocol, anything goes.
               |(?:                   # Or, if not, be more strict to avoid false-positives
                    (?:[\w-]+\.)+     # One or more domain components, separated by dots
                    (?:{tlds})        # TLDs
                )
            )
            (?:/             # A path, beginning with /
                {nested_paren_chunk}           # zero-to-6 sets of paired parens
            )?)              # Path is optional
            | (?:[\w.-]+\@[\w.-]+\.[\w]+) # Email is separate, since it can't have a path
            {file_links}               # File path start with file:///, enable by setting ENABLE_FILE_LINKS=True
            | (?:bitcoin:[13][a-km-zA-HJ-NP-Z1-9]{{25,34}})  # Bitcoin address pattern, see https://mokagio.github.io/tech-journal/2014/11/21/regex-bitcoin.html
        )
        (?=                            # URL must be followed by (not included in group)
            [!:;\?\),\.\'\"\>]*         # Optional punctuation characters
            (?:\Z|\s)                  # followed by whitespace or end of string
        )
        """
    LINK_REGEX = verbose_compile(REGEX)
    return LINK_REGEX


def clear_state_for_testing() -> None:
    # The link regex never changes in production, but our tests
    # try out both sides of ENABLE_FILE_LINKS, so we need
    # a way to clear it.
    global LINK_REGEX
    LINK_REGEX = None


markdown_logger = logging.getLogger()


def rewrite_local_links_to_relative(db_data: Optional[DbData], link: str) -> str:
    """If the link points to a local destination (e.g. #narrow/...),
    generate a relative link that will open it in the current window.
    """

    if db_data:
        realm_uri_prefix = db_data["realm_uri"] + "/"
        if (
            link.startswith(realm_uri_prefix)
            and urllib.parse.urljoin(realm_uri_prefix, link[len(realm_uri_prefix) :]) == link
        ):
            return link[len(realm_uri_prefix) :]

    return link


def url_embed_preview_enabled(
    message: Optional[Message] = None, realm: Optional[Realm] = None, no_previews: bool = False
) -> bool:
    if not settings.INLINE_URL_EMBED_PREVIEW:
        return False

    if no_previews:
        return False

    if realm is None:
        if message is not None:
            realm = message.get_realm()

    if realm is None:
        # realm can be None for odd use cases
        # like generating documentation or running
        # test code
        return True

    return realm.inline_url_embed_preview


def image_preview_enabled(
    message: Optional[Message] = None, realm: Optional[Realm] = None, no_previews: bool = False
) -> bool:
    if not settings.INLINE_IMAGE_PREVIEW:
        return False

    if no_previews:
        return False

    if realm is None:
        if message is not None:
            realm = message.get_realm()

    if realm is None:
        # realm can be None for odd use cases
        # like generating documentation or running
        # test code
        return True

    return realm.inline_image_preview


def list_of_tlds() -> List[str]:
    # Skip a few overly-common false-positives from file extensions
    common_false_positives = set(["java", "md", "mov", "py", "zip"])
    tlds = list(tld_set - common_false_positives)

    tlds.sort(key=len, reverse=True)
    return tlds


def walk_tree(
    root: Element, processor: Callable[[Element], Optional[_T]], stop_after_first: bool = False
) -> List[_T]:
    results = []
    queue = deque([root])

    while queue:
        currElement = queue.popleft()
        for child in currElement:
            if child:
                queue.append(child)

            result = processor(child)
            if result is not None:
                results.append(result)
                if stop_after_first:
                    return results

    return results


@dataclass
class ElementFamily:
    grandparent: Optional[Element]
    parent: Element
    child: Element
    in_blockquote: bool


T = TypeVar("T")


class ResultWithFamily(Generic[T]):
    family: ElementFamily
    result: T

    def __init__(self, family: ElementFamily, result: T):
        self.family = family
        self.result = result


class ElementPair:
    parent: Optional["ElementPair"]
    value: Element

    def __init__(self, parent: Optional["ElementPair"], value: Element):
        self.parent = parent
        self.value = value


def walk_tree_with_family(
    root: Element,
    processor: Callable[[Element], Optional[_T]],
) -> List[ResultWithFamily[_T]]:
    results = []

    queue = deque([ElementPair(parent=None, value=root)])
    while queue:
        currElementPair = queue.popleft()
        for child in currElementPair.value:
            if child:
                queue.append(ElementPair(parent=currElementPair, value=child))
            result = processor(child)
            if result is not None:
                if currElementPair.parent is not None:
                    grandparent_element = currElementPair.parent
                    grandparent: Optional[Element] = grandparent_element.value
                else:
                    grandparent = None
                family = ElementFamily(
                    grandparent=grandparent,
                    parent=currElementPair.value,
                    child=child,
                    in_blockquote=has_blockquote_ancestor(currElementPair),
                )

                results.append(
                    ResultWithFamily(
                        family=family,
                        result=result,
                    )
                )

    return results


def has_blockquote_ancestor(element_pair: Optional[ElementPair]) -> bool:
    if element_pair is None:
        return False
    elif element_pair.value.tag == "blockquote":
        return True
    else:
        return has_blockquote_ancestor(element_pair.parent)


@cache_with_key(lambda tweet_id: tweet_id, cache_name="database", with_statsd_key="tweet_data")
def fetch_tweet_data(tweet_id: str) -> Optional[Dict[str, Any]]:
    if settings.TEST_SUITE:
        from . import testing_mocks

        res = testing_mocks.twitter(tweet_id)
    else:
        creds = {
            "consumer_key": settings.TWITTER_CONSUMER_KEY,
            "consumer_secret": settings.TWITTER_CONSUMER_SECRET,
            "access_token_key": settings.TWITTER_ACCESS_TOKEN_KEY,
            "access_token_secret": settings.TWITTER_ACCESS_TOKEN_SECRET,
        }
        if not all(creds.values()):
            return None

        # We lazily import twitter here because its import process is
        # surprisingly slow, and doing so has a significant impact on
        # the startup performance of `manage.py` commands.
        import twitter

        api = twitter.Api(tweet_mode="extended", **creds)

        try:
            # Sometimes Twitter hangs on responses.  Timing out here
            # will cause the Tweet to go through as-is with no inline
            # preview, rather than having the message be rejected
            # entirely. This timeout needs to be less than our overall
            # formatting timeout.
            tweet = timeout(3, lambda: api.GetStatus(tweet_id))
            res = tweet.AsDict()
        except TimeoutExpired:
            # We'd like to try again later and not cache the bad result,
            # so we need to re-raise the exception (just as though
            # we were being rate-limited)
            raise
        except twitter.TwitterError as e:
            t = e.args[0]
            if len(t) == 1 and ("code" in t[0]):
                # https://developer.twitter.com/en/docs/basics/response-codes
                code = t[0]["code"]
                if code in [34, 144, 421, 422]:
                    # All these "correspond with HTTP 404," and mean
                    # that the message doesn't exist; return None so
                    # that we will cache the error.
                    return None
                elif code in [63, 179]:
                    # 63 is that the account is suspended, 179 is that
                    # it is now locked; cache the None.
                    return None
                elif code in [88, 130, 131]:
                    # Code 88 means that we were rate-limited, 130
                    # means Twitter is having capacity issues, and 131
                    # is other 400-equivalent; in these cases, raise
                    # the error so we don't cache None and will try
                    # again later.
                    raise
            # It's not clear what to do in cases of other errors,
            # but for now it seems reasonable to log at error
            # level (so that we get notified), but then cache the
            # failure to proceed with our usual work
            markdown_logger.exception("Unknown error fetching tweet data", stack_info=True)
            return None
    return res


HEAD_START_RE = re.compile("^head[ >]")
HEAD_END_RE = re.compile("^/head[ >]")
META_START_RE = re.compile("^meta[ >]")
META_END_RE = re.compile("^/meta[ >]")


def fetch_open_graph_image(url: str) -> Optional[Dict[str, Any]]:
    in_head = False
    # HTML will auto close meta tags, when we start the next tag add
    # a closing tag if it has not been closed yet.
    last_closed = True
    head = []
    # TODO: What if response content is huge? Should we get headers first?
    try:
        content = requests.get(url, timeout=1).text
    except requests.RequestException:
        return None
    # Extract the head and meta tags
    # All meta tags are self closing, have no children or are closed
    # automatically.
    for part in content.split("<"):
        if not in_head and HEAD_START_RE.match(part):
            # Started the head node output it to have a document root
            in_head = True
            head.append("<head>")
        elif in_head and HEAD_END_RE.match(part):
            # Found the end of the head close any remaining tag then stop
            # processing
            in_head = False
            if not last_closed:
                last_closed = True
                head.append("</meta>")
            head.append("</head>")
            break

        elif in_head and META_START_RE.match(part):
            # Found a meta node copy it
            if not last_closed:
                head.append("</meta>")
                last_closed = True
            head.append("<")
            head.append(part)
            if "/>" not in part:
                last_closed = False

        elif in_head and META_END_RE.match(part):
            # End of a meta node just copy it to close the tag
            head.append("<")
            head.append(part)
            last_closed = True

    try:
        doc = etree.fromstring("".join(head))
    except etree.ParseError:
        return None
    og_image = doc.find('meta[@property="og:image"]')
    og_title = doc.find('meta[@property="og:title"]')
    og_desc = doc.find('meta[@property="og:description"]')
    title = None
    desc = None
    if og_image is not None:
        image = og_image.get("content")
    else:
        return None
    if og_title is not None:
        title = og_title.get("content")
    if og_desc is not None:
        desc = og_desc.get("content")
    return {"image": image, "title": title, "desc": desc}


def get_tweet_id(url: str) -> Optional[str]:
    parsed_url = urllib.parse.urlparse(url)
    if not (parsed_url.netloc == "twitter.com" or parsed_url.netloc.endswith(".twitter.com")):
        return None
    to_match = parsed_url.path
    # In old-style twitter.com/#!/wdaher/status/1231241234-style URLs,
    # we need to look at the fragment instead
    if parsed_url.path == "/" and len(parsed_url.fragment) > 5:
        to_match = parsed_url.fragment

    tweet_id_match = re.match(
        r"^!?/.*?/status(es)?/(?P<tweetid>\d{10,30})(/photo/[0-9])?/?$", to_match
    )
    if not tweet_id_match:
        return None
    return tweet_id_match.group("tweetid")


class InlineImageProcessor(markdown.treeprocessors.Treeprocessor):
    """
    Rewrite inline img tags to serve external content via Camo.

    This rewrites all images, except ones that are served from the current
    realm or global STATIC_URL. This is to ensure that each realm only loads
    images that are hosted on that realm or by the global installation,
    avoiding information leakage to external domains or between realms. We need
    to disable proxying of images hosted on the same realm, because otherwise
    we will break images in /user_uploads/, which require authorization to
    view.
    """

    def run(self, root: Element) -> None:
        # Get all URLs from the blob
        found_imgs = walk_tree(root, lambda e: e if e.tag == "img" else None)
        for img in found_imgs:
            url = img.get("src")
            assert url is not None
            if is_static_or_current_realm_url(url, self.md.zulip_realm):
                # Don't rewrite images on our own site (e.g. emoji, user uploads).
                continue
            img.set("src", get_camo_url(url))


class BacktickInlineProcessor(markdown.inlinepatterns.BacktickInlineProcessor):
    """Return a `<code>` element containing the matching text."""

    def handleMatch(  # type: ignore[override] # supertype incompatible with supersupertype
        self, m: Match[str], data: str
    ) -> Union[Tuple[None, None, None], Tuple[Element, int, int]]:
        # Let upstream's implementation do its job as it is, we'll
        # just replace the text to not strip the group because it
        # makes it impossible to put leading/trailing whitespace in
        # an inline code block.
        el, start, end = ret = super().handleMatch(m, data)
        if el is not None and m.group(3):
            # upstream's code here is: m.group(3).strip() rather than m.group(3).
            el.text = markdown.util.AtomicString(markdown.util.code_escape(m.group(3)))
        return ret


class InlineInterestingLinkProcessor(markdown.treeprocessors.Treeprocessor):
    TWITTER_MAX_IMAGE_HEIGHT = 400
    TWITTER_MAX_TO_PREVIEW = 3
    INLINE_PREVIEW_LIMIT_PER_MESSAGE = 5

    def __init__(self, md: markdown.Markdown) -> None:
        markdown.treeprocessors.Treeprocessor.__init__(self, md)

    def add_a(
        self,
        root: Element,
        url: str,
        link: str,
        title: Optional[str] = None,
        desc: Optional[str] = None,
        class_attr: str = "message_inline_image",
        data_id: Optional[str] = None,
        insertion_index: Optional[int] = None,
        already_thumbnailed: bool = False,
    ) -> None:
        desc = desc if desc is not None else ""

        # Update message.has_image attribute.
        if "message_inline_image" in class_attr and self.md.zulip_message:
            self.md.zulip_message.has_image = True

        if insertion_index is not None:
            div = Element("div")
            root.insert(insertion_index, div)
        else:
            div = SubElement(root, "div")

        div.set("class", class_attr)
        a = SubElement(div, "a")
        a.set("href", link)
        if title is not None:
            a.set("title", title)
        if data_id is not None:
            a.set("data-id", data_id)
        img = SubElement(a, "img")
        if (
            settings.THUMBNAIL_IMAGES
            and (not already_thumbnailed)
            and user_uploads_or_external(url)
        ):
            # See docs/thumbnailing.md for some high-level documentation.
            #
            # We strip leading '/' from relative URLs here to ensure
            # consistency in what gets passed to /thumbnail
            url = url.lstrip("/")
            img.set("src", "/thumbnail?" + urlencode({"url": url, "size": "thumbnail"}))
            img.set("data-src-fullsize", "/thumbnail?" + urlencode({"url": url, "size": "full"}))
        else:
            img.set("src", url)

        if class_attr == "message_inline_ref":
            summary_div = SubElement(div, "div")
            title_div = SubElement(summary_div, "div")
            title_div.set("class", "message_inline_image_title")
            title_div.text = title
            desc_div = SubElement(summary_div, "desc")
            desc_div.set("class", "message_inline_image_desc")

    def add_oembed_data(self, root: Element, link: str, extracted_data: Dict[str, Any]) -> bool:
        oembed_resource_type = extracted_data.get("type", "")
        title = extracted_data.get("title")

        if oembed_resource_type == "photo":
            image = extracted_data.get("image")
            if image:
                self.add_a(root, image, link, title=title)
                return True

        elif oembed_resource_type == "video":
            html = extracted_data["html"]
            image = extracted_data["image"]
            title = extracted_data.get("title")
            description = extracted_data.get("description")
            self.add_a(
                root,
                image,
                link,
                title,
                description,
                "embed-video message_inline_image",
                html,
                already_thumbnailed=True,
            )
            return True

        return False

    def add_embed(self, root: Element, link: str, extracted_data: Dict[str, Any]) -> None:
        oembed = extracted_data.get("oembed", False)
        if oembed and self.add_oembed_data(root, link, extracted_data):
            return

        img_link = extracted_data.get("image")
        if not img_link:
            # Don't add an embed if an image is not found
            return

        container = SubElement(root, "div")
        container.set("class", "message_embed")

        parsed_img_link = urllib.parse.urlparse(img_link)
        # Append domain where relative img_link url is given
        if not parsed_img_link.netloc:
            parsed_url = urllib.parse.urlparse(link)
            domain = "{url.scheme}://{url.netloc}/".format(url=parsed_url)
            img_link = urllib.parse.urljoin(domain, img_link)
        img = SubElement(container, "a")
        img.set("style", "background-image: url(" + img_link + ")")
        img.set("href", link)
        img.set("class", "message_embed_image")

        data_container = SubElement(container, "div")
        data_container.set("class", "data-container")

        title = extracted_data.get("title")
        if title:
            title_elm = SubElement(data_container, "div")
            title_elm.set("class", "message_embed_title")
            a = SubElement(title_elm, "a")
            a.set("href", link)
            a.set("title", title)
            a.text = title
        description = extracted_data.get("description")
        if description:
            description_elm = SubElement(data_container, "div")
            description_elm.set("class", "message_embed_description")
            description_elm.text = description

    def get_actual_image_url(self, url: str) -> str:
        # Add specific per-site cases to convert image-preview URLs to image URLs.
        # See https://github.com/zulip/zulip/issues/4658 for more information
        parsed_url = urllib.parse.urlparse(url)
        if parsed_url.netloc == "github.com" or parsed_url.netloc.endswith(".github.com"):
            # https://github.com/zulip/zulip/blob/main/static/images/logo/zulip-icon-128x128.png ->
            # https://raw.githubusercontent.com/zulip/zulip/main/static/images/logo/zulip-icon-128x128.png
            split_path = parsed_url.path.split("/")
            if len(split_path) > 3 and split_path[3] == "blob":
                return urllib.parse.urljoin(
                    "https://raw.githubusercontent.com", "/".join(split_path[0:3] + split_path[4:])
                )

        return url

    def is_image(self, url: str) -> bool:
        if not self.md.image_preview_enabled:
            return False
        parsed_url = urllib.parse.urlparse(url)
        # remove HTML URLs which end with image extensions that can not be shorted
        if parsed_url.netloc == "pasteboard.co":
            return False

        # List from https://support.google.com/chromeos/bin/answer.py?hl=en&answer=183093
        for ext in [".bmp", ".gif", ".jpe", "jpeg", ".jpg", ".png", ".webp"]:
            if parsed_url.path.lower().endswith(ext):
                return True
        return False

    def corrected_image_source(self, url: str) -> Optional[str]:
        # This function adjusts any URLs from linx.li and
        # wikipedia.org to point to the actual image URL.  It's
        # structurally very similar to dropbox_image, and possibly
        # should be rewritten to use open graph, but has some value.
        parsed_url = urllib.parse.urlparse(url)
        if parsed_url.netloc.lower().endswith(".wikipedia.org"):
            # Redirecting from "/wiki/File:" to "/wiki/Special:FilePath/File:"
            # A possible alternative, that avoids the redirect after hitting "Special:"
            # is using the first characters of md5($filename) to generate the URL
            domain = parsed_url.scheme + "://" + parsed_url.netloc
            correct_url = domain + parsed_url.path[:6] + "Special:FilePath" + parsed_url.path[5:]
            return correct_url
        if parsed_url.netloc == "linx.li":
            return "https://linx.li/s" + parsed_url.path
        return None

    def dropbox_image(self, url: str) -> Optional[Dict[str, Any]]:
        # TODO: The returned Dict could possibly be a TypedDict in future.
        parsed_url = urllib.parse.urlparse(url)
        if parsed_url.netloc == "dropbox.com" or parsed_url.netloc.endswith(".dropbox.com"):
            is_album = parsed_url.path.startswith("/sc/") or parsed_url.path.startswith("/photos/")
            # Only allow preview Dropbox shared links
            if not (
                parsed_url.path.startswith("/s/") or parsed_url.path.startswith("/sh/") or is_album
            ):
                return None

            # Try to retrieve open graph protocol info for a preview
            # This might be redundant right now for shared links for images.
            # However, we might want to make use of title and description
            # in the future. If the actual image is too big, we might also
            # want to use the open graph image.
            image_info = fetch_open_graph_image(url)

            is_image = is_album or self.is_image(url)

            # If it is from an album or not an actual image file,
            # just use open graph image.
            if is_album or not is_image:
                # Failed to follow link to find an image preview so
                # use placeholder image and guess filename
                if image_info is None:
                    return None

                image_info["is_image"] = is_image
                return image_info

            # Otherwise, try to retrieve the actual image.
            # This is because open graph image from Dropbox may have padding
            # and gifs do not work.
            # TODO: What if image is huge? Should we get headers first?
            if image_info is None:
                image_info = {}
            image_info["is_image"] = True
            parsed_url_list = list(parsed_url)
            parsed_url_list[4] = "raw=1"  # Replaces query
            image_info["image"] = urllib.parse.urlunparse(parsed_url_list)

            return image_info
        return None

    def youtube_id(self, url: str) -> Optional[str]:
        if not self.md.image_preview_enabled:
            return None
        # YouTube video id extraction regular expression from https://pastebin.com/KyKAFv1s
        # Slightly modified to support URLs of the forms
        #   - youtu.be/<id>
        #   - youtube.com/playlist?v=<id>&list=<list-id>
        #   - youtube.com/watch_videos?video_ids=<id1>,<id2>,<id3>
        # If it matches, match.group(2) is the video id.
        schema_re = r"(?:https?://)"
        host_re = r"(?:youtu\.be/|(?:\w+\.)?youtube(?:-nocookie)?\.com/)"
        param_re = (
            r"(?:(?:(?:v|embed)/)|"
            + r"(?:(?:(?:watch|playlist)(?:_popup|_videos)?(?:\.php)?)?(?:\?|#!?)(?:.+&)?v(?:ideo_ids)?=))"
        )
        id_re = r"([0-9A-Za-z_-]+)"
        youtube_re = r"^({schema_re}?{host_re}{param_re}?)?{id_re}(?(1).+)?$"
        youtube_re = youtube_re.format(
            schema_re=schema_re, host_re=host_re, id_re=id_re, param_re=param_re
        )
        match = re.match(youtube_re, url)
        # URLs of the form youtube.com/playlist?list=<list-id> are incorrectly matched
        if match is None or match.group(2) == "playlist":
            return None
        return match.group(2)

    def youtube_title(self, extracted_data: Dict[str, Any]) -> Optional[str]:
        title = extracted_data.get("title")
        if title is not None:
            return f"YouTube - {title}"
        return None

    def youtube_image(self, url: str) -> Optional[str]:
        yt_id = self.youtube_id(url)

        if yt_id is not None:
            return f"https://i.ytimg.com/vi/{yt_id}/default.jpg"
        return None

    def vimeo_id(self, url: str) -> Optional[str]:
        if not self.md.image_preview_enabled:
            return None
        # (http|https)?:\/\/(www\.)?vimeo.com\/(?:channels\/(?:\w+\/)?|groups\/([^\/]*)\/videos\/|)(\d+)(?:|\/\?)
        # If it matches, match.group('id') is the video id.

        vimeo_re = (
            r"^((http|https)?:\/\/(www\.)?vimeo.com\/"
            + r"(?:channels\/(?:\w+\/)?|groups\/"
            + r"([^\/]*)\/videos\/|)(\d+)(?:|\/\?))$"
        )
        match = re.match(vimeo_re, url)
        if match is None:
            return None
        return match.group(5)

    def vimeo_title(self, extracted_data: Dict[str, Any]) -> Optional[str]:
        title = extracted_data.get("title")
        if title is not None:
            return f"Vimeo - {title}"
        return None

    def twitter_text(
        self,
        text: str,
        urls: List[Dict[str, str]],
        user_mentions: List[Dict[str, Any]],
        media: List[Dict[str, Any]],
    ) -> Element:
        """
        Use data from the Twitter API to turn links, mentions and media into A
        tags. Also convert Unicode emojis to images.

        This works by using the URLs, user_mentions and media data from
        the twitter API and searching for Unicode emojis in the text using
        `unicode_emoji_regex`.

        The first step is finding the locations of the URLs, mentions, media and
        emoji in the text. For each match we build a dictionary with type, the start
        location, end location, the URL to link to, and the text(codepoint and title
        in case of emojis) to be used in the link(image in case of emojis).

        Next we sort the matches by start location. And for each we add the
        text from the end of the last link to the start of the current link to
        the output. The text needs to added to the text attribute of the first
        node (the P tag) or the tail the last link created.

        Finally we add any remaining text to the last node.
        """

        to_process: List[Dict[str, Any]] = []
        # Build dicts for URLs
        for url_data in urls:
            short_url = url_data["url"]
            full_url = url_data["expanded_url"]
            for match in re.finditer(re.escape(short_url), text, re.IGNORECASE):
                to_process.append(
                    {
                        "type": "url",
                        "start": match.start(),
                        "end": match.end(),
                        "url": short_url,
                        "text": full_url,
                    }
                )
        # Build dicts for mentions
        for user_mention in user_mentions:
            screen_name = user_mention["screen_name"]
            mention_string = "@" + screen_name
            for match in re.finditer(re.escape(mention_string), text, re.IGNORECASE):
                to_process.append(
                    {
                        "type": "mention",
                        "start": match.start(),
                        "end": match.end(),
                        "url": "https://twitter.com/" + urllib.parse.quote(screen_name),
                        "text": mention_string,
                    }
                )
        # Build dicts for media
        for media_item in media:
            short_url = media_item["url"]
            expanded_url = media_item["expanded_url"]
            for match in re.finditer(re.escape(short_url), text, re.IGNORECASE):
                to_process.append(
                    {
                        "type": "media",
                        "start": match.start(),
                        "end": match.end(),
                        "url": short_url,
                        "text": expanded_url,
                    }
                )
        # Build dicts for emojis
        for match in re.finditer(unicode_emoji_regex, text, re.IGNORECASE):
            orig_syntax = match.group("syntax")
            codepoint = unicode_emoji_to_codepoint(orig_syntax)
            if codepoint in codepoint_to_name:
                display_string = ":" + codepoint_to_name[codepoint] + ":"
                to_process.append(
                    {
                        "type": "emoji",
                        "start": match.start(),
                        "end": match.end(),
                        "codepoint": codepoint,
                        "title": display_string,
                    }
                )

        to_process.sort(key=lambda x: x["start"])
        p = current_node = Element("p")

        def set_text(text: str) -> None:
            """
            Helper to set the text or the tail of the current_node
            """
            if current_node == p:
                current_node.text = text
            else:
                current_node.tail = text

        db_data = self.md.zulip_db_data
        current_index = 0
        for item in to_process:
            # The text we want to link starts in already linked text skip it
            if item["start"] < current_index:
                continue
            # Add text from the end of last link to the start of the current
            # link
            set_text(text[current_index : item["start"]])
            current_index = item["end"]
            if item["type"] != "emoji":
                elem = url_to_a(db_data, item["url"], item["text"])
                assert isinstance(elem, Element)
            else:
                elem = make_emoji(item["codepoint"], item["title"])
            current_node = elem
            p.append(elem)

        # Add any unused text
        set_text(text[current_index:])
        return p

    def twitter_link(self, url: str) -> Optional[Element]:
        tweet_id = get_tweet_id(url)

        if tweet_id is None:
            return None

        try:
            res = fetch_tweet_data(tweet_id)
            if res is None:
                return None
            user: Dict[str, Any] = res["user"]
            tweet = Element("div")
            tweet.set("class", "twitter-tweet")
            img_a = SubElement(tweet, "a")
            img_a.set("href", url)
            profile_img = SubElement(img_a, "img")
            profile_img.set("class", "twitter-avatar")
            # For some reason, for, e.g. tweet 285072525413724161,
            # python-twitter does not give us a
            # profile_image_url_https, but instead puts that URL in
            # profile_image_url. So use _https if available, but fall
            # back gracefully.
            image_url = user.get("profile_image_url_https", user["profile_image_url"])
            profile_img.set("src", image_url)

            text = html.unescape(res["full_text"])
            urls = res.get("urls", [])
            user_mentions = res.get("user_mentions", [])
            media: List[Dict[str, Any]] = res.get("media", [])
            p = self.twitter_text(text, urls, user_mentions, media)
            tweet.append(p)

            span = SubElement(tweet, "span")
            span.text = "- {} (@{})".format(user["name"], user["screen_name"])

            # Add image previews
            for media_item in media:
                # Only photos have a preview image
                if media_item["type"] != "photo":
                    continue

                # Find the image size that is smaller than
                # TWITTER_MAX_IMAGE_HEIGHT px tall or the smallest
                size_name_tuples = list(media_item["sizes"].items())
                size_name_tuples.sort(reverse=True, key=lambda x: x[1]["h"])
                for size_name, size in size_name_tuples:
                    if size["h"] < self.TWITTER_MAX_IMAGE_HEIGHT:
                        break

                media_url = "{}:{}".format(media_item["media_url_https"], size_name)
                img_div = SubElement(tweet, "div")
                img_div.set("class", "twitter-image")
                img_a = SubElement(img_div, "a")
                img_a.set("href", media_item["url"])
                img = SubElement(img_a, "img")
                img.set("src", media_url)

            return tweet
        except Exception:
            # We put this in its own try-except because it requires external
            # connectivity. If Twitter flakes out, we don't want to not-render
            # the entire message; we just want to not show the Twitter preview.
            markdown_logger.warning("Error building Twitter link", exc_info=True)
            return None

    def get_url_data(self, e: Element) -> Optional[Tuple[str, Optional[str]]]:
        if e.tag == "a":
            url = e.get("href")
            assert url is not None
            return (url, e.text)
        return None

    def get_inlining_information(
        self,
        root: Element,
        found_url: ResultWithFamily[Tuple[str, Optional[str]]],
    ) -> LinkInfo:

        grandparent = found_url.family.grandparent
        parent = found_url.family.parent
        ahref_element = found_url.family.child
        (url, text) = found_url.result

        # url != text usually implies a named link, which we opt not to remove
        url_eq_text = text is None or url == text
        title = None if url_eq_text else text
        info: LinkInfo = {
            "parent": root,
            "title": title,
            "index": None,
            "remove": None,
        }

        if parent.tag == "li":
            info["parent"] = parent
            if not parent.text and not ahref_element.tail and url_eq_text:
                info["remove"] = ahref_element

        elif parent.tag == "p":
            assert grandparent is not None
            parent_index = None
            for index, uncle in enumerate(grandparent):
                if uncle is parent:
                    parent_index = index
                    break

            # Append to end of list of grandparent's children as normal
            info["parent"] = grandparent

            if (
                len(parent) == 1
                and (not parent.text or parent.text == "\n")
                and not ahref_element.tail
                and url_eq_text
            ):
                info["remove"] = parent

            if parent_index is not None:
                info["index"] = self.find_proper_insertion_index(grandparent, parent, parent_index)

        return info

    def handle_image_inlining(
        self,
        root: Element,
        found_url: ResultWithFamily[Tuple[str, Optional[str]]],
    ) -> None:
        info = self.get_inlining_information(root, found_url)
        (url, text) = found_url.result
        actual_url = self.get_actual_image_url(url)
        self.add_a(
            info["parent"], actual_url, url, title=info["title"], insertion_index=info["index"]
        )
        if info["remove"] is not None:
            info["parent"].remove(info["remove"])

    def handle_tweet_inlining(
        self,
        root: Element,
        found_url: ResultWithFamily[Tuple[str, Optional[str]]],
        twitter_data: Element,
    ) -> None:
        info = self.get_inlining_information(root, found_url)

        if info["index"] is not None:
            div = Element("div")
            root.insert(info["index"], div)
        else:
            div = SubElement(root, "div")

        div.set("class", "inline-preview-twitter")
        div.insert(0, twitter_data)

    def handle_youtube_url_inlining(
        self,
        root: Element,
        found_url: ResultWithFamily[Tuple[str, Optional[str]]],
        yt_image: str,
    ) -> None:
        info = self.get_inlining_information(root, found_url)
        (url, text) = found_url.result
        yt_id = self.youtube_id(url)
        self.add_a(
            info["parent"],
            yt_image,
            url,
            None,
            None,
            "youtube-video message_inline_image",
            yt_id,
            insertion_index=info["index"],
            already_thumbnailed=True,
        )

    def find_proper_insertion_index(
        self, grandparent: Element, parent: Element, parent_index_in_grandparent: int
    ) -> int:
        # If there are several inline images from same paragraph, ensure that
        # they are in correct (and not opposite) order by inserting after last
        # inline image from paragraph 'parent'

        parent_links = [ele.attrib["href"] for ele in parent.iter(tag="a")]
        insertion_index = parent_index_in_grandparent

        while True:
            insertion_index += 1
            if insertion_index >= len(grandparent):
                return insertion_index

            uncle = grandparent[insertion_index]
            inline_image_classes = [
                "message_inline_image",
                "message_inline_ref",
                "inline-preview-twitter",
            ]
            if (
                uncle.tag != "div"
                or "class" not in uncle.keys()
                or uncle.attrib["class"] not in inline_image_classes
            ):
                return insertion_index

            uncle_link = list(uncle.iter(tag="a"))[0].attrib["href"]
            if uncle_link not in parent_links:
                return insertion_index

    def is_absolute_url(self, url: str) -> bool:
        return bool(urllib.parse.urlparse(url).netloc)

    def run(self, root: Element) -> None:
        # Get all URLs from the blob
        found_urls = walk_tree_with_family(root, self.get_url_data)
        unique_urls = {found_url.result[0] for found_url in found_urls}
        # Collect unique URLs which are not quoted as we don't do
        # inline previews for links inside blockquotes.
        unique_previewable_urls = {
            found_url.result[0] for found_url in found_urls if not found_url.family.in_blockquote
        }

        # Set has_link and similar flags whenever a message is processed by Markdown
        if self.md.zulip_message:
            self.md.zulip_message.has_link = len(found_urls) > 0
            self.md.zulip_message.has_image = False  # This is updated in self.add_a
            self.md.zulip_message.potential_attachment_path_ids = []

            for url in unique_urls:
                # Due to rewrite_local_links_to_relative, we need to
                # handle both relative URLs beginning with
                # `/user_uploads` and beginning with `user_uploads`.
                # This urllib construction converts the latter into
                # the former.
                parsed_url = urllib.parse.urlsplit(urllib.parse.urljoin("/", url))
                host = parsed_url.netloc

                if host != "" and host != self.md.zulip_realm.host:
                    continue

                if not parsed_url.path.startswith("/user_uploads/"):
                    continue

                path_id = parsed_url.path[len("/user_uploads/") :]
                self.md.zulip_message.potential_attachment_path_ids.append(path_id)

        if len(found_urls) == 0:
            return

        if len(unique_previewable_urls) > self.INLINE_PREVIEW_LIMIT_PER_MESSAGE:
            return

        processed_urls: Set[str] = set()
        rendered_tweet_count = 0

        for found_url in found_urls:
            (url, text) = found_url.result

            if url in unique_previewable_urls and url not in processed_urls:
                processed_urls.add(url)
            else:
                continue

            if not self.is_absolute_url(url):
                if self.is_image(url):
                    self.handle_image_inlining(root, found_url)
                # We don't have a strong use case for doing URL preview for relative links.
                continue

            dropbox_image = self.dropbox_image(url)
            if dropbox_image is not None:
                class_attr = "message_inline_ref"
                is_image = dropbox_image["is_image"]
                if is_image:
                    class_attr = "message_inline_image"
                    # Not making use of title and description of images
                self.add_a(
                    root,
                    dropbox_image["image"],
                    url,
                    title=dropbox_image.get("title"),
                    desc=dropbox_image.get("desc", ""),
                    class_attr=class_attr,
                    already_thumbnailed=True,
                )
                continue

            if self.is_image(url):
                image_source = self.corrected_image_source(url)
                if image_source is not None:
                    found_url = ResultWithFamily(
                        family=found_url.family,
                        result=(image_source, image_source),
                    )
                self.handle_image_inlining(root, found_url)
                continue

            if get_tweet_id(url) is not None:
                if rendered_tweet_count >= self.TWITTER_MAX_TO_PREVIEW:
                    # Only render at most one tweet per message
                    continue
                twitter_data = self.twitter_link(url)
                if twitter_data is None:
                    # This link is not actually a tweet known to twitter
                    continue
                rendered_tweet_count += 1
                self.handle_tweet_inlining(root, found_url, twitter_data)
                continue
            youtube = self.youtube_image(url)
            if youtube is not None:
                self.handle_youtube_url_inlining(root, found_url, youtube)
                # NOTE: We don't `continue` here, to allow replacing the URL with
                # the title, if INLINE_URL_EMBED_PREVIEW feature is enabled.
                # The entire preview would ideally be shown only if the feature
                # is enabled, but URL previews are a beta feature and YouTube
                # previews are pretty stable.

            db_data = self.md.zulip_db_data
            if db_data and db_data["sent_by_bot"]:
                continue

            if not self.md.url_embed_preview_enabled:
                continue

            try:
                extracted_data = link_preview.link_embed_data_from_cache(url)
            except NotFoundInCache:
                self.md.zulip_message.links_for_preview.add(url)
                continue

            if extracted_data:
                if youtube is not None:
                    title = self.youtube_title(extracted_data)
                    if title is not None:
                        if url == text:
                            found_url.family.child.text = title
                        else:
                            found_url.family.child.text = text
                    continue
                self.add_embed(root, url, extracted_data)
                if self.vimeo_id(url):
                    title = self.vimeo_title(extracted_data)
                    if title:
                        if url == text:
                            found_url.family.child.text = title
                        else:
                            found_url.family.child.text = text


class Timestamp(markdown.inlinepatterns.Pattern):
    def handleMatch(self, match: Match[str]) -> Optional[Element]:
        time_input_string = match.group("time")
        timestamp = None
        try:
            timestamp = dateutil.parser.parse(time_input_string, tzinfos=common_timezones)
        except ValueError:
            try:
                timestamp = datetime.datetime.fromtimestamp(float(time_input_string))
            except ValueError:
                pass

        if not timestamp:
            error_element = Element("span")
            error_element.set("class", "timestamp-error")
            error_element.text = markdown.util.AtomicString(
                f"Invalid time format: {time_input_string}"
            )
            return error_element

        # Use HTML5 <time> element for valid timestamps.
        time_element = Element("time")
        if timestamp.tzinfo:
            timestamp = timestamp.astimezone(datetime.timezone.utc)
        else:
            timestamp = timestamp.replace(tzinfo=datetime.timezone.utc)
        time_element.set("datetime", timestamp.isoformat().replace("+00:00", "Z"))
        # Set text to initial input, so simple clients translating
        # HTML to text will at least display something.
        time_element.text = markdown.util.AtomicString(time_input_string)
        return time_element


# All of our emojis(non ZWJ sequences) belong to one of these Unicode blocks:
# \U0001f100-\U0001f1ff - Enclosed Alphanumeric Supplement
# \U0001f200-\U0001f2ff - Enclosed Ideographic Supplement
# \U0001f300-\U0001f5ff - Miscellaneous Symbols and Pictographs
# \U0001f600-\U0001f64f - Emoticons (Emoji)
# \U0001f680-\U0001f6ff - Transport and Map Symbols
# \U0001f900-\U0001f9ff - Supplemental Symbols and Pictographs
# \u2000-\u206f         - General Punctuation
# \u2300-\u23ff         - Miscellaneous Technical
# \u2400-\u243f         - Control Pictures
# \u2440-\u245f         - Optical Character Recognition
# \u2460-\u24ff         - Enclosed Alphanumerics
# \u2500-\u257f         - Box Drawing
# \u2580-\u259f         - Block Elements
# \u25a0-\u25ff         - Geometric Shapes
# \u2600-\u26ff         - Miscellaneous Symbols
# \u2700-\u27bf         - Dingbats
# \u2900-\u297f         - Supplemental Arrows-B
# \u2b00-\u2bff         - Miscellaneous Symbols and Arrows
# \u3000-\u303f         - CJK Symbols and Punctuation
# \u3200-\u32ff         - Enclosed CJK Letters and Months
unicode_emoji_regex = (
    "(?P<syntax>["
    "\U0001F100-\U0001F64F"
    "\U0001F680-\U0001F6FF"
    "\U0001F900-\U0001F9FF"
    "\u2000-\u206F"
    "\u2300-\u27BF"
    "\u2900-\u297F"
    "\u2B00-\u2BFF"
    "\u3000-\u303F"
    "\u3200-\u32FF"
    "])"
)
# The equivalent JS regex is \ud83c[\udd00-\udfff]|\ud83d[\udc00-\ude4f]|\ud83d[\ude80-\udeff]|
# \ud83e[\udd00-\uddff]|[\u2000-\u206f]|[\u2300-\u27bf]|[\u2b00-\u2bff]|[\u3000-\u303f]|
# [\u3200-\u32ff]. See below comments for explanation. The JS regex is used by marked.js for
# frontend Unicode emoji processing.
# The JS regex \ud83c[\udd00-\udfff]|\ud83d[\udc00-\ude4f] represents U0001f100-\U0001f64f
# The JS regex \ud83d[\ude80-\udeff] represents \U0001f680-\U0001f6ff
# The JS regex \ud83e[\udd00-\uddff] represents \U0001f900-\U0001f9ff
# The JS regex [\u2000-\u206f] represents \u2000-\u206f
# The JS regex [\u2300-\u27bf] represents \u2300-\u27bf
# Similarly other JS regexes can be mapped to the respective Unicode blocks.
# For more information, please refer to the following article:
# http://crocodillon.com/blog/parsing-emoji-unicode-in-javascript


def make_emoji(codepoint: str, display_string: str) -> Element:
    # Replace underscore in emoji's title with space
    title = display_string[1:-1].replace("_", " ")
    span = Element("span")
    span.set("class", f"emoji emoji-{codepoint}")
    span.set("title", title)
    span.set("role", "img")
    span.set("aria-label", title)
    span.text = markdown.util.AtomicString(display_string)
    return span


def make_realm_emoji(src: str, display_string: str) -> Element:
    elt = Element("img")
    elt.set("src", src)
    elt.set("class", "emoji")
    elt.set("alt", display_string)
    elt.set("title", display_string[1:-1].replace("_", " "))
    return elt


def unicode_emoji_to_codepoint(unicode_emoji: str) -> str:
    codepoint = hex(ord(unicode_emoji))[2:]
    # Unicode codepoints are minimum of length 4, padded
    # with zeroes if the length is less than zero.
    while len(codepoint) < 4:
        codepoint = "0" + codepoint
    return codepoint


class EmoticonTranslation(markdown.inlinepatterns.Pattern):
    """Translates emoticons like `:)` into emoji like `:smile:`."""

    def handleMatch(self, match: Match[str]) -> Optional[Element]:
        db_data = self.md.zulip_db_data
        if db_data is None or not db_data["translate_emoticons"]:
            return None

        emoticon = match.group("emoticon")
        translated = translate_emoticons(emoticon)
        name = translated[1:-1]
        return make_emoji(name_to_codepoint[name], translated)


class UnicodeEmoji(markdown.inlinepatterns.Pattern):
    def handleMatch(self, match: Match[str]) -> Optional[Element]:
        orig_syntax = match.group("syntax")
        codepoint = unicode_emoji_to_codepoint(orig_syntax)
        if codepoint in codepoint_to_name:
            display_string = ":" + codepoint_to_name[codepoint] + ":"
            return make_emoji(codepoint, display_string)
        else:
            return None


class Emoji(markdown.inlinepatterns.Pattern):
    def handleMatch(self, match: Match[str]) -> Optional[Union[str, Element]]:
        orig_syntax = match.group("syntax")
        name = orig_syntax[1:-1]

        active_realm_emoji: Dict[str, Dict[str, str]] = {}
        db_data = self.md.zulip_db_data
        if db_data is not None:
            active_realm_emoji = db_data["active_realm_emoji"]

        if self.md.zulip_message and name in active_realm_emoji:
            return make_realm_emoji(active_realm_emoji[name]["source_url"], orig_syntax)
        elif name == "zulip":
            return make_realm_emoji(
                "/static/generated/emoji/images/emoji/unicode/zulip.png", orig_syntax
            )
        elif name in name_to_codepoint:
            return make_emoji(name_to_codepoint[name], orig_syntax)
        else:
            return orig_syntax


def content_has_emoji_syntax(content: str) -> bool:
    return re.search(EMOJI_REGEX, content) is not None


class Tex(markdown.inlinepatterns.Pattern):
    def handleMatch(self, match: Match[str]) -> Element:
        rendered = render_tex(match.group("body"), is_inline=True)
        if rendered is not None:
            return self.md.htmlStash.store(rendered)
        else:  # Something went wrong while rendering
            span = Element("span")
            span.set("class", "tex-error")
            span.text = markdown.util.AtomicString("$$" + match.group("body") + "$$")
            return span


def sanitize_url(url: str) -> Optional[str]:
    """
    Sanitize a URL against XSS attacks.
    See the docstring on markdown.inlinepatterns.LinkPattern.sanitize_url.
    """
    try:
        parts = urllib.parse.urlparse(url.replace(" ", "%20"))
        scheme, netloc, path, params, query, fragment = parts
    except ValueError:
        # Bad URL - so bad it couldn't be parsed.
        return ""

    # If there is no scheme or netloc and there is a '@' in the path,
    # treat it as a mailto: and set the appropriate scheme
    if scheme == "" and netloc == "" and "@" in path:
        scheme = "mailto"
    elif scheme == "" and netloc == "" and len(path) > 0 and path[0] == "/":
        # Allow domain-relative links
        return urllib.parse.urlunparse(("", "", path, params, query, fragment))
    elif (scheme, netloc, path, params, query) == ("", "", "", "", "") and len(fragment) > 0:
        # Allow fragment links
        return urllib.parse.urlunparse(("", "", "", "", "", fragment))

    # Zulip modification: If scheme is not specified, assume http://
    # We re-enter sanitize_url because netloc etc. need to be re-parsed.
    if not scheme:
        return sanitize_url("http://" + url)

    locless_schemes = ["mailto", "news", "file", "bitcoin", "sms", "tel"]
    if netloc == "" and scheme not in locless_schemes:
        # This fails regardless of anything else.
        # Return immediately to save additional processing
        return None

    # Upstream code will accept a URL like javascript://foo because it
    # appears to have a netloc.  Additionally there are plenty of other
    # schemes that do weird things like launch external programs.  To be
    # on the safe side, we whitelist the scheme.
    if scheme not in ("http", "https", "ftp", "mailto", "file", "bitcoin", "sms", "tel"):
        return None

    # Upstream code scans path, parameters, and query for colon characters
    # because
    #
    #    some aliases [for javascript:] will appear to urllib.parse to have
    #    no scheme. On top of that relative links (i.e.: "foo/bar.html")
    #    have no scheme.
    #
    # We already converted an empty scheme to http:// above, so we skip
    # the colon check, which would also forbid a lot of legitimate URLs.

    # URL passes all tests. Return URL as-is.
    return urllib.parse.urlunparse((scheme, netloc, path, params, query, fragment))


def url_to_a(
    db_data: Optional[DbData], url: str, text: Optional[str] = None
) -> Union[Element, str]:
    a = Element("a")

    href = sanitize_url(url)
    if href is None:
        # Rejected by sanitize_url; render it as plain text.
        return url
    if text is None:
        text = markdown.util.AtomicString(url)

    href = rewrite_local_links_to_relative(db_data, href)

    a.set("href", href)
    a.text = text
    return a


class CompiledPattern(markdown.inlinepatterns.Pattern):
    def __init__(self, compiled_re: Pattern[str], md: markdown.Markdown) -> None:
        # This is similar to the superclass's small __init__ function,
        # but we skip the compilation step and let the caller give us
        # a compiled regex.
        self.compiled_re = compiled_re
        self.md = md


class AutoLink(CompiledPattern):
    def handleMatch(self, match: Match[str]) -> ElementStringNone:
        url = match.group("url")
        db_data = self.md.zulip_db_data
        return url_to_a(db_data, url)


class OListProcessor(sane_lists.SaneOListProcessor):
    def __init__(self, parser: BlockParser) -> None:
        parser.md.tab_length = 2
        super().__init__(parser)
        parser.md.tab_length = 4


class UListProcessor(sane_lists.SaneUListProcessor):
    """Unordered lists, but with 2-space indent"""

    def __init__(self, parser: BlockParser) -> None:
        parser.md.tab_length = 2
        super().__init__(parser)
        parser.md.tab_length = 4


class ListIndentProcessor(markdown.blockprocessors.ListIndentProcessor):
    """Process unordered list blocks.

    Based on markdown.blockprocessors.ListIndentProcessor, but with 2-space indent
    """

    def __init__(self, parser: BlockParser) -> None:

        # HACK: Set the tab length to 2 just for the initialization of
        # this class, so that bulleted lists (and only bulleted lists)
        # work off 2-space indentation.
        parser.md.tab_length = 2
        super().__init__(parser)
        parser.md.tab_length = 4


class HashHeaderProcessor(markdown.blockprocessors.HashHeaderProcessor):
    """Process hash headers.

    Based on markdown.blockprocessors.HashHeaderProcessor, but requires space for heading.
    """

    # Original regex for hashheader is
    # RE = re.compile(r'(?:^|\n)(?P<level>#{1,6})(?P<header>(?:\\.|[^\\])*?)#*(?:\n|$)')
    RE = re.compile(r"(?:^|\n)(?P<level>#{1,6})\s(?P<header>(?:\\.|[^\\])*?)#*(?:\n|$)")


class BlockQuoteProcessor(markdown.blockprocessors.BlockQuoteProcessor):
    """Process block quotes.

    Based on markdown.blockprocessors.BlockQuoteProcessor, but with 2-space indent
    """

    # Original regex for blockquote is RE = re.compile(r'(^|\n)[ ]{0,3}>[ ]?(.*)')
    RE = re.compile(r"(^|\n)(?!(?:[ ]{0,3}>\s*(?:$|\n))*(?:$|\n))" r"[ ]{0,3}>[ ]?(.*)")
    mention_re = re.compile(mention.find_mentions)

    # run() is very slightly forked from the base class; see notes below.
    def run(self, parent: Element, blocks: List[str]) -> None:
        block = blocks.pop(0)
        m = self.RE.search(block)
        if m:
            before = block[: m.start()]  # Lines before blockquote
            # Pass lines before blockquote in recursively for parsing first.
            self.parser.parseBlocks(parent, [before])
            # Remove ``> `` from beginning of each line.
            block = "\n".join([self.clean(line) for line in block[m.start() :].split("\n")])

        # Zulip modification: The next line is patched to match
        # CommonMark rather than original Markdown.  In original
        # Markdown, blockquotes with a blank line between them were
        # merged, which makes it impossible to break a blockquote with
        # a blank line intentionally.
        #
        # This is a new blockquote. Create a new parent element.
        quote = etree.SubElement(parent, "blockquote")

        # Recursively parse block with blockquote as parent.
        # change parser state so blockquotes embedded in lists use p tags
        self.parser.state.set("blockquote")
        self.parser.parseChunk(quote, block)
        self.parser.state.reset()

    def clean(self, line: str) -> str:
        # Silence all the mentions inside blockquotes
        line = re.sub(self.mention_re, lambda m: "@_{}".format(m.group("match")), line)

        # And then run the upstream processor's code for removing the '>'
        return super().clean(line)


@dataclass
class Fence:
    fence_str: str
    is_code: bool


class MarkdownListPreprocessor(markdown.preprocessors.Preprocessor):
    """Allows list blocks that come directly after another block
    to be rendered as a list.

    Detects paragraphs that have a matching list item that comes
    directly after a line of text, and inserts a newline between
    to satisfy Markdown"""

    LI_RE = re.compile(r"^[ ]*([*+-]|\d\.)[ ]+(.*)", re.MULTILINE)

    def run(self, lines: List[str]) -> List[str]:
        """Insert a newline between a paragraph and ulist if missing"""
        inserts = 0
        in_code_fence: bool = False
        open_fences: List[Fence] = []
        copy = lines[:]
        for i in range(len(lines) - 1):
            # Ignore anything that is inside a fenced code block but not quoted.
            # We ignore all lines where some parent is a non quote code block.
            m = FENCE_RE.match(lines[i])
            if m:
                fence_str = m.group("fence")
                lang: Optional[str] = m.group("lang")
                is_code = lang not in ("quote", "quoted")
                has_open_fences = not len(open_fences) == 0
                matches_last_fence = (
                    fence_str == open_fences[-1].fence_str if has_open_fences else False
                )
                closes_last_fence = not lang and matches_last_fence

                if closes_last_fence:
                    open_fences.pop()
                else:
                    open_fences.append(Fence(fence_str, is_code))

                in_code_fence = any(fence.is_code for fence in open_fences)

            # If we're not in a fenced block and we detect an upcoming list
            # hanging off any block (including a list of another type), add
            # a newline.
            li1 = self.LI_RE.match(lines[i])
            li2 = self.LI_RE.match(lines[i + 1])
            if not in_code_fence and lines[i]:
                if (li2 and not li1) or (
                    li1 and li2 and (len(li1.group(1)) == 1) != (len(li2.group(1)) == 1)
                ):
                    copy.insert(i + inserts + 1, "")
                    inserts += 1
        return copy


# Name for the outer capture group we use to separate whitespace and
# other delimiters from the actual content.  This value won't be an
# option in user-entered capture groups.
BEFORE_CAPTURE_GROUP = "linkifier_before_match"
OUTER_CAPTURE_GROUP = "linkifier_actual_match"
AFTER_CAPTURE_GROUP = "linkifier_after_match"


def prepare_linkifier_pattern(source: str) -> str:
    """Augment a linkifier so it only matches after start-of-string,
    whitespace, or opening delimiters, won't match if there are word
    characters directly after, and saves what was matched as
    OUTER_CAPTURE_GROUP."""
    return fr"""(?P<{BEFORE_CAPTURE_GROUP}>^|\s|['"\(,:<])(?P<{OUTER_CAPTURE_GROUP}>{source})(?P<{AFTER_CAPTURE_GROUP}>$|[^\pL\pN])"""


# Given a regular expression pattern, linkifies groups that match it
# using the provided format string to construct the URL.
class LinkifierPattern(markdown.inlinepatterns.InlineProcessor):
    """Applied a given linkifier to the input"""

    def __init__(
        self,
        source_pattern: str,
        format_string: str,
        md: markdown.Markdown,
    ) -> None:
        # Do not write errors to stderr (this still raises exceptions)
        options = re2.Options()
        options.log_errors = False

        self.md = md
        self.compiled_re = re2.compile(prepare_linkifier_pattern(source_pattern), options=options)
        self.format_string = format_string

    def handleMatch(  # type: ignore[override] # supertype incompatible with supersupertype
        self, m: Match[str], data: str
    ) -> Union[Tuple[Element, int, int], Tuple[None, None, None]]:
        db_data = self.md.zulip_db_data
        url = url_to_a(
            db_data,
            self.format_string % m.groupdict(),
            markdown.util.AtomicString(m.group(OUTER_CAPTURE_GROUP)),
        )
        if isinstance(url, str):
            return None, None, None

        return (
            url,
            m.start(2),
            m.end(2),
        )


class UserMentionPattern(markdown.inlinepatterns.InlineProcessor):
    def handleMatch(  # type: ignore[override] # supertype incompatible with supersupertype
        self, m: Match[str], data: str
    ) -> Union[Tuple[None, None, None], Tuple[Element, int, int]]:
        match = m.group("match")
        silent = m.group("silent") == "_"

        db_data = self.md.zulip_db_data
        if self.md.zulip_message and db_data is not None:
            if match.startswith("**") and match.endswith("**"):
                name = match[2:-2]
            else:
                return None, None, None

            wildcard = mention.user_mention_matches_wildcard(name)

            # For @**|id** and @**name|id** mention syntaxes.
            id_syntax_match = re.match(r"(?P<full_name>.+)?\|(?P<user_id>\d+)$", name)
            if id_syntax_match:
                full_name = id_syntax_match.group("full_name")
                id = int(id_syntax_match.group("user_id"))
                user = db_data["mention_data"].get_user_by_id(id)

                # For @**name|id**, we need to specifically check that
                # name matches the full_name of user in mention_data.
                # This enforces our decision that
                # @**user_1_name|id_for_user_2** should be invalid syntax.
                if full_name:
                    if user and user["full_name"] != full_name:
                        return None, None, None
            else:
                # For @**name** syntax.
                user = db_data["mention_data"].get_user_by_name(name)

            if wildcard:
                if not silent:
                    self.md.zulip_message.mentions_wildcard = True
                user_id = "*"
            elif user:
                if not silent:
                    self.md.zulip_message.mentions_user_ids.add(user["id"])
                name = user["full_name"]
                user_id = str(user["id"])
            else:
                # Don't highlight @mentions that don't refer to a valid user
                return None, None, None

            el = Element("span")
            el.set("data-user-id", user_id)
            text = f"{name}"
            if silent:
                el.set("class", "user-mention silent")
            else:
                el.set("class", "user-mention")
                text = f"@{text}"
            el.text = markdown.util.AtomicString(text)
            return el, m.start(), m.end()
        return None, None, None


class UserGroupMentionPattern(markdown.inlinepatterns.InlineProcessor):
    def handleMatch(  # type: ignore[override] # supertype incompatible with supersupertype
        self, m: Match[str], data: str
    ) -> Union[Tuple[None, None, None], Tuple[Element, int, int]]:
        match = m.group(1)
        db_data = self.md.zulip_db_data

        if self.md.zulip_message and db_data is not None:
            name = extract_user_group(match)
            user_group = db_data["mention_data"].get_user_group(name)
            if user_group:
                self.md.zulip_message.mentions_user_group_ids.add(user_group.id)
                name = user_group.name
                user_group_id = str(user_group.id)
            else:
                # Don't highlight @-mentions that don't refer to a valid user
                # group.
                return None, None, None

            el = Element("span")
            el.set("class", "user-group-mention")
            el.set("data-user-group-id", user_group_id)
            text = f"@{name}"
            el.text = markdown.util.AtomicString(text)
            return el, m.start(), m.end()
        return None, None, None


class StreamPattern(markdown.inlinepatterns.InlineProcessor):
    def __init__(self, compiled_re: Pattern[str], md: markdown.Markdown) -> None:
        # This is similar to the superclass's small __init__ function,
        # but we skip the compilation step and let the caller give us
        # a compiled regex.
        self.compiled_re = compiled_re
        self.md = md

    def find_stream_by_name(self, name: str) -> Optional[Dict[str, Any]]:
        db_data = self.md.zulip_db_data
        if db_data is None:
            return None
        stream = db_data["stream_names"].get(name)
        return stream

    def handleMatch(  # type: ignore[override] # supertype incompatible with supersupertype
        self, m: Match[str], data: str
    ) -> Union[Tuple[None, None, None], Tuple[Element, int, int]]:
        name = m.group("stream_name")

        if self.md.zulip_message:
            stream = self.find_stream_by_name(name)
            if stream is None:
                return None, None, None
            el = Element("a")
            el.set("class", "stream")
            el.set("data-stream-id", str(stream["id"]))
            # TODO: We should quite possibly not be specifying the
            # href here and instead having the browser auto-add the
            # href when it processes a message with one of these, to
            # provide more clarity to API clients.
            # Also do the same for StreamTopicPattern.
            stream_url = encode_stream(stream["id"], name)
            el.set("href", f"/#narrow/stream/{stream_url}")
            text = f"#{name}"
            el.text = markdown.util.AtomicString(text)
            return el, m.start(), m.end()
        return None, None, None


class StreamTopicPattern(markdown.inlinepatterns.InlineProcessor):
    def __init__(self, compiled_re: Pattern[str], md: markdown.Markdown) -> None:
        # This is similar to the superclass's small __init__ function,
        # but we skip the compilation step and let the caller give us
        # a compiled regex.
        self.compiled_re = compiled_re
        self.md = md

    def find_stream_by_name(self, name: str) -> Optional[Dict[str, Any]]:
        db_data = self.md.zulip_db_data
        if db_data is None:
            return None
        stream = db_data["stream_names"].get(name)
        return stream

    def handleMatch(  # type: ignore[override] # supertype incompatible with supersupertype
        self, m: Match[str], data: str
    ) -> Union[Tuple[None, None, None], Tuple[Element, int, int]]:
        stream_name = m.group("stream_name")
        topic_name = m.group("topic_name")

        if self.md.zulip_message:
            stream = self.find_stream_by_name(stream_name)
            if stream is None or topic_name is None:
                return None, None, None
            el = Element("a")
            el.set("class", "stream-topic")
            el.set("data-stream-id", str(stream["id"]))
            stream_url = encode_stream(stream["id"], stream_name)
            topic_url = hash_util_encode(topic_name)
            link = f"/#narrow/stream/{stream_url}/topic/{topic_url}"
            el.set("href", link)
            text = f"#{stream_name} > {topic_name}"
            el.text = markdown.util.AtomicString(text)
            return el, m.start(), m.end()
        return None, None, None


def possible_linked_stream_names(content: str) -> Set[str]:
    matches = re.findall(STREAM_LINK_REGEX, content, re.VERBOSE)
    for match in re.finditer(STREAM_TOPIC_LINK_REGEX, content, re.VERBOSE):
        matches.append(match.group("stream_name"))
    return set(matches)


class AlertWordNotificationProcessor(markdown.preprocessors.Preprocessor):

    allowed_before_punctuation = {" ", "\n", "(", '"', ".", ",", "'", ";", "[", "*", "`", ">"}
    allowed_after_punctuation = {
        " ",
        "\n",
        ")",
        '",',
        "?",
        ":",
        ".",
        ",",
        "'",
        ";",
        "]",
        "!",
        "*",
        "`",
    }

    def check_valid_start_position(self, content: str, index: int) -> bool:
        if index <= 0 or content[index] in self.allowed_before_punctuation:
            return True
        return False

    def check_valid_end_position(self, content: str, index: int) -> bool:
        if index >= len(content) or content[index] in self.allowed_after_punctuation:
            return True
        return False

    def run(self, lines: List[str]) -> List[str]:
        db_data = self.md.zulip_db_data
        if self.md.zulip_message and db_data is not None:
            # We check for alert words here, the set of which are
            # dependent on which users may see this message.
            #
            # Our caller passes in the list of possible_words.  We
            # don't do any special rendering; we just append the alert words
            # we find to the set self.md.zulip_message.alert_words.

            realm_alert_words_automaton = db_data["realm_alert_words_automaton"]

            if realm_alert_words_automaton is not None:
                content = "\n".join(lines).lower()
                for end_index, (original_value, user_ids) in realm_alert_words_automaton.iter(
                    content
                ):
                    if self.check_valid_start_position(
                        content, end_index - len(original_value)
                    ) and self.check_valid_end_position(content, end_index + 1):
                        self.md.zulip_message.user_ids_with_alert_words.update(user_ids)
        return lines


class LinkInlineProcessor(markdown.inlinepatterns.LinkInlineProcessor):
    def zulip_specific_link_changes(self, el: Element) -> Union[None, Element]:
        href = el.get("href")
        assert href is not None

        # Sanitize URL or don't parse link. See linkify_tests in markdown_test_cases for banned syntax.
        href = sanitize_url(self.unescape(href.strip()))
        if href is None:
            return None  # no-op; the link is not processed.

        # Rewrite local links to be relative
        db_data = self.md.zulip_db_data
        href = rewrite_local_links_to_relative(db_data, href)

        # Make changes to <a> tag attributes
        el.set("href", href)

        # Show link href if title is empty
        if not el.text or not el.text.strip():
            el.text = href

        # Prevent linkifiers from running on the content of a Markdown link, breaking up the link.
        # This is a monkey-patch, but it might be worth sending a version of this change upstream.
        el.text = markdown.util.AtomicString(el.text)

        return el

    def handleMatch(  # type: ignore[override] # supertype incompatible with supersupertype
        self, m: Match[str], data: str
    ) -> Union[Tuple[None, None, None], Tuple[Element, int, int]]:
        ret = super().handleMatch(m, data)
        if ret[0] is not None:
            el: Optional[Element]
            el, match_start, index = ret
            el = self.zulip_specific_link_changes(el)
            if el is not None:
                return el, match_start, index
        return None, None, None


def get_sub_registry(r: markdown.util.Registry, keys: List[str]) -> markdown.util.Registry:
    # Registry is a new class added by Python-Markdown to replace OrderedDict.
    # Since Registry doesn't support .keys(), it is easier to make a new
    # object instead of removing keys from the existing object.
    new_r = markdown.util.Registry()
    for k in keys:
        new_r.register(r[k], k, r.get_index_for_name(k))
    return new_r


# These are used as keys ("linkifiers_keys") to md_engines and the respective
# linkifier caches
DEFAULT_MARKDOWN_KEY = -1
ZEPHYR_MIRROR_MARKDOWN_KEY = -2


class Markdown(markdown.Markdown):
    zulip_message: Optional[Message]
    zulip_realm: Optional[Realm]
    zulip_db_data: Optional[DbData]
    image_preview_enabled: bool
    url_embed_preview_enabled: bool

    def __init__(
        self,
        linkifiers: List[LinkifierDict],
        linkifiers_key: int,
        email_gateway: bool,
    ) -> None:
        self.linkifiers = linkifiers
        self.linkifiers_key = linkifiers_key
        self.email_gateway = email_gateway

        super().__init__(
            extensions=[
                nl2br.makeExtension(),
                tables.makeExtension(),
                codehilite.makeExtension(
                    linenums=False,
                    guess_lang=False,
                ),
            ],
        )
        self.set_output_format("html")

    def build_parser(self) -> markdown.Markdown:
        # Build the parser using selected default features from Python-Markdown.
        # The complete list of all available processors can be found in the
        # super().build_parser() function.
        #
        # Note: for any Python-Markdown updates, manually check if we want any
        # of the new features added upstream or not; they wouldn't get
        # included by default.
        self.preprocessors = self.build_preprocessors()
        self.parser = self.build_block_parser()
        self.inlinePatterns = self.build_inlinepatterns()
        self.treeprocessors = self.build_treeprocessors()
        self.postprocessors = self.build_postprocessors()
        self.handle_zephyr_mirror()
        return self

    def build_preprocessors(self) -> markdown.util.Registry:
        # We disable the following preprocessors from upstream:
        #
        # html_block - insecure
        # reference - references don't make sense in a chat context.
        preprocessors = markdown.util.Registry()
        preprocessors.register(MarkdownListPreprocessor(self), "hanging_lists", 35)
        preprocessors.register(
            markdown.preprocessors.NormalizeWhitespace(self), "normalize_whitespace", 30
        )
        preprocessors.register(fenced_code.FencedBlockPreprocessor(self), "fenced_code_block", 25)
        preprocessors.register(
            AlertWordNotificationProcessor(self), "custom_text_notifications", 20
        )
        return preprocessors

    def build_block_parser(self) -> BlockParser:
        # We disable the following blockparsers from upstream:
        #
        # indent - replaced by ours
        # setextheader - disabled; we only support hashheaders for headings
        # olist - replaced by ours
        # ulist - replaced by ours
        # quote - replaced by ours
        parser = BlockParser(self)
        parser.blockprocessors.register(
            markdown.blockprocessors.EmptyBlockProcessor(parser), "empty", 95
        )
        parser.blockprocessors.register(ListIndentProcessor(parser), "indent", 90)
        if not self.email_gateway:
            parser.blockprocessors.register(
                markdown.blockprocessors.CodeBlockProcessor(parser), "code", 85
            )
        parser.blockprocessors.register(HashHeaderProcessor(parser), "hashheader", 80)
        # We get priority 75 from 'table' extension
        parser.blockprocessors.register(markdown.blockprocessors.HRProcessor(parser), "hr", 70)
        parser.blockprocessors.register(OListProcessor(parser), "olist", 65)
        parser.blockprocessors.register(UListProcessor(parser), "ulist", 60)
        parser.blockprocessors.register(BlockQuoteProcessor(parser), "quote", 55)
        parser.blockprocessors.register(
            markdown.blockprocessors.ParagraphProcessor(parser), "paragraph", 50
        )
        return parser

    def build_inlinepatterns(self) -> markdown.util.Registry:
        # We disable the following upstream inline patterns:
        #
        # backtick -        replaced by ours
        # escape -          probably will re-add at some point.
        # link -            replaced by ours
        # image_link -      replaced by ours
        # autolink -        replaced by ours
        # automail -        replaced by ours
        # linebreak -       we use nl2br and consider that good enough
        # html -            insecure
        # reference -       references not useful
        # image_reference - references not useful
        # short_reference - references not useful
        # ---------------------------------------------------
        # strong_em -       for these three patterns,
        # strong2 -         we have our own versions where
        # emphasis2 -       we disable _ for bold and emphasis

        # Declare regexes for clean single line calls to .register().
        NOT_STRONG_RE = markdown.inlinepatterns.NOT_STRONG_RE
        # Custom strikethrough syntax: ~~foo~~
        DEL_RE = r"(?<!~)(\~\~)([^~\n]+?)(\~\~)(?!~)"
        # Custom bold syntax: **foo** but not __foo__
        # str inside ** must start and end with a word character
        # it need for things like "const char *x = (char *)y"
        EMPHASIS_RE = r"(\*)(?!\s+)([^\*^\n]+)(?<!\s)\*"
        ENTITY_RE = markdown.inlinepatterns.ENTITY_RE
        STRONG_EM_RE = r"(\*\*\*)(?!\s+)([^\*^\n]+)(?<!\s)\*\*\*"

        # Add inline patterns.  We use a custom numbering of the
        # rules, that preserves the order from upstream but leaves
        # space for us to add our own.
        reg = markdown.util.Registry()
        reg.register(BacktickInlineProcessor(markdown.inlinepatterns.BACKTICK_RE), "backtick", 105)
        reg.register(
            markdown.inlinepatterns.DoubleTagPattern(STRONG_EM_RE, "strong,em"), "strong_em", 100
        )
        reg.register(UserMentionPattern(mention.find_mentions, self), "usermention", 95)
        reg.register(
            Tex(r"\B(?<!\$)\$\$(?P<body>[^\n_$](\\\$|[^$\n])*)\$\$(?!\$)\B", self), "tex", 90
        )
        reg.register(StreamTopicPattern(get_compiled_stream_topic_link_regex(), self), "topic", 87)
        reg.register(StreamPattern(get_compiled_stream_link_regex(), self), "stream", 85)
        reg.register(Timestamp(r"<time:(?P<time>[^>]*?)>"), "timestamp", 75)
        reg.register(
            UserGroupMentionPattern(mention.user_group_mentions, self), "usergroupmention", 65
        )
        reg.register(LinkInlineProcessor(markdown.inlinepatterns.LINK_RE, self), "link", 60)
        reg.register(AutoLink(get_web_link_regex(), self), "autolink", 55)
        # Reserve priority 45-54 for linkifiers
        reg = self.register_linkifiers(reg)
        reg.register(markdown.inlinepatterns.HtmlInlineProcessor(ENTITY_RE, self), "entity", 40)
        reg.register(
            markdown.inlinepatterns.SimpleTagPattern(r"(\*\*)([^\n]+?)\2", "strong"), "strong", 35
        )
        reg.register(markdown.inlinepatterns.SimpleTagPattern(EMPHASIS_RE, "em"), "emphasis", 30)
        reg.register(markdown.inlinepatterns.SimpleTagPattern(DEL_RE, "del"), "del", 25)
        reg.register(
            markdown.inlinepatterns.SimpleTextInlineProcessor(NOT_STRONG_RE), "not_strong", 20
        )
        reg.register(Emoji(EMOJI_REGEX, self), "emoji", 15)
        reg.register(EmoticonTranslation(emoticon_regex, self), "translate_emoticons", 10)
        # We get priority 5 from 'nl2br' extension
        reg.register(UnicodeEmoji(unicode_emoji_regex), "unicodeemoji", 0)
        return reg

    def register_linkifiers(self, inlinePatterns: markdown.util.Registry) -> markdown.util.Registry:
        for linkifier in self.linkifiers:
            pattern = linkifier["pattern"]
            inlinePatterns.register(
                LinkifierPattern(pattern, linkifier["url_format"], self),
                f"linkifiers/{pattern}",
                45,
            )
        return inlinePatterns

    def build_treeprocessors(self) -> markdown.util.Registry:
        # Here we build all the processors from upstream, plus a few of our own.
        treeprocessors = markdown.util.Registry()
        # We get priority 30 from 'hilite' extension
        treeprocessors.register(markdown.treeprocessors.InlineProcessor(self), "inline", 25)
        treeprocessors.register(markdown.treeprocessors.PrettifyTreeprocessor(self), "prettify", 20)
        treeprocessors.register(
            InlineInterestingLinkProcessor(self), "inline_interesting_links", 15
        )
        if settings.CAMO_URI:
            treeprocessors.register(InlineImageProcessor(self), "rewrite_images_proxy", 10)
        return treeprocessors

    def build_postprocessors(self) -> markdown.util.Registry:
        # These are the default Python-Markdown processors, unmodified.
        postprocessors = markdown.util.Registry()
        postprocessors.register(markdown.postprocessors.RawHtmlPostprocessor(self), "raw_html", 20)
        postprocessors.register(
            markdown.postprocessors.AndSubstitutePostprocessor(), "amp_substitute", 15
        )
        postprocessors.register(markdown.postprocessors.UnescapePostprocessor(), "unescape", 10)
        return postprocessors

    def handle_zephyr_mirror(self) -> None:
        if self.linkifiers_key == ZEPHYR_MIRROR_MARKDOWN_KEY:
            # Disable almost all inline patterns for zephyr mirror
            # users' traffic that is mirrored.  Note that
            # inline_interesting_links is a treeprocessor and thus is
            # not removed
            self.inlinePatterns = get_sub_registry(self.inlinePatterns, ["autolink"])
            self.treeprocessors = get_sub_registry(
                self.treeprocessors, ["inline_interesting_links", "rewrite_images_proxy"]
            )
            # insert new 'inline' processor because we have changed self.inlinePatterns
            # but InlineProcessor copies md as self.md in __init__.
            self.treeprocessors.register(
                markdown.treeprocessors.InlineProcessor(self), "inline", 25
            )
            self.preprocessors = get_sub_registry(self.preprocessors, ["custom_text_notifications"])
            self.parser.blockprocessors = get_sub_registry(
                self.parser.blockprocessors, ["paragraph"]
            )


md_engines: Dict[Tuple[int, bool], Markdown] = {}
linkifier_data: Dict[int, List[LinkifierDict]] = {}


def make_md_engine(linkifiers_key: int, email_gateway: bool) -> None:
    md_engine_key = (linkifiers_key, email_gateway)
    if md_engine_key in md_engines:
        del md_engines[md_engine_key]

    linkifiers = linkifier_data[linkifiers_key]
    md_engines[md_engine_key] = Markdown(
        linkifiers=linkifiers,
        linkifiers_key=linkifiers_key,
        email_gateway=email_gateway,
    )


# Split the topic name into multiple sections so that we can easily use
# our common single link matching regex on it.
basic_link_splitter = re.compile(r"[ !;\?\),\'\"]")

# Security note: We don't do any HTML escaping in this
# function on the URLs; they are expected to be HTML-escaped when
# rendered by clients (just as links rendered into message bodies
# are validated and escaped inside `url_to_a`).
def topic_links(linkifiers_key: int, topic_name: str) -> List[Dict[str, str]]:
    matches: List[Dict[str, Union[str, int]]] = []
    linkifiers = linkifiers_for_realm(linkifiers_key)

    options = re2.Options()
    options.log_errors = False
    for linkifier in linkifiers:
        raw_pattern = linkifier["pattern"]
        url_format_string = linkifier["url_format"]
        try:
            pattern = re2.compile(prepare_linkifier_pattern(raw_pattern), options=options)
        except re2.error:
            # An invalid regex shouldn't be possible here, and logging
            # here on an invalid regex would spam the logs with every
            # message sent; simply move on.
            continue
        for m in pattern.finditer(topic_name):
            match_details = m.groupdict()
            match_text = match_details["linkifier_actual_match"]
            # We format the linkifier's url string using the matched text.
            # Also, we include the matched text in the response, so that our clients
            # don't have to implement any logic of their own to get back the text.
            matches += [
                dict(
                    url=url_format_string % match_details,
                    text=match_text,
                    index=topic_name.find(match_text),
                )
            ]

    # Also make raw URLs navigable.
    for sub_string in basic_link_splitter.split(topic_name):
        link_match = re.match(get_web_link_regex(), sub_string)
        if link_match:
            actual_match_url = link_match.group("url")
            result = urlsplit(actual_match_url)
            if not result.scheme:
                if not result.netloc:
                    i = (result.path + "/").index("/")
                    result = result._replace(netloc=result.path[:i], path=result.path[i:])
                url = result._replace(scheme="https").geturl()
            else:
                url = actual_match_url
            matches.append(
                dict(url=url, text=actual_match_url, index=topic_name.find(actual_match_url))
            )

    # In order to preserve the order in which the links occur, we sort the matched text
    # based on its starting index in the topic. We pop the index field before returning.
    matches = sorted(matches, key=lambda k: k["index"])
    return [{k: str(v) for k, v in match.items() if k != "index"} for match in matches]


def maybe_update_markdown_engines(linkifiers_key: int, email_gateway: bool) -> None:
    global linkifier_data

    linkifiers = linkifiers_for_realm(linkifiers_key)
    if linkifiers_key not in linkifier_data or linkifier_data[linkifiers_key] != linkifiers:
        # Linkifier data has changed, update `linkifier_data` and any
        # of the existing Markdown engines using this set of linkifiers.
        linkifier_data[linkifiers_key] = linkifiers
        for email_gateway_flag in [True, False]:
            if (linkifiers_key, email_gateway_flag) in md_engines:
                # Update only existing engines(if any), don't create new one.
                make_md_engine(linkifiers_key, email_gateway_flag)

    if (linkifiers_key, email_gateway) not in md_engines:
        # Markdown engine corresponding to this key doesn't exists so create one.
        make_md_engine(linkifiers_key, email_gateway)


# We want to log Markdown parser failures, but shouldn't log the actual input
# message for privacy reasons.  The compromise is to replace all alphanumeric
# characters with 'x'.
#
# We also use repr() to improve reproducibility, and to escape terminal control
# codes, which can do surprisingly nasty things.
_privacy_re = re.compile("\\w", flags=re.UNICODE)


def privacy_clean_markdown(content: str) -> str:
    return repr(_privacy_re.sub("x", content))


def get_possible_mentions_info(realm_id: int, mention_texts: Set[str]) -> List[FullNameInfo]:
    if not mention_texts:
        return []

    q_list = set()

    name_re = r"(?P<full_name>.+)?\|(?P<mention_id>\d+)$"
    for mention_text in mention_texts:
        name_syntax_match = re.match(name_re, mention_text)
        if name_syntax_match:
            full_name = name_syntax_match.group("full_name")
            mention_id = name_syntax_match.group("mention_id")
            if full_name:
                # For **name|id** mentions as mention_id
                # cannot be null inside this block.
                q_list.add(Q(full_name__iexact=full_name, id=mention_id))
            else:
                # For **|id** syntax.
                q_list.add(Q(id=mention_id))
        else:
            # For **name** syntax.
            q_list.add(Q(full_name__iexact=mention_text))

    rows = (
        UserProfile.objects.filter(
            realm_id=realm_id,
            is_active=True,
        )
        .filter(
            functools.reduce(lambda a, b: a | b, q_list),
        )
        .values(
            "id",
            "full_name",
            "email",
        )
    )
    return list(rows)


class MentionData:
    def __init__(self, realm_id: int, content: str) -> None:
        mention_texts, has_wildcards = possible_mentions(content)
        possible_mentions_info = get_possible_mentions_info(realm_id, mention_texts)
        self.full_name_info = {row["full_name"].lower(): row for row in possible_mentions_info}
        self.user_id_info = {row["id"]: row for row in possible_mentions_info}
        self.init_user_group_data(realm_id=realm_id, content=content)
        self.has_wildcards = has_wildcards

    def message_has_wildcards(self) -> bool:
        return self.has_wildcards

    def init_user_group_data(self, realm_id: int, content: str) -> None:
        user_group_names = possible_user_group_mentions(content)
        self.user_group_name_info = get_user_group_name_info(realm_id, user_group_names)
        self.user_group_members: Dict[int, List[int]] = defaultdict(list)
        group_ids = [group.id for group in self.user_group_name_info.values()]

        if not group_ids:
            # Early-return to avoid the cost of hitting the ORM,
            # which shows up in profiles.
            return

        membership = UserGroupMembership.objects.filter(user_group_id__in=group_ids)
        for info in membership.values("user_group_id", "user_profile_id"):
            group_id = info["user_group_id"]
            user_profile_id = info["user_profile_id"]
            self.user_group_members[group_id].append(user_profile_id)

    def get_user_by_name(self, name: str) -> Optional[FullNameInfo]:
        # warning: get_user_by_name is not dependable if two
        # users of the same full name are mentioned. Use
        # get_user_by_id where possible.
        return self.full_name_info.get(name.lower(), None)

    def get_user_by_id(self, id: int) -> Optional[FullNameInfo]:
        return self.user_id_info.get(id, None)

    def get_user_ids(self) -> Set[int]:
        """
        Returns the user IDs that might have been mentioned by this
        content.  Note that because this data structure has not parsed
        the message and does not know about escaping/code blocks, this
        will overestimate the list of user ids.
        """
        return set(self.user_id_info.keys())

    def get_user_group(self, name: str) -> Optional[UserGroup]:
        return self.user_group_name_info.get(name.lower(), None)

    def get_group_members(self, user_group_id: int) -> List[int]:
        return self.user_group_members.get(user_group_id, [])


def get_user_group_name_info(realm_id: int, user_group_names: Set[str]) -> Dict[str, UserGroup]:
    if not user_group_names:
        return {}

    rows = UserGroup.objects.filter(realm_id=realm_id, name__in=user_group_names)
    dct = {row.name.lower(): row for row in rows}
    return dct


def get_stream_name_info(realm: Realm, stream_names: Set[str]) -> Dict[str, FullNameInfo]:
    if not stream_names:
        return {}

    q_list = {Q(name=name) for name in stream_names}

    rows = (
        get_active_streams(
            realm=realm,
        )
        .filter(
            functools.reduce(lambda a, b: a | b, q_list),
        )
        .values(
            "id",
            "name",
        )
    )

    dct = {row["name"]: row for row in rows}
    return dct


def do_convert(
    content: str,
    realm_alert_words_automaton: Optional[ahocorasick.Automaton] = None,
    message: Optional[Message] = None,
    message_realm: Optional[Realm] = None,
    sent_by_bot: bool = False,
    translate_emoticons: bool = False,
    mention_data: Optional[MentionData] = None,
    email_gateway: bool = False,
    no_previews: bool = False,
) -> str:
    """Convert Markdown to HTML, with Zulip-specific settings and hacks."""
    # This logic is a bit convoluted, but the overall goal is to support a range of use cases:
    # * Nothing is passed in other than content -> just run default options (e.g. for docs)
    # * message is passed, but no realm is -> look up realm from message
    # * message_realm is passed -> use that realm for Markdown purposes
    if message is not None:
        if message_realm is None:
            message_realm = message.get_realm()
    if message_realm is None:
        linkifiers_key = DEFAULT_MARKDOWN_KEY
    else:
        linkifiers_key = message_realm.id

    if message and hasattr(message, "id") and message.id:
        logging_message_id = "id# " + str(message.id)
    else:
        logging_message_id = "unknown"

    if message is not None and message_realm is not None:
        if message_realm.is_zephyr_mirror_realm:
            if message.sending_client.name == "zephyr_mirror":
                # Use slightly customized Markdown processor for content
                # delivered via zephyr_mirror
                linkifiers_key = ZEPHYR_MIRROR_MARKDOWN_KEY

    maybe_update_markdown_engines(linkifiers_key, email_gateway)
    md_engine_key = (linkifiers_key, email_gateway)
    _md_engine = md_engines[md_engine_key]
    # Reset the parser; otherwise it will get slower over time.
    _md_engine.reset()

    # Filters such as UserMentionPattern need a message.
    _md_engine.zulip_message = message
    _md_engine.zulip_realm = message_realm
    _md_engine.zulip_db_data = None  # for now
    _md_engine.image_preview_enabled = image_preview_enabled(message, message_realm, no_previews)
    _md_engine.url_embed_preview_enabled = url_embed_preview_enabled(
        message, message_realm, no_previews
    )

    # Pre-fetch data from the DB that is used in the Markdown thread
    if message_realm is not None:

        # Here we fetch the data structures needed to render
        # mentions/stream mentions from the database, but only
        # if there is syntax in the message that might use them, since
        # the fetches are somewhat expensive and these types of syntax
        # are uncommon enough that it's a useful optimization.

        if mention_data is None:
            mention_data = MentionData(message_realm.id, content)

        stream_names = possible_linked_stream_names(content)
        stream_name_info = get_stream_name_info(message_realm, stream_names)

        if content_has_emoji_syntax(content):
            active_realm_emoji = message_realm.get_active_emoji()
        else:
            active_realm_emoji = {}

        _md_engine.zulip_db_data = {
            "realm_alert_words_automaton": realm_alert_words_automaton,
            "mention_data": mention_data,
            "active_realm_emoji": active_realm_emoji,
            "realm_uri": message_realm.uri,
            "sent_by_bot": sent_by_bot,
            "stream_names": stream_name_info,
            "translate_emoticons": translate_emoticons,
        }

    try:
        # Spend at most 5 seconds rendering; this protects the backend
        # from being overloaded by bugs (e.g. Markdown logic that is
        # extremely inefficient in corner cases) as well as user
        # errors (e.g. a linkifier that makes some syntax
        # infinite-loop).
        rendered_content = timeout(5, lambda: _md_engine.convert(content))

        # Throw an exception if the content is huge; this protects the
        # rest of the codebase from any bugs where we end up rendering
        # something huge.
        if len(rendered_content) > MAX_MESSAGE_LENGTH * 10:
            raise MarkdownRenderingException(
                f"Rendered content exceeds {MAX_MESSAGE_LENGTH * 10} characters (message {logging_message_id})"
            )
        return rendered_content
    except Exception:
        cleaned = privacy_clean_markdown(content)
        # NOTE: Don't change this message without also changing the
        # logic in logging_handlers.py or we can create recursive
        # exceptions.
        markdown_logger.exception(
            "Exception in Markdown parser; input (sanitized) was: %s\n (message %s)",
            cleaned,
            logging_message_id,
        )

        raise MarkdownRenderingException()
    finally:
        # These next three lines are slightly paranoid, since
        # we always set these right before actually using the
        # engine, but better safe then sorry.
        _md_engine.zulip_message = None
        _md_engine.zulip_realm = None
        _md_engine.zulip_db_data = None


markdown_time_start = 0.0
markdown_total_time = 0.0
markdown_total_requests = 0


def get_markdown_time() -> float:
    return markdown_total_time


def get_markdown_requests() -> int:
    return markdown_total_requests


def markdown_stats_start() -> None:
    global markdown_time_start
    markdown_time_start = time.time()


def markdown_stats_finish() -> None:
    global markdown_total_time
    global markdown_total_requests
    global markdown_time_start
    markdown_total_requests += 1
    markdown_total_time += time.time() - markdown_time_start


def markdown_convert(
    content: str,
    realm_alert_words_automaton: Optional[ahocorasick.Automaton] = None,
    message: Optional[Message] = None,
    message_realm: Optional[Realm] = None,
    sent_by_bot: bool = False,
    translate_emoticons: bool = False,
    mention_data: Optional[MentionData] = None,
    email_gateway: bool = False,
    no_previews: bool = False,
) -> str:
    markdown_stats_start()
    ret = do_convert(
        content,
        realm_alert_words_automaton,
        message,
        message_realm,
        sent_by_bot,
        translate_emoticons,
        mention_data,
        email_gateway,
        no_previews=no_previews,
    )
    markdown_stats_finish()
    return ret
