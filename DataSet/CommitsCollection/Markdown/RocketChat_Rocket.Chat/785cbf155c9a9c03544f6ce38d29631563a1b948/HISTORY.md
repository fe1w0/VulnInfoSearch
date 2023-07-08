
# 3.11.2
`2021-02-28  ·  4 🐛  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.22.2`

### 🐛 Bug fixes


- Security Hotfix (https://docs.rocket.chat/guides/security/security-updates)

- External systems not being able to change Omnichannel Inquiry priorities  ([#20740](https://github.com/RocketChat/Rocket.Chat/pull/20740))

  Due to a wrong property name, external applications were not able to change the priority of Omnichannel Inquires.

- Prevent Message Attachment rendering ([#20860](https://github.com/RocketChat/Rocket.Chat/pull/20860))

- Room owner not being able to override global retention policy ([#20727](https://github.com/RocketChat/Rocket.Chat/pull/20727))

  use correct permissions to check if room owner can override global retention policy

### 👩‍💻👨‍💻 Core Team 🤓

- [@g-thome](https://github.com/g-thome)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.11.1
`2021-02-10  ·  5 🐛  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.22.2`

### 🐛 Bug fixes


- Attachment download from title fixed ([#20585](https://github.com/RocketChat/Rocket.Chat/pull/20585) by [@yash-rajpal](https://github.com/yash-rajpal))

  Added target = '_self' to attachment link, this seems to fix the problem, without this attribute, error page is displayed.

- Gif images aspect ratio on preview ([#20654](https://github.com/RocketChat/Rocket.Chat/pull/20654))

- Livechat bridge permission checkers ([#20653](https://github.com/RocketChat/Rocket.Chat/pull/20653))

  Update to latest patch version of the Apps-Engine with a fix for the Livechat bridge, as seen in https://github.com/RocketChat/Rocket.Chat.Apps-engine/pull/379

- Omnichannel Routing System not assigning chats to Bot agents ([#20662](https://github.com/RocketChat/Rocket.Chat/pull/20662))

  The `Omnichannel Routing System` is no longer assigning chats to `bot` agents when the `bot` agent is the default agent of the inquiry.

- Update NPS banner when changing score ([#20611](https://github.com/RocketChat/Rocket.Chat/pull/20611))

### 👩‍💻👨‍💻 Contributors 😍

- [@yash-rajpal](https://github.com/yash-rajpal)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@lolimay](https://github.com/lolimay)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tiagoevanp](https://github.com/tiagoevanp)

# 3.11.0
`2021-01-31  ·  8 🎉  ·  9 🚀  ·  52 🐛  ·  44 🔍  ·  32 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.22.1`

### 🎉 New features


- **Apps:** Apps Permission System ([#20078](https://github.com/RocketChat/Rocket.Chat/pull/20078))

- **Apps:** IPreFileUpload event ([#20285](https://github.com/RocketChat/Rocket.Chat/pull/20285))

- **ENTERPRISE:** Automatic transfer of unanswered conversations to another agent ([#20090](https://github.com/RocketChat/Rocket.Chat/pull/20090))

- **ENTERPRISE:** Omnichannel Contact Manager as preferred agent for routing ([#20244](https://github.com/RocketChat/Rocket.Chat/pull/20244))

  If the `Contact-Manager` is assigned to a Visitor, the chat will automatically get transferred to the respective Contact-Manager, provided the Contact-Manager is online. In-case the Contact-Manager is offline, the chat will be transferred to any other online agent.
  We have provided a setting to control this auto-assignment feature
  ![image](https://user-images.githubusercontent.com/34130764/104880961-8104d780-5986-11eb-9d87-82b99814b028.png)

  Behavior based-on Routing method


  1. Auto-selection, Load-Balancing, or External Service (`autoAssignAgent = true`)
    This is straightforward,
        - if the Contact-manager is online, the chat will be transferred to the Contact-Manger only
        - if the Contact-manager is offline, the chat will be transferred to any other online-agent based on the Routing system

  2. Manual-selection (`autoAssignAgent = false`)
        - If the Contact-Manager is online, the chat will appear in the Queue of Contact-Manager **ONLY**
        - If the Contact-Manager is offline, the chat will appear in the Queue of all related Agents/Manager ( like it's done right now )

- Banner system and NPS ([#20221](https://github.com/RocketChat/Rocket.Chat/pull/20221))

  More robust and scalable banner system for alerting users.

- Email Inboxes for Omnichannel ([#20101](https://github.com/RocketChat/Rocket.Chat/pull/20101))

  With this new feature, email accounts will receive email messages(threads) which will be transformed into Omnichannel chats. It'll be possible to set up multiple email accounts, test the connection with email server(email provider) and define the behaviour of each account.

  https://user-images.githubusercontent.com/2493803/105430398-242d4980-5c32-11eb-835a-450c94837d23.mp4

  ### New item on admin menu

  ![image](https://user-images.githubusercontent.com/2493803/105428723-bc293400-5c2e-11eb-8c02-e8d36ea82726.png)


  ### Send test email tooltip

  ![image](https://user-images.githubusercontent.com/2493803/104366986-eaa16380-54f8-11eb-9ba7-831cfde2319c.png)


  ### Inbox Info

  ![image](https://user-images.githubusercontent.com/2493803/104366796-ab731280-54f8-11eb-9941-a3cc8eb610e1.png)

  ### SMTP Info

  ![image](https://user-images.githubusercontent.com/2493803/104366868-c47bc380-54f8-11eb-969e-ccc29070957c.png)

  ### IMAP Info

  ![image](https://user-images.githubusercontent.com/2493803/104366897-cd6c9500-54f8-11eb-80c4-97d5b0c002d5.png)

  ### Messages

  ![image](https://user-images.githubusercontent.com/2493803/105428971-45d90180-5c2f-11eb-992a-022a3df94471.png)

- Encrypted Discussions and new Encryption Permissions ([#20201](https://github.com/RocketChat/Rocket.Chat/pull/20201))

- Server Info page ([#19517](https://github.com/RocketChat/Rocket.Chat/pull/19517))

### 🚀 Improvements


- Add extra SAML settings to update room subs and add private room subs. ([#19489](https://github.com/RocketChat/Rocket.Chat/pull/19489) by [@tlskinneriv](https://github.com/tlskinneriv))

  Added a SAML setting to support updating room subscriptions each time a user logs in via SAML.
  Added a SAML setting to support including private rooms in SAML updated subscriptions (whether initial or on each logon).

- Autofocus on directory ([#20509](https://github.com/RocketChat/Rocket.Chat/pull/20509))

- Don't use global search by default ([#19777](https://github.com/RocketChat/Rocket.Chat/pull/19777) by [@i-kychukov](https://github.com/i-kychukov) & [@ikyuchukov](https://github.com/ikyuchukov))

  Global chat search is not set by default now.

- Message Collection Hooks ([#20121](https://github.com/RocketChat/Rocket.Chat/pull/20121))

  Integrating a list of messages into a React component imposes some challenges. Its content is provided by some REST API calls and live-updated by streamer events. To avoid too much coupling with React Hooks, the structures `RecordList`, `MessageList` and their derivatives are simple event emitters created and connected on components via some simple hooks, like `useThreadsList()` and `useRecordList()`.

- Rewrite Announcement as React component ([#20172](https://github.com/RocketChat/Rocket.Chat/pull/20172))

- Rewrite Prune Messages as React component ([#19900](https://github.com/RocketChat/Rocket.Chat/pull/19900))

- Rewrite User Dropdown and Kebab menu. ([#20070](https://github.com/RocketChat/Rocket.Chat/pull/20070))

  ![image](https://user-images.githubusercontent.com/40830821/103699786-3a74ad80-4f82-11eb-913e-2e09d5f7eac6.png)

- Title for user avatar buttons ([#20083](https://github.com/RocketChat/Rocket.Chat/pull/20083) by [@sushant52](https://github.com/sushant52))

  Made user avatar change buttons to be descriptive of what they do.

- Tooltip added for Kebab menu on chat header ([#20116](https://github.com/RocketChat/Rocket.Chat/pull/20116) by [@yash-rajpal](https://github.com/yash-rajpal))

  Added the missing Tooltip for kebab menu on chat header.
  ![tooltip after](https://user-images.githubusercontent.com/58601732/104031406-b07f4b80-51f2-11eb-87a4-1e8da78a254f.gif)

### 🐛 Bug fixes


- "Open_thread" English tooltip correction ([#20164](https://github.com/RocketChat/Rocket.Chat/pull/20164) by [@aKn1ghtOut](https://github.com/aKn1ghtOut))

  Remove unnecessary spaces from the translation key, and added English translation value for the key.

- **Apps:** Don't show the "review permissions" modal when there's none to review ([#20506](https://github.com/RocketChat/Rocket.Chat/pull/20506))

- **ENTERPRISE:** Auditing RoomAutocomplete ([#20311](https://github.com/RocketChat/Rocket.Chat/pull/20311))

- **ENTERPRISE:** Omnichannel custom fields not storing additional form values  ([#19953](https://github.com/RocketChat/Rocket.Chat/pull/19953))

- Actions from User Info panel ([#20073](https://github.com/RocketChat/Rocket.Chat/pull/20073) by [@Darshilp326](https://github.com/Darshilp326))

  Users can be removed from channels without any error message.

- Added context check for closing active tabbar for member-list ([#20228](https://github.com/RocketChat/Rocket.Chat/pull/20228) by [@yash-rajpal](https://github.com/yash-rajpal))

  When we click on a username and then click on see user's full profile, a tab gets active and shows us the user's profile, the problem occurs when the tab is still active and we try to see another user's profile. In this case, tabbar gets closed.
  To resolve this, added context check for closing action of active tabbar.

- Added Margin between status bullet and status label ([#20199](https://github.com/RocketChat/Rocket.Chat/pull/20199) by [@yash-rajpal](https://github.com/yash-rajpal))

  Added Margins between status bullet and status label

- Added success message on saving notification preference. ([#20220](https://github.com/RocketChat/Rocket.Chat/pull/20220) by [@Darshilp326](https://github.com/Darshilp326))

  Added success message after saving notification preferences.

  https://user-images.githubusercontent.com/55157259/104774617-03ca3e80-579d-11eb-8fa4-990b108dd8d9.mp4

- Admin User Info email verified status ([#20110](https://github.com/RocketChat/Rocket.Chat/pull/20110) by [@bdelwood](https://github.com/bdelwood))

- Agent information panel not rendering ([#19965](https://github.com/RocketChat/Rocket.Chat/pull/19965))

- Change header's favorite icon to filled star ([#20174](https://github.com/RocketChat/Rocket.Chat/pull/20174))

  ### Before:
  ![image](https://user-images.githubusercontent.com/27704687/104351819-a60bcd00-54e4-11eb-8b43-7d281a6e5dcb.png)

  ### After:
  ![image](https://user-images.githubusercontent.com/27704687/104351632-67761280-54e4-11eb-87ba-25b940494bb5.png)

- Changed success message for adding custom sound. ([#20272](https://github.com/RocketChat/Rocket.Chat/pull/20272) by [@Darshilp326](https://github.com/Darshilp326))

  https://user-images.githubusercontent.com/55157259/105151351-daf2d200-5b2b-11eb-8223-eae5d60f770d.mp4

- Changed success message for ignoring member. ([#19996](https://github.com/RocketChat/Rocket.Chat/pull/19996) by [@Darshilp326](https://github.com/Darshilp326))

  Different messages for ignoring/unignoring will be displayed.

  https://user-images.githubusercontent.com/55157259/103310307-4241c880-4a3d-11eb-8c6c-4c9b99d023db.mp4

- Creation of Omnichannel rooms not working correctly through the Apps when the agent parameter is set ([#19997](https://github.com/RocketChat/Rocket.Chat/pull/19997))

- Engagement dashboard graphs labels superposing each other ([#20267](https://github.com/RocketChat/Rocket.Chat/pull/20267))

  Now after a certain breakpoint, the graphs should stack vertically, and overlapping text rotated.

  ![image](https://user-images.githubusercontent.com/40830821/105098926-93b40500-5a89-11eb-9a56-2fc3b1552914.png)

- Fields overflowing page ([#20287](https://github.com/RocketChat/Rocket.Chat/pull/20287))

  ### Before
  ![image](https://user-images.githubusercontent.com/40830821/105246952-c1b14c00-5b52-11eb-8671-cff88edf242d.png)

  ### After
  ![image](https://user-images.githubusercontent.com/40830821/105247125-0a690500-5b53-11eb-9f3c-d6a68108e336.png)

- Fix error that occurs on changing archive status of room ([#20098](https://github.com/RocketChat/Rocket.Chat/pull/20098) by [@aKn1ghtOut](https://github.com/aKn1ghtOut))

  This PR fixes an issue that happens when you try to edit the info of a room, and save changes after changing the value of "Archived". The archive functionality is handled separately from other room settings. The archived key is not used in the saveRoomSettings method but was still being sent over. Hence, the request was being considered invalid. I deleted the "archived" key from the data being sent in the request, making the request valid again.

- Incorrect translations ZN ([#20245](https://github.com/RocketChat/Rocket.Chat/pull/20245) by [@moniang](https://github.com/moniang))

- Initial values update on Account Preferences ([#19938](https://github.com/RocketChat/Rocket.Chat/pull/19938))

- Invalid filters on the Omnichannel Analytics page ([#19899](https://github.com/RocketChat/Rocket.Chat/pull/19899))

- Jump to message ([#20265](https://github.com/RocketChat/Rocket.Chat/pull/20265))

- Livechat.RegisterGuest method removing unset fields ([#20124](https://github.com/RocketChat/Rocket.Chat/pull/20124))

  After changes made on https://github.com/RocketChat/Rocket.Chat/pull/19931, the `Livechat.RegisterGuest` method started removing properties from the visitor inappropriately. The properties that did not receive value were removed from the object.
  Those changes were made to support the new Contact Form, but now the form has its own method to deal with Contact data so those changes are no longer necessary.

- Markdown added for Header Room topic ([#20021](https://github.com/RocketChat/Rocket.Chat/pull/20021) by [@yash-rajpal](https://github.com/yash-rajpal))

  With the new 3.10.0 version update the Links in topic section below room name were not working, for more info refer issue #20018

- Messages being updated when not required after user changes his profile ([#20114](https://github.com/RocketChat/Rocket.Chat/pull/20114))

- Meteor errors not translating for toast messages ([#19993](https://github.com/RocketChat/Rocket.Chat/pull/19993))

- minWidth in FileIcon to prevent layout to broke ([#19942](https://github.com/RocketChat/Rocket.Chat/pull/19942))

  ![image](https://user-images.githubusercontent.com/27704687/102934691-69b7f480-4483-11eb-995b-a8a9b72246aa.png)

- Normalize messages for users in endpoint chat.getStarredMessages ([#19962](https://github.com/RocketChat/Rocket.Chat/pull/19962))

- OAuth users being asked to change password on second login ([#20003](https://github.com/RocketChat/Rocket.Chat/pull/20003))

- Omnichannel - Contact Center form is not validating custom fields properly ([#20196](https://github.com/RocketChat/Rocket.Chat/pull/20196))

  The contact form is accepting undefined values in required custom fields when creating or editing contacts, and, the errror message isn't following Rocket.chat design system.

  ### Before
  ![image](https://user-images.githubusercontent.com/2493803/104522668-31688980-55dd-11eb-92c5-83f96073edc4.png)

  ### After

  #### New
  ![image](https://user-images.githubusercontent.com/2493803/104770494-68f74300-574f-11eb-94a3-c8fd73365308.png)


  #### Edit
  ![image](https://user-images.githubusercontent.com/2493803/104770538-7b717c80-574f-11eb-829f-1ae304103369.png)

- Omnichannel Agents unable to take new chats in the queue ([#20022](https://github.com/RocketChat/Rocket.Chat/pull/20022))

- Omnichannel Business Hours form is not being rendered ([#20007](https://github.com/RocketChat/Rocket.Chat/pull/20007))

- Omnichannel raw model importing meteor dependency ([#20093](https://github.com/RocketChat/Rocket.Chat/pull/20093))

- Omnichannel rooms breaking after return to queue or forward ([#20089](https://github.com/RocketChat/Rocket.Chat/pull/20089))

- Profile picture changing with username ([#19992](https://github.com/RocketChat/Rocket.Chat/pull/19992))

  ![bug avatar](https://user-images.githubusercontent.com/40830821/103305935-24e40e80-49eb-11eb-9e35-9bd4c167898a.gif)

- Remove duplicate blaze events call for EmojiActions from roomOld ([#20159](https://github.com/RocketChat/Rocket.Chat/pull/20159) by [@aKn1ghtOut](https://github.com/aKn1ghtOut))

  A few methods concerning Emojis are bound multiple times to the DOM using the Template events() call, once in the reactions init.js and the other time after they get exported from app/ui/client/views/app/lib/getCommonRoomEvents.js to whatever page binds all the functions. The getCommonRoomEvents methods are always bound, hence negating a need to bind in a lower-level component.

- Room special name in prompts ([#20277](https://github.com/RocketChat/Rocket.Chat/pull/20277) by [@aKn1ghtOut](https://github.com/aKn1ghtOut))

  The "Hide room" and "Leave Room" confirmation prompts use the "name" key from the room info. When the setting "
  Allow Special Characters in Room Names" is enabled, the prompts show the normalized names instead of those that contain the special characters.

  Changed the value being used from name to fname, which always has the user-set name.

  Previous:
  ![Screenshot from 2021-01-20 15-52-29](https://user-images.githubusercontent.com/38764067/105161642-9b31e780-5b37-11eb-8b0c-ec4b1414c948.png)

  Updated:
  ![Screenshot from 2021-01-20 15-50-19](https://user-images.githubusercontent.com/38764067/105161627-966d3380-5b37-11eb-9812-3dd9352b4f95.png)

- Room's list showing all rooms with same name ([#20176](https://github.com/RocketChat/Rocket.Chat/pull/20176))

  Add a migration to fix the room's list for those who ran version 3.10.1 and got it scrambled when a new user was registered.

- RoomManager validation broken on IE ([#20490](https://github.com/RocketChat/Rocket.Chat/pull/20490))

- Saving with blank email in edit user ([#20259](https://github.com/RocketChat/Rocket.Chat/pull/20259) by [@RonLek](https://github.com/RonLek))

  Disallows showing a success popup when email field is made blank in Edit User and instead shows the relevant error popup.


  https://user-images.githubusercontent.com/28918901/104960749-dbd81680-59fa-11eb-9c7b-2b257936f894.mp4

- Search list filter ([#19937](https://github.com/RocketChat/Rocket.Chat/pull/19937))

- Sidebar palette color broken on IE ([#20457](https://github.com/RocketChat/Rocket.Chat/pull/20457))

  ![image](https://user-images.githubusercontent.com/27704687/106056093-0a29b600-60cd-11eb-8038-eabbc0d8fb03.png)

- Status circle in profile section ([#20016](https://github.com/RocketChat/Rocket.Chat/pull/20016) by [@yash-rajpal](https://github.com/yash-rajpal))

  The Status Circle in status message text input is now centered vertically.

- Tabbar is opened ([#20122](https://github.com/RocketChat/Rocket.Chat/pull/20122))

- Translate keyword for 'Showing results of' in tables ([#20134](https://github.com/RocketChat/Rocket.Chat/pull/20134) by [@Karting06](https://github.com/Karting06))

  Change translation keyword in order to allow the translation of `Showing results %s   - %s of %s` in tables.

- Unable to reset password by Email if upper case character is pr… ([#19643](https://github.com/RocketChat/Rocket.Chat/pull/19643) by [@bhavayAnand9](https://github.com/bhavayAnand9))

- User Audio notification preference not being applied ([#20061](https://github.com/RocketChat/Rocket.Chat/pull/20061))

- User info 'Full Name' translation keyword ([#20028](https://github.com/RocketChat/Rocket.Chat/pull/20028) by [@Karting06](https://github.com/Karting06))

  Fix the `Full Name` translation keyword, so that it can be translated.

- User registration updating wrong subscriptions ([#20128](https://github.com/RocketChat/Rocket.Chat/pull/20128))

- Video call message not translated ([#18722](https://github.com/RocketChat/Rocket.Chat/pull/18722))

  Fixed video call message not translated.

- ViewLogs title translation keyword ([#20029](https://github.com/RocketChat/Rocket.Chat/pull/20029) by [@Karting06](https://github.com/Karting06))

  Fix `View Logs` title translation keyword to enable translation of the title

- White screen after 2FA code entered ([#20225](https://github.com/RocketChat/Rocket.Chat/pull/20225) by [@wggdeveloper](https://github.com/wggdeveloper))

- Wrong userId when open own user profile ([#20181](https://github.com/RocketChat/Rocket.Chat/pull/20181))

<details>
<summary>🔍 Minor changes</summary>


- Add translation of Edit Status in all languages ([#19916](https://github.com/RocketChat/Rocket.Chat/pull/19916) by [@sushant52](https://github.com/sushant52))

  Closes [#19915](https://github.com/RocketChat/Rocket.Chat/issues/19915)
  The profile options menu is well translated in many languages. However, Edit Status is the only button which is not well translated. With this change, the whole profile options will be properly translated in a lot of languages.

- Bump axios from 0.18.0 to 0.18.1 ([#20055](https://github.com/RocketChat/Rocket.Chat/pull/20055) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Chore: Add tests for the api/licenses.* endpoints ([#20041](https://github.com/RocketChat/Rocket.Chat/pull/20041))

  Adding api tests for the new `licenses.*` endpoints (`licenses.get` and `licenses.add`)

- Chore: add tests to api/instances.get endpoint  ([#19988](https://github.com/RocketChat/Rocket.Chat/pull/19988))

- Chore: Change console.warning() to console.warn() ([#20200](https://github.com/RocketChat/Rocket.Chat/pull/20200))

- chore: Change return button ([#20045](https://github.com/RocketChat/Rocket.Chat/pull/20045))

- Chore: Fix i18n duplicated keys ([#19998](https://github.com/RocketChat/Rocket.Chat/pull/19998))

- Chore: Recover and update Storybook ([#20047](https://github.com/RocketChat/Rocket.Chat/pull/20047))

  It reenables Storybook's usage.

- Language update from LingoHub 🤖 on 2020-12-30Z ([#20013](https://github.com/RocketChat/Rocket.Chat/pull/20013))

- Language update from LingoHub 🤖 on 2021-01-04Z ([#20034](https://github.com/RocketChat/Rocket.Chat/pull/20034))

- Language update from LingoHub 🤖 on 2021-01-11Z ([#20146](https://github.com/RocketChat/Rocket.Chat/pull/20146))

- Language update from LingoHub 🤖 on 2021-01-18Z ([#20246](https://github.com/RocketChat/Rocket.Chat/pull/20246))

- Regression: Add tests to new banners REST endpoints ([#20492](https://github.com/RocketChat/Rocket.Chat/pull/20492))

  Add tests for the new `banners.*` endpoints: `banners.getNew` and `banners.dismiss`.

- Regression: Announcement bar not showing properly Markdown content ([#20290](https://github.com/RocketChat/Rocket.Chat/pull/20290))

  **Before**:
  ![image](https://user-images.githubusercontent.com/27704687/105273746-a4907380-5b7a-11eb-8121-aff665251c44.png)

  **After**:
  ![image](https://user-images.githubusercontent.com/27704687/105274050-2e404100-5b7b-11eb-93b2-b6282a7bed95.png)

- regression: Announcement link open in new tab ([#20435](https://github.com/RocketChat/Rocket.Chat/pull/20435))

- Regression: Apps-Engine - Convert streams to buffers on file upload ([#20523](https://github.com/RocketChat/Rocket.Chat/pull/20523))

  This is an implementation to accommodate the changes in API for the `IPreFileUpload` hook in the Apps-Engine. Explanation on the reasoning for it is here https://github.com/RocketChat/Rocket.Chat.Apps-engine/pull/376

- Regression: Attachments ([#20291](https://github.com/RocketChat/Rocket.Chat/pull/20291))

- Regression: Bio page not rendering ([#20450](https://github.com/RocketChat/Rocket.Chat/pull/20450))

- Regression: Change sort icon ([#20177](https://github.com/RocketChat/Rocket.Chat/pull/20177))

  ### Before
  ![image](https://user-images.githubusercontent.com/40830821/104366414-1bcd6400-54f8-11eb-9fc7-c6f13f07a61e.png)

  ### After
  ![image](https://user-images.githubusercontent.com/40830821/104366542-4cad9900-54f8-11eb-83ca-acb99899515a.png)

- Regression: Custom field labels are not displayed properly on Omnichannel Contact Profile form ([#20393](https://github.com/RocketChat/Rocket.Chat/pull/20393))

  ### Before
  ![image](https://user-images.githubusercontent.com/2493803/105780399-20116c80-5f4f-11eb-9620-0901472e453b.png)

  ![image](https://user-images.githubusercontent.com/2493803/105780420-2e5f8880-5f4f-11eb-8e93-8115ebc685be.png)

  ### After

  ![image](https://user-images.githubusercontent.com/2493803/105780832-1ccab080-5f50-11eb-8042-188dd0c41904.png)

  ![image](https://user-images.githubusercontent.com/2493803/105780911-500d3f80-5f50-11eb-96e0-7df3f179dbd5.png)

- Regression: ESLint Warning - explicit-function-return-type ([#20434](https://github.com/RocketChat/Rocket.Chat/pull/20434) by [@aditya-mitra](https://github.com/aditya-mitra))

  Added explicit Return Type (Promise<void>) on the function to fix eslint warning (`explicit-function-return-type`)

- Regression: Fix banners sync data types ([#20517](https://github.com/RocketChat/Rocket.Chat/pull/20517))

- Regression: Fix Cron statistics TypeError ([#20343](https://github.com/RocketChat/Rocket.Chat/pull/20343) by [@RonLek](https://github.com/RonLek))

- Regression: Fix duplicate email messages in multiple instances ([#20495](https://github.com/RocketChat/Rocket.Chat/pull/20495))

- Regression: Fix e2e paused state ([#20511](https://github.com/RocketChat/Rocket.Chat/pull/20511))

- Regression: Fixed update room avatar issue. ([#20433](https://github.com/RocketChat/Rocket.Chat/pull/20433) by [@Darshilp326](https://github.com/Darshilp326))

  Users can now update their room avatar without any error.

  https://user-images.githubusercontent.com/55157259/105951602-560d3880-6096-11eb-97a5-b5eb9a28b58d.mp4

- Regression: Info Page Icon style and usage graph breaking ([#20180](https://github.com/RocketChat/Rocket.Chat/pull/20180))

- Regression: Lint warnings and some datepicker ([#20280](https://github.com/RocketChat/Rocket.Chat/pull/20280))

- Regression: NPS ([#20514](https://github.com/RocketChat/Rocket.Chat/pull/20514))

- Regression: reactAttachments cpu ([#20255](https://github.com/RocketChat/Rocket.Chat/pull/20255))

- Regression: Room not scrolling to bottom ([#20516](https://github.com/RocketChat/Rocket.Chat/pull/20516))

- Regression: Set image sizes based on rotation ([#20531](https://github.com/RocketChat/Rocket.Chat/pull/20531))

- Regression: Unread superposing announcement. ([#20306](https://github.com/RocketChat/Rocket.Chat/pull/20306))

  ### Before
  ![image](https://user-images.githubusercontent.com/40830821/105412619-c2f67d80-5c13-11eb-8204-5932ea880c8a.png)


  ### After
  ![image](https://user-images.githubusercontent.com/40830821/105411176-d1439a00-5c11-11eb-8d1b-ea27c8485214.png)

- Regression: User Dropdown margin ([#20222](https://github.com/RocketChat/Rocket.Chat/pull/20222))

- Rewrite : Message Thread metrics ([#20051](https://github.com/RocketChat/Rocket.Chat/pull/20051))

  ![image](https://user-images.githubusercontent.com/5263975/103585504-e904e980-4ec1-11eb-8d8c-3113ac812ead.png)

- Rewrite Broadcast ([#20119](https://github.com/RocketChat/Rocket.Chat/pull/20119))

  ![image](https://user-images.githubusercontent.com/5263975/104035912-7fcaf200-51b1-11eb-91df-228c23d97448.png)

- Rewrite Discussion Metric ([#20117](https://github.com/RocketChat/Rocket.Chat/pull/20117))

  https://user-images.githubusercontent.com/5263975/104031909-23190880-51ac-11eb-93dd-5d4b5295886d.mp4

- Rewrite Message action links ([#20123](https://github.com/RocketChat/Rocket.Chat/pull/20123))

- Rewrite: Message Attachments ([#20106](https://github.com/RocketChat/Rocket.Chat/pull/20106))

  ![image](https://user-images.githubusercontent.com/5263975/104783709-69023d80-5765-11eb-968f-a2b93fdfb51e.png)

- Security sync ([#20430](https://github.com/RocketChat/Rocket.Chat/pull/20430))

- Update "Industry" setting ([#20510](https://github.com/RocketChat/Rocket.Chat/pull/20510))

- Update Apps-Engine and permissions translations ([#20491](https://github.com/RocketChat/Rocket.Chat/pull/20491))

  Update Apps-Engine version and apply changes in translations for the changed permissions. Please review the texts on the translation files to make sure they're clear.

- Update Apps-Engine version ([#20482](https://github.com/RocketChat/Rocket.Chat/pull/20482))

  Update Apps-Engine version with some fixes for the current RC cycle.

- Update password policy English translation ([#20118](https://github.com/RocketChat/Rocket.Chat/pull/20118) by [@zdumitru](https://github.com/zdumitru))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Darshilp326](https://github.com/Darshilp326)
- [@Karting06](https://github.com/Karting06)
- [@RonLek](https://github.com/RonLek)
- [@aKn1ghtOut](https://github.com/aKn1ghtOut)
- [@aditya-mitra](https://github.com/aditya-mitra)
- [@bdelwood](https://github.com/bdelwood)
- [@bhavayAnand9](https://github.com/bhavayAnand9)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@i-kychukov](https://github.com/i-kychukov)
- [@ikyuchukov](https://github.com/ikyuchukov)
- [@moniang](https://github.com/moniang)
- [@sushant52](https://github.com/sushant52)
- [@tlskinneriv](https://github.com/tlskinneriv)
- [@wggdeveloper](https://github.com/wggdeveloper)
- [@yash-rajpal](https://github.com/yash-rajpal)
- [@zdumitru](https://github.com/zdumitru)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@dougfabris](https://github.com/dougfabris)
- [@gabriellsh](https://github.com/gabriellsh)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@lucassartor](https://github.com/lucassartor)
- [@murtaza98](https://github.com/murtaza98)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@rafaelblink](https://github.com/rafaelblink)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)
- [@thassiov](https://github.com/thassiov)
- [@tiagoevanp](https://github.com/tiagoevanp)

# 3.10.5
`2021-01-27  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🐛 Bug fixes


- Security Hotfix

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.10.4
`2021-01-14  ·  1 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🐛 Bug fixes


- Room's list showing all rooms with same name ([#20176](https://github.com/RocketChat/Rocket.Chat/pull/20176))

  Add a migration to fix the room's list for those who ran version 3.10.1 and got it scrambled when a new user was registered.

<details>
<summary>🔍 Minor changes</summary>


- Chore: Change console.warning() to console.warn() ([#20200](https://github.com/RocketChat/Rocket.Chat/pull/20200))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@lucassartor](https://github.com/lucassartor)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.10.3
`2021-01-09  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🐛 Bug fixes


- User registration updating wrong subscriptions ([#20128](https://github.com/RocketChat/Rocket.Chat/pull/20128))

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.10.2
`2021-01-08  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🐛 Bug fixes


- Tabbar is opened ([#20122](https://github.com/RocketChat/Rocket.Chat/pull/20122))

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)

# 3.10.1
`2021-01-08  ·  11 🐛  ·  7 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🐛 Bug fixes


- **ENTERPRISE:** Omnichannel custom fields not storing additional form values  ([#19953](https://github.com/RocketChat/Rocket.Chat/pull/19953))

- Actions from User Info panel ([#20073](https://github.com/RocketChat/Rocket.Chat/pull/20073) by [@Darshilp326](https://github.com/Darshilp326))

  Users can be removed from channels without any error message.

- Agent information panel not rendering ([#19965](https://github.com/RocketChat/Rocket.Chat/pull/19965))

- Creation of Omnichannel rooms not working correctly through the Apps when the agent parameter is set ([#19997](https://github.com/RocketChat/Rocket.Chat/pull/19997))

- Messages being updated when not required after user changes his profile ([#20114](https://github.com/RocketChat/Rocket.Chat/pull/20114))

- OAuth users being asked to change password on second login ([#20003](https://github.com/RocketChat/Rocket.Chat/pull/20003))

- Omnichannel Agents unable to take new chats in the queue ([#20022](https://github.com/RocketChat/Rocket.Chat/pull/20022))

- Omnichannel Business Hours form is not being rendered ([#20007](https://github.com/RocketChat/Rocket.Chat/pull/20007))

- Omnichannel raw model importing meteor dependency ([#20093](https://github.com/RocketChat/Rocket.Chat/pull/20093))

- Omnichannel rooms breaking after return to queue or forward ([#20089](https://github.com/RocketChat/Rocket.Chat/pull/20089))

- User Audio notification preference not being applied ([#20061](https://github.com/RocketChat/Rocket.Chat/pull/20061))

### 👩‍💻👨‍💻 Contributors 😍

- [@Darshilp326](https://github.com/Darshilp326)

### 👩‍💻👨‍💻 Core Team 🤓

- [@gabriellsh](https://github.com/gabriellsh)
- [@murtaza98](https://github.com/murtaza98)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@rafaelblink](https://github.com/rafaelblink)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.10.0
`2020-12-29  ·  6 🎉  ·  10 🚀  ·  29 🐛  ·  39 🔍  ·  20 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🎉 New features


- Custom scroll ([#19701](https://github.com/RocketChat/Rocket.Chat/pull/19701))

- Omnichannel Contact Center (Directory) ([#19931](https://github.com/RocketChat/Rocket.Chat/pull/19931))

- REST Endpoint `instances.get` ([#19926](https://github.com/RocketChat/Rocket.Chat/pull/19926))

  Returns an array of instances on the cluster.

- REST endpoints to add and retrieve Enterprise licenses ([#19925](https://github.com/RocketChat/Rocket.Chat/pull/19925))

- Update Checker Description ([#19892](https://github.com/RocketChat/Rocket.Chat/pull/19892))

- User preference for audio notifications ([#19924](https://github.com/RocketChat/Rocket.Chat/pull/19924))

  ![image](https://user-images.githubusercontent.com/40830821/102808922-dfe32b00-439f-11eb-9268-6d0cf69dc64c.png)

### 🚀 Improvements


- Removed useEndpointDataExperimental hook usage ([#19496](https://github.com/RocketChat/Rocket.Chat/pull/19496))

- Replace useClipboard ([#19764](https://github.com/RocketChat/Rocket.Chat/pull/19764))

- Replace usePrefersReducedMotion ([#19759](https://github.com/RocketChat/Rocket.Chat/pull/19759))

- Rewrite contextualbar OTR panel ([#19674](https://github.com/RocketChat/Rocket.Chat/pull/19674))

- Rewrite contextualbar RoomMembers - AddUsers as React Component ([#19803](https://github.com/RocketChat/Rocket.Chat/pull/19803))

- Rewrite contextualbar RoomMembers - InviteUsers ([#19694](https://github.com/RocketChat/Rocket.Chat/pull/19694))

- Rewrite contextualbar RoomMembers as React Component  ([#19841](https://github.com/RocketChat/Rocket.Chat/pull/19841))

- Rewrite NotificationPreferences to React component ([#19672](https://github.com/RocketChat/Rocket.Chat/pull/19672))

- Rewrite Room Files as React Component ([#19580](https://github.com/RocketChat/Rocket.Chat/pull/19580))

- Show all screen when printing screen ([#19928](https://github.com/RocketChat/Rocket.Chat/pull/19928))

### 🐛 Bug fixes


- 'Not Allowed' in message auditing ([#19762](https://github.com/RocketChat/Rocket.Chat/pull/19762))

- **ENTERPRISE:** Omnichannel Department form is not correctly storing the list of departments allowed for forwarding ([#19793](https://github.com/RocketChat/Rocket.Chat/pull/19793))

- Add fallback message when show notification content is disabled ([#19516](https://github.com/RocketChat/Rocket.Chat/pull/19516) by [@youssef-md](https://github.com/youssef-md))

- Admin Users screen sorting showing deactivated users in wrong order ([#19898](https://github.com/RocketChat/Rocket.Chat/pull/19898))

- Custom Avatar ([#19805](https://github.com/RocketChat/Rocket.Chat/pull/19805))

- Download my data with file uploads ([#19862](https://github.com/RocketChat/Rocket.Chat/pull/19862))

- Emails not showing up in Admin/Users ([#19727](https://github.com/RocketChat/Rocket.Chat/pull/19727))

- File Tab Order ([#19729](https://github.com/RocketChat/Rocket.Chat/pull/19729))

- Forgot password endpoint return status ([#19842](https://github.com/RocketChat/Rocket.Chat/pull/19842))

- Group DMs title when user changes his/her name ([#19834](https://github.com/RocketChat/Rocket.Chat/pull/19834))

- Hightlights validation on Account Preferences page ([#19902](https://github.com/RocketChat/Rocket.Chat/pull/19902) by [@aKn1ghtOut](https://github.com/aKn1ghtOut))

  This PR fixes two issues in the account settings "preferences" panel.
  Once set, the "Highlighted Words" setting cannot be reset to an empty string. This was fixed by changing the string validation from checking the length to checking the type of variable.
  Secondly, it tracks the changes to correctly identify if changes after the last "save changes" action have been made, using an "updates" state variable, instead of just comparing against the initialValue that does not change on clicking "save changes".

- Image preview for image URLs on messages ([#19734](https://github.com/RocketChat/Rocket.Chat/pull/19734))

- Issue with oembed ([#19923](https://github.com/RocketChat/Rocket.Chat/pull/19923))

- Issue with oembed ([#19886](https://github.com/RocketChat/Rocket.Chat/pull/19886))

- Issue with special message rendering ([#19817](https://github.com/RocketChat/Rocket.Chat/pull/19817))

- Omnichannel Departments Canned Responses ([#19830](https://github.com/RocketChat/Rocket.Chat/pull/19830))

- Problem with attachment render ([#19854](https://github.com/RocketChat/Rocket.Chat/pull/19854))

- Room scrolling to top after returns to a opened room ([#19945](https://github.com/RocketChat/Rocket.Chat/pull/19945))

- RoomForeword ([#19875](https://github.com/RocketChat/Rocket.Chat/pull/19875))

- Sidebar presence will now correctly update for Omnichannel rooms ([#19746](https://github.com/RocketChat/Rocket.Chat/pull/19746))

- Sidebar UI disappearing ([#19725](https://github.com/RocketChat/Rocket.Chat/pull/19725))

- Some apps were not correctly enabled during startup in HA environments ([#19763](https://github.com/RocketChat/Rocket.Chat/pull/19763))

- Spotify oEmbed ([#19825](https://github.com/RocketChat/Rocket.Chat/pull/19825))

- Startup error when using MongoDB with a password containing special characters ([#19749](https://github.com/RocketChat/Rocket.Chat/pull/19749))

- Status on searchlist ([#19935](https://github.com/RocketChat/Rocket.Chat/pull/19935))

- UIKit Modal not scrolling ([#19690](https://github.com/RocketChat/Rocket.Chat/pull/19690))

- Update base image in Dockerfile.rhel ([#19036](https://github.com/RocketChat/Rocket.Chat/pull/19036) by [@andykrohg](https://github.com/andykrohg))

- User email showing [object Object] ([#19870](https://github.com/RocketChat/Rocket.Chat/pull/19870))

- User Info 'Local Time' translation keyword ([#19879](https://github.com/RocketChat/Rocket.Chat/pull/19879) by [@J4r3tt](https://github.com/J4r3tt))

<details>
<summary>🔍 Minor changes</summary>


- bump fuselage ([#19736](https://github.com/RocketChat/Rocket.Chat/pull/19736))

- Bump ini from 1.3.5 to 1.3.8 in /ee/server/services ([#19844](https://github.com/RocketChat/Rocket.Chat/pull/19844) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Bump systeminformation from 4.30.1 to 4.33.0 in /ee/server/services ([#19929](https://github.com/RocketChat/Rocket.Chat/pull/19929) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Chore:  Fix Caddy download URL in Snaps ([#19912](https://github.com/RocketChat/Rocket.Chat/pull/19912))

- Chore: Add watch.settings to events whitelist ([#19850](https://github.com/RocketChat/Rocket.Chat/pull/19850))

- Chore: Change Youtube test to verify if has an iframe with max-width ([#19863](https://github.com/RocketChat/Rocket.Chat/pull/19863))

- Chore: Remove extra parentheses from return type ([#19598](https://github.com/RocketChat/Rocket.Chat/pull/19598) by [@ArnoSaine](https://github.com/ArnoSaine))

- Chore: Update Pull Request template ([#19768](https://github.com/RocketChat/Rocket.Chat/pull/19768))

  Improve the template of Pull Requests in order to make it clear reducing duplicated information and removing the visible checklists that were generating noise and misunderstanding with the PR progress.

  - Moved the checklists to inside comments

  - Merge the changelog and proposed changes sections to have a single source of description that goes to the changelog

  - Remove the screenshot section, they can be added inside the description

  - Changed the proposed changes title to incentivizing the usage of images and videos

- Frontend folder structure ([#19631](https://github.com/RocketChat/Rocket.Chat/pull/19631))

- Improve Docker container size by adding chown to ADD command ([#19796](https://github.com/RocketChat/Rocket.Chat/pull/19796))

- Improve: Report Weekly Active Users to statistics ([#19843](https://github.com/RocketChat/Rocket.Chat/pull/19843))

  Add the fields `uniqueUsersOfLastWeek`, `uniqueDevicesOfLastWeek` and `uniqueOSOfLastWeek` to the statistics report among the daily and monthly already reported.

- Language update from LingoHub 🤖 on 2020-12-21Z ([#19922](https://github.com/RocketChat/Rocket.Chat/pull/19922))

- Merge EE and Community translations and LingoHub manual sync ([#19723](https://github.com/RocketChat/Rocket.Chat/pull/19723))

- Merge master into develop & Set version to 3.10.0-develop ([#19720](https://github.com/RocketChat/Rocket.Chat/pull/19720))

- Message parsing and rendering - Phase 1 ([#19654](https://github.com/RocketChat/Rocket.Chat/pull/19654))

- Regression:  "My Account" page doesn't load ([#19753](https://github.com/RocketChat/Rocket.Chat/pull/19753))

- Regression: Add currently running instance to instances.get endpoint ([#19955](https://github.com/RocketChat/Rocket.Chat/pull/19955))

- Regression: Add Members showing the wrong template ([#19748](https://github.com/RocketChat/Rocket.Chat/pull/19748))

- Regression: Add missing translations on the Omnichannel Contact Center(Directory) ([#19968](https://github.com/RocketChat/Rocket.Chat/pull/19968))

- Regression: Admin Sidebar Scroll ([#19944](https://github.com/RocketChat/Rocket.Chat/pull/19944))

- Regression: Check permissions properly when fetching rooms in Omnichannel Directory ([#19951](https://github.com/RocketChat/Rocket.Chat/pull/19951))

- Regression: contextualBar folder structure ([#19761](https://github.com/RocketChat/Rocket.Chat/pull/19761))

- Regression: Double Scrollbars on tables ([#19980](https://github.com/RocketChat/Rocket.Chat/pull/19980))

  Before:
  ![image](https://user-images.githubusercontent.com/40830821/103242719-0ec84680-4936-11eb-87a7-68b6eea8de7b.png)


  After:
  ![image](https://user-images.githubusercontent.com/40830821/103242680-ee988780-4935-11eb-99e2-a95de99f78f1.png)

- Regression: Failed autolinker and markdown rendering ([#19831](https://github.com/RocketChat/Rocket.Chat/pull/19831))

- Regression: fix broken members list ([#19806](https://github.com/RocketChat/Rocket.Chat/pull/19806))

- Regression: Fix member list Actions ([#19876](https://github.com/RocketChat/Rocket.Chat/pull/19876))

- Regression: Fix oembed ([#19978](https://github.com/RocketChat/Rocket.Chat/pull/19978))

- Regression: Fix Room Files for DMs ([#19874](https://github.com/RocketChat/Rocket.Chat/pull/19874))

- Regression: Fix sorting indicators on Admin Users page ([#19950](https://github.com/RocketChat/Rocket.Chat/pull/19950))

- Regression: Header Styles fixes ([#19946](https://github.com/RocketChat/Rocket.Chat/pull/19946))

- Regression: Omnichannel Custom Fields Form no longer working after refactoring ([#19948](https://github.com/RocketChat/Rocket.Chat/pull/19948))

  The Omnichannel `Custom Fields` form is not working anymore after some refactorings on client-side.
  When the user clicks on `Custom Field` in the Omnichannel menu, a blank page appears.

- Regression: polishing licenses endpoints  ([#19981](https://github.com/RocketChat/Rocket.Chat/pull/19981))

- Regression: roomInfo folder structure ([#19787](https://github.com/RocketChat/Rocket.Chat/pull/19787))

- Regression: RoomMembers Permission ([#19867](https://github.com/RocketChat/Rocket.Chat/pull/19867))

- Regression: User Info Context bar breaking. ([#19807](https://github.com/RocketChat/Rocket.Chat/pull/19807))

- Regression: UserCard "See full profile" link broken ([#19941](https://github.com/RocketChat/Rocket.Chat/pull/19941))

- Regression: UserInfoWithData endpoint variable ([#19816](https://github.com/RocketChat/Rocket.Chat/pull/19816))

- Remove Heroku from readme ([#19901](https://github.com/RocketChat/Rocket.Chat/pull/19901))

- Rewrite: Room Header ([#19808](https://github.com/RocketChat/Rocket.Chat/pull/19808))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@ArnoSaine](https://github.com/ArnoSaine)
- [@J4r3tt](https://github.com/J4r3tt)
- [@aKn1ghtOut](https://github.com/aKn1ghtOut)
- [@andykrohg](https://github.com/andykrohg)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@youssef-md](https://github.com/youssef-md)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@alansikora](https://github.com/alansikora)
- [@dougfabris](https://github.com/dougfabris)
- [@g-thome](https://github.com/g-thome)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@rafaelblink](https://github.com/rafaelblink)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)
- [@thassiov](https://github.com/thassiov)
- [@tiagoevanp](https://github.com/tiagoevanp)

# 3.9.4
`2020-12-31  ·  3 🐛  ·  1 🔍  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🐛 Bug fixes


- Omnichannel Departments Canned Responses ([#19830](https://github.com/RocketChat/Rocket.Chat/pull/19830))

- Room scrolling to top after returns to a opened room ([#19945](https://github.com/RocketChat/Rocket.Chat/pull/19945))

- Status on searchlist ([#19935](https://github.com/RocketChat/Rocket.Chat/pull/19935))

<details>
<summary>🔍 Minor changes</summary>


- Regression: Fix oembed ([#19978](https://github.com/RocketChat/Rocket.Chat/pull/19978))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@dougfabris](https://github.com/dougfabris)
- [@gabriellsh](https://github.com/gabriellsh)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 3.9.3
`2020-12-18  ·  2 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🐛 Bug fixes


- Issue with special message rendering ([#19817](https://github.com/RocketChat/Rocket.Chat/pull/19817))

- Problem with attachment render ([#19854](https://github.com/RocketChat/Rocket.Chat/pull/19854))

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)

# 3.9.2
`2020-12-17  ·  5 🐛  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.21.0-alpha.4235`

### 🐛 Bug fixes


- 'Not Allowed' in message auditing ([#19762](https://github.com/RocketChat/Rocket.Chat/pull/19762))

- **ENTERPRISE:** Omnichannel Department form is not correctly storing the list of departments allowed for forwarding ([#19793](https://github.com/RocketChat/Rocket.Chat/pull/19793))

- Download my data with file uploads ([#19862](https://github.com/RocketChat/Rocket.Chat/pull/19862))

- Forgot password endpoint return status ([#19842](https://github.com/RocketChat/Rocket.Chat/pull/19842))

- Some apps were not correctly enabled during startup in HA environments ([#19763](https://github.com/RocketChat/Rocket.Chat/pull/19763))

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@g-thome](https://github.com/g-thome)
- [@rafaelblink](https://github.com/rafaelblink)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@thassiov](https://github.com/thassiov)

# 3.9.1
`2020-12-05  ·  5 🐛  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.20.0`

### 🐛 Bug fixes


- Exception on certain login cases including SAML

- Image preview for image URLs on messages ([#19734](https://github.com/RocketChat/Rocket.Chat/pull/19734))

- Sidebar presence will now correctly update for Omnichannel rooms ([#19746](https://github.com/RocketChat/Rocket.Chat/pull/19746))

- Sidebar UI disappearing ([#19725](https://github.com/RocketChat/Rocket.Chat/pull/19725))

- Startup error when using MongoDB with a password containing special characters ([#19749](https://github.com/RocketChat/Rocket.Chat/pull/19749))

### 👩‍💻👨‍💻 Core Team 🤓

- [@alansikora](https://github.com/alansikora)
- [@g-thome](https://github.com/g-thome)
- [@gabriellsh](https://github.com/gabriellsh)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.9.0
`2020-11-28  ·  2 🎉  ·  16 🚀  ·  27 🐛  ·  31 🔍  ·  21 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.20.0`

### 🎉 New features


- 2 Factor Authentication when using OAuth and SAML ([#11726](https://github.com/RocketChat/Rocket.Chat/pull/11726) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added setting to disable password changes for users who log in using SSO ([#10391](https://github.com/RocketChat/Rocket.Chat/pull/10391) by [@Hudell](https://github.com/Hudell))

### 🚀 Improvements


- **ENTERPRISE:** UI/UX enhancements in Omnichannel Monitors page ([#19495](https://github.com/RocketChat/Rocket.Chat/pull/19495))

- **ENTERPRISE:** UI/UX enhancements in Omnichannel Priorities page ([#19512](https://github.com/RocketChat/Rocket.Chat/pull/19512))

- **ENTERPRISE:** UI/UX enhancements in Omnichannel Tags page ([#19510](https://github.com/RocketChat/Rocket.Chat/pull/19510))

- **ENTERPRISE:** UI/UX enhancements in Omnichannel Units page ([#19500](https://github.com/RocketChat/Rocket.Chat/pull/19500))

- Add support to `replace` operation when using Change Stream ([#19579](https://github.com/RocketChat/Rocket.Chat/pull/19579))

- Bundle Size Client ([#19533](https://github.com/RocketChat/Rocket.Chat/pull/19533))

  temporarily removes some codeblock languages
  Moved some libraries to dynamic imports
  Removed some shared code not used on the client side

- Forward Omnichannel room to agent in another department ([#19576](https://github.com/RocketChat/Rocket.Chat/pull/19576) by [@mrfigueiredo](https://github.com/mrfigueiredo))

- KeyboardShortcuts as React component ([#19518](https://github.com/RocketChat/Rocket.Chat/pull/19518))

- Remove Box dependence from Tag and Badge components ([#19467](https://github.com/RocketChat/Rocket.Chat/pull/19467))

- Remove Box props from Avatar component ([#19491](https://github.com/RocketChat/Rocket.Chat/pull/19491))

- Rewrite Auto-Translate as a React component  ([#19633](https://github.com/RocketChat/Rocket.Chat/pull/19633))

- Rewrite Room Info ([#19511](https://github.com/RocketChat/Rocket.Chat/pull/19511))

- SlackBridge threads performance improvement ([#19338](https://github.com/RocketChat/Rocket.Chat/pull/19338) by [@antkaz](https://github.com/antkaz))

- UI/UX enhancements in department pages following the design system ([#19421](https://github.com/RocketChat/Rocket.Chat/pull/19421))

- UI/UX enhancements in Omnichannel Triggers page ([#19485](https://github.com/RocketChat/Rocket.Chat/pull/19485))

- UI/UX enhancements in Omnichannnel Current Chats page ([#19397](https://github.com/RocketChat/Rocket.Chat/pull/19397))

### 🐛 Bug fixes


- Allow username change if LDAP is enabled but their username is not linked to an LDAP field ([#19381](https://github.com/RocketChat/Rocket.Chat/pull/19381) by [@robertfromont](https://github.com/robertfromont))

  LDAP users can change their username if the LDAP_Username_Field setting is blank.

- Auto Translate ([#19599](https://github.com/RocketChat/Rocket.Chat/pull/19599))

- Channel actions not working when reduce motion is active ([#19638](https://github.com/RocketChat/Rocket.Chat/pull/19638))

- Column width was not following the design system in Omnichannel Departments page ([#19601](https://github.com/RocketChat/Rocket.Chat/pull/19601))

- Engagement dashboard on old Mongo versions ([#19616](https://github.com/RocketChat/Rocket.Chat/pull/19616))

- Engagement dashboard: graphs adjustment ([#19450](https://github.com/RocketChat/Rocket.Chat/pull/19450))

- IE11 - Update ui kit and fuselage bundle ([#19561](https://github.com/RocketChat/Rocket.Chat/pull/19561))

- Input without label and email ordering missing on Omnichannel Agents page ([#19414](https://github.com/RocketChat/Rocket.Chat/pull/19414))

- Issue with drag and drop ([#19593](https://github.com/RocketChat/Rocket.Chat/pull/19593))

- LDAP Unique Identifier Field can not use operational attributes ([#19571](https://github.com/RocketChat/Rocket.Chat/pull/19571) by [@truongtx8](https://github.com/truongtx8))

- Omnichannel Analytics page doesn't have field labels ([#19400](https://github.com/RocketChat/Rocket.Chat/pull/19400))

- Outgoing integrations without trigger words or with multiple commas ([#19488](https://github.com/RocketChat/Rocket.Chat/pull/19488))

- Prevent headerRoom's click to open room/direct info ([#19596](https://github.com/RocketChat/Rocket.Chat/pull/19596))

- Regex was not working properly on visitors.search endpoint ([#19577](https://github.com/RocketChat/Rocket.Chat/pull/19577))

- Restore Message View Mode Preference ([#19458](https://github.com/RocketChat/Rocket.Chat/pull/19458))

  [FIX] Restore Message View Mode Preference

- Role description not updating ([#19236](https://github.com/RocketChat/Rocket.Chat/pull/19236))

- Save button enabled by default in Omnichannel Business Hours Form ([#19493](https://github.com/RocketChat/Rocket.Chat/pull/19493))

- Settings may not update internal cache immediately ([#19628](https://github.com/RocketChat/Rocket.Chat/pull/19628))

- Setup Wizard User Creation Locking up ([#19509](https://github.com/RocketChat/Rocket.Chat/pull/19509))

  [FIX] Setup Wizard User Creation Locking up

- Size of embed Youtube on threads for small screens ([#19514](https://github.com/RocketChat/Rocket.Chat/pull/19514))

- The width of list columns was not following the design system in Omnichannel Agents page ([#19625](https://github.com/RocketChat/Rocket.Chat/pull/19625))

- The width of list columns was not following the design system in Omnichannel Managers page ([#19624](https://github.com/RocketChat/Rocket.Chat/pull/19624))

- TOTP Being ignored when changing our own avatar ([#19475](https://github.com/RocketChat/Rocket.Chat/pull/19475))

  [FIX] TOTP Being ignored when changing our own avatar

- Typo in custom oauth from environment variable ([#19570](https://github.com/RocketChat/Rocket.Chat/pull/19570))

- UI/UX issues on Omnichannel Managers page ([#19410](https://github.com/RocketChat/Rocket.Chat/pull/19410))

- Unread count for all messages when mentioning an user ([#16884](https://github.com/RocketChat/Rocket.Chat/pull/16884) by [@subham103](https://github.com/subham103))

- Wrong margin of description field in Omnichannel Webhooks page ([#19487](https://github.com/RocketChat/Rocket.Chat/pull/19487))

<details>
<summary>🔍 Minor changes</summary>


- [IMPROVES] Omnichannel - Custom Fields pages. ([#19473](https://github.com/RocketChat/Rocket.Chat/pull/19473))

- Bump bcrypt from 4.0.1 to 5.0.0 in /ee/server/services ([#19387](https://github.com/RocketChat/Rocket.Chat/pull/19387) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Bump systeminformation from 4.27.3 to 4.30.1 in /ee/server/services ([#19543](https://github.com/RocketChat/Rocket.Chat/pull/19543) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Bump xml-crypto from 1.5.3 to 2.0.0 ([#19383](https://github.com/RocketChat/Rocket.Chat/pull/19383) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- chore: Debounce sidebar list ([#19590](https://github.com/RocketChat/Rocket.Chat/pull/19590))

- Fix Docker preview image build ([#19627](https://github.com/RocketChat/Rocket.Chat/pull/19627))

- Fix permission duplicated error on startup causing CI to halt ([#19653](https://github.com/RocketChat/Rocket.Chat/pull/19653))

- Improve performance of migration 211 (adding mostImportantRole to sessions) ([#19700](https://github.com/RocketChat/Rocket.Chat/pull/19700))

- Improve REST endpoint to log user out from other clients ([#19642](https://github.com/RocketChat/Rocket.Chat/pull/19642))

- LingoHub based on develop ([#19592](https://github.com/RocketChat/Rocket.Chat/pull/19592))

- LingoHub based on develop ([#19131](https://github.com/RocketChat/Rocket.Chat/pull/19131))

- Manual LingoHub update ([#19620](https://github.com/RocketChat/Rocket.Chat/pull/19620))

- Merge master into develop & Set version to 3.9.0-develop ([#19534](https://github.com/RocketChat/Rocket.Chat/pull/19534))

- React Room Container ([#19634](https://github.com/RocketChat/Rocket.Chat/pull/19634))

- Regression: Collapsed messages container in safari ([#19668](https://github.com/RocketChat/Rocket.Chat/pull/19668))

- Regression: Fix Avatar x40 ([#19564](https://github.com/RocketChat/Rocket.Chat/pull/19564))

- Regression: Fix Custom OAuth 2FA ([#19691](https://github.com/RocketChat/Rocket.Chat/pull/19691))

- Regression: Fix LDAP 2FA not working when Login Fallback is off ([#19659](https://github.com/RocketChat/Rocket.Chat/pull/19659))

- Regression: Fix multiple react blazed template rendering at the same time ([#19679](https://github.com/RocketChat/Rocket.Chat/pull/19679))

- Regression: Fix wrong template on photoswipe ([#19575](https://github.com/RocketChat/Rocket.Chat/pull/19575))

- Regression: Issues with Safari ([#19671](https://github.com/RocketChat/Rocket.Chat/pull/19671))

- Regression: object-fit for image element and Box margin in AppAvatar component ([#19698](https://github.com/RocketChat/Rocket.Chat/pull/19698))

- REGRESSION: Photoswipe not working  ([#19569](https://github.com/RocketChat/Rocket.Chat/pull/19569))

- Regression: Room Info Edit action ([#19581](https://github.com/RocketChat/Rocket.Chat/pull/19581))

- Regression: Room Info maxAgeDefault variable ([#19582](https://github.com/RocketChat/Rocket.Chat/pull/19582))

- Regression: URL preview problem ([#19685](https://github.com/RocketChat/Rocket.Chat/pull/19685))

- Regression: Verticalbar size ([#19670](https://github.com/RocketChat/Rocket.Chat/pull/19670))

- Release 3.8.2 ([#19705](https://github.com/RocketChat/Rocket.Chat/pull/19705))

- Report DAU and MAU by role ([#19657](https://github.com/RocketChat/Rocket.Chat/pull/19657))

- Update Apps-Engine version ([#19639](https://github.com/RocketChat/Rocket.Chat/pull/19639))

- Update Apps-Engine version ([#19702](https://github.com/RocketChat/Rocket.Chat/pull/19702))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@antkaz](https://github.com/antkaz)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@mrfigueiredo](https://github.com/mrfigueiredo)
- [@robertfromont](https://github.com/robertfromont)
- [@subham103](https://github.com/subham103)
- [@truongtx8](https://github.com/truongtx8)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@dougfabris](https://github.com/dougfabris)
- [@g-thome](https://github.com/g-thome)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@rafaelblink](https://github.com/rafaelblink)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tiagoevanp](https://github.com/tiagoevanp)

# 3.8.5
`2020-12-31  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.19.0`

<details>
<summary>🔍 Minor changes</summary>


- Regression: Fix oembed ([#19978](https://github.com/RocketChat/Rocket.Chat/pull/19978))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.8.4
`2020-12-18  ·  2 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.19.0`

### 🐛 Bug fixes


- Issue with special message rendering ([#19817](https://github.com/RocketChat/Rocket.Chat/pull/19817))

- Problem with attachment render ([#19854](https://github.com/RocketChat/Rocket.Chat/pull/19854))

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)

# 3.8.3
`2020-12-05  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.19.0`

### 🐛 Bug fixes


- Exception on certain login cases including SAML

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.8.2
`2020-11-27  ·  2 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.19.0`

### 🐛 Bug fixes


- Room avatar update event doesn't properly broadcast room id ([#19684](https://github.com/RocketChat/Rocket.Chat/pull/19684))

- Server crash while reading settings for allowed and blocked email domain lists ([#19683](https://github.com/RocketChat/Rocket.Chat/pull/19683))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.8.2 ([#19705](https://github.com/RocketChat/Rocket.Chat/pull/19705))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@g-thome](https://github.com/g-thome)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.8.1
`2020-11-19  ·  3 🐛  ·  1 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.19.0`

### 🐛 Bug fixes


- Engagement dashboard on old Mongo versions ([#19616](https://github.com/RocketChat/Rocket.Chat/pull/19616))

- IE11 - Update ui kit and fuselage bundle ([#19561](https://github.com/RocketChat/Rocket.Chat/pull/19561))

- Typo in custom oauth from environment variable ([#19570](https://github.com/RocketChat/Rocket.Chat/pull/19570))

<details>
<summary>🔍 Minor changes</summary>


- Fix Docker preview image build ([#19627](https://github.com/RocketChat/Rocket.Chat/pull/19627))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.8.0
`2020-11-14  ·  14 🎉  ·  4 🚀  ·  40 🐛  ·  54 🔍  ·  30 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.19.0`

### 🎉 New features


- **Apps:** Add new typing bridge method (Typing-Indicator) ([#19228](https://github.com/RocketChat/Rocket.Chat/pull/19228))

- **APPS:** New Scheduler API ([#19290](https://github.com/RocketChat/Rocket.Chat/pull/19290))

- **Apps:** Remove TS compiler ([#18687](https://github.com/RocketChat/Rocket.Chat/pull/18687))

- **Enterprise:** Micro services ([#19000](https://github.com/RocketChat/Rocket.Chat/pull/19000))

- Add enterprise data to statistics ([#19363](https://github.com/RocketChat/Rocket.Chat/pull/19363))

- Admin option to reset users’ 2FA ([#19341](https://github.com/RocketChat/Rocket.Chat/pull/19341))

  Admins can reset the 2FA of other users if they have the permission `edit-other-user-totp` and the `Accounts > Two Factor Authentication > Enforce password fallback` setting is enabled.

- Apps prometheus metrics ([#19320](https://github.com/RocketChat/Rocket.Chat/pull/19320))

- Audits search by User ([#19275](https://github.com/RocketChat/Rocket.Chat/pull/19275))

- Branding updated with new logos ([#19440](https://github.com/RocketChat/Rocket.Chat/pull/19440))

- feat(CAS): Adding option to enable/disable user creation from CAS auth ([#17154](https://github.com/RocketChat/Rocket.Chat/pull/17154) by [@jgribonvald](https://github.com/jgribonvald))

- OAuth groups to channels mapping ([#18146](https://github.com/RocketChat/Rocket.Chat/pull/18146) by [@arminfelder](https://github.com/arminfelder))

- Reaction view ([#18272](https://github.com/RocketChat/Rocket.Chat/pull/18272))

- Replace client-side event emitters ([#19368](https://github.com/RocketChat/Rocket.Chat/pull/19368))

- Whitelisting bad words ([#17120](https://github.com/RocketChat/Rocket.Chat/pull/17120) by [@aryankoul](https://github.com/aryankoul))

### 🚀 Improvements


- **APPS:** Apps list page on servers without internet connection ([#19088](https://github.com/RocketChat/Rocket.Chat/pull/19088))

- Display channel avatar on the Header ([#19132](https://github.com/RocketChat/Rocket.Chat/pull/19132) by [@ba-9](https://github.com/ba-9) & [@bhavayAnand9](https://github.com/bhavayAnand9))

- New sidebar layout ([#19089](https://github.com/RocketChat/Rocket.Chat/pull/19089))

- React Avatar Provider ([#19321](https://github.com/RocketChat/Rocket.Chat/pull/19321))

### 🐛 Bug fixes


- "Export Messages" only works for global roles  ([#19264](https://github.com/RocketChat/Rocket.Chat/pull/19264))

- **ENTERPRISE:** Race condition on Omnichannel queues ([#19352](https://github.com/RocketChat/Rocket.Chat/pull/19352))

- 2FA required rendering blank page ([#19364](https://github.com/RocketChat/Rocket.Chat/pull/19364))

- Adding missing custom fields translation in my account's profile ([#19179](https://github.com/RocketChat/Rocket.Chat/pull/19179))

- Admin not working on IE11 ([#19348](https://github.com/RocketChat/Rocket.Chat/pull/19348))

- Admin Sidebar overflowing ([#19101](https://github.com/RocketChat/Rocket.Chat/pull/19101))

- Agent status offline and wrong i18n key ([#19199](https://github.com/RocketChat/Rocket.Chat/pull/19199))

- Anonymous users are counted on the server statistics and engagement dashboard ([#19263](https://github.com/RocketChat/Rocket.Chat/pull/19263))

- Broken user info when a user don't have an email address ([#19339](https://github.com/RocketChat/Rocket.Chat/pull/19339))

- Channel creation not working on IE ([#19524](https://github.com/RocketChat/Rocket.Chat/pull/19524))

- Cloud Register Allowing Empty Tokens ([#19501](https://github.com/RocketChat/Rocket.Chat/pull/19501))

- Custom Emojis PNGs on IE11 ([#19519](https://github.com/RocketChat/Rocket.Chat/pull/19519))

- Don't send room name on notification ([#19247](https://github.com/RocketChat/Rocket.Chat/pull/19247))

- Error preventing from removing users without a role ([#19204](https://github.com/RocketChat/Rocket.Chat/pull/19204) by [@RohitKumar-200](https://github.com/RohitKumar-200))

- Error when editing priority and required description ([#19170](https://github.com/RocketChat/Rocket.Chat/pull/19170))

- Integrations history page not reacting to changes. ([#19114](https://github.com/RocketChat/Rocket.Chat/pull/19114))

- Invalid attachments on User Data downloads ([#19203](https://github.com/RocketChat/Rocket.Chat/pull/19203))

- IRC Bridge not working ([#19009](https://github.com/RocketChat/Rocket.Chat/pull/19009))

- LDAP Sync Error Dup Key ([#19337](https://github.com/RocketChat/Rocket.Chat/pull/19337))

- Livechat Appearance label and reset button ([#19171](https://github.com/RocketChat/Rocket.Chat/pull/19171))

- Message actions on top of text ([#19316](https://github.com/RocketChat/Rocket.Chat/pull/19316))

- Missing "Bio" in user's profile view (#18821) ([#19166](https://github.com/RocketChat/Rocket.Chat/pull/19166))

- Non admin cannot add custom avatar to group ([#18960](https://github.com/RocketChat/Rocket.Chat/pull/18960) by [@FelipeParreira](https://github.com/FelipeParreira))

  Allow non-admins to change room avatar.

- OAuth create via environment variable ([#19472](https://github.com/RocketChat/Rocket.Chat/pull/19472))

- Omnichannel - typo error label at current chats page ([#19379](https://github.com/RocketChat/Rocket.Chat/pull/19379))

- Omnichannel auditing required field ([#19201](https://github.com/RocketChat/Rocket.Chat/pull/19201))

- Omnichannel: triggers page not rendering. ([#19134](https://github.com/RocketChat/Rocket.Chat/pull/19134))

- Performance issues when using new Oplog implementation ([#19181](https://github.com/RocketChat/Rocket.Chat/pull/19181))

  A missing configuration was not limiting the new oplog tailing to pool the database frequently even when no data was available, leading to both node and mongodb process been consuming high CPU even with low usage. This case was happening for installations using `mmapv1` database engine or when no admin access was granted to the database user, both preventing the usage of the new [Change Streams](https://docs.mongodb.com/manual/changeStreams/) implementation and fallbacking to our custom oplog implementation in replacement to the Meteor's one what was able to be disabled and use the native implementation via the environmental variable `USE_NATIVE_OPLOG=true`.

- Push notifications with lower priority for Android devices ([#19061](https://github.com/RocketChat/Rocket.Chat/pull/19061) by [@ceefour](https://github.com/ceefour))

  fix(push): Set push notification priority to 'high' for FCM

- Remove requirements to tag description and department ([#19169](https://github.com/RocketChat/Rocket.Chat/pull/19169))

- SAML login undefined error message ([#18649](https://github.com/RocketChat/Rocket.Chat/pull/18649) by [@galshiff](https://github.com/galshiff))

  Fixed the SAML login undefined error message

- Selecting the same department for multiple units ([#19168](https://github.com/RocketChat/Rocket.Chat/pull/19168))

- Server Errors on new Client Connections ([#19266](https://github.com/RocketChat/Rocket.Chat/pull/19266))

- Setting values being showed up in logs when using log level for debug ([#18239](https://github.com/RocketChat/Rocket.Chat/pull/18239))

- Thread List showing wrong items ([#19351](https://github.com/RocketChat/Rocket.Chat/pull/19351))

- Thread view in a channel user haven't joined (#19008)  ([#19172](https://github.com/RocketChat/Rocket.Chat/pull/19172))

- Use etag on user info ([#19349](https://github.com/RocketChat/Rocket.Chat/pull/19349))

- UserCard Roles Description ([#19200](https://github.com/RocketChat/Rocket.Chat/pull/19200))

- VisitorAutoComplete component ([#19133](https://github.com/RocketChat/Rocket.Chat/pull/19133))

- Wrong avatar urls when using providers ([#18929](https://github.com/RocketChat/Rocket.Chat/pull/18929))

<details>
<summary>🔍 Minor changes</summary>


- Build micro services Docker images with correct tags ([#19418](https://github.com/RocketChat/Rocket.Chat/pull/19418))

- Bump Livechat widget ([#19361](https://github.com/RocketChat/Rocket.Chat/pull/19361))

- Bump Livechat widget  ([#19478](https://github.com/RocketChat/Rocket.Chat/pull/19478))

- Bump object-path from 0.11.4 to 0.11.5 ([#19298](https://github.com/RocketChat/Rocket.Chat/pull/19298) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Fix Indie Hosters install image ([#19192](https://github.com/RocketChat/Rocket.Chat/pull/19192) by [@aradhya-gupta](https://github.com/aradhya-gupta))

- Merge master into develop & Set version to 3.8.0-develop ([#19060](https://github.com/RocketChat/Rocket.Chat/pull/19060))

- Micro Services: Add metrics capability to Services ([#19448](https://github.com/RocketChat/Rocket.Chat/pull/19448))

- Micro Services: Create internal services and allowed services list ([#19427](https://github.com/RocketChat/Rocket.Chat/pull/19427))

- Micro Services: Do not wait forever for a service. Fail after 10s or 10 minutes if whitelisted ([#19484](https://github.com/RocketChat/Rocket.Chat/pull/19484))

- Micro Services: Fix logout issue ([#19423](https://github.com/RocketChat/Rocket.Chat/pull/19423))

- Micro Services: Prevent duplicated events ([#19435](https://github.com/RocketChat/Rocket.Chat/pull/19435))

- Non-idiomatic React code ([#19303](https://github.com/RocketChat/Rocket.Chat/pull/19303))

- Reassessment of client helpers ([#19249](https://github.com/RocketChat/Rocket.Chat/pull/19249))

- Refactor some React Pages and Components  ([#19202](https://github.com/RocketChat/Rocket.Chat/pull/19202))

- Refactor: Omnichannel departments ([#18920](https://github.com/RocketChat/Rocket.Chat/pull/18920))

- Regression: `Leave Room` modal not closing ([#19460](https://github.com/RocketChat/Rocket.Chat/pull/19460))

- Regression: Agent Status leading to broken page ([#19409](https://github.com/RocketChat/Rocket.Chat/pull/19409))

- Regression: Allow apps to schedule jobs along with processor register ([#19416](https://github.com/RocketChat/Rocket.Chat/pull/19416))

- Regression: Attachment without title or description show "sent attachment" in view mode extended  ([#19443](https://github.com/RocketChat/Rocket.Chat/pull/19443))

- Regression: Fix broadcast events when running as monolith ([#19498](https://github.com/RocketChat/Rocket.Chat/pull/19498))

- Regression: Fix ephemeral message stream ([#19513](https://github.com/RocketChat/Rocket.Chat/pull/19513))

- Regression: Fix livechat permission validations ([#19468](https://github.com/RocketChat/Rocket.Chat/pull/19468))

- Regression: Fix presence request logic ([#19527](https://github.com/RocketChat/Rocket.Chat/pull/19527))

- Regression: Fix presence status ([#19474](https://github.com/RocketChat/Rocket.Chat/pull/19474))

- Regression: Fix React warnings ([#19508](https://github.com/RocketChat/Rocket.Chat/pull/19508))

- Regression: Fix setting value not being sent over websocket ([#19477](https://github.com/RocketChat/Rocket.Chat/pull/19477))

- Regression: Fix stream-room-data payload ([#19407](https://github.com/RocketChat/Rocket.Chat/pull/19407))

- Regression: Fix Thread List order ([#19486](https://github.com/RocketChat/Rocket.Chat/pull/19486))

- Regression: Fix visitor field missing on subscription payload ([#19412](https://github.com/RocketChat/Rocket.Chat/pull/19412))

- Regression: GenericTable.HeaderCell does not accept on click anymore ([#19358](https://github.com/RocketChat/Rocket.Chat/pull/19358))

- Regression: Pass `unset` parameter of updated `userData` notification ([#19380](https://github.com/RocketChat/Rocket.Chat/pull/19380))

- Regression: Prevent network broker from starting when not needed ([#19532](https://github.com/RocketChat/Rocket.Chat/pull/19532))

- Regression: Reassessment of client helpers 'XYZ key should not contain .' ([#19310](https://github.com/RocketChat/Rocket.Chat/pull/19310))

- Regression: Rocket.Chat Apps updates always fail ([#19411](https://github.com/RocketChat/Rocket.Chat/pull/19411))

- Regression: Room item menu display delay ([#19401](https://github.com/RocketChat/Rocket.Chat/pull/19401))

- Regression: Sidebar message preview escaping html ([#19382](https://github.com/RocketChat/Rocket.Chat/pull/19382))

- Regression: Sidebar reactivity when read last messages ([#19449](https://github.com/RocketChat/Rocket.Chat/pull/19449))

- Regression: Thread component not updating its message list ([#19390](https://github.com/RocketChat/Rocket.Chat/pull/19390))

- Regression: Thread list misbehaving ([#19413](https://github.com/RocketChat/Rocket.Chat/pull/19413))

- Regression: Thread not showing for unloaded message ([#19402](https://github.com/RocketChat/Rocket.Chat/pull/19402))

- Regression: unable to mark room as read ([#19419](https://github.com/RocketChat/Rocket.Chat/pull/19419))

- Regression: User card closing ([#19322](https://github.com/RocketChat/Rocket.Chat/pull/19322))

- Remove legacy modal template ([#19276](https://github.com/RocketChat/Rocket.Chat/pull/19276))

- Remove legacy slider ([#19255](https://github.com/RocketChat/Rocket.Chat/pull/19255))

- Remove unecessary return at the send code api ([#19494](https://github.com/RocketChat/Rocket.Chat/pull/19494))

- Remove WeDeploy from README ([#19342](https://github.com/RocketChat/Rocket.Chat/pull/19342) by [@lucas-andre](https://github.com/lucas-andre))

- Rewrite: Reset Login Form ([#18237](https://github.com/RocketChat/Rocket.Chat/pull/18237))

- Unify ephemeral message events ([#19464](https://github.com/RocketChat/Rocket.Chat/pull/19464))

- Update Apps-Engine to latest release ([#19499](https://github.com/RocketChat/Rocket.Chat/pull/19499))

- Update Apps-Engine version ([#19385](https://github.com/RocketChat/Rocket.Chat/pull/19385))

- Update comment of "issue-close-app" ([#19078](https://github.com/RocketChat/Rocket.Chat/pull/19078))

- Update feature-request opening process on README ([#19240](https://github.com/RocketChat/Rocket.Chat/pull/19240) by [@brij1999](https://github.com/brij1999))

- Update Fuselage Version ([#19359](https://github.com/RocketChat/Rocket.Chat/pull/19359))

- Use GitHub Container Registry ([#19297](https://github.com/RocketChat/Rocket.Chat/pull/19297))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@FelipeParreira](https://github.com/FelipeParreira)
- [@RohitKumar-200](https://github.com/RohitKumar-200)
- [@aradhya-gupta](https://github.com/aradhya-gupta)
- [@arminfelder](https://github.com/arminfelder)
- [@aryankoul](https://github.com/aryankoul)
- [@ba-9](https://github.com/ba-9)
- [@bhavayAnand9](https://github.com/bhavayAnand9)
- [@brij1999](https://github.com/brij1999)
- [@ceefour](https://github.com/ceefour)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@galshiff](https://github.com/galshiff)
- [@jgribonvald](https://github.com/jgribonvald)
- [@lucas-andre](https://github.com/lucas-andre)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@dougfabris](https://github.com/dougfabris)
- [@frdmn](https://github.com/frdmn)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@lolimay](https://github.com/lolimay)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@rafaelblink](https://github.com/rafaelblink)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)
- [@thassiov](https://github.com/thassiov)
- [@tiagoevanp](https://github.com/tiagoevanp)

# 3.7.4
`2020-12-18  ·  2 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.18.0`

### 🐛 Bug fixes


- Issue with special message rendering ([#19817](https://github.com/RocketChat/Rocket.Chat/pull/19817))

- Problem with attachment render ([#19854](https://github.com/RocketChat/Rocket.Chat/pull/19854))

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)

# 3.7.3
`2020-12-05  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.18.0`

### 🐛 Bug fixes


- Exception on certain login cases including SAML

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.7.2
`2020-11-13  ·  4 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.18.0`

### 🐛 Bug fixes


- Admin not working on IE11 ([#19348](https://github.com/RocketChat/Rocket.Chat/pull/19348))

- Channel creation not working on IE ([#19524](https://github.com/RocketChat/Rocket.Chat/pull/19524))

- Custom Emojis PNGs on IE11 ([#19519](https://github.com/RocketChat/Rocket.Chat/pull/19519))

- Update Polyfills and fix directory in IE ([#19525](https://github.com/RocketChat/Rocket.Chat/pull/19525))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.7.2 ([#19529](https://github.com/RocketChat/Rocket.Chat/pull/19529))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@dougfabris](https://github.com/dougfabris)
- [@ggazzo](https://github.com/ggazzo)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.7.1
`2020-10-09  ·  6 🐛  ·  5 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.18.0`

### 🐛 Bug fixes


- Adding missing custom fields translation in my account's profile ([#19179](https://github.com/RocketChat/Rocket.Chat/pull/19179))

- Admin Sidebar overflowing ([#19101](https://github.com/RocketChat/Rocket.Chat/pull/19101))

- Missing "Bio" in user's profile view (#18821) ([#19166](https://github.com/RocketChat/Rocket.Chat/pull/19166))

- Omnichannel: triggers page not rendering. ([#19134](https://github.com/RocketChat/Rocket.Chat/pull/19134))

- Performance issues when using new Oplog implementation ([#19181](https://github.com/RocketChat/Rocket.Chat/pull/19181))

  A missing configuration was not limiting the new oplog tailing to pool the database frequently even when no data was available, leading to both node and mongodb process been consuming high CPU even with low usage. This case was happening for installations using `mmapv1` database engine or when no admin access was granted to the database user, both preventing the usage of the new [Change Streams](https://docs.mongodb.com/manual/changeStreams/) implementation and fallbacking to our custom oplog implementation in replacement to the Meteor's one what was able to be disabled and use the native implementation via the environmental variable `USE_NATIVE_OPLOG=true`.

- VisitorAutoComplete component ([#19133](https://github.com/RocketChat/Rocket.Chat/pull/19133))

### 👩‍💻👨‍💻 Core Team 🤓

- [@dougfabris](https://github.com/dougfabris)
- [@gabriellsh](https://github.com/gabriellsh)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.7.0
`2020-09-28  ·  10 🎉  ·  3 🚀  ·  39 🐛  ·  26 🔍  ·  22 👩‍💻👨‍💻`

### Engine versions
- Node: `12.18.4`
- NPM: `6.14.8`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.18.0`

### 🎉 New features


- "Room avatar changed" system messages ([#18839](https://github.com/RocketChat/Rocket.Chat/pull/18839))

- **Apps:** Add a Livechat API - setCustomFields ([#18912](https://github.com/RocketChat/Rocket.Chat/pull/18912))

- **Apps:** Add a new upload API ([#18955](https://github.com/RocketChat/Rocket.Chat/pull/18955))

- **Apps:** Add support for new livechat guest's and room's events ([#18946](https://github.com/RocketChat/Rocket.Chat/pull/18946))

- **Apps:** Add support to the "encoding" option in http requests from Apps ([#19002](https://github.com/RocketChat/Rocket.Chat/pull/19002))

- Apps-Engine v1.18.0 ([#19047](https://github.com/RocketChat/Rocket.Chat/pull/19047))

- Option to require settings on wizard UI via ENV variables ([#18974](https://github.com/RocketChat/Rocket.Chat/pull/18974))

  [NEW] Option to require settings on wizard UI via ENV variables

- Retention policy precision defined by a cron job expression ([#18975](https://github.com/RocketChat/Rocket.Chat/pull/18975))

- Send E2E encrypted messages’ content on push notifications ([#18882](https://github.com/RocketChat/Rocket.Chat/pull/18882))

  Sends the content of end to end encrypted messages on Push Notifications allowing new versions of mobile apps to decrypt them and displays the content correctly.

- UploadFS respects $TMPDIR environment variable ([#17012](https://github.com/RocketChat/Rocket.Chat/pull/17012) by [@d-sko](https://github.com/d-sko))

### 🚀 Improvements


- Add "Allow_Save_Media_to_Gallery" setting ([#18875](https://github.com/RocketChat/Rocket.Chat/pull/18875))

  - Added a new setting to allow/disallow saving media to device's gallery on mobile client

- Move jump to message outside menu ([#18928](https://github.com/RocketChat/Rocket.Chat/pull/18928))

- Stop re-sending push notifications rejected by the gateway ([#18608](https://github.com/RocketChat/Rocket.Chat/pull/18608))

### 🐛 Bug fixes


- "Download my data" popup showing HTML code. ([#18947](https://github.com/RocketChat/Rocket.Chat/pull/18947))

- "Save to WebDav" not working ([#18883](https://github.com/RocketChat/Rocket.Chat/pull/18883))

- **ENTERPRISE:** Omnichannel service status switching to unavailable ([#18835](https://github.com/RocketChat/Rocket.Chat/pull/18835))

- API call users.setStatus does not trigger status update of clients ([#18961](https://github.com/RocketChat/Rocket.Chat/pull/18961) by [@FelipeParreira](https://github.com/FelipeParreira))

  Notify logged users via WebSockets message when a user changes status via REST API.

- Block user action ([#18950](https://github.com/RocketChat/Rocket.Chat/pull/18950))

- Can't change password ([#18836](https://github.com/RocketChat/Rocket.Chat/pull/18836))

- Create Custom OAuth services from environment variables ([#17377](https://github.com/RocketChat/Rocket.Chat/pull/17377) by [@mrtndwrd](https://github.com/mrtndwrd))

- Custom fields required if minLength set and no text typed. ([#18838](https://github.com/RocketChat/Rocket.Chat/pull/18838))

- Deactivate users that are the last owner of a room using REST API ([#18864](https://github.com/RocketChat/Rocket.Chat/pull/18864) by [@FelipeParreira](https://github.com/FelipeParreira))

  Allow for user deactivation through REST API (even if user is the last owner of a room)

- Deactivated users show as offline ([#18767](https://github.com/RocketChat/Rocket.Chat/pull/18767))

- Dutch: add translations for missing variables ([#18814](https://github.com/RocketChat/Rocket.Chat/pull/18814) by [@Karting06](https://github.com/Karting06))

- e.sendToBottomIfNecessaryDebounced is not a function ([#18834](https://github.com/RocketChat/Rocket.Chat/pull/18834))

- Errors in LDAP avatar sync preventing login ([#18948](https://github.com/RocketChat/Rocket.Chat/pull/18948))

- Federation issues ([#18978](https://github.com/RocketChat/Rocket.Chat/pull/18978))

- File upload (Avatars, Emoji, Sounds) ([#18841](https://github.com/RocketChat/Rocket.Chat/pull/18841))

- French: Add missing __online__ var ([#18813](https://github.com/RocketChat/Rocket.Chat/pull/18813) by [@Karting06](https://github.com/Karting06))

- IE11 support livechat widget ([#18850](https://github.com/RocketChat/Rocket.Chat/pull/18850))

- If there is `ufs` somewhere in url the request to api always returns 404 ([#18874](https://github.com/RocketChat/Rocket.Chat/pull/18874) by [@FelipeParreira](https://github.com/FelipeParreira))

- Ignore User action from user card ([#18866](https://github.com/RocketChat/Rocket.Chat/pull/18866))

- invite-all-from and invite-all-to commands don't work with multibyte room names ([#18919](https://github.com/RocketChat/Rocket.Chat/pull/18919) by [@FelipeParreira](https://github.com/FelipeParreira))

- Jitsi call start updating subscriptions ([#18837](https://github.com/RocketChat/Rocket.Chat/pull/18837))

- LDAP avatar upload ([#18994](https://github.com/RocketChat/Rocket.Chat/pull/18994))

- Non-upload requests being passed to UFS proxy middleware ([#18931](https://github.com/RocketChat/Rocket.Chat/pull/18931) by [@FelipeParreira](https://github.com/FelipeParreira))

  Avoid non-upload request to be caught by UFS proxy middleware.

- Omnichannel Current Chats open status filter not working ([#18795](https://github.com/RocketChat/Rocket.Chat/pull/18795))

- Open room after guest registration ([#18755](https://github.com/RocketChat/Rocket.Chat/pull/18755))

- PDF not rendering ([#18956](https://github.com/RocketChat/Rocket.Chat/pull/18956))

- Purged threads still show as unread ([#18944](https://github.com/RocketChat/Rocket.Chat/pull/18944) by [@FelipeParreira](https://github.com/FelipeParreira))

  Remove threads from subscription (and update counter) when messages are purged (or threads are disabled).

- Reaction buttons not behaving properly ([#18832](https://github.com/RocketChat/Rocket.Chat/pull/18832))

- Read receipts showing blank names and not marking messages as read ([#18918](https://github.com/RocketChat/Rocket.Chat/pull/18918) by [@wreiske](https://github.com/wreiske))

- Scrollbar mention ticks always rendering as white ([#18979](https://github.com/RocketChat/Rocket.Chat/pull/18979))

- Show custom fields of invalid type ([#18794](https://github.com/RocketChat/Rocket.Chat/pull/18794))

- Showing alerts during setup wizard ([#18862](https://github.com/RocketChat/Rocket.Chat/pull/18862))

- Spurious expert role in startup data ([#18667](https://github.com/RocketChat/Rocket.Chat/pull/18667))

- Stop adding push messages to queue if push is disabled ([#18830](https://github.com/RocketChat/Rocket.Chat/pull/18830))

- User administration throwing a blank page if user has no role ([#18851](https://github.com/RocketChat/Rocket.Chat/pull/18851))

- User can't invite or join other Omnichannel rooms ([#18852](https://github.com/RocketChat/Rocket.Chat/pull/18852))

- User Info: Email and name/username display, alignment on big screens, make admin action ([#18976](https://github.com/RocketChat/Rocket.Chat/pull/18976))

- Users not being able to activate/deactivate E2E in DMs ([#18943](https://github.com/RocketChat/Rocket.Chat/pull/18943))

  [FIX] Users not being able to activate/deactivate E2E in DMs

- Version update check cron job ([#18916](https://github.com/RocketChat/Rocket.Chat/pull/18916) by [@wreiske](https://github.com/wreiske))

<details>
<summary>🔍 Minor changes</summary>


- Bump Livechat widget ([#18977](https://github.com/RocketChat/Rocket.Chat/pull/18977))

- Bump lodash.merge from 4.6.1 to 4.6.2 ([#18800](https://github.com/RocketChat/Rocket.Chat/pull/18800) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Bump marked from 0.6.3 to 0.7.0 ([#18801](https://github.com/RocketChat/Rocket.Chat/pull/18801) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Check i18n file for missing variables ([#18762](https://github.com/RocketChat/Rocket.Chat/pull/18762))

- Do not use deprecated express API ([#18686](https://github.com/RocketChat/Rocket.Chat/pull/18686))

- Fix french translations ([#18746](https://github.com/RocketChat/Rocket.Chat/pull/18746) by [@lsignac](https://github.com/lsignac))

- Fix saveRoomSettings method complexity ([#18840](https://github.com/RocketChat/Rocket.Chat/pull/18840))

- Fix: Missing WebDav upload errors logs ([#18849](https://github.com/RocketChat/Rocket.Chat/pull/18849))

- LingoHub based on develop ([#18973](https://github.com/RocketChat/Rocket.Chat/pull/18973))

- LingoHub based on develop ([#18828](https://github.com/RocketChat/Rocket.Chat/pull/18828))

- LingoHub based on develop ([#18761](https://github.com/RocketChat/Rocket.Chat/pull/18761))

- Merge master into develop & Set version to 3.7.0-develop ([#18752](https://github.com/RocketChat/Rocket.Chat/pull/18752) by [@thirsch](https://github.com/thirsch))

- New: Use database change streams when available ([#18892](https://github.com/RocketChat/Rocket.Chat/pull/18892))

- Obey to settings properties ([#19020](https://github.com/RocketChat/Rocket.Chat/pull/19020))

- Refactor: Admin permissions page ([#18932](https://github.com/RocketChat/Rocket.Chat/pull/18932))

- Refactor: Message Audit page & Audit logs ([#18808](https://github.com/RocketChat/Rocket.Chat/pull/18808))

- Refactor: Omnichannel Analytics ([#18766](https://github.com/RocketChat/Rocket.Chat/pull/18766))

- Refactor: Omnichannel Realtime Monitoring ([#18666](https://github.com/RocketChat/Rocket.Chat/pull/18666))

- Regression: Elements select & multiSelect not rendered correctly in the App Settings ([#19005](https://github.com/RocketChat/Rocket.Chat/pull/19005))

- Regression: File upload via apps not working in some scenarios ([#18995](https://github.com/RocketChat/Rocket.Chat/pull/18995))

- Regression: Fix login screen reactivity of external login providers ([#19033](https://github.com/RocketChat/Rocket.Chat/pull/19033))

- Regression: Handle MongoDB authentication issues ([#18993](https://github.com/RocketChat/Rocket.Chat/pull/18993))

- Replace copying assets on post-install with symlinks ([#18707](https://github.com/RocketChat/Rocket.Chat/pull/18707))

- Set some queries to prefer the secondary database ([#18887](https://github.com/RocketChat/Rocket.Chat/pull/18887))

- Update Meteor to 1.11 ([#18754](https://github.com/RocketChat/Rocket.Chat/pull/18754))

- Update Meteor to 1.11.1 ([#18959](https://github.com/RocketChat/Rocket.Chat/pull/18959))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@FelipeParreira](https://github.com/FelipeParreira)
- [@Karting06](https://github.com/Karting06)
- [@d-sko](https://github.com/d-sko)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@lsignac](https://github.com/lsignac)
- [@mrtndwrd](https://github.com/mrtndwrd)
- [@thirsch](https://github.com/thirsch)
- [@wreiske](https://github.com/wreiske)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@alansikora](https://github.com/alansikora)
- [@d-gubert](https://github.com/d-gubert)
- [@diegolmello](https://github.com/diegolmello)
- [@engelgabriel](https://github.com/engelgabriel)
- [@gabriellsh](https://github.com/gabriellsh)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)
- [@thassiov](https://github.com/thassiov)

# 3.6.3
`2020-09-25  ·  4 🐛  ·  2 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.17.0`

### 🐛 Bug fixes


- Errors in LDAP avatar sync preventing login ([#18948](https://github.com/RocketChat/Rocket.Chat/pull/18948))

- Federation issues ([#18978](https://github.com/RocketChat/Rocket.Chat/pull/18978))

- LDAP avatar upload ([#18994](https://github.com/RocketChat/Rocket.Chat/pull/18994))

- PDF not rendering ([#18956](https://github.com/RocketChat/Rocket.Chat/pull/18956))

<details>
<summary>🔍 Minor changes</summary>


- Obey to settings properties ([#19020](https://github.com/RocketChat/Rocket.Chat/pull/19020))

- Release 3.6.3 ([#19022](https://github.com/RocketChat/Rocket.Chat/pull/19022))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@alansikora](https://github.com/alansikora)
- [@gabriellsh](https://github.com/gabriellsh)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.6.2
`2020-09-18  ·  7 🐛  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.17.0`

### 🐛 Bug fixes


- Create Custom OAuth services from environment variables ([#17377](https://github.com/RocketChat/Rocket.Chat/pull/17377) by [@mrtndwrd](https://github.com/mrtndwrd))

- Deactivate users that are the last owner of a room using REST API ([#18864](https://github.com/RocketChat/Rocket.Chat/pull/18864) by [@FelipeParreira](https://github.com/FelipeParreira))

  Allow for user deactivation through REST API (even if user is the last owner of a room)

- Ignore User action from user card ([#18866](https://github.com/RocketChat/Rocket.Chat/pull/18866))

- invite-all-from and invite-all-to commands don't work with multibyte room names ([#18919](https://github.com/RocketChat/Rocket.Chat/pull/18919) by [@FelipeParreira](https://github.com/FelipeParreira))

  Fix slash commands (invite-all-from and invite-all-to) to accept  multi-byte room names.

- Read receipts showing blank names and not marking messages as read ([#18918](https://github.com/RocketChat/Rocket.Chat/pull/18918) by [@wreiske](https://github.com/wreiske))

- Show custom fields of invalid type ([#18794](https://github.com/RocketChat/Rocket.Chat/pull/18794))

- Version update check cron job ([#18916](https://github.com/RocketChat/Rocket.Chat/pull/18916) by [@wreiske](https://github.com/wreiske))

### 👩‍💻👨‍💻 Contributors 😍

- [@FelipeParreira](https://github.com/FelipeParreira)
- [@mrtndwrd](https://github.com/mrtndwrd)
- [@wreiske](https://github.com/wreiske)

### 👩‍💻👨‍💻 Core Team 🤓

- [@gabriellsh](https://github.com/gabriellsh)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.6.1
`2020-09-11  ·  7 🐛  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.17.0`

### 🐛 Bug fixes


- **ENTERPRISE:** Omnichannel service status switching to unavailable ([#18835](https://github.com/RocketChat/Rocket.Chat/pull/18835))

- File upload (Avatars, Emoji, Sounds) ([#18841](https://github.com/RocketChat/Rocket.Chat/pull/18841))

- IE11 support livechat widget ([#18850](https://github.com/RocketChat/Rocket.Chat/pull/18850))

- Omnichannel Current Chats open status filter not working ([#18795](https://github.com/RocketChat/Rocket.Chat/pull/18795))

- Showing alerts during setup wizard ([#18862](https://github.com/RocketChat/Rocket.Chat/pull/18862))

- User administration throwing a blank page if user has no role ([#18851](https://github.com/RocketChat/Rocket.Chat/pull/18851))

- User can't invite or join other Omnichannel rooms ([#18852](https://github.com/RocketChat/Rocket.Chat/pull/18852))

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.6.0
`2020-08-29  ·  10 🎉  ·  5 🚀  ·  26 🐛  ·  36 🔍  ·  23 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.17.0`

### 🎉 New features


- **APPS-ENGINE:** Implement new IPostLivechatRoomTransferred event ([#18625](https://github.com/RocketChat/Rocket.Chat/pull/18625))

- **Jitsi:** Setting to use room's name instead of room's id to generate the URL ([#17481](https://github.com/RocketChat/Rocket.Chat/pull/17481))

- **Omnichannel:** Ability to set character message limit on Livechat widget ([#18261](https://github.com/RocketChat/Rocket.Chat/pull/18261) by [@oguhpereira](https://github.com/oguhpereira))

- **Omnichannel:** Livechat widget support for rich messages via UiKit ([#18643](https://github.com/RocketChat/Rocket.Chat/pull/18643))

- **Omnichannel/API:** Endpoint `livechat/room.visitor` to change Omnichannel room's visitor ([#18528](https://github.com/RocketChat/Rocket.Chat/pull/18528))

- **Omnichannel/API:** Endpoint `livechat/visitors.search` to search Livechat visitors ([#18514](https://github.com/RocketChat/Rocket.Chat/pull/18514))

- Admin option to reset other users’ E2E encryption key ([#18642](https://github.com/RocketChat/Rocket.Chat/pull/18642))

  Requires the 2FA password fallback enforcement enabled to work

- Banner for servers in the middle of the cloud registration process ([#18623](https://github.com/RocketChat/Rocket.Chat/pull/18623))

- Export room messages as file or directly via email ([#18606](https://github.com/RocketChat/Rocket.Chat/pull/18606))

- Support for custom avatar images in channels ([#18443](https://github.com/RocketChat/Rocket.Chat/pull/18443))

### 🚀 Improvements


- **2FA:** Password enforcement setting and 2FA protection when saving settings or resetting E2E encryption ([#18640](https://github.com/RocketChat/Rocket.Chat/pull/18640))

  - Increase the 2FA remembering time from 5min to 30min

  - Add new setting to enforce 2FA password fallback (enabled only for new installations)

  - Require 2FA to save settings and reset E2E Encryption keys

- **Omnichannel:** Allow set other agent status via method `livechat:changeLivechatStatus ` ([#18571](https://github.com/RocketChat/Rocket.Chat/pull/18571))

- **Security:** Admin info page requires permission `view-statistics` ([#18408](https://github.com/RocketChat/Rocket.Chat/pull/18408))

  Users now require the `view-statistics` permission to be access the `admin/info` page

- **Slack bridge:** Add support to sync threads ([#15992](https://github.com/RocketChat/Rocket.Chat/pull/15992) by [@antkaz](https://github.com/antkaz))

- New component and better look for tooltips ([#18399](https://github.com/RocketChat/Rocket.Chat/pull/18399))

### 🐛 Bug fixes


- 2FA by Email setting showing for the user even when disabled by the admin ([#18473](https://github.com/RocketChat/Rocket.Chat/pull/18473))

  The option to disable/enable the **Two-factor authentication via Email** at `Account > Security > Two Factor Authentication
  ` was visible even when the setting **Enable Two Factor Authentication via Email** at `Admin > Accounts > Two Factor Authentication` was disabled leading to misbehavior since the functionality was disabled.

- Agents enabledDepartment attribute not set on collection ([#18614](https://github.com/RocketChat/Rocket.Chat/pull/18614) by [@paulobernardoaf](https://github.com/paulobernardoaf))

- Anonymous users were created as inactive if the manual approval setting was enabled ([#17427](https://github.com/RocketChat/Rocket.Chat/pull/17427) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Auto complete user suggestions ([#18437](https://github.com/RocketChat/Rocket.Chat/pull/18437))

  Fixes the issue with broken user suggestions in threads when using `@`

- Backdrop on front of modal. ([#18596](https://github.com/RocketChat/Rocket.Chat/pull/18596))

- Custom fields title when no custom fields ([#18374](https://github.com/RocketChat/Rocket.Chat/pull/18374))

- Emojis on thread replies ([#18407](https://github.com/RocketChat/Rocket.Chat/pull/18407))

  Users can now see the emojis on thread replies

- Enabling Apple OAuth crashes other OAuth services ([#18563](https://github.com/RocketChat/Rocket.Chat/pull/18563))

- Error when reading uploads from Livechat Visitor through the Apps Engine ([#18474](https://github.com/RocketChat/Rocket.Chat/pull/18474))

- findOrCreateInvite REST endpoint ignoring `days` and `maxUses` params ([#18565](https://github.com/RocketChat/Rocket.Chat/pull/18565))

- Invalid sample JSON on admin settings ([#18595](https://github.com/RocketChat/Rocket.Chat/pull/18595))

- MarkdownText usage ([#18621](https://github.com/RocketChat/Rocket.Chat/pull/18621))

- Marking room as read with unread threads still ([#18410](https://github.com/RocketChat/Rocket.Chat/pull/18410))

- Random generated password not matching the Password Policy ([#18475](https://github.com/RocketChat/Rocket.Chat/pull/18475))

  Generates a password with all the possible requirements of the Password Policy and matching the size limitations when enabled.

- React being loaded on the main bundle ([#18597](https://github.com/RocketChat/Rocket.Chat/pull/18597))

- Read receipts duplicate key error ([#18560](https://github.com/RocketChat/Rocket.Chat/pull/18560) by [@galshiff](https://github.com/galshiff))

  Fixed receipt duplicate key error bug

- Room Mentions on Threads ([#18336](https://github.com/RocketChat/Rocket.Chat/pull/18336))

- Sending notifications from senders without a name ([#18479](https://github.com/RocketChat/Rocket.Chat/pull/18479))

- SMS integration not storing media files ([#18491](https://github.com/RocketChat/Rocket.Chat/pull/18491))

- Thread reply disappearing and threads result on search ([#18349](https://github.com/RocketChat/Rocket.Chat/pull/18349))

- UIKit Select and Multiselects not working ([#18598](https://github.com/RocketChat/Rocket.Chat/pull/18598))

- Uncaught (in promise) undefined ([#18393](https://github.com/RocketChat/Rocket.Chat/pull/18393))

- UserCard and UserInfo not respecting the setting to use real names ([#18628](https://github.com/RocketChat/Rocket.Chat/pull/18628))

- UserCard avatar cache (avatarETag) ([#18466](https://github.com/RocketChat/Rocket.Chat/pull/18466))

- Users page in admin not working for inactive user joining ([#18594](https://github.com/RocketChat/Rocket.Chat/pull/18594))

- Wrong rooms list order when last message date is missing ([#18639](https://github.com/RocketChat/Rocket.Chat/pull/18639))

<details>
<summary>🔍 Minor changes</summary>


- Add new enterprise bundle option `omnichannel-mobile-enterprise` ([#18533](https://github.com/RocketChat/Rocket.Chat/pull/18533))

- Add type checking to CI ([#18411](https://github.com/RocketChat/Rocket.Chat/pull/18411))

- Bump bcrypt from 3.0.7 to 5.0.0 ([#18622](https://github.com/RocketChat/Rocket.Chat/pull/18622) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Defer startup checks ([#18547](https://github.com/RocketChat/Rocket.Chat/pull/18547))

- Do not retry and log warning when push notification was not authorised ([#18562](https://github.com/RocketChat/Rocket.Chat/pull/18562))

- Explain why issue is closed when not using an issue template ([#18420](https://github.com/RocketChat/Rocket.Chat/pull/18420))

- Fix typo in setting description ([#18476](https://github.com/RocketChat/Rocket.Chat/pull/18476))

- Improve performance of client presence monitor ([#18645](https://github.com/RocketChat/Rocket.Chat/pull/18645))

- LingoHub based on develop ([#18586](https://github.com/RocketChat/Rocket.Chat/pull/18586))

- LingoHub based on develop ([#18516](https://github.com/RocketChat/Rocket.Chat/pull/18516))

- LingoHub based on develop ([#18465](https://github.com/RocketChat/Rocket.Chat/pull/18465))

- Merge master into develop & Set version to 3.6.0-develop ([#18401](https://github.com/RocketChat/Rocket.Chat/pull/18401) by [@densik](https://github.com/densik) & [@dudizilla](https://github.com/dudizilla) & [@omarchehab98](https://github.com/omarchehab98) & [@paulobernardoaf](https://github.com/paulobernardoaf))

- Missing email notification when an admin resets your E2E key ([#18673](https://github.com/RocketChat/Rocket.Chat/pull/18673))

- Omnichannel Admin rewritten in React (#18438) ([#18438](https://github.com/RocketChat/Rocket.Chat/pull/18438))

- Prevent directory API to return emails if the user has no permission ([#18478](https://github.com/RocketChat/Rocket.Chat/pull/18478))

- Reduce Push Notifications retry from max 31 hours to max 31 minutes ([#18558](https://github.com/RocketChat/Rocket.Chat/pull/18558))

  Previews logic was retring in **0.1s, 1s, 11s, 2m, 18m, 3h and 31h**, now it’s retrying in **1m, 3m, 7m, 15m and 31m**

- Regression: Accept visitors for uikit interactions ([#18706](https://github.com/RocketChat/Rocket.Chat/pull/18706))

- Regression: Add remove popup to omnichannel custom fields ([#18719](https://github.com/RocketChat/Rocket.Chat/pull/18719))

- Regression: Agents Page issues ([#18684](https://github.com/RocketChat/Rocket.Chat/pull/18684))

- Regression: Bundle the package `hepburn` ([#18715](https://github.com/RocketChat/Rocket.Chat/pull/18715))

- Regression: Fix room avatar file name ([#18544](https://github.com/RocketChat/Rocket.Chat/pull/18544))

- Regression: Omnichannel Business Hours Issues ([#18723](https://github.com/RocketChat/Rocket.Chat/pull/18723))

- Regression: Omnichannel Current Chat issues ([#18718](https://github.com/RocketChat/Rocket.Chat/pull/18718))

- Regression: Omnichannel Tags and Units issues ([#18705](https://github.com/RocketChat/Rocket.Chat/pull/18705))

- Regression: Priorities Page issues ([#18685](https://github.com/RocketChat/Rocket.Chat/pull/18685))

- Regression: Revert silent: true ([#18671](https://github.com/RocketChat/Rocket.Chat/pull/18671))

- Regression: Split date fields on export messages contextual bar ([#18724](https://github.com/RocketChat/Rocket.Chat/pull/18724))

- Regression: Toast Messages ([#18674](https://github.com/RocketChat/Rocket.Chat/pull/18674))

- Regression: UI margins on Export Messages ([#18682](https://github.com/RocketChat/Rocket.Chat/pull/18682))

- Regression: Update checker not being disabled properly. ([#18676](https://github.com/RocketChat/Rocket.Chat/pull/18676))

- Regression: Use user autocomplete on export messages ([#18726](https://github.com/RocketChat/Rocket.Chat/pull/18726))

- Release 3.6.0 ([#18727](https://github.com/RocketChat/Rocket.Chat/pull/18727) by [@oguhpereira](https://github.com/oguhpereira) & [@thirsch](https://github.com/thirsch))

- Set default timeout of 20s for HTTP calls ([#18549](https://github.com/RocketChat/Rocket.Chat/pull/18549))

- Update Apps-Engine version ([#18641](https://github.com/RocketChat/Rocket.Chat/pull/18641))

- Update dependencies ([#18593](https://github.com/RocketChat/Rocket.Chat/pull/18593))

- Update README.md ([#18503](https://github.com/RocketChat/Rocket.Chat/pull/18503))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@antkaz](https://github.com/antkaz)
- [@densik](https://github.com/densik)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@dudizilla](https://github.com/dudizilla)
- [@galshiff](https://github.com/galshiff)
- [@oguhpereira](https://github.com/oguhpereira)
- [@omarchehab98](https://github.com/omarchehab98)
- [@paulobernardoaf](https://github.com/paulobernardoaf)
- [@thirsch](https://github.com/thirsch)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@Sing-Li](https://github.com/Sing-Li)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@gabriellsh](https://github.com/gabriellsh)
- [@ggazzo](https://github.com/ggazzo)
- [@juliagrala](https://github.com/juliagrala)
- [@murtaza98](https://github.com/murtaza98)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 3.5.4
`2020-08-24  ·  1 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.16.0`

### 🐛 Bug fixes


- MarkdownText usage ([#18621](https://github.com/RocketChat/Rocket.Chat/pull/18621))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.5.4 ([#18665](https://github.com/RocketChat/Rocket.Chat/pull/18665))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.5.3
`2020-08-19  ·  3 🐛  ·  1 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.16.0`

### 🐛 Bug fixes


- React being loaded on the main bundle ([#18597](https://github.com/RocketChat/Rocket.Chat/pull/18597))

- UIKit Select and Multiselects not working ([#18598](https://github.com/RocketChat/Rocket.Chat/pull/18598))

- Users page in admin not working for inactive user joining ([#18594](https://github.com/RocketChat/Rocket.Chat/pull/18594))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.5.3 ([#18610](https://github.com/RocketChat/Rocket.Chat/pull/18610))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.5.2
`2020-08-13  ·  1 🐛  ·  2 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.16.0`

### 🐛 Bug fixes


- Sending notifications from senders without a name ([#18479](https://github.com/RocketChat/Rocket.Chat/pull/18479))

<details>
<summary>🔍 Minor changes</summary>


- Defer startup checks ([#18547](https://github.com/RocketChat/Rocket.Chat/pull/18547))

- Release 3.5.2 ([#18548](https://github.com/RocketChat/Rocket.Chat/pull/18548))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.5.1
`2020-08-03  ·  8 🐛  ·  1 🔍  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.16.0`

### 🐛 Bug fixes


- Appending 'false' to Jitsi URL ([#18430](https://github.com/RocketChat/Rocket.Chat/pull/18430))

- Can't send long messages as attachment ([#18355](https://github.com/RocketChat/Rocket.Chat/pull/18355))

- Error when updating omnichannel department without agents parameter ([#18428](https://github.com/RocketChat/Rocket.Chat/pull/18428))

- Invalid MIME type when uploading audio files ([#18426](https://github.com/RocketChat/Rocket.Chat/pull/18426))

- Migration 194 ([#18457](https://github.com/RocketChat/Rocket.Chat/pull/18457) by [@thirsch](https://github.com/thirsch))

- Multiple push notifications sent via native drivers ([#18442](https://github.com/RocketChat/Rocket.Chat/pull/18442))

- Omnichannel session monitor is not starting ([#18412](https://github.com/RocketChat/Rocket.Chat/pull/18412))

- Omnichannel Take Inquiry endpoint checking wrong permission ([#18446](https://github.com/RocketChat/Rocket.Chat/pull/18446))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.5.1 ([#18452](https://github.com/RocketChat/Rocket.Chat/pull/18452) by [@thirsch](https://github.com/thirsch))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@thirsch](https://github.com/thirsch)

### 👩‍💻👨‍💻 Core Team 🤓

- [@gabriellsh](https://github.com/gabriellsh)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 3.5.0
`2020-07-27  ·  8 🎉  ·  5 🚀  ·  29 🐛  ·  34 🔍  ·  21 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.16.0`

### 🎉 New features


- **ENTERPRISE:** Add support to license tags ([#18093](https://github.com/RocketChat/Rocket.Chat/pull/18093))

  Enterprise installations will show tags on Admin panel with the type of the license applied. The tag will be visible on the top-left corner of the administration area as a badge helping administrators to identify which license they have.

- **ENTERPRISE:** Push Notification Data Privacy ([#18254](https://github.com/RocketChat/Rocket.Chat/pull/18254))

- Added profile field to inform Nickname for users in order to be searchable ([#18260](https://github.com/RocketChat/Rocket.Chat/pull/18260))

  Nickname is a new user field that can be used to better identify users when searching for someone to add in a channel or do a mention. Useful for large organizations or countries where name repetition is common.

- External MP3 encoder worker for audio recording ([#18277](https://github.com/RocketChat/Rocket.Chat/pull/18277))

- Sign in with apple (iOS client only) ([#18258](https://github.com/RocketChat/Rocket.Chat/pull/18258) by [@djorkaeffalexandre](https://github.com/djorkaeffalexandre))

  Add Sign in with Apple service for the iOS client-only, support for the Web and Android clients will land in future releases.

- Update Apps-Engine version ([#18271](https://github.com/RocketChat/Rocket.Chat/pull/18271))

- Update Apps-Engine version ([#18212](https://github.com/RocketChat/Rocket.Chat/pull/18212))

- User profile and User card ([#18194](https://github.com/RocketChat/Rocket.Chat/pull/18194))

### 🚀 Improvements


- Change setting that blocks unauthenticated access to avatar to public ([#18316](https://github.com/RocketChat/Rocket.Chat/pull/18316) by [@djorkaeffalexandre](https://github.com/djorkaeffalexandre))

- Improve performance and remove agents when the department is removed ([#17049](https://github.com/RocketChat/Rocket.Chat/pull/17049) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- List dropdown ([#18081](https://github.com/RocketChat/Rocket.Chat/pull/18081))

- Mention autocomplete UI and performance improvements  ([#18309](https://github.com/RocketChat/Rocket.Chat/pull/18309))

  * New setting to configure the number of suggestions `Admin > Layout > User Interface > Number of users' autocomplete suggestions` (default 5)

  * The UI shows whenever the user is not a member of the room

  * The UI shows when the suggestion came from the last messages for quick selection/reply

  * The suggestions follow this order:
    * The user with the exact username and member of the room
    * The user with the exact username but not a member of the room (if allowed to list non-members)
    * The users containing the text in username, name or nickname and member of the room
    * The users containing the text in username, name or nickname and not a member of the room (if allowed to list non-members)

- Message action styles ([#18190](https://github.com/RocketChat/Rocket.Chat/pull/18190))

### 🐛 Bug fixes


- "Join" button on thread when room is read only ([#18314](https://github.com/RocketChat/Rocket.Chat/pull/18314))

- App details returns to apps table, instead of previous page. ([#18080](https://github.com/RocketChat/Rocket.Chat/pull/18080))

- Application not loading due to reverse proxy decoding API calls unnecessarily ([#18222](https://github.com/RocketChat/Rocket.Chat/pull/18222))

- Apps page loading indefinitely if no Markeplace data ([#18274](https://github.com/RocketChat/Rocket.Chat/pull/18274))

- Bug on entering token in connectivity services ([#18317](https://github.com/RocketChat/Rocket.Chat/pull/18317))

- Cannot open admin when server uses ROOT_URL with subpath (#18105) ([#18147](https://github.com/RocketChat/Rocket.Chat/pull/18147) by [@omarchehab98](https://github.com/omarchehab98))

- CAS login not merging users with local accounts ([#18238](https://github.com/RocketChat/Rocket.Chat/pull/18238))

- Clipboard not working when permalinking a pinned message ([#18047](https://github.com/RocketChat/Rocket.Chat/pull/18047))

- Closing the admin does not return to last opened room ([#18308](https://github.com/RocketChat/Rocket.Chat/pull/18308))

- Corrects Typo in Analytics section of the admin page ([#17984](https://github.com/RocketChat/Rocket.Chat/pull/17984) by [@darigovresearch](https://github.com/darigovresearch))

- Delete user warning message undefined ([#18310](https://github.com/RocketChat/Rocket.Chat/pull/18310))

- Don't show agent info in the transcript if the setting is disabled ([#18044](https://github.com/RocketChat/Rocket.Chat/pull/18044) by [@antkaz](https://github.com/antkaz))

- Error when fetching a nonexistent business hour from the server ([#18315](https://github.com/RocketChat/Rocket.Chat/pull/18315))

- Few adjustments to accept fuselage theme ([#18009](https://github.com/RocketChat/Rocket.Chat/pull/18009))

- File uploads for unknown file types but nothing is blocked ([#18263](https://github.com/RocketChat/Rocket.Chat/pull/18263) by [@20051231](https://github.com/20051231))

- Fix sticky notifications not working ([#18285](https://github.com/RocketChat/Rocket.Chat/pull/18285))

- Geolocation permission being asked on load ([#18030](https://github.com/RocketChat/Rocket.Chat/pull/18030))

- Local Account login error when both LDAP and Email 2FA are enabled ([#18318](https://github.com/RocketChat/Rocket.Chat/pull/18318))

- Merge user custom fields on LDAP sync ([#17339](https://github.com/RocketChat/Rocket.Chat/pull/17339) by [@tobiasge](https://github.com/tobiasge))

- Misleading labels in Prune Messages ([#18006](https://github.com/RocketChat/Rocket.Chat/pull/18006))

- Missing Privacy Terms Cloud Register warning ([#18383](https://github.com/RocketChat/Rocket.Chat/pull/18383))

- Old Data Migrations breaking upgrades ([#18185](https://github.com/RocketChat/Rocket.Chat/pull/18185))

- Push gateway and cloud integration ([#18377](https://github.com/RocketChat/Rocket.Chat/pull/18377))

- SAML login crashing when receiving an array of roles ([#18224](https://github.com/RocketChat/Rocket.Chat/pull/18224))

- SAML login saves invalid username when receiving multiple values ([#18213](https://github.com/RocketChat/Rocket.Chat/pull/18213))

- SlackBridge error ([#18320](https://github.com/RocketChat/Rocket.Chat/pull/18320))

- Update check not able to be disabled ([#18339](https://github.com/RocketChat/Rocket.Chat/pull/18339))

  Update checker can now be disabled.

- Update link URL at AppsWhatIsIt ([#18240](https://github.com/RocketChat/Rocket.Chat/pull/18240))

- View close uikit event sending wrong payload ([#18289](https://github.com/RocketChat/Rocket.Chat/pull/18289))

<details>
<summary>🔍 Minor changes</summary>


- Broken link on readme ([#18358](https://github.com/RocketChat/Rocket.Chat/pull/18358))

- LingoHub based on develop ([#18307](https://github.com/RocketChat/Rocket.Chat/pull/18307))

- LingoHub based on develop ([#18176](https://github.com/RocketChat/Rocket.Chat/pull/18176))

- Merge master into develop & Set version to 3.5.0-develop ([#18083](https://github.com/RocketChat/Rocket.Chat/pull/18083) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@cking-vonix](https://github.com/cking-vonix) & [@lpilz](https://github.com/lpilz) & [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Move the development guidelines to our handbook ([#18026](https://github.com/RocketChat/Rocket.Chat/pull/18026))

- Regression - Profile page crashing for users without password ([#18287](https://github.com/RocketChat/Rocket.Chat/pull/18287))

- Regression: Account Sidebar not rendering properly ([#18288](https://github.com/RocketChat/Rocket.Chat/pull/18288))

- Regression: Admin User password  ([#18350](https://github.com/RocketChat/Rocket.Chat/pull/18350))

- Regression: Close UserCard if action opens a new page ([#18319](https://github.com/RocketChat/Rocket.Chat/pull/18319))

- Regression: Edit messages after opening thread ([#18375](https://github.com/RocketChat/Rocket.Chat/pull/18375))

- Regression: Fix defaultFields for null values ([#18360](https://github.com/RocketChat/Rocket.Chat/pull/18360))

- Regression: Fix useUserSubscription usage ([#18378](https://github.com/RocketChat/Rocket.Chat/pull/18378))

- Regression: Mentions in thread title ([#18369](https://github.com/RocketChat/Rocket.Chat/pull/18369))

- Regression: Message actions under "unread messages" warning ([#18273](https://github.com/RocketChat/Rocket.Chat/pull/18273))

- Regression: MP3 worker ([#18371](https://github.com/RocketChat/Rocket.Chat/pull/18371))

- Regression: nickname field in user profile. ([#18359](https://github.com/RocketChat/Rocket.Chat/pull/18359))

- Regression: Notification with id-only isn't showed by iOS devices ([#18353](https://github.com/RocketChat/Rocket.Chat/pull/18353) by [@djorkaeffalexandre](https://github.com/djorkaeffalexandre))

- Regression: Preferences crashing when User has no preferences set. ([#18341](https://github.com/RocketChat/Rocket.Chat/pull/18341))

- Regression: Provide a fallback text when push notification is idOnly ([#18373](https://github.com/RocketChat/Rocket.Chat/pull/18373) by [@djorkaeffalexandre](https://github.com/djorkaeffalexandre))

- Regression: Remove calls to Console API in useForm hook ([#18244](https://github.com/RocketChat/Rocket.Chat/pull/18244))

- Regression: Return original message on push API ([#18386](https://github.com/RocketChat/Rocket.Chat/pull/18386))

- Regression: Thread Title not being escaped ([#18356](https://github.com/RocketChat/Rocket.Chat/pull/18356))

- Regression: User Status selector ([#18343](https://github.com/RocketChat/Rocket.Chat/pull/18343))

- Regression: Userinfo center avatar image ([#18354](https://github.com/RocketChat/Rocket.Chat/pull/18354))

- Regression: useStorage ([#18370](https://github.com/RocketChat/Rocket.Chat/pull/18370))

- Regression: useUserContext ([#18385](https://github.com/RocketChat/Rocket.Chat/pull/18385))

- Regression: Wrong background in disabled inputs ([#18372](https://github.com/RocketChat/Rocket.Chat/pull/18372))

- Release 3.4.2 ([#18241](https://github.com/RocketChat/Rocket.Chat/pull/18241) by [@omarchehab98](https://github.com/omarchehab98))

- Rewrite Contextual Bar Discussion List in React ([#18127](https://github.com/RocketChat/Rocket.Chat/pull/18127))

- Rewrite: My Account > Integrations rewritten ([#18290](https://github.com/RocketChat/Rocket.Chat/pull/18290))

- Rewrite: My Account using React ([#18106](https://github.com/RocketChat/Rocket.Chat/pull/18106))

- Update Apps Engine ([#18389](https://github.com/RocketChat/Rocket.Chat/pull/18389))

- Update Apps-Engine to Beta version ([#18294](https://github.com/RocketChat/Rocket.Chat/pull/18294))

- Update the API of React Hooks using Meteor's reactive system ([#18226](https://github.com/RocketChat/Rocket.Chat/pull/18226))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@20051231](https://github.com/20051231)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@antkaz](https://github.com/antkaz)
- [@cking-vonix](https://github.com/cking-vonix)
- [@darigovresearch](https://github.com/darigovresearch)
- [@djorkaeffalexandre](https://github.com/djorkaeffalexandre)
- [@lpilz](https://github.com/lpilz)
- [@mariaeduardacunha](https://github.com/mariaeduardacunha)
- [@omarchehab98](https://github.com/omarchehab98)
- [@tobiasge](https://github.com/tobiasge)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 3.4.2
`2020-07-10  ·  6 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.15.0`

### 🐛 Bug fixes


- App details returns to apps table, instead of previous page. ([#18080](https://github.com/RocketChat/Rocket.Chat/pull/18080))

- Application not loading due to reverse proxy decoding API calls unnecessarily ([#18222](https://github.com/RocketChat/Rocket.Chat/pull/18222))

- Cannot open admin when server uses ROOT_URL with subpath (#18105) ([#18147](https://github.com/RocketChat/Rocket.Chat/pull/18147) by [@omarchehab98](https://github.com/omarchehab98))

- CAS login not merging users with local accounts ([#18238](https://github.com/RocketChat/Rocket.Chat/pull/18238))

- Old Data Migrations breaking upgrades ([#18185](https://github.com/RocketChat/Rocket.Chat/pull/18185))

- SAML login crashing when receiving an array of roles ([#18224](https://github.com/RocketChat/Rocket.Chat/pull/18224))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.4.2 ([#18241](https://github.com/RocketChat/Rocket.Chat/pull/18241) by [@omarchehab98](https://github.com/omarchehab98))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@omarchehab98](https://github.com/omarchehab98)

### 👩‍💻👨‍💻 Core Team 🤓

- [@gabriellsh](https://github.com/gabriellsh)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@rodrigok](https://github.com/rodrigok)

# 3.4.1
`2020-07-02  ·  7 🐛  ·  1 🔍  ·  8 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.15.0`

### 🐛 Bug fixes


- "Add reaction" icon missing when the viewport size is smaller than 500px ([#18110](https://github.com/RocketChat/Rocket.Chat/pull/18110) by [@dudizilla](https://github.com/dudizilla))

- Avatar ETag missing from User ([#18109](https://github.com/RocketChat/Rocket.Chat/pull/18109))

- Email notifications were still being sent for online users ([#18088](https://github.com/RocketChat/Rocket.Chat/pull/18088) by [@densik](https://github.com/densik))

- Jitsi opening twice ([#18111](https://github.com/RocketChat/Rocket.Chat/pull/18111))

- Not possible to read encrypted messages after disable E2E on channel level ([#18101](https://github.com/RocketChat/Rocket.Chat/pull/18101))

- Omnichannel close room callback returning promise ([#18102](https://github.com/RocketChat/Rocket.Chat/pull/18102))

- The livechat agent activity monitor wasn't being initialised because due to an internal error ([#18090](https://github.com/RocketChat/Rocket.Chat/pull/18090) by [@paulobernardoaf](https://github.com/paulobernardoaf))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.4.1 ([#18134](https://github.com/RocketChat/Rocket.Chat/pull/18134) by [@densik](https://github.com/densik) & [@dudizilla](https://github.com/dudizilla) & [@paulobernardoaf](https://github.com/paulobernardoaf))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@densik](https://github.com/densik)
- [@dudizilla](https://github.com/dudizilla)
- [@paulobernardoaf](https://github.com/paulobernardoaf)

### 👩‍💻👨‍💻 Core Team 🤓

- [@gabriellsh](https://github.com/gabriellsh)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.4.0
`2020-06-30  ·  18 🎉  ·  19 🚀  ·  42 🐛  ·  52 🔍  ·  52 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.15.0`

### 🎉 New features


- **API:** Add `interation.update` endpoint ([#13618](https://github.com/RocketChat/Rocket.Chat/pull/13618) by [@tonobo](https://github.com/tonobo))

- **API:** Endpoint `groups.setEncrypted` ([#13477](https://github.com/RocketChat/Rocket.Chat/pull/13477))

- **API:** Endpoint `settings.addCustomOAuth` to create Custom OAuth services ([#14912](https://github.com/RocketChat/Rocket.Chat/pull/14912) by [@g-rauhoeft](https://github.com/g-rauhoeft))

- **API:** New endpoints to manage User Custom Status `custom-user-status.create`, custom-user-status.delete` and `custom-user-status.update` ([#16550](https://github.com/RocketChat/Rocket.Chat/pull/16550) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- **ENTERPRISE:** Download engagement data ([#17920](https://github.com/RocketChat/Rocket.Chat/pull/17920))

- **ENTERPRISE:** Omnichannel multiple business hours ([#17947](https://github.com/RocketChat/Rocket.Chat/pull/17947) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Ability to configure Jitsi room options via new setting `URL Suffix` ([#17950](https://github.com/RocketChat/Rocket.Chat/pull/17950) by [@fthiery](https://github.com/fthiery))

- Accept variable `#{userdn}` on LDAP group filter ([#16273](https://github.com/RocketChat/Rocket.Chat/pull/16273) by [@ChrissW-R1](https://github.com/ChrissW-R1))

- Add ability to block failed login attempts by user and IP ([#17783](https://github.com/RocketChat/Rocket.Chat/pull/17783) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Allows agents to send chat transcript to omnichannel end-users ([#17774](https://github.com/RocketChat/Rocket.Chat/pull/17774))

- Assign oldest active user as owner when deleting last room owner ([#16088](https://github.com/RocketChat/Rocket.Chat/pull/16088))

- Blocked Media Types setting ([#17617](https://github.com/RocketChat/Rocket.Chat/pull/17617))

- Highlight matching words in message search results ([#16166](https://github.com/RocketChat/Rocket.Chat/pull/16166) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Make ldap avatar source field customizable ([#12958](https://github.com/RocketChat/Rocket.Chat/pull/12958) by [@alexbartsch](https://github.com/alexbartsch))

- Reply notification email to sender's email when the Direct Reply feature is disabled ([#15767](https://github.com/RocketChat/Rocket.Chat/pull/15767) by [@localguru](https://github.com/localguru))

- Rewrite Apps ([#17906](https://github.com/RocketChat/Rocket.Chat/pull/17906))

- Setting to determine if the LDAP user active state should be synced ([#17645](https://github.com/RocketChat/Rocket.Chat/pull/17645))

- Skip Export Operations that haven't been updated in over a day ([#16135](https://github.com/RocketChat/Rocket.Chat/pull/16135))

### 🚀 Improvements


- **Federation:** Add support for _tcp and protocol DNS entries ([#17818](https://github.com/RocketChat/Rocket.Chat/pull/17818))

- **Performance:** Add new database indexes to improve data query performance ([#17839](https://github.com/RocketChat/Rocket.Chat/pull/17839))

- Add rate limiter to UiKit endpoints ([#17859](https://github.com/RocketChat/Rocket.Chat/pull/17859))

- Allow webhook message to respond in thread ([#17863](https://github.com/RocketChat/Rocket.Chat/pull/17863) by [@Karting06](https://github.com/Karting06))

- Change default upload settings to only block SVG files ([#17933](https://github.com/RocketChat/Rocket.Chat/pull/17933))

- Don't send emails to online users and remove delay when away/idle ([#17907](https://github.com/RocketChat/Rocket.Chat/pull/17907))

- Make the implementation of custom code easier by having placeholders for a custom folder ([#15106](https://github.com/RocketChat/Rocket.Chat/pull/15106) by [@justinr1234](https://github.com/justinr1234))

- Performance editing Admin settings ([#17916](https://github.com/RocketChat/Rocket.Chat/pull/17916))

- React hooks lint rules ([#17941](https://github.com/RocketChat/Rocket.Chat/pull/17941))

- Refactor Omnichannel Office Hours feature ([#17824](https://github.com/RocketChat/Rocket.Chat/pull/17824) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Refactor Omnichannel Past Chats List ([#17346](https://github.com/RocketChat/Rocket.Chat/pull/17346) by [@nitinkumartiwari](https://github.com/nitinkumartiwari))

- Rewrite admin sidebar in React ([#17801](https://github.com/RocketChat/Rocket.Chat/pull/17801))

- Rewrite Federation Dashboard ([#17900](https://github.com/RocketChat/Rocket.Chat/pull/17900))

- SAML implementation ([#17742](https://github.com/RocketChat/Rocket.Chat/pull/17742))

- Slack import: Parse channel and user mentions ([#17637](https://github.com/RocketChat/Rocket.Chat/pull/17637))

- Split NOTIFICATIONS_SCHEDULE_DELAY into three separate variables ([#17669](https://github.com/RocketChat/Rocket.Chat/pull/17669) by [@jazztickets](https://github.com/jazztickets))

  Email notification delay can now be customized with the following environment variables:
  NOTIFICATIONS_SCHEDULE_DELAY_ONLINE
  NOTIFICATIONS_SCHEDULE_DELAY_AWAY
  NOTIFICATIONS_SCHEDULE_DELAY_OFFLINE
  Setting the value to -1 disable notifications for that type.

- Threads ([#17416](https://github.com/RocketChat/Rocket.Chat/pull/17416))

- Use REST for DDP calls by default ([#17934](https://github.com/RocketChat/Rocket.Chat/pull/17934))

- User avatar cache invalidation ([#17925](https://github.com/RocketChat/Rocket.Chat/pull/17925))

### 🐛 Bug fixes


- Add Authorization Bearer to allowed Headers ([#8566](https://github.com/RocketChat/Rocket.Chat/pull/8566) by [@Siedlerchr](https://github.com/Siedlerchr))

- Add missing i18n entry for LDAP connection test success message ([#17691](https://github.com/RocketChat/Rocket.Chat/pull/17691) by [@AbhinavTalari](https://github.com/AbhinavTalari))

- Added explicit server oembed provider for Twitter ([#17954](https://github.com/RocketChat/Rocket.Chat/pull/17954) by [@Cleod9](https://github.com/Cleod9))

- Autocomplete component is not working property when searching channels in the Livechat Departments form ([#17970](https://github.com/RocketChat/Rocket.Chat/pull/17970))

- Cannot react while "Allow reaction" is set to true ([#17964](https://github.com/RocketChat/Rocket.Chat/pull/17964))

- Channel/Room inconsistency for leave and hide options ([#10165](https://github.com/RocketChat/Rocket.Chat/pull/10165) by [@c0dzilla](https://github.com/c0dzilla))

- Close the user info context panel does not navigate back to the user's list ([#14085](https://github.com/RocketChat/Rocket.Chat/pull/14085) by [@mohamedar97](https://github.com/mohamedar97))

- Disabling `Json Web Tokens protection to file uploads` disables the File Upload protection entirely ([#16262](https://github.com/RocketChat/Rocket.Chat/pull/16262) by [@antkaz](https://github.com/antkaz))

- Discussion List paddings ([#17955](https://github.com/RocketChat/Rocket.Chat/pull/17955))

- Discussion not updating rooms list and not checking right permissions ([#17959](https://github.com/RocketChat/Rocket.Chat/pull/17959))

- Discussion sort option even with discussions disabled ([#17963](https://github.com/RocketChat/Rocket.Chat/pull/17963))

- double slashes in avatar url ([#17739](https://github.com/RocketChat/Rocket.Chat/pull/17739))

- Duplicated password placeholder  ([#17898](https://github.com/RocketChat/Rocket.Chat/pull/17898) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Encode custom oauth2 URL params ([#13373](https://github.com/RocketChat/Rocket.Chat/pull/13373) by [@InstinctBas](https://github.com/InstinctBas))

- Hide system message add/remove owner  ([#17938](https://github.com/RocketChat/Rocket.Chat/pull/17938))

- Importers progress sending too much update events to clients ([#17857](https://github.com/RocketChat/Rocket.Chat/pull/17857))

- Link preview containing HTML encoded chars ([#16512](https://github.com/RocketChat/Rocket.Chat/pull/16512))

- Links being escaped twice leading to visible encoded characters ([#16481](https://github.com/RocketChat/Rocket.Chat/pull/16481))

- Markdown links not accepting URLs with parentheses ([#13605](https://github.com/RocketChat/Rocket.Chat/pull/13605) by [@knrt10](https://github.com/knrt10))

- Message action popup doesn't adjust itself on screen resize ([#16508](https://github.com/RocketChat/Rocket.Chat/pull/16508) by [@ritvikjain99](https://github.com/ritvikjain99))

- Missing i18n key for setting: Verify Email for External Accounts ([#18002](https://github.com/RocketChat/Rocket.Chat/pull/18002))

- Missing pinned icon indicator for messages pinned ([#16448](https://github.com/RocketChat/Rocket.Chat/pull/16448) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Missing User when forwarding Omnichannel conversations via Apps-Engine ([#17918](https://github.com/RocketChat/Rocket.Chat/pull/17918))

- New Omnichannel Past Chats list padding ([#17994](https://github.com/RocketChat/Rocket.Chat/pull/17994))

- No rotate option, to prevent image quality loss ([#15196](https://github.com/RocketChat/Rocket.Chat/pull/15196) by [@stleitner](https://github.com/stleitner))

- No Way to Display Password Policy on Password Reset Screen ([#16400](https://github.com/RocketChat/Rocket.Chat/pull/16400) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Not possible to translate the label of custom fields in user's Info ([#15595](https://github.com/RocketChat/Rocket.Chat/pull/15595) by [@antkaz](https://github.com/antkaz))

- Outgoing webhook: Excessive spacing between trigger words ([#17830](https://github.com/RocketChat/Rocket.Chat/pull/17830) by [@Karting06](https://github.com/Karting06))

- Profile save button not activates properly when changing the username field ([#16541](https://github.com/RocketChat/Rocket.Chat/pull/16541) by [@ritvikjain99](https://github.com/ritvikjain99))

- ReadOnly Rooms permission checks ([#17709](https://github.com/RocketChat/Rocket.Chat/pull/17709))

- Reorder hljs ([#17854](https://github.com/RocketChat/Rocket.Chat/pull/17854))

- Set `x-content-type-options: nosniff` header ([#16232](https://github.com/RocketChat/Rocket.Chat/pull/16232) by [@aviral243](https://github.com/aviral243))

- Some Login Buttons disappear after refreshing OAuth Services ([#17808](https://github.com/RocketChat/Rocket.Chat/pull/17808))

- Spotify embed link opens in same tab ([#13637](https://github.com/RocketChat/Rocket.Chat/pull/13637) by [@bhardwajaditya](https://github.com/bhardwajaditya))

- StreamCast stream to server only streamers ([#17942](https://github.com/RocketChat/Rocket.Chat/pull/17942))

- UI is not rendering when trying to edit an user ([#17972](https://github.com/RocketChat/Rocket.Chat/pull/17972))

- Undesirable message updates after user saving profile ([#17930](https://github.com/RocketChat/Rocket.Chat/pull/17930))

- Update AmazonS3 file upload with error handling and sync operation ([#10372](https://github.com/RocketChat/Rocket.Chat/pull/10372) by [@madhavmalhotra3089](https://github.com/madhavmalhotra3089))

- User can resend email verification if email is invalid or is empty ([#16095](https://github.com/RocketChat/Rocket.Chat/pull/16095) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- User is prompted to reset their password when logging with OAuth ([#18001](https://github.com/RocketChat/Rocket.Chat/pull/18001))

- Video conferences being started by users without permission ([#17948](https://github.com/RocketChat/Rocket.Chat/pull/17948))

- When the message is too long declining to send as an attachment does not restore the content into the composer ([#16332](https://github.com/RocketChat/Rocket.Chat/pull/16332))

<details>
<summary>🔍 Minor changes</summary>


- Add Apps to control GitHub issues ([#17807](https://github.com/RocketChat/Rocket.Chat/pull/17807))

- Add Apps-Engine to Engine Versions on History ([#17810](https://github.com/RocketChat/Rocket.Chat/pull/17810))

- Always initialize CIRCLE_BRANCH env var on CI ([#17874](https://github.com/RocketChat/Rocket.Chat/pull/17874))

- Bump websocket-extensions from 0.1.3 to 0.1.4 ([#17837](https://github.com/RocketChat/Rocket.Chat/pull/17837) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Change some components' location ([#17893](https://github.com/RocketChat/Rocket.Chat/pull/17893))

- Chatpal: limit results to current room ([#17718](https://github.com/RocketChat/Rocket.Chat/pull/17718) by [@mrsimpson](https://github.com/mrsimpson))

  Adds an option to Chatpal Search to limit results to the current room searched from

- Do not build Docker image for fork PRs ([#17370](https://github.com/RocketChat/Rocket.Chat/pull/17370))

- Federation performance and bug fixes ([#17504](https://github.com/RocketChat/Rocket.Chat/pull/17504) by [@hyfen](https://github.com/hyfen))

- Fix invalid develop payload to release service ([#17799](https://github.com/RocketChat/Rocket.Chat/pull/17799))

- Fix typo "coorosponding" ([#17840](https://github.com/RocketChat/Rocket.Chat/pull/17840) by [@toshokan](https://github.com/toshokan))

  Fix typo on English LDAP page

- Fix typo on Contributing.md ([#17769](https://github.com/RocketChat/Rocket.Chat/pull/17769) by [@onurtemiz](https://github.com/onurtemiz))

  Typo fixes on contributing page.

- Fixes some italian wording ([#14008](https://github.com/RocketChat/Rocket.Chat/pull/14008) by [@dadokkio](https://github.com/dadokkio))

- LDAP typo ([#17835](https://github.com/RocketChat/Rocket.Chat/pull/17835) by [@thomas-mc-work](https://github.com/thomas-mc-work))

- LingoHub based on develop ([#17796](https://github.com/RocketChat/Rocket.Chat/pull/17796))

- Merge master into develop & Set version to 3.4.0-develop ([#17764](https://github.com/RocketChat/Rocket.Chat/pull/17764) by [@lpilz](https://github.com/lpilz) & [@mtmr0x](https://github.com/mtmr0x))

- Readme: Update Raspberry Pi 2 to Pi 4 ([#17031](https://github.com/RocketChat/Rocket.Chat/pull/17031) by [@EwoutH](https://github.com/EwoutH))

- Refactor components and views to Storybook compatibility ([#17800](https://github.com/RocketChat/Rocket.Chat/pull/17800))

- Regresion: Issue with reply button on broadcast channels ([#18057](https://github.com/RocketChat/Rocket.Chat/pull/18057))

- Regression - Incoming WebHook messages not showing up on the channel ([#18005](https://github.com/RocketChat/Rocket.Chat/pull/18005))

- Regression - Unable to edit status on the Edit User panel of the admin ([#18032](https://github.com/RocketChat/Rocket.Chat/pull/18032))

- Regression: Admin User Edit panel is broken ([#17992](https://github.com/RocketChat/Rocket.Chat/pull/17992))

- Regression: App info broken ([#17979](https://github.com/RocketChat/Rocket.Chat/pull/17979))

- Regression: Cannot save avatar change on admin ([#17999](https://github.com/RocketChat/Rocket.Chat/pull/17999))

- Regression: Deprecate check permission on integrations ([#18024](https://github.com/RocketChat/Rocket.Chat/pull/18024))

- Regression: Favorite and Featured fields not triggering changes ([#18010](https://github.com/RocketChat/Rocket.Chat/pull/18010))

- Regression: Fix AWS S3 file retrieval ([#17982](https://github.com/RocketChat/Rocket.Chat/pull/17982))

- Regression: Fix exit-room on livechat ([#18067](https://github.com/RocketChat/Rocket.Chat/pull/18067))

- Regression: Fix mentions on thread preview ([#18071](https://github.com/RocketChat/Rocket.Chat/pull/18071))

- Regression: Fix setting reply-to email header ([#18008](https://github.com/RocketChat/Rocket.Chat/pull/18008))

- Regression: Fix threads badge color indicators ([#18048](https://github.com/RocketChat/Rocket.Chat/pull/18048))

- Regression: Fix update last message on delete ([#18077](https://github.com/RocketChat/Rocket.Chat/pull/18077))

- Regression: Fix wrong message grouping inside threads ([#18039](https://github.com/RocketChat/Rocket.Chat/pull/18039))

- Regression: Grouping Thread messages ([#18042](https://github.com/RocketChat/Rocket.Chat/pull/18042))

- Regression: Image Upload not working ([#17993](https://github.com/RocketChat/Rocket.Chat/pull/17993))

- Regression: Improve Omnichannel Business Hours ([#18050](https://github.com/RocketChat/Rocket.Chat/pull/18050) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Improve the logic to get request IPs ([#18033](https://github.com/RocketChat/Rocket.Chat/pull/18033))

- Regression: Infinite loop in CodeSettingInput ([#17949](https://github.com/RocketChat/Rocket.Chat/pull/17949))

- Regression: Infinite render loop on Setup Wizard ([#18074](https://github.com/RocketChat/Rocket.Chat/pull/18074))

- Regression: Only add reply-to if sender has emails ([#17998](https://github.com/RocketChat/Rocket.Chat/pull/17998))

- Regression: Repair CodeMirror component reactivity ([#18037](https://github.com/RocketChat/Rocket.Chat/pull/18037))

- Regression: Reset section button ([#18007](https://github.com/RocketChat/Rocket.Chat/pull/18007))

- Regression: Room flickering if open a thread ([#18004](https://github.com/RocketChat/Rocket.Chat/pull/18004))

- Regression: Wrong padding and colors on some tabs ([#18068](https://github.com/RocketChat/Rocket.Chat/pull/18068))

- Release 3.3.3 ([#17875](https://github.com/RocketChat/Rocket.Chat/pull/17875))

- Remove unused accounts-js integration ([#17921](https://github.com/RocketChat/Rocket.Chat/pull/17921))

- Remove useLazyRef hook usage ([#18003](https://github.com/RocketChat/Rocket.Chat/pull/18003))

- Revert "Regression: Fix wrong message grouping inside threads" ([#18043](https://github.com/RocketChat/Rocket.Chat/pull/18043))

- Submit a payload to the release service when a release happens ([#17775](https://github.com/RocketChat/Rocket.Chat/pull/17775))

- Update Dockerfile to not depend on custom base image ([#17802](https://github.com/RocketChat/Rocket.Chat/pull/17802))

- Update stale bot to v3 and run every 6 hours ([#17958](https://github.com/RocketChat/Rocket.Chat/pull/17958))

- Upgrade Livechat Widget version to 1.6.0 ([#18070](https://github.com/RocketChat/Rocket.Chat/pull/18070))

- Wrap Info Page components with React.memo ([#17899](https://github.com/RocketChat/Rocket.Chat/pull/17899))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AbhinavTalari](https://github.com/AbhinavTalari)
- [@ChrissW-R1](https://github.com/ChrissW-R1)
- [@Cleod9](https://github.com/Cleod9)
- [@EwoutH](https://github.com/EwoutH)
- [@InstinctBas](https://github.com/InstinctBas)
- [@Karting06](https://github.com/Karting06)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Siedlerchr](https://github.com/Siedlerchr)
- [@alexbartsch](https://github.com/alexbartsch)
- [@antkaz](https://github.com/antkaz)
- [@ashwaniYDV](https://github.com/ashwaniYDV)
- [@aviral243](https://github.com/aviral243)
- [@bhardwajaditya](https://github.com/bhardwajaditya)
- [@c0dzilla](https://github.com/c0dzilla)
- [@dadokkio](https://github.com/dadokkio)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@fthiery](https://github.com/fthiery)
- [@g-rauhoeft](https://github.com/g-rauhoeft)
- [@hyfen](https://github.com/hyfen)
- [@jazztickets](https://github.com/jazztickets)
- [@justinr1234](https://github.com/justinr1234)
- [@knrt10](https://github.com/knrt10)
- [@localguru](https://github.com/localguru)
- [@lpilz](https://github.com/lpilz)
- [@madhavmalhotra3089](https://github.com/madhavmalhotra3089)
- [@mariaeduardacunha](https://github.com/mariaeduardacunha)
- [@mohamedar97](https://github.com/mohamedar97)
- [@mrsimpson](https://github.com/mrsimpson)
- [@mtmr0x](https://github.com/mtmr0x)
- [@nitinkumartiwari](https://github.com/nitinkumartiwari)
- [@onurtemiz](https://github.com/onurtemiz)
- [@ritvikjain99](https://github.com/ritvikjain99)
- [@stleitner](https://github.com/stleitner)
- [@thomas-mc-work](https://github.com/thomas-mc-work)
- [@tonobo](https://github.com/tonobo)
- [@toshokan](https://github.com/toshokan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@Sing-Li](https://github.com/Sing-Li)
- [@alansikora](https://github.com/alansikora)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@lolimay](https://github.com/lolimay)
- [@murtaza98](https://github.com/murtaza98)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 3.3.3
`2020-06-11  ·  2 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.15.0`

<details>
<summary>🔍 Minor changes</summary>


- Always initialize CIRCLE_BRANCH env var on CI ([#17874](https://github.com/RocketChat/Rocket.Chat/pull/17874))

- Release 3.3.3 ([#17875](https://github.com/RocketChat/Rocket.Chat/pull/17875))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.3.2
`2020-06-10  ·  3 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.15.0`

<details>
<summary>🔍 Minor changes</summary>


- Fix invalid develop payload to release service ([#17799](https://github.com/RocketChat/Rocket.Chat/pull/17799))

- Release 3.3.2 ([#17870](https://github.com/RocketChat/Rocket.Chat/pull/17870))

- Submit a payload to the release service when a release happens ([#17775](https://github.com/RocketChat/Rocket.Chat/pull/17775))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@graywolf336](https://github.com/graywolf336)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.3.1
`2020-06-10  ·  8 🐛  ·  4 🔍  ·  10 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.15.0`

### 🐛 Bug fixes


- Administration User page blank opening users without email ([#17836](https://github.com/RocketChat/Rocket.Chat/pull/17836) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Apps room events losing data ([#17827](https://github.com/RocketChat/Rocket.Chat/pull/17827))

- Email link "go to message" being incorrectly escaped ([#17803](https://github.com/RocketChat/Rocket.Chat/pull/17803))

- Error when re-installing an App ([#17789](https://github.com/RocketChat/Rocket.Chat/pull/17789))

- Logic for room type was inverted on Admin panel (#17851) ([#17853](https://github.com/RocketChat/Rocket.Chat/pull/17853) by [@cking-vonix](https://github.com/cking-vonix))

  Fixed logic for public/private room types on room edit panel

- Omnichannel message link is broken in email notifications ([#17843](https://github.com/RocketChat/Rocket.Chat/pull/17843))

- SAML LogoutRequest sending wrong NameID ([#17860](https://github.com/RocketChat/Rocket.Chat/pull/17860))

- Slack importer settings object ([#17776](https://github.com/RocketChat/Rocket.Chat/pull/17776) by [@lpilz](https://github.com/lpilz))

<details>
<summary>🔍 Minor changes</summary>


- [REGRESSION] Omnichannel visitor forward was applying wrong restrictions ([#17826](https://github.com/RocketChat/Rocket.Chat/pull/17826) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix the update check not working ([#17809](https://github.com/RocketChat/Rocket.Chat/pull/17809))

- Release 3.3.1 ([#17865](https://github.com/RocketChat/Rocket.Chat/pull/17865) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@cking-vonix](https://github.com/cking-vonix) & [@lpilz](https://github.com/lpilz) & [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Update Apps-Engine version ([#17804](https://github.com/RocketChat/Rocket.Chat/pull/17804))

  Update Apps-Engine version

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@cking-vonix](https://github.com/cking-vonix)
- [@lpilz](https://github.com/lpilz)
- [@mariaeduardacunha](https://github.com/mariaeduardacunha)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@graywolf336](https://github.com/graywolf336)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.3.0
`2020-05-27  ·  20 🎉  ·  8 🚀  ·  41 🐛  ·  45 🔍  ·  37 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.14.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🎉 New features


- **APPS-ENGINE:** Essentials mechanism ([#17656](https://github.com/RocketChat/Rocket.Chat/pull/17656))

- **Apps-Engine:** New Livechat event handlers ([#17033](https://github.com/RocketChat/Rocket.Chat/pull/17033))

- **Apps-Engine:** New Room events ([#17487](https://github.com/RocketChat/Rocket.Chat/pull/17487))

- **ENTERPRISE:** Omnichannel Last-Chatted Agent Preferred option ([#17666](https://github.com/RocketChat/Rocket.Chat/pull/17666))

  If activated, this feature will store the last agent that assisted each Omnichannel visitor when a conversation is taken. So, when a visitor returns(it works with any entry point, Livechat, Facebook, REST API, and so on) and starts a new chat, the routing system checks:

  1   - The visitor object for any stored agent that the visitor has previously talked to;
  2   - If a previous agent is not found, the system will try to find a previous conversation of the same visitor. If a room is found, the system will get the previous agent from the room;

  After this process, if an agent has been found, the system will check the agent's availability to assist the new chat. If it's not available, then the routing system will get the next available agent in the queue.

- **ENTERPRISE:** Support for custom Livechat registration form fields ([#17581](https://github.com/RocketChat/Rocket.Chat/pull/17581))

- **ENTERPRISE:** Support Omnichannel conversations auditing ([#17692](https://github.com/RocketChat/Rocket.Chat/pull/17692))

- Add Livechat website URL to the offline message e-mail ([#17429](https://github.com/RocketChat/Rocket.Chat/pull/17429) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add permissions to deal with Omnichannel custom fields ([#17567](https://github.com/RocketChat/Rocket.Chat/pull/17567) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add Permissions to deal with Omnichannel visitor past chats history ([#17580](https://github.com/RocketChat/Rocket.Chat/pull/17580) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add the ability to send Livechat offline messages to a channel ([#17442](https://github.com/RocketChat/Rocket.Chat/pull/17442))

- Added "Add custom emoji" link to emoji picker ([#16250](https://github.com/RocketChat/Rocket.Chat/pull/16250))

- Added custom fields to Add/Edit user ([#17681](https://github.com/RocketChat/Rocket.Chat/pull/17681))

- Admin refactor  Second phase ([#17551](https://github.com/RocketChat/Rocket.Chat/pull/17551))

- Allow filtering Omnichannel analytics dashboards by department ([#17463](https://github.com/RocketChat/Rocket.Chat/pull/17463) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- API endpoint to fetch Omnichannel's room transfer history ([#17694](https://github.com/RocketChat/Rocket.Chat/pull/17694))

- Option to remove users from RocketChat if not found in Crowd ([#17619](https://github.com/RocketChat/Rocket.Chat/pull/17619) by [@ocanema](https://github.com/ocanema))

- Rewrite admin pages ([#17388](https://github.com/RocketChat/Rocket.Chat/pull/17388) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Screen Lock settings - mobile client ([#17523](https://github.com/RocketChat/Rocket.Chat/pull/17523) by [@djorkaeffalexandre](https://github.com/djorkaeffalexandre))

- Show user's status description by the usernames in messages list ([#14892](https://github.com/RocketChat/Rocket.Chat/pull/14892) by [@wreiske](https://github.com/wreiske))

  ![image](https://user-images.githubusercontent.com/6295044/60321979-5d191580-994c-11e9-9cd6-15f4565ff0ae.png)

- Unread bars on sidebar (#16853) ([#16862](https://github.com/RocketChat/Rocket.Chat/pull/16862) by [@juzser](https://github.com/juzser))

### 🚀 Improvements


- **Apps-Engine:** App user as the default notifier ([#17050](https://github.com/RocketChat/Rocket.Chat/pull/17050))

- Add env var to configure Chatpal URL and remove it from beta ([#16665](https://github.com/RocketChat/Rocket.Chat/pull/16665) by [@tkurz](https://github.com/tkurz))

- Add new webhooks to the Omnichannel integration feature ([#17503](https://github.com/RocketChat/Rocket.Chat/pull/17503))

- Added divider between tables and paginations ([#17680](https://github.com/RocketChat/Rocket.Chat/pull/17680))

- Always shows the exact match first on user's and room's autocomplete for mentions and on sidebar search ([#16394](https://github.com/RocketChat/Rocket.Chat/pull/16394))

- Display status information in the Omnichannel Agents list ([#17701](https://github.com/RocketChat/Rocket.Chat/pull/17701))

- Starred Messages ([#17685](https://github.com/RocketChat/Rocket.Chat/pull/17685))

- Unused styles ([#17554](https://github.com/RocketChat/Rocket.Chat/pull/17554))

### 🐛 Bug fixes


- Agent's custom fields being leaked through the Livechat configuration endpoint ([#17640](https://github.com/RocketChat/Rocket.Chat/pull/17640))

- Allow owners to react inside broadcast channels ([#17687](https://github.com/RocketChat/Rocket.Chat/pull/17687) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Avatar url provider ignoring subfolders ([#17675](https://github.com/RocketChat/Rocket.Chat/pull/17675))

- Can't click on room's actions menu of sidebar list when in search mode ([#16548](https://github.com/RocketChat/Rocket.Chat/pull/16548) by [@ritvikjain99](https://github.com/ritvikjain99))

- Change email verification label ([#17450](https://github.com/RocketChat/Rocket.Chat/pull/17450))

- Default filters on Omnichannel Current Chats screen not showing on first load ([#17522](https://github.com/RocketChat/Rocket.Chat/pull/17522))

- Directory search user placeholder ([#17652](https://github.com/RocketChat/Rocket.Chat/pull/17652) by [@zdumitru](https://github.com/zdumitru))

- Do not allow passwords on private channels ([#15642](https://github.com/RocketChat/Rocket.Chat/pull/15642))

- Elements of  "Personal Access Tokens" section out of alignment and unusable on very small screens ([#17129](https://github.com/RocketChat/Rocket.Chat/pull/17129) by [@Nikhil713](https://github.com/Nikhil713))

- Email configs not updating after setting changes ([#17578](https://github.com/RocketChat/Rocket.Chat/pull/17578))

- Emoji picker search broken ([#17570](https://github.com/RocketChat/Rocket.Chat/pull/17570))

- Error during data export for DMs ([#17577](https://github.com/RocketChat/Rocket.Chat/pull/17577) by [@mtmr0x](https://github.com/mtmr0x))

- Federation attachment URL for audio and video files ([#16430](https://github.com/RocketChat/Rocket.Chat/pull/16430) by [@qwertiko](https://github.com/qwertiko))

- Hyper.sh went out of business in early 2019 ([#17622](https://github.com/RocketChat/Rocket.Chat/pull/17622) by [@fbartels](https://github.com/fbartels))

- Increasing highlight time in 3 seconds ([#17540](https://github.com/RocketChat/Rocket.Chat/pull/17540) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Invalid CSS syntax ([#17541](https://github.com/RocketChat/Rocket.Chat/pull/17541))

- LDAP login on Enteprise Version ([#17508](https://github.com/RocketChat/Rocket.Chat/pull/17508))

- Login Forbidden on servers that had LDAP enabled in the past ([#17579](https://github.com/RocketChat/Rocket.Chat/pull/17579))

- Mail Messages > Cannot mail own user ([#17625](https://github.com/RocketChat/Rocket.Chat/pull/17625))

- Marketplace tiered pricing plan wording ([#17644](https://github.com/RocketChat/Rocket.Chat/pull/17644))

- Missing dropdown to select custom status color on user's profile ([#16537](https://github.com/RocketChat/Rocket.Chat/pull/16537) by [@ritwizsinha](https://github.com/ritwizsinha))

- Not redirecting to `First Channel After Login` on register ([#17664](https://github.com/RocketChat/Rocket.Chat/pull/17664))

- Notification sounds ([#17616](https://github.com/RocketChat/Rocket.Chat/pull/17616))

  * Global CDN config was ignored when loading the sound files

  * Upload of custom sounds wasn't getting the file extension correctly

  * Some translations were missing

  * Edit and delete of custom sounds were not working correctly

- Omnichannel departments are not saved when the offline channel name is not defined ([#17553](https://github.com/RocketChat/Rocket.Chat/pull/17553))

- Omnichannel room priorities system messages were create on every saved room info ([#17479](https://github.com/RocketChat/Rocket.Chat/pull/17479) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Password reset/change accepting current password as new password ([#16331](https://github.com/RocketChat/Rocket.Chat/pull/16331) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Push settings enabled when push gateway is selected ([#17582](https://github.com/RocketChat/Rocket.Chat/pull/17582))

- Queued Omnichannel webhook being triggered unnecessarily ([#17661](https://github.com/RocketChat/Rocket.Chat/pull/17661))

- Reactions may present empty names of who reacted when using Real Names ([#17536](https://github.com/RocketChat/Rocket.Chat/pull/17536))

  When changing usernames the reactions became outdated since it's not possible to update the usernames stored there, so when the server users Real Name setting enabled the system process all messages before return to the clients and get the names of the usernames to show since the usernames are outdated the names will not be found. Now the usernames will be displayed when the name can't be found as a temporary fix until we change the architecture of the data to fix the issue.

- Relative image path in oembededUrlWidget ([#15902](https://github.com/RocketChat/Rocket.Chat/pull/15902) by [@machester4](https://github.com/machester4))

- Remove a non working setting "Notification Duration" ([#15737](https://github.com/RocketChat/Rocket.Chat/pull/15737))

- Remove deprecated Omnichannel Knowledge Base feature ([#17387](https://github.com/RocketChat/Rocket.Chat/pull/17387))

- remove multiple options from dontAskMeAgain ([#17514](https://github.com/RocketChat/Rocket.Chat/pull/17514) by [@TaimurAzhar](https://github.com/TaimurAzhar))

- Replace obsolete X-FRAME-OPTIONS header on Livechat route ([#17419](https://github.com/RocketChat/Rocket.Chat/pull/17419) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace postcss Meteor package ([#15929](https://github.com/RocketChat/Rocket.Chat/pull/15929))

- Resolve 'app already exists' error on app update ([#17544](https://github.com/RocketChat/Rocket.Chat/pull/17544))

- SAML IDP initiated logout error ([#17482](https://github.com/RocketChat/Rocket.Chat/pull/17482))

- Secret Registration not properly validating Invite Token ([#17618](https://github.com/RocketChat/Rocket.Chat/pull/17618))

- Slack importer Link handling ([#17595](https://github.com/RocketChat/Rocket.Chat/pull/17595) by [@lpilz](https://github.com/lpilz))

- UI KIT Modal Width ([#17697](https://github.com/RocketChat/Rocket.Chat/pull/17697))

- Uncessary updates on Settings, Roles and Permissions on startup ([#17160](https://github.com/RocketChat/Rocket.Chat/pull/17160))

<details>
<summary>🔍 Minor changes</summary>


- Add engine versions for houston with templates ([#17403](https://github.com/RocketChat/Rocket.Chat/pull/17403))

- Add snapcraft files to be bumped with Houston ([#17611](https://github.com/RocketChat/Rocket.Chat/pull/17611))

- Add some missing metadata information ([#17524](https://github.com/RocketChat/Rocket.Chat/pull/17524))

- Bump jquery from 3.3.1 to 3.5.0 ([#17486](https://github.com/RocketChat/Rocket.Chat/pull/17486) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Deprecate compatibility cordova setting ([#17586](https://github.com/RocketChat/Rocket.Chat/pull/17586))

- DPlatform is deprecated and the replacement does not support rocket.chat ([#17040](https://github.com/RocketChat/Rocket.Chat/pull/17040) by [@ryjones](https://github.com/ryjones))

- Fix typo "You aren't part of any channel yet" ([#17498](https://github.com/RocketChat/Rocket.Chat/pull/17498) by [@huzaifahj](https://github.com/huzaifahj))

- Improve: New PR Template ([#16968](https://github.com/RocketChat/Rocket.Chat/pull/16968) by [@regalstreak](https://github.com/regalstreak))

- Improve: Remove index files from action-links, accounts and assets ([#17607](https://github.com/RocketChat/Rocket.Chat/pull/17607) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Improve: Remove uncessary RegExp query by email ([#17654](https://github.com/RocketChat/Rocket.Chat/pull/17654))

- LingoHub based on develop ([#17693](https://github.com/RocketChat/Rocket.Chat/pull/17693))

- LingoHub based on develop ([#17520](https://github.com/RocketChat/Rocket.Chat/pull/17520))

- Livechat iframe allow microphone and camera ([#9956](https://github.com/RocketChat/Rocket.Chat/pull/9956) by [@kolorafa](https://github.com/kolorafa))

- Merge master into develop & Set version to 3.3.0-develop ([#17468](https://github.com/RocketChat/Rocket.Chat/pull/17468))

- Meteor update to version 1.10.2 ([#17533](https://github.com/RocketChat/Rocket.Chat/pull/17533))

- RegExp improvements suggested by LGTM ([#17500](https://github.com/RocketChat/Rocket.Chat/pull/17500))

- Regression:  Fix error when performing Omnichannel queue checking ([#17700](https://github.com/RocketChat/Rocket.Chat/pull/17700))

- Regression: Add missing return to afterSaveMessage callbacks ([#17715](https://github.com/RocketChat/Rocket.Chat/pull/17715))

- Regression: Adjusting spaces between OAuth login buttons ([#17745](https://github.com/RocketChat/Rocket.Chat/pull/17745) by [@dudizilla](https://github.com/dudizilla))

- Regression: Click to join button not working ([#17705](https://github.com/RocketChat/Rocket.Chat/pull/17705))

- Regression: Do not show custom status inside sequential messages ([#17613](https://github.com/RocketChat/Rocket.Chat/pull/17613))

- Regression: Fix Avatar Url Provider when CDN_PREFIX_ALL is false ([#17542](https://github.com/RocketChat/Rocket.Chat/pull/17542))

- Regression: Fix error preventing creation of group DMs ([#17726](https://github.com/RocketChat/Rocket.Chat/pull/17726))

- Regression: Fix incorrect imports of the Apps-Engine ([#17695](https://github.com/RocketChat/Rocket.Chat/pull/17695))

- Regression: Fix Unread bar design ([#17750](https://github.com/RocketChat/Rocket.Chat/pull/17750) by [@dudizilla](https://github.com/dudizilla))

- Regression: Force unread-rooms bar to appears over the room list ([#17728](https://github.com/RocketChat/Rocket.Chat/pull/17728) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Regression: Integrations edit/history crashing ([#17702](https://github.com/RocketChat/Rocket.Chat/pull/17702))

- Regression: Outgoing List ([#17667](https://github.com/RocketChat/Rocket.Chat/pull/17667))

- Regression: Override via env for string settings not working ([#17576](https://github.com/RocketChat/Rocket.Chat/pull/17576))

- Regression: Pressing enter on search reloads the page - admin pages ([#17663](https://github.com/RocketChat/Rocket.Chat/pull/17663))

- Regression: RegExp callbacks of settings were not being called ([#17552](https://github.com/RocketChat/Rocket.Chat/pull/17552))

- Regression: Removed status border on mentions list ([#17741](https://github.com/RocketChat/Rocket.Chat/pull/17741) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Regression: Scroll on admin user info ([#17711](https://github.com/RocketChat/Rocket.Chat/pull/17711))

- Regression: Set retryWrites=false as default Mongo options ([#17683](https://github.com/RocketChat/Rocket.Chat/pull/17683))

- Regression: Status presence  color ([#17707](https://github.com/RocketChat/Rocket.Chat/pull/17707) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Regression: status-color-online ([#17684](https://github.com/RocketChat/Rocket.Chat/pull/17684))

- Regression: Threads list was fetching all threads ([#17716](https://github.com/RocketChat/Rocket.Chat/pull/17716))

- Regression: User edit form missing fields ([#17699](https://github.com/RocketChat/Rocket.Chat/pull/17699))

- Release 3.2.2 ([#17600](https://github.com/RocketChat/Rocket.Chat/pull/17600) by [@mtmr0x](https://github.com/mtmr0x))

- Remove unnecessary setting redefinition ([#17587](https://github.com/RocketChat/Rocket.Chat/pull/17587))

- Update Apps-Engine version ([#17706](https://github.com/RocketChat/Rocket.Chat/pull/17706))

- Update Contributing Guide ([#17653](https://github.com/RocketChat/Rocket.Chat/pull/17653))

- Update Fuselage version ([#17708](https://github.com/RocketChat/Rocket.Chat/pull/17708))

- Upgrade Livechat Widget version to 1.5.0 ([#17710](https://github.com/RocketChat/Rocket.Chat/pull/17710))

- Use Users.findOneByAppId instead of querying directly ([#16480](https://github.com/RocketChat/Rocket.Chat/pull/16480))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Nikhil713](https://github.com/Nikhil713)
- [@TaimurAzhar](https://github.com/TaimurAzhar)
- [@ashwaniYDV](https://github.com/ashwaniYDV)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@djorkaeffalexandre](https://github.com/djorkaeffalexandre)
- [@dudizilla](https://github.com/dudizilla)
- [@fbartels](https://github.com/fbartels)
- [@huzaifahj](https://github.com/huzaifahj)
- [@juzser](https://github.com/juzser)
- [@kolorafa](https://github.com/kolorafa)
- [@lpilz](https://github.com/lpilz)
- [@machester4](https://github.com/machester4)
- [@mariaeduardacunha](https://github.com/mariaeduardacunha)
- [@mtmr0x](https://github.com/mtmr0x)
- [@ocanema](https://github.com/ocanema)
- [@qwertiko](https://github.com/qwertiko)
- [@regalstreak](https://github.com/regalstreak)
- [@ritvikjain99](https://github.com/ritvikjain99)
- [@ritwizsinha](https://github.com/ritwizsinha)
- [@ryjones](https://github.com/ryjones)
- [@tkurz](https://github.com/tkurz)
- [@wreiske](https://github.com/wreiske)
- [@zdumitru](https://github.com/zdumitru)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)
- [@thassiov](https://github.com/thassiov)

# 3.2.2
`2020-05-11  ·  7 🐛  ·  1 🔍  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Email configs not updating after setting changes ([#17578](https://github.com/RocketChat/Rocket.Chat/pull/17578))

- Emoji picker search broken ([#17570](https://github.com/RocketChat/Rocket.Chat/pull/17570))

- Error during data export for DMs ([#17577](https://github.com/RocketChat/Rocket.Chat/pull/17577) by [@mtmr0x](https://github.com/mtmr0x))

- LDAP login on Enteprise Version ([#17508](https://github.com/RocketChat/Rocket.Chat/pull/17508))

- Login Forbidden on servers that had LDAP enabled in the past ([#17579](https://github.com/RocketChat/Rocket.Chat/pull/17579))

- Push settings enabled when push gateway is selected ([#17582](https://github.com/RocketChat/Rocket.Chat/pull/17582))

- Reactions may present empty names of who reacted when using Real Names ([#17536](https://github.com/RocketChat/Rocket.Chat/pull/17536))

  When changing usernames the reactions became outdated since it's not possible to update the usernames stored there, so when the server users Real Name setting enabled the system process all messages before return to the clients and get the names of the usernames to show since the usernames are outdated the names will not be found. Now the usernames will be displayed when the name can't be found as a temporary fix until we change the architecture of the data to fix the issue.

<details>
<summary>🔍 Minor changes</summary>


- Release 3.2.2 ([#17600](https://github.com/RocketChat/Rocket.Chat/pull/17600) by [@mtmr0x](https://github.com/mtmr0x))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@mtmr0x](https://github.com/mtmr0x)

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.2.1
`2020-05-01  ·  1 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- LDAP login error on Enterprise version ([#17497](https://github.com/RocketChat/Rocket.Chat/pull/17497))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.2.1 ([#17506](https://github.com/RocketChat/Rocket.Chat/pull/17506))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.2.0
`2020-04-27  ·  19 🎉  ·  10 🚀  ·  34 🐛  ·  19 🔍  ·  34 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🎉 New features


- **ENTERPRISE:** Allows to set a group of departments accepted for forwarding chats ([#17335](https://github.com/RocketChat/Rocket.Chat/pull/17335) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- **ENTERPRISE:** Auto close abandoned Omnichannel rooms ([#17055](https://github.com/RocketChat/Rocket.Chat/pull/17055) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- **ENTERPRISE:** Omnichannel queue priorities ([#17141](https://github.com/RocketChat/Rocket.Chat/pull/17141) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- **ENTERPRISE:** Restrict the permissions configuration for guest users  ([#17333](https://github.com/RocketChat/Rocket.Chat/pull/17333))

  The **Guest** role is blocked for edition on the EE version. This will allow the EE customers to receive licenses with extra seats for Guests for free. The CE version continues to have the Guest role configurable.

- Add ability to set tags in the Omnichannel room closing dialog ([#17254](https://github.com/RocketChat/Rocket.Chat/pull/17254))

- Add Color variable to left sidebar ([#16806](https://github.com/RocketChat/Rocket.Chat/pull/16806))

- Add MMS support to Voxtelesys ([#17217](https://github.com/RocketChat/Rocket.Chat/pull/17217) by [@john08burke](https://github.com/john08burke))

- Adds ability for Rocket.Chat Apps to create discussions ([#16683](https://github.com/RocketChat/Rocket.Chat/pull/16683))

- Allow to send Agent custom fields through the Omnichannel CRM integration ([#16286](https://github.com/RocketChat/Rocket.Chat/pull/16286))

- Allow to set a comment when forwarding Omnichannel chats ([#17353](https://github.com/RocketChat/Rocket.Chat/pull/17353))

- Better Push and Email Notification logic ([#17357](https://github.com/RocketChat/Rocket.Chat/pull/17357))

  We are still using the same logic to define which notifications every new message will generate, it takes some servers' settings, users's preferences and subscriptions' settings in consideration to determine who will receive each notification type (desktop, audio, email and mobile push), but now it doesn't check the user's status (online, away, offline) for email and mobile push notifications but send those notifications to a new queue with the following rules:


  - When the user is online the notification is scheduled to be sent in 120 seconds

  - When the user is away the notification is scheduled to be sent in 120 seconds minus the amount of time he is away

  - When the user is offline the notification is scheduled to be sent right away

  - When the user reads a channel all the notifications for that user are removed (clear queue)

  - When a notification is processed to be sent to a user and there are other scheduled notifications:
    - All the scheduled notifications for that user are rescheduled to now
    - The current notification goes back to the queue to be processed ordered by creation date

- Buttons to check/uncheck all users and channels on import ([#17207](https://github.com/RocketChat/Rocket.Chat/pull/17207))

- Default favorite channels ([#16025](https://github.com/RocketChat/Rocket.Chat/pull/16025))

- Enable the IDP to choose the best authnContext ([#17222](https://github.com/RocketChat/Rocket.Chat/pull/17222) by [@felipecrp](https://github.com/felipecrp))

- Error page when browser is not supported ([#17372](https://github.com/RocketChat/Rocket.Chat/pull/17372))

- Feature/custom oauth mail field and interpolation for mapped fields ([#15690](https://github.com/RocketChat/Rocket.Chat/pull/15690) by [@benkroeger](https://github.com/benkroeger))

- Federation event for when users left rooms ([#17091](https://github.com/RocketChat/Rocket.Chat/pull/17091))

- Make the header for rooms clickable ([#16762](https://github.com/RocketChat/Rocket.Chat/pull/16762) by [@aKn1ghtOut](https://github.com/aKn1ghtOut))

- Support importing Slack threads ([#17130](https://github.com/RocketChat/Rocket.Chat/pull/17130) by [@lpilz](https://github.com/lpilz))

### 🚀 Improvements


- Add `file-title` and `file-desc` as new filter tag options on message search ([#16858](https://github.com/RocketChat/Rocket.Chat/pull/16858) by [@subham103](https://github.com/subham103))

- Add possibility to sort the Omnichannel current chats list by column ([#17347](https://github.com/RocketChat/Rocket.Chat/pull/17347) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Administration -> Mailer Rewrite. ([#17191](https://github.com/RocketChat/Rocket.Chat/pull/17191))

- Administration Pages root rewritten ([#17209](https://github.com/RocketChat/Rocket.Chat/pull/17209))

- Change the SAML metadata order to conform to XSD specification ([#15488](https://github.com/RocketChat/Rocket.Chat/pull/15488) by [@fcrespo82](https://github.com/fcrespo82))

- Filter markdown in notifications ([#9995](https://github.com/RocketChat/Rocket.Chat/pull/9995) by [@c0dzilla](https://github.com/c0dzilla))

- Increase decoupling between React components and Blaze templates ([#16642](https://github.com/RocketChat/Rocket.Chat/pull/16642))

- Move CSS imports to `/app` modules ([#17261](https://github.com/RocketChat/Rocket.Chat/pull/17261))

- Redesign Administration > Import ([#17289](https://github.com/RocketChat/Rocket.Chat/pull/17289))

- User gets UI feedback when message is pinned or unpinned ([#16056](https://github.com/RocketChat/Rocket.Chat/pull/16056) by [@ashwaniYDV](https://github.com/ashwaniYDV))

### 🐛 Bug fixes


- "Invalid Invite" message when registration is disabled ([#17226](https://github.com/RocketChat/Rocket.Chat/pull/17226))

- 2FA not showing codes for Spanish translation ([#17378](https://github.com/RocketChat/Rocket.Chat/pull/17378) by [@RavenSystem](https://github.com/RavenSystem))

- 404 error when clicking an username ([#17275](https://github.com/RocketChat/Rocket.Chat/pull/17275))

- Admin panel custom sounds, multiple sound playback fix and added single play/pause button ([#16215](https://github.com/RocketChat/Rocket.Chat/pull/16215) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Allow Screensharing in BBB Iframe ([#17290](https://github.com/RocketChat/Rocket.Chat/pull/17290) by [@wolbernd](https://github.com/wolbernd))

- Avatar on sidebar when showing real names ([#17286](https://github.com/RocketChat/Rocket.Chat/pull/17286))

- Can not save Unread Tray Icon Alert user preference ([#16313](https://github.com/RocketChat/Rocket.Chat/pull/16313) by [@taiju271](https://github.com/taiju271))

- Change wording to start DM from info panel ([#8799](https://github.com/RocketChat/Rocket.Chat/pull/8799))

- CSV Importer fails when there are no users to import ([#16790](https://github.com/RocketChat/Rocket.Chat/pull/16790))

- Directory default tab ([#17283](https://github.com/RocketChat/Rocket.Chat/pull/17283))

- Discussions created from inside DMs were not working and some errors accessing recently created rooms ([#17282](https://github.com/RocketChat/Rocket.Chat/pull/17282))

- Email not verified message ([#16236](https://github.com/RocketChat/Rocket.Chat/pull/16236))

- Fixed email sort button in directory -> users ([#16606](https://github.com/RocketChat/Rocket.Chat/pull/16606) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Global event click-message-link not fired ([#16771](https://github.com/RocketChat/Rocket.Chat/pull/16771))

- Import slack's multiple direct messages as direct rooms instead of private groups ([#17206](https://github.com/RocketChat/Rocket.Chat/pull/17206))

- In Create a New Channel, input should be focused on channel name instead of invite users ([#16405](https://github.com/RocketChat/Rocket.Chat/pull/16405) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- LDAP users lose session on refresh ([#17302](https://github.com/RocketChat/Rocket.Chat/pull/17302))

- No maxlength(120) defined for custom user status ([#16534](https://github.com/RocketChat/Rocket.Chat/pull/16534) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Omnichannel SMS / WhatsApp integration errors due to missing location data ([#17288](https://github.com/RocketChat/Rocket.Chat/pull/17288))

- Popover component doesn't have scroll ([#17198](https://github.com/RocketChat/Rocket.Chat/pull/17198) by [@Nikhil713](https://github.com/Nikhil713))

- Prevent user from getting stuck on login, if there is some bad fname ([#17331](https://github.com/RocketChat/Rocket.Chat/pull/17331))

- Red color error outline is not removed after password update on profile details ([#16536](https://github.com/RocketChat/Rocket.Chat/pull/16536) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Remove properties from users.info response ([#17238](https://github.com/RocketChat/Rocket.Chat/pull/17238) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- SAML assertion signature enforcement ([#17278](https://github.com/RocketChat/Rocket.Chat/pull/17278))

- SAML Idp Initiated Logout Error ([#17324](https://github.com/RocketChat/Rocket.Chat/pull/17324))

- Search valid for emoji with dual name ([#16887](https://github.com/RocketChat/Rocket.Chat/pull/16887) by [@subham103](https://github.com/subham103))

- Show active admin and user account menu item ([#17047](https://github.com/RocketChat/Rocket.Chat/pull/17047) by [@hullen](https://github.com/hullen))

- Spotify embed and collapsed ([#17356](https://github.com/RocketChat/Rocket.Chat/pull/17356) by [@ffauvel](https://github.com/ffauvel))

- Threads: Hide Usernames hides Full names. ([#16959](https://github.com/RocketChat/Rocket.Chat/pull/16959))

- Translation for nl ([#16742](https://github.com/RocketChat/Rocket.Chat/pull/16742) by [@CC007](https://github.com/CC007))

- Unsafe React portals mount/unmount  ([#17265](https://github.com/RocketChat/Rocket.Chat/pull/17265))

- Update ru.i18n.json ([#16869](https://github.com/RocketChat/Rocket.Chat/pull/16869) by [@1rV1N-git](https://github.com/1rV1N-git))

- User search on directory not working correctly ([#17299](https://github.com/RocketChat/Rocket.Chat/pull/17299))

- Variable rendering problem on Import recent history page ([#15997](https://github.com/RocketChat/Rocket.Chat/pull/15997) by [@ritwizsinha](https://github.com/ritwizsinha))

<details>
<summary>🔍 Minor changes</summary>


- [CHORE] Move polyfills to client/ ([#17266](https://github.com/RocketChat/Rocket.Chat/pull/17266))

- Apply $and helper to message template ([#17280](https://github.com/RocketChat/Rocket.Chat/pull/17280))

- Bump https-proxy-agent from 2.2.1 to 2.2.4 ([#17323](https://github.com/RocketChat/Rocket.Chat/pull/17323) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Complement Guest role restrictions for Enterprise ([#17393](https://github.com/RocketChat/Rocket.Chat/pull/17393))

- Fix moving-to-a-single-codebase link in README ([#17297](https://github.com/RocketChat/Rocket.Chat/pull/17297) by [@Krinkle](https://github.com/Krinkle))

- Improve: Better Push Notification code ([#17338](https://github.com/RocketChat/Rocket.Chat/pull/17338))

- LingoHub based on develop ([#17365](https://github.com/RocketChat/Rocket.Chat/pull/17365))

- LingoHub based on develop ([#17274](https://github.com/RocketChat/Rocket.Chat/pull/17274))

- Mailer Scrollbar ([#17322](https://github.com/RocketChat/Rocket.Chat/pull/17322))

- Merge master into develop & Set version to 3.2.0-develop ([#17241](https://github.com/RocketChat/Rocket.Chat/pull/17241) by [@1rV1N-git](https://github.com/1rV1N-git))

- New hooks for RouterContext ([#17305](https://github.com/RocketChat/Rocket.Chat/pull/17305))

- Regression: Import data pagination ([#17355](https://github.com/RocketChat/Rocket.Chat/pull/17355))

- Regression: Storybook ([#17321](https://github.com/RocketChat/Rocket.Chat/pull/17321))

- Release 3.1.2 ([#17454](https://github.com/RocketChat/Rocket.Chat/pull/17454) by [@fastrde](https://github.com/fastrde))

- Remove `@typescript-eslint/explicit-function-return-type` rule ([#17428](https://github.com/RocketChat/Rocket.Chat/pull/17428))

- Remove set as alias setting ([#16343](https://github.com/RocketChat/Rocket.Chat/pull/16343))

- Static props for Administration route components ([#17285](https://github.com/RocketChat/Rocket.Chat/pull/17285))

- Update Apps-Engine to stable version ([#17287](https://github.com/RocketChat/Rocket.Chat/pull/17287))

- Upgrade file storage packages ([#17107](https://github.com/RocketChat/Rocket.Chat/pull/17107))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@1rV1N-git](https://github.com/1rV1N-git)
- [@CC007](https://github.com/CC007)
- [@Krinkle](https://github.com/Krinkle)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Nikhil713](https://github.com/Nikhil713)
- [@RavenSystem](https://github.com/RavenSystem)
- [@aKn1ghtOut](https://github.com/aKn1ghtOut)
- [@ashwaniYDV](https://github.com/ashwaniYDV)
- [@benkroeger](https://github.com/benkroeger)
- [@c0dzilla](https://github.com/c0dzilla)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@fastrde](https://github.com/fastrde)
- [@fcrespo82](https://github.com/fcrespo82)
- [@felipecrp](https://github.com/felipecrp)
- [@ffauvel](https://github.com/ffauvel)
- [@hullen](https://github.com/hullen)
- [@john08burke](https://github.com/john08burke)
- [@lpilz](https://github.com/lpilz)
- [@ritwizsinha](https://github.com/ritwizsinha)
- [@subham103](https://github.com/subham103)
- [@taiju271](https://github.com/taiju271)
- [@wolbernd](https://github.com/wolbernd)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@alansikora](https://github.com/alansikora)
- [@d-gubert](https://github.com/d-gubert)
- [@gabriellsh](https://github.com/gabriellsh)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 3.1.3
`2020-05-11  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Email configs not updating after setting changes ([#17578](https://github.com/RocketChat/Rocket.Chat/pull/17578))

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)

# 3.1.2
`2020-04-27  ·  8 🐛  ·  3 🔍  ·  5 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Allowing blocking a user on channels ([#17406](https://github.com/RocketChat/Rocket.Chat/pull/17406))

- Bot Agents not being able to get Omnichannel Inquiries ([#17404](https://github.com/RocketChat/Rocket.Chat/pull/17404))

- Empty Incoming webhook script field  ([#17422](https://github.com/RocketChat/Rocket.Chat/pull/17422))

- LDAP error when trying to add room with spaces in the name ([#17453](https://github.com/RocketChat/Rocket.Chat/pull/17453))

- LDAP Sync error ([#17417](https://github.com/RocketChat/Rocket.Chat/pull/17417) by [@fastrde](https://github.com/fastrde))

- New user added by admin doesn't receive random password email ([#17249](https://github.com/RocketChat/Rocket.Chat/pull/17249))

- Omnichannel room info panel opening whenever a message is sent ([#17348](https://github.com/RocketChat/Rocket.Chat/pull/17348))

- Web Client memory leak caused by the Emoji rendering ([#17320](https://github.com/RocketChat/Rocket.Chat/pull/17320))

<details>
<summary>🔍 Minor changes</summary>


- Regression: Add missing cacheKey to mem ([#17430](https://github.com/RocketChat/Rocket.Chat/pull/17430))

- Regression: Fix mem usage with more than one argument ([#17391](https://github.com/RocketChat/Rocket.Chat/pull/17391))

- Release 3.1.2 ([#17454](https://github.com/RocketChat/Rocket.Chat/pull/17454) by [@fastrde](https://github.com/fastrde))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@fastrde](https://github.com/fastrde)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.1.1
`2020-04-14  ·  8 🐛  ·  1 🔍  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- 404 error when clicking an username ([#17275](https://github.com/RocketChat/Rocket.Chat/pull/17275))

- Avatar on sidebar when showing real names ([#17286](https://github.com/RocketChat/Rocket.Chat/pull/17286))

- Directory default tab ([#17283](https://github.com/RocketChat/Rocket.Chat/pull/17283))

- Discussions created from inside DMs were not working and some errors accessing recently created rooms ([#17282](https://github.com/RocketChat/Rocket.Chat/pull/17282))

- LDAP users lose session on refresh ([#17302](https://github.com/RocketChat/Rocket.Chat/pull/17302))

- Omnichannel SMS / WhatsApp integration errors due to missing location data ([#17288](https://github.com/RocketChat/Rocket.Chat/pull/17288))

- SAML assertion signature enforcement ([#17278](https://github.com/RocketChat/Rocket.Chat/pull/17278))

- User search on directory not working correctly ([#17299](https://github.com/RocketChat/Rocket.Chat/pull/17299))

<details>
<summary>🔍 Minor changes</summary>


- Update Apps-Engine to stable version ([#17287](https://github.com/RocketChat/Rocket.Chat/pull/17287))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@ggazzo](https://github.com/ggazzo)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.1.0
`2020-04-09  ·  23 🎉  ·  22 🚀  ·  71 🐛  ·  86 🔍  ·  41 👩‍💻👨‍💻`

### Engine versions
- Node: `12.16.1`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🎉 New features


- **ENTERPRISE:** Engagement Dashboard ([#16960](https://github.com/RocketChat/Rocket.Chat/pull/16960) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add default chat closing tags in Omnichannel departments ([#16859](https://github.com/RocketChat/Rocket.Chat/pull/16859))

- Add omnichannel external frame feature ([#17038](https://github.com/RocketChat/Rocket.Chat/pull/17038))

- Add update method for user bridge ([#17077](https://github.com/RocketChat/Rocket.Chat/pull/17077))

- Allow to set default department and location sharing on SMS / WhatsApp integration ([#16557](https://github.com/RocketChat/Rocket.Chat/pull/16557))

- API `users.deactivateIdle` for mass-disabling of idle users ([#16849](https://github.com/RocketChat/Rocket.Chat/pull/16849))

- API `users.logoutOtherClient` to logout from other locations ([#16193](https://github.com/RocketChat/Rocket.Chat/pull/16193) by [@jschirrmacher](https://github.com/jschirrmacher))

- Direct message between multiple users ([#16761](https://github.com/RocketChat/Rocket.Chat/pull/16761))

- Directory page refactored, new user's bio field ([#17043](https://github.com/RocketChat/Rocket.Chat/pull/17043))

- Enterprise Edition ([#16944](https://github.com/RocketChat/Rocket.Chat/pull/16944))

- Experimental Game Center (externalComponents implementation) ([#15123](https://github.com/RocketChat/Rocket.Chat/pull/15123))

- Home button on sidebar ([#17052](https://github.com/RocketChat/Rocket.Chat/pull/17052))

- Merge Sort List and View Mode menus and improve its UI/UX ([#17103](https://github.com/RocketChat/Rocket.Chat/pull/17103))

  ![image](https://user-images.githubusercontent.com/5263975/78036622-e8db2a80-7340-11ea-91d0-65728eabdcb6.png)

- Open the Visitor Info panel automatically when the agent enters an Omnichannel room ([#16496](https://github.com/RocketChat/Rocket.Chat/pull/16496))

- Route to get updated roles after a date ([#16610](https://github.com/RocketChat/Rocket.Chat/pull/16610) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@ashwaniYDV](https://github.com/ashwaniYDV))

- SAML config to allow clock drift ([#16751](https://github.com/RocketChat/Rocket.Chat/pull/16751) by [@localguru](https://github.com/localguru))

- Save default filters in the Omnichannel Current Chats list ([#16653](https://github.com/RocketChat/Rocket.Chat/pull/16653))

- Settings to enable E2E encryption for Private and Direct Rooms by default ([#16928](https://github.com/RocketChat/Rocket.Chat/pull/16928))

- Sort channel directory listing by latest message ([#16604](https://github.com/RocketChat/Rocket.Chat/pull/16604) by [@subham103](https://github.com/subham103))

- Synchronize saml roles to local user (#16152) ([#16158](https://github.com/RocketChat/Rocket.Chat/pull/16158) by [@col-panic](https://github.com/col-panic))

- Translation via MS translate ([#16363](https://github.com/RocketChat/Rocket.Chat/pull/16363) by [@mrsimpson](https://github.com/mrsimpson))

  Adds Microsoft's translation service (https://translator.microsoft.com/) as a provider for translation of messages.
  In addition to implementing the interface (similar to google and DeepL), a small change has been done in order to display the translation provider on the UI.

- Two Factor authentication via email ([#15949](https://github.com/RocketChat/Rocket.Chat/pull/15949))

- Update Meteor to 1.9.2 and Node to 12.16.1 ([#16718](https://github.com/RocketChat/Rocket.Chat/pull/16718))

### 🚀 Improvements


- Ability to change offline message button link on emails notifications ([#16784](https://github.com/RocketChat/Rocket.Chat/pull/16784) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Accept open formarts of text, spreadsheet, presentation for upload by default ([#16502](https://github.com/RocketChat/Rocket.Chat/pull/16502))

- Add option to require authentication on user's shield endpoint ([#16845](https://github.com/RocketChat/Rocket.Chat/pull/16845) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added autofocus to Directory ([#16217](https://github.com/RocketChat/Rocket.Chat/pull/16217) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Added timer in video message recorder ([#16221](https://github.com/RocketChat/Rocket.Chat/pull/16221) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Allow login of non LDAP users when LDAP is enabled ([#16949](https://github.com/RocketChat/Rocket.Chat/pull/16949))

- Apps Engine: Reduce some stream calls and remove a find user from the app's status changes ([#17115](https://github.com/RocketChat/Rocket.Chat/pull/17115))

- Change sidebar sort mode to activity by default ([#17189](https://github.com/RocketChat/Rocket.Chat/pull/17189))

- Contextual bar autofocus ([#16915](https://github.com/RocketChat/Rocket.Chat/pull/16915))

- Displays `Nothing found` on admin sidebar when search returns nothing ([#16255](https://github.com/RocketChat/Rocket.Chat/pull/16255) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Fallback content-type as application/octet-stream for FileSystem uploads ([#16776](https://github.com/RocketChat/Rocket.Chat/pull/16776) by [@georgmu](https://github.com/georgmu))

- First data load from existing data on engagement dashboard ([#17035](https://github.com/RocketChat/Rocket.Chat/pull/17035) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Increase the push throughput to prevent queuing ([#17194](https://github.com/RocketChat/Rocket.Chat/pull/17194))

- Omnichannel aggregations performance improvements ([#16755](https://github.com/RocketChat/Rocket.Chat/pull/16755) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Removed the 'reply in thread' from thread replies ([#16630](https://github.com/RocketChat/Rocket.Chat/pull/16630) by [@ritwizsinha](https://github.com/ritwizsinha))

- Rename client-side term "Livechat" to "Omnichannel" ([#16752](https://github.com/RocketChat/Rocket.Chat/pull/16752))

- Repeat “Reply In Thread” and “Add Reaction” inside the message actions menu ([#17073](https://github.com/RocketChat/Rocket.Chat/pull/17073))

- Replace the Department select component by an Autocomplete input in Omnichannel UI ([#16669](https://github.com/RocketChat/Rocket.Chat/pull/16669))

- Send files over REST API ([#16617](https://github.com/RocketChat/Rocket.Chat/pull/16617))

- Tab Bar actions reorder ([#17072](https://github.com/RocketChat/Rocket.Chat/pull/17072))

- Use `rocket.cat` as default bot If `InternalHubot_Username` is undefined ([#16371](https://github.com/RocketChat/Rocket.Chat/pull/16371) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- User gets feedback when a message has been starred or unstarred ([#13860](https://github.com/RocketChat/Rocket.Chat/pull/13860) by [@fliptrail](https://github.com/fliptrail))

### 🐛 Bug fixes


- "Jump to message" is rendered twice when message is starred. ([#16170](https://github.com/RocketChat/Rocket.Chat/pull/16170) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- `users.setStatus` API was ignoring the user from params when trying to set status of other users ([#16128](https://github.com/RocketChat/Rocket.Chat/pull/16128) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@rm-yakovenko](https://github.com/rm-yakovenko))

- Additional scroll when contextual bar is open ([#16667](https://github.com/RocketChat/Rocket.Chat/pull/16667))

- Admin height if the blue banner is opened ([#16629](https://github.com/RocketChat/Rocket.Chat/pull/16629))

- Admins can't sort users by email in directory view ([#15796](https://github.com/RocketChat/Rocket.Chat/pull/15796) by [@sneakson](https://github.com/sneakson))

- Ancestral departments were not updated when an Omnichannel room is forwarded to another department ([#16958](https://github.com/RocketChat/Rocket.Chat/pull/16958))

- Block user option inside admin view ([#16626](https://github.com/RocketChat/Rocket.Chat/pull/16626))

- Cannot edit Profile when Full Name is empty and not required ([#16744](https://github.com/RocketChat/Rocket.Chat/pull/16744))

- Cannot pin on direct messages ([#16759](https://github.com/RocketChat/Rocket.Chat/pull/16759) by [@ritwizsinha](https://github.com/ritwizsinha))

- Cannot unfollow message from thread's panel ([#16560](https://github.com/RocketChat/Rocket.Chat/pull/16560) by [@subham103](https://github.com/subham103))

- CAS ignores username attribute map ([#16942](https://github.com/RocketChat/Rocket.Chat/pull/16942) by [@pmayer](https://github.com/pmayer))

- Check agent status when starting a new conversation with an agent assigned ([#16618](https://github.com/RocketChat/Rocket.Chat/pull/16618))

- Clear unread red line when the ESC key is pressed  ([#16668](https://github.com/RocketChat/Rocket.Chat/pull/16668))

- Color setting editing issues ([#16706](https://github.com/RocketChat/Rocket.Chat/pull/16706))

- Custom OAuth Bug ([#16811](https://github.com/RocketChat/Rocket.Chat/pull/16811))

- Data converters overriding fields added by apps ([#16639](https://github.com/RocketChat/Rocket.Chat/pull/16639))

- Deleting messages while searching causes the whole room chat to disappear ([#16568](https://github.com/RocketChat/Rocket.Chat/pull/16568) by [@karimelghazouly](https://github.com/karimelghazouly))

- Discussions were not inheriting the public status of parent's channel ([#17070](https://github.com/RocketChat/Rocket.Chat/pull/17070))

- Display user status along with icon ([#16875](https://github.com/RocketChat/Rocket.Chat/pull/16875) by [@Nikhil713](https://github.com/Nikhil713))

- Emit livechat events to instace only ([#17086](https://github.com/RocketChat/Rocket.Chat/pull/17086))

- Error when websocket received status update event ([#17089](https://github.com/RocketChat/Rocket.Chat/pull/17089))

- Explicitly set text of confirmation button ([#16138](https://github.com/RocketChat/Rocket.Chat/pull/16138) by [@jschirrmacher](https://github.com/jschirrmacher))

- Facebook integration missing visitor data after registerGuest ([#16810](https://github.com/RocketChat/Rocket.Chat/pull/16810) by [@antkaz](https://github.com/antkaz))

- Federation delete room event not being dispatched ([#16861](https://github.com/RocketChat/Rocket.Chat/pull/16861) by [@1rV1N-git](https://github.com/1rV1N-git))

- Federation Event ROOM_ADD_USER not being dispatched ([#16878](https://github.com/RocketChat/Rocket.Chat/pull/16878) by [@1rV1N-git](https://github.com/1rV1N-git))

- File uploads out of threads are not visible in regular message view ([#16416](https://github.com/RocketChat/Rocket.Chat/pull/16416) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Flextab information is not working when clicking on visitor or agent username in Omnichannel messages ([#16797](https://github.com/RocketChat/Rocket.Chat/pull/16797) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- ie11 support ([#16682](https://github.com/RocketChat/Rocket.Chat/pull/16682))

- Integrations page pagination ([#16838](https://github.com/RocketChat/Rocket.Chat/pull/16838))

- Invite links counting users already joined ([#16591](https://github.com/RocketChat/Rocket.Chat/pull/16591) by [@ritwizsinha](https://github.com/ritwizsinha))

- Keeps the agent in the room after accepting a new Omnichannel request ([#16787](https://github.com/RocketChat/Rocket.Chat/pull/16787))

- Language country has been ignored on translation load ([#16757](https://github.com/RocketChat/Rocket.Chat/pull/16757))

  Languages including country variations like `pt-BR` were ignoring the country party because the user's preference has been saved in lowercase `pt-br` causing the language to not match the available languages. Now we enforce the uppercase of the country part when loading the language.

- LDAP sync admin action was not syncing existent users ([#16671](https://github.com/RocketChat/Rocket.Chat/pull/16671))

- livechat/rooms endpoint not working with big amount of livechats ([#16623](https://github.com/RocketChat/Rocket.Chat/pull/16623) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Login with LinkedIn not mapping name and picture correctly ([#16955](https://github.com/RocketChat/Rocket.Chat/pull/16955))

- Manual Register use correct state for determining registered ([#16726](https://github.com/RocketChat/Rocket.Chat/pull/16726))

- Member's list only filtering users already on screen ([#17110](https://github.com/RocketChat/Rocket.Chat/pull/17110))

- Messages doesn't send to Slack via SlackBridge after renaming channel ([#16565](https://github.com/RocketChat/Rocket.Chat/pull/16565) by [@antkaz](https://github.com/antkaz))

- Omnichannel endpoint `inquiries.getOne` returning only queued inquiries ([#17132](https://github.com/RocketChat/Rocket.Chat/pull/17132))

- Omnichannel Inquiry names not being updated when the guest name changes ([#16782](https://github.com/RocketChat/Rocket.Chat/pull/16782))

- Omnichannel Inquiry queues when removing chats ([#16603](https://github.com/RocketChat/Rocket.Chat/pull/16603))

- Option BYPASS_OPLOG_VALIDATION not working ([#17143](https://github.com/RocketChat/Rocket.Chat/pull/17143))

- Pinned messages wouldn't collapse ([#16188](https://github.com/RocketChat/Rocket.Chat/pull/16188))

- Pressing Cancel while 'deleting by edit' message blocks sending messages ([#16315](https://github.com/RocketChat/Rocket.Chat/pull/16315) by [@ritwizsinha](https://github.com/ritwizsinha))

- Prune message saying `files deleted` and `messages deleted` even when singular message or file in prune ([#16322](https://github.com/RocketChat/Rocket.Chat/pull/16322) by [@ritwizsinha](https://github.com/ritwizsinha))

- Public channel cannot be accessed via URL when 'Allow Anonymous Read' is active ([#16914](https://github.com/RocketChat/Rocket.Chat/pull/16914) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Race conditions on/before login ([#16989](https://github.com/RocketChat/Rocket.Chat/pull/16989))

- Random errors on SAML logout ([#17227](https://github.com/RocketChat/Rocket.Chat/pull/17227))

- Real-time data rendering on Omnichannel room info panel  ([#16783](https://github.com/RocketChat/Rocket.Chat/pull/16783))

- Regression: Jitsi on external window infinite loop ([#16625](https://github.com/RocketChat/Rocket.Chat/pull/16625))

- Regression: New 'app' role with no permissions when updating to 3.0.0 ([#16637](https://github.com/RocketChat/Rocket.Chat/pull/16637))

- Remove Reply in DM from Omnichannel rooms ([#16957](https://github.com/RocketChat/Rocket.Chat/pull/16957) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Remove spaces from i18n placeholders to show Personal access token ([#16724](https://github.com/RocketChat/Rocket.Chat/pull/16724) by [@harakiwi1](https://github.com/harakiwi1))

- Rocket.Chat takes too long to set the username when it fails to send enrollment email ([#16723](https://github.com/RocketChat/Rocket.Chat/pull/16723))

- Room event emitter passing an invalid parameter when finding removed subscriptions ([#17224](https://github.com/RocketChat/Rocket.Chat/pull/17224))

- SAML login errors not showing on UI ([#17219](https://github.com/RocketChat/Rocket.Chat/pull/17219))

- Show error message if password and confirm password not equal  ([#16247](https://github.com/RocketChat/Rocket.Chat/pull/16247) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Slackbridge-import command doesn't work ([#16645](https://github.com/RocketChat/Rocket.Chat/pull/16645) by [@antkaz](https://github.com/antkaz))

- SlackBridge: Get all channels from Slack via REST API ([#16767](https://github.com/RocketChat/Rocket.Chat/pull/16767) by [@antkaz](https://github.com/antkaz))

- Slash command preview: Wrong item being selected, Horizontal scroll ([#16750](https://github.com/RocketChat/Rocket.Chat/pull/16750))

- Text formatted to remain within button even on screen resize  ([#14136](https://github.com/RocketChat/Rocket.Chat/pull/14136) by [@Rodriq](https://github.com/Rodriq))

- There is no option to pin a thread message by admin ([#16457](https://github.com/RocketChat/Rocket.Chat/pull/16457) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- TypeError when trying to load avatar of an invalid room. ([#16699](https://github.com/RocketChat/Rocket.Chat/pull/16699))

- UiKit not updating new actionIds received as responses from actions ([#16624](https://github.com/RocketChat/Rocket.Chat/pull/16624))

- users.info endpoint not handling the error if the user does not exist ([#16495](https://github.com/RocketChat/Rocket.Chat/pull/16495) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Verification email body ([#17062](https://github.com/RocketChat/Rocket.Chat/pull/17062) by [@GOVINDDIXIT](https://github.com/GOVINDDIXIT))

- WebRTC echo while talking ([#17145](https://github.com/RocketChat/Rocket.Chat/pull/17145) by [@1rV1N-git](https://github.com/1rV1N-git) & [@ndroo](https://github.com/ndroo))

- When trying to quote messages inside threads the quote would be sent to room instead to the thread ([#16925](https://github.com/RocketChat/Rocket.Chat/pull/16925))

- Wrong message count statistics in Admin info page ([#16680](https://github.com/RocketChat/Rocket.Chat/pull/16680) by [@subham103](https://github.com/subham103))

- Wrong SAML Response Signature Validation ([#16922](https://github.com/RocketChat/Rocket.Chat/pull/16922))

- Wrong thread messages display in contextual bar ([#16835](https://github.com/RocketChat/Rocket.Chat/pull/16835) by [@ritwizsinha](https://github.com/ritwizsinha))

<details>
<summary>🔍 Minor changes</summary>


- [Apps] Lazy load categories and marketplaceVersion in admin - apps page ([#16258](https://github.com/RocketChat/Rocket.Chat/pull/16258))

- [CHORE] Changed remaining SelectInput's to Select ([#16719](https://github.com/RocketChat/Rocket.Chat/pull/16719))

- [CHORE] Look for Storybook stories on `app/` too ([#16595](https://github.com/RocketChat/Rocket.Chat/pull/16595))

- [CHORE] Update snap install instructions ([#16720](https://github.com/RocketChat/Rocket.Chat/pull/16720))

- [CHORE] Use REST API for sending audio messages ([#17237](https://github.com/RocketChat/Rocket.Chat/pull/17237))

- Add an index to the name field for omnichannel department ([#16953](https://github.com/RocketChat/Rocket.Chat/pull/16953))

- Add Enterprise Edition license ([#16801](https://github.com/RocketChat/Rocket.Chat/pull/16801))

- Add lint to `.less` files ([#16893](https://github.com/RocketChat/Rocket.Chat/pull/16893))

- Add methods to include room types on dashboard ([#16576](https://github.com/RocketChat/Rocket.Chat/pull/16576) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add new Omnichannel department forwarding callback ([#16779](https://github.com/RocketChat/Rocket.Chat/pull/16779))

- Add some missing ES translations ([#16120](https://github.com/RocketChat/Rocket.Chat/pull/16120) by [@ivanape](https://github.com/ivanape))

- Add statistics and metrics about push queue ([#17208](https://github.com/RocketChat/Rocket.Chat/pull/17208))

- Add User’s index for field `appId` ([#17075](https://github.com/RocketChat/Rocket.Chat/pull/17075))

- Add wrapper to make Meteor methods calls over REST ([#17092](https://github.com/RocketChat/Rocket.Chat/pull/17092))

- Added border to page header ([#16792](https://github.com/RocketChat/Rocket.Chat/pull/16792))

- Bump acorn from 6.0.7 to 6.4.1 ([#16876](https://github.com/RocketChat/Rocket.Chat/pull/16876) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Change license version requested ([#16956](https://github.com/RocketChat/Rocket.Chat/pull/16956))

- Changed Opt_In message, removed translations ([#16631](https://github.com/RocketChat/Rocket.Chat/pull/16631))

- Collect metrics about meteor facts ([#17216](https://github.com/RocketChat/Rocket.Chat/pull/17216))

- Fix Docker preview image ([#16736](https://github.com/RocketChat/Rocket.Chat/pull/16736))

- Fix self DMs created during release candidate ([#17239](https://github.com/RocketChat/Rocket.Chat/pull/17239))

- Fix StreamCast info ([#16995](https://github.com/RocketChat/Rocket.Chat/pull/16995))

- Fix: 2FA DDP method not getting code on API call that doesn’t requires 2FA ([#16998](https://github.com/RocketChat/Rocket.Chat/pull/16998))

- fix: add option to mount media on snap ([#13591](https://github.com/RocketChat/Rocket.Chat/pull/13591) by [@knrt10](https://github.com/knrt10))

- Fix: Adding margin to click to load text ([#16210](https://github.com/RocketChat/Rocket.Chat/pull/16210) by [@ritwizsinha](https://github.com/ritwizsinha))

- Fix: Console error on login ([#16704](https://github.com/RocketChat/Rocket.Chat/pull/16704))

- Fix: Correctly aligned input element of custom user status component ([#16151](https://github.com/RocketChat/Rocket.Chat/pull/16151) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Fix: Error message on startup of multiple instances related to the metrics’ server ([#17152](https://github.com/RocketChat/Rocket.Chat/pull/17152))

- Fix: Huge amount of hasLicense calls to the server ([#17169](https://github.com/RocketChat/Rocket.Chat/pull/17169))

- Fix: Last message of Group DMs not showing the sender ([#17059](https://github.com/RocketChat/Rocket.Chat/pull/17059))

- Fix: Make the AppLivechatBridge.createMessage works properly as a promise ([#16941](https://github.com/RocketChat/Rocket.Chat/pull/16941))

- Fix: Missing checks for Troubleshoot > Disable Notifications ([#17155](https://github.com/RocketChat/Rocket.Chat/pull/17155))

- Fix: Notifications of Group DM were not showing the room name ([#17058](https://github.com/RocketChat/Rocket.Chat/pull/17058))

- Fix: Padding required in the Facebook Messenger option in Livechat ([#16202](https://github.com/RocketChat/Rocket.Chat/pull/16202) by [@ritwizsinha](https://github.com/ritwizsinha))

- Fix: Removed some hardcoded texts ([#16304](https://github.com/RocketChat/Rocket.Chat/pull/16304) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Fix: StreamCast was not working correctly ([#16983](https://github.com/RocketChat/Rocket.Chat/pull/16983))

- Fixed Line break incorrectly being called apostrophe in code ([#16918](https://github.com/RocketChat/Rocket.Chat/pull/16918) by [@aKn1ghtOut](https://github.com/aKn1ghtOut))

- Fixed translate variable in UnarchiveRoom Modal ([#16310](https://github.com/RocketChat/Rocket.Chat/pull/16310) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Improve room types usage ([#16753](https://github.com/RocketChat/Rocket.Chat/pull/16753))

- Improve: Apps-engine E2E tests ([#16781](https://github.com/RocketChat/Rocket.Chat/pull/16781) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- LingoHub based on develop ([#16837](https://github.com/RocketChat/Rocket.Chat/pull/16837))

- LingoHub based on develop ([#16640](https://github.com/RocketChat/Rocket.Chat/pull/16640))

- Merge master into develop & Set version to 3.1.0-develop ([#16609](https://github.com/RocketChat/Rocket.Chat/pull/16609))

- Metrics: New metrics, performance and size improvements ([#17183](https://github.com/RocketChat/Rocket.Chat/pull/17183))

- New metric to track oplog queue ([#17142](https://github.com/RocketChat/Rocket.Chat/pull/17142))

- New Troubleshoot section for disabling features ([#17114](https://github.com/RocketChat/Rocket.Chat/pull/17114))

- Redirected to home when a room has been deleted instead of getting broken link(blank page) of deleted room ([#16227](https://github.com/RocketChat/Rocket.Chat/pull/16227) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Reduce notifyUser propagation ([#17088](https://github.com/RocketChat/Rocket.Chat/pull/17088))

- Regression: `users.setStatus` throwing an error if message is empty ([#17036](https://github.com/RocketChat/Rocket.Chat/pull/17036) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Admin create user button ([#17186](https://github.com/RocketChat/Rocket.Chat/pull/17186))

- Regression: Block users was not possible for 1:1 DMs ([#17105](https://github.com/RocketChat/Rocket.Chat/pull/17105))

- Regression: Broken Search if users without DM subscriptions are listed ([#17074](https://github.com/RocketChat/Rocket.Chat/pull/17074))

- Regression: Can't login with 2FA over REST API when 2FA via Email is enabled ([#17128](https://github.com/RocketChat/Rocket.Chat/pull/17128) by [@djorkaeffalexandre](https://github.com/djorkaeffalexandre))

- Regression: Check Omnichannel routing system before emitting queue changes ([#17087](https://github.com/RocketChat/Rocket.Chat/pull/17087))

- Regression: Collapsible elements didn't respect attachment parameter. ([#16994](https://github.com/RocketChat/Rocket.Chat/pull/16994))

- Regression: Direct message creation by REST ([#17109](https://github.com/RocketChat/Rocket.Chat/pull/17109))

- Regression: Do not refresh statistics when opening the info panel ([#17060](https://github.com/RocketChat/Rocket.Chat/pull/17060))

- Regression: Files were been deleted when deleting users as last members of private rooms ([#17111](https://github.com/RocketChat/Rocket.Chat/pull/17111))

- Regression: Fix auditing for Multiple Direct Messages ([#17192](https://github.com/RocketChat/Rocket.Chat/pull/17192))

- Regression: Fix calling readmessage after mark as unread ([#17193](https://github.com/RocketChat/Rocket.Chat/pull/17193))

- Regression: fix design review of Directory ([#17133](https://github.com/RocketChat/Rocket.Chat/pull/17133))

- Regression: Fix engagement dashboard urls, fixing Flowrouter imports ([#17127](https://github.com/RocketChat/Rocket.Chat/pull/17127))

- Regression: fix fuselage import, remove directory css ([#17116](https://github.com/RocketChat/Rocket.Chat/pull/17116))

- Regression: Fix issue with opening rooms ([#17028](https://github.com/RocketChat/Rocket.Chat/pull/17028))

- Regression: Fix omnichannel icon missing on sidebar ([#16775](https://github.com/RocketChat/Rocket.Chat/pull/16775))

- Regression: Fix removing user not removing his 1-on-1 DMs ([#17057](https://github.com/RocketChat/Rocket.Chat/pull/17057))

- Regression: fix scroll after room loads ([#17188](https://github.com/RocketChat/Rocket.Chat/pull/17188))

- Regression: Fix users raw model ([#17204](https://github.com/RocketChat/Rocket.Chat/pull/17204))

- Regression: IE11 Support ([#17125](https://github.com/RocketChat/Rocket.Chat/pull/17125))

- Regression: Invite links working for group DMs ([#17056](https://github.com/RocketChat/Rocket.Chat/pull/17056))

- Regression: OmniChannel agent activity monitor was counting time wrongly ([#16979](https://github.com/RocketChat/Rocket.Chat/pull/16979) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: omnichannel manual queued sidebarlist ([#17048](https://github.com/RocketChat/Rocket.Chat/pull/17048))

- Regression: Omnichannel notification on new conversations displaying incorrect information ([#16346](https://github.com/RocketChat/Rocket.Chat/pull/16346))

- Regression: Overwrite model functions on EE only when license applied ([#17061](https://github.com/RocketChat/Rocket.Chat/pull/17061))

- Regression: Remove deprecated Omnichannel setting used to fetch the queue data through subscription  ([#17017](https://github.com/RocketChat/Rocket.Chat/pull/17017))

- Regression: Remove old and closed Omnichannel inquiries ([#17113](https://github.com/RocketChat/Rocket.Chat/pull/17113))

- Regression: Replace the Omnichannel queue model observe with Stream ([#16999](https://github.com/RocketChat/Rocket.Chat/pull/16999))

- Regression: Show upload errors ([#16681](https://github.com/RocketChat/Rocket.Chat/pull/16681))

- Regression: Small fixes for Game Center ([#17018](https://github.com/RocketChat/Rocket.Chat/pull/17018))

- Regression: Wrong size of Directory search/sort icons and Sort Channels menu not showing on production build ([#17118](https://github.com/RocketChat/Rocket.Chat/pull/17118))

- Release 3.0.12 ([#17158](https://github.com/RocketChat/Rocket.Chat/pull/17158))

- Removing Trailing Space ([#16470](https://github.com/RocketChat/Rocket.Chat/pull/16470) by [@aryamanpuri](https://github.com/aryamanpuri))

- Single codebase announcement ([#17081](https://github.com/RocketChat/Rocket.Chat/pull/17081))

- Update cypress to version 4.0.2 ([#16685](https://github.com/RocketChat/Rocket.Chat/pull/16685))

- Update presence package ([#16786](https://github.com/RocketChat/Rocket.Chat/pull/16786))

- Upgrade Livechat Widget version to 1.4.0 ([#16950](https://github.com/RocketChat/Rocket.Chat/pull/16950))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@1rV1N-git](https://github.com/1rV1N-git)
- [@GOVINDDIXIT](https://github.com/GOVINDDIXIT)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Nikhil713](https://github.com/Nikhil713)
- [@Rodriq](https://github.com/Rodriq)
- [@aKn1ghtOut](https://github.com/aKn1ghtOut)
- [@antkaz](https://github.com/antkaz)
- [@aryamanpuri](https://github.com/aryamanpuri)
- [@ashwaniYDV](https://github.com/ashwaniYDV)
- [@col-panic](https://github.com/col-panic)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@djorkaeffalexandre](https://github.com/djorkaeffalexandre)
- [@fliptrail](https://github.com/fliptrail)
- [@georgmu](https://github.com/georgmu)
- [@harakiwi1](https://github.com/harakiwi1)
- [@ivanape](https://github.com/ivanape)
- [@jschirrmacher](https://github.com/jschirrmacher)
- [@karimelghazouly](https://github.com/karimelghazouly)
- [@knrt10](https://github.com/knrt10)
- [@localguru](https://github.com/localguru)
- [@mrsimpson](https://github.com/mrsimpson)
- [@ndroo](https://github.com/ndroo)
- [@pmayer](https://github.com/pmayer)
- [@ritwizsinha](https://github.com/ritwizsinha)
- [@rm-yakovenko](https://github.com/rm-yakovenko)
- [@sneakson](https://github.com/sneakson)
- [@subham103](https://github.com/subham103)

### 👩‍💻👨‍💻 Core Team 🤓

- [@PrajvalRaval](https://github.com/PrajvalRaval)
- [@Sing-Li](https://github.com/Sing-Li)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 3.0.13
`2020-05-11  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Email configs not updating after setting changes ([#17578](https://github.com/RocketChat/Rocket.Chat/pull/17578))

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)

# 3.0.12
`2020-04-03  ·  3 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Fix: Error message on startup of multiple instances related to the metrics’ server ([#17152](https://github.com/RocketChat/Rocket.Chat/pull/17152))

- Fix: Missing checks for Troubleshoot > Disable Notifications ([#17155](https://github.com/RocketChat/Rocket.Chat/pull/17155))

- Release 3.0.12 ([#17158](https://github.com/RocketChat/Rocket.Chat/pull/17158))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.11
`2020-04-02  ·  2 🐛  ·  2 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Omnichannel endpoint `inquiries.getOne` returning only queued inquiries ([#17132](https://github.com/RocketChat/Rocket.Chat/pull/17132))

- Option BYPASS_OPLOG_VALIDATION not working ([#17143](https://github.com/RocketChat/Rocket.Chat/pull/17143))

<details>
<summary>🔍 Minor changes</summary>


- New metric to track oplog queue ([#17142](https://github.com/RocketChat/Rocket.Chat/pull/17142))

- Release 3.0.11 ([#17148](https://github.com/RocketChat/Rocket.Chat/pull/17148))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.10
`2020-04-01  ·  1 🚀  ·  2 🐛  ·  4 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🚀 Improvements


- Apps Engine: Reduce some stream calls and remove a find user from the app's status changes ([#17115](https://github.com/RocketChat/Rocket.Chat/pull/17115))

### 🐛 Bug fixes


- Federation delete room event not being dispatched ([#16861](https://github.com/RocketChat/Rocket.Chat/pull/16861) by [@1rV1N-git](https://github.com/1rV1N-git))

- Federation Event ROOM_ADD_USER not being dispatched ([#16878](https://github.com/RocketChat/Rocket.Chat/pull/16878) by [@1rV1N-git](https://github.com/1rV1N-git))

<details>
<summary>🔍 Minor changes</summary>


- Add User’s index for field `appId` ([#17075](https://github.com/RocketChat/Rocket.Chat/pull/17075))

- New Troubleshoot section for disabling features ([#17114](https://github.com/RocketChat/Rocket.Chat/pull/17114))

- Regression: Do not refresh statistics when opening the info panel ([#17060](https://github.com/RocketChat/Rocket.Chat/pull/17060))

- Release 3.0.10 ([#17126](https://github.com/RocketChat/Rocket.Chat/pull/17126) by [@1rV1N-git](https://github.com/1rV1N-git))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@1rV1N-git](https://github.com/1rV1N-git)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.9
`2020-03-31  ·  1 🐛  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Apps Engine notifyRoom sending notification to wrong users ([#17093](https://github.com/RocketChat/Rocket.Chat/pull/17093))

<details>
<summary>🔍 Minor changes</summary>


- Release 3.0.9 ([#17094](https://github.com/RocketChat/Rocket.Chat/pull/17094))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.8
`2020-03-30  ·  2 🐛  ·  2 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Emit livechat events to instace only ([#17086](https://github.com/RocketChat/Rocket.Chat/pull/17086))

- Error when websocket received status update event ([#17089](https://github.com/RocketChat/Rocket.Chat/pull/17089))

<details>
<summary>🔍 Minor changes</summary>


- Reduce notifyUser propagation ([#17088](https://github.com/RocketChat/Rocket.Chat/pull/17088))

- Regression: Remove model observe that was used to control the status of the Omnichannel agents ([#17078](https://github.com/RocketChat/Rocket.Chat/pull/17078))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.7
`2020-03-25  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Regression: Remove deprecated Omnichannel setting used to fetch the queue data through subscription  ([#17017](https://github.com/RocketChat/Rocket.Chat/pull/17017))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@renatobecker](https://github.com/renatobecker)

# 3.0.6
`2020-03-25  ·  1 🐛  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Keeps the agent in the room after accepting a new Omnichannel request ([#16787](https://github.com/RocketChat/Rocket.Chat/pull/16787))

<details>
<summary>🔍 Minor changes</summary>


- Regression: Replace the Omnichannel queue model observe with Stream ([#16999](https://github.com/RocketChat/Rocket.Chat/pull/16999))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@renatobecker](https://github.com/renatobecker)

# 3.0.5
`2020-03-24  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Race conditions on/before login ([#16989](https://github.com/RocketChat/Rocket.Chat/pull/16989))

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.4
`2020-03-16  ·  1 🚀  ·  2 🐛  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🚀 Improvements


- Send files over REST API ([#16617](https://github.com/RocketChat/Rocket.Chat/pull/16617))

### 🐛 Bug fixes


- Integrations page pagination ([#16838](https://github.com/RocketChat/Rocket.Chat/pull/16838))

- TypeError when trying to load avatar of an invalid room. ([#16699](https://github.com/RocketChat/Rocket.Chat/pull/16699))

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.3
`2020-03-02  ·  5 🐛  ·  5 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Check agent status when starting a new conversation with an agent assigned ([#16618](https://github.com/RocketChat/Rocket.Chat/pull/16618))

- Language country has been ignored on translation load ([#16757](https://github.com/RocketChat/Rocket.Chat/pull/16757))

- LDAP sync admin action was not syncing existent users ([#16671](https://github.com/RocketChat/Rocket.Chat/pull/16671))

- Manual Register use correct state for determining registered ([#16726](https://github.com/RocketChat/Rocket.Chat/pull/16726))

- Rocket.Chat takes too long to set the username when it fails to send enrollment email ([#16723](https://github.com/RocketChat/Rocket.Chat/pull/16723))

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)

# 3.0.2
`2020-02-21  ·  4 🐛  ·  5 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Clear unread red line when the ESC key is pressed  ([#16668](https://github.com/RocketChat/Rocket.Chat/pull/16668))

- ie11 support ([#16682](https://github.com/RocketChat/Rocket.Chat/pull/16682))

- Omnichannel Inquiry queues when removing chats ([#16603](https://github.com/RocketChat/Rocket.Chat/pull/16603))

- users.info endpoint not handling the error if the user does not exist ([#16495](https://github.com/RocketChat/Rocket.Chat/pull/16495) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@gabriellsh](https://github.com/gabriellsh)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.1
`2020-02-19  ·  7 🐛  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Admin height if the blue banner is opened ([#16629](https://github.com/RocketChat/Rocket.Chat/pull/16629))

- Block user option inside admin view ([#16626](https://github.com/RocketChat/Rocket.Chat/pull/16626))

- Data converters overriding fields added by apps ([#16639](https://github.com/RocketChat/Rocket.Chat/pull/16639))

- livechat/rooms endpoint not working with big amount of livechats ([#16623](https://github.com/RocketChat/Rocket.Chat/pull/16623) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Jitsi on external window infinite loop ([#16625](https://github.com/RocketChat/Rocket.Chat/pull/16625))

- Regression: New 'app' role with no permissions when updating to 3.0.0 ([#16637](https://github.com/RocketChat/Rocket.Chat/pull/16637))

- UiKit not updating new actionIds received as responses from actions ([#16624](https://github.com/RocketChat/Rocket.Chat/pull/16624))

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@ggazzo](https://github.com/ggazzo)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 3.0.0
`2020-02-14  ·  7 ️️️⚠️  ·  10 🎉  ·  11 🚀  ·  41 🐛  ·  49 🔍  ·  21 👩‍💻👨‍💻`

### Engine versions
- Node: `12.14.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### ⚠️ BREAKING CHANGES


- Change apps/icon endpoint to return app's icon and use it to show on Ui Kit modal ([#16522](https://github.com/RocketChat/Rocket.Chat/pull/16522))

- Filter System messages per room ([#16369](https://github.com/RocketChat/Rocket.Chat/pull/16369) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Hide system messages ([#16243](https://github.com/RocketChat/Rocket.Chat/pull/16243) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Remove deprecated publications ([#16351](https://github.com/RocketChat/Rocket.Chat/pull/16351) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Removed room counter from sidebar ([#16036](https://github.com/RocketChat/Rocket.Chat/pull/16036))

- TLS v1.0 and TLS v1.1 were disabled by due to NodeJS update to v12. You can still enable them by using flags like `--tls-min-v1.0` and `--tls-min-v1.1`

- Upgrade to Meteor 1.9 and NodeJS 12 ([#16252](https://github.com/RocketChat/Rocket.Chat/pull/16252))

### 🎉 New features


- Add GUI for customFields in Omnichannel conversations ([#15840](https://github.com/RocketChat/Rocket.Chat/pull/15840) by [@antkaz](https://github.com/antkaz))

- Button to download admin server info ([#16059](https://github.com/RocketChat/Rocket.Chat/pull/16059) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Check the Omnichannel service status per Department ([#16425](https://github.com/RocketChat/Rocket.Chat/pull/16425))

- Create a user for the Apps during installation ([#15896](https://github.com/RocketChat/Rocket.Chat/pull/15896) by [@Cool-fire](https://github.com/Cool-fire))

- Enforce plain text emails converting from HTML when no text version supplied ([#16063](https://github.com/RocketChat/Rocket.Chat/pull/16063))

- Setting to only send plain text emails ([#16065](https://github.com/RocketChat/Rocket.Chat/pull/16065))

- Setting Top navbar in embedded mode  ([#16064](https://github.com/RocketChat/Rocket.Chat/pull/16064))

- Sort the Omnichannel Chat list according to the user preferences ([#16437](https://github.com/RocketChat/Rocket.Chat/pull/16437))

- UiKit - Interactive UI elements for Rocket.Chat Apps ([#16048](https://github.com/RocketChat/Rocket.Chat/pull/16048))

- update on mongo, node and caddy on snap ([#16167](https://github.com/RocketChat/Rocket.Chat/pull/16167))

### 🚀 Improvements


- Changes App user's status when the app was enabled/disabled ([#16392](https://github.com/RocketChat/Rocket.Chat/pull/16392))

- Improve function to check if setting has changed ([#16181](https://github.com/RocketChat/Rocket.Chat/pull/16181))

- Log as info level when Method Rate Limiters are reached ([#16446](https://github.com/RocketChat/Rocket.Chat/pull/16446))

- Major overhaul on data importers ([#16279](https://github.com/RocketChat/Rocket.Chat/pull/16279))

- Prevent "App user" from being deleted by the admin ([#16373](https://github.com/RocketChat/Rocket.Chat/pull/16373))

- Remove NRR ([#16071](https://github.com/RocketChat/Rocket.Chat/pull/16071))

- Request user presence on demand ([#16348](https://github.com/RocketChat/Rocket.Chat/pull/16348))

- Set the color of the cancel button on modals to #bdbebf for enhanced  visibiity ([#15913](https://github.com/RocketChat/Rocket.Chat/pull/15913) by [@ritwizsinha](https://github.com/ritwizsinha))

- Show more information related to the Omnichannel room closing data ([#16414](https://github.com/RocketChat/Rocket.Chat/pull/16414))

- Status Text form validation ([#16121](https://github.com/RocketChat/Rocket.Chat/pull/16121))

- Update katex version ([#16393](https://github.com/RocketChat/Rocket.Chat/pull/16393))

### 🐛 Bug fixes


- "User not found" for direct messages ([#16047](https://github.com/RocketChat/Rocket.Chat/pull/16047))

- `stdout` streamer infinite loop ([#16452](https://github.com/RocketChat/Rocket.Chat/pull/16452))

- Adding 'lang' tag ([#16375](https://github.com/RocketChat/Rocket.Chat/pull/16375) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- api-bypass-rate-limiter permission was not working ([#16080](https://github.com/RocketChat/Rocket.Chat/pull/16080) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- App removal was moving logs to the trash collection ([#16362](https://github.com/RocketChat/Rocket.Chat/pull/16362))

- auto translate cache ([#15768](https://github.com/RocketChat/Rocket.Chat/pull/15768) by [@vickyokrm](https://github.com/vickyokrm))

- Break message-attachment text to the next line ([#16039](https://github.com/RocketChat/Rocket.Chat/pull/16039) by [@ritwizsinha](https://github.com/ritwizsinha))

- Bug on starting Jitsi video calls , multiple messages ([#16601](https://github.com/RocketChat/Rocket.Chat/pull/16601))

- Container heights ([#16388](https://github.com/RocketChat/Rocket.Chat/pull/16388))

- Do not stop on DM imports if one of users was not found ([#16547](https://github.com/RocketChat/Rocket.Chat/pull/16547))

- Drag and drop disabled when file upload is disabled ([#16049](https://github.com/RocketChat/Rocket.Chat/pull/16049) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Embedded style when using 'go' command ([#16051](https://github.com/RocketChat/Rocket.Chat/pull/16051))

- Error when successfully joining room by invite link ([#16571](https://github.com/RocketChat/Rocket.Chat/pull/16571))

- FileUpload.getBuffer was not working through the Apps-Engine ([#16234](https://github.com/RocketChat/Rocket.Chat/pull/16234))

- Highlight freezing the UI ([#16378](https://github.com/RocketChat/Rocket.Chat/pull/16378))

- Integrations admin page ([#16183](https://github.com/RocketChat/Rocket.Chat/pull/16183))

- Integrations list without pagination and outgoing integration creation ([#16233](https://github.com/RocketChat/Rocket.Chat/pull/16233) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Introduce AppLivechatBridge.isOnlineAsync method ([#16467](https://github.com/RocketChat/Rocket.Chat/pull/16467))

- Invite links proxy URLs not working when using CDN ([#16581](https://github.com/RocketChat/Rocket.Chat/pull/16581))

- Invite links usage by channel owners/moderators ([#16176](https://github.com/RocketChat/Rocket.Chat/pull/16176))

- Livechat Widget version 1.3.1 ([#16580](https://github.com/RocketChat/Rocket.Chat/pull/16580))

- Login change language button ([#16085](https://github.com/RocketChat/Rocket.Chat/pull/16085) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Mail Msg Cancel button not closing the flexbar ([#16263](https://github.com/RocketChat/Rocket.Chat/pull/16263) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Missing edited icon in newly created messages ([#16484](https://github.com/RocketChat/Rocket.Chat/pull/16484))

- Option to make a channel default ([#16433](https://github.com/RocketChat/Rocket.Chat/pull/16433) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Read Message after receive a message and the room is opened ([#16473](https://github.com/RocketChat/Rocket.Chat/pull/16473))

- Readme Help wanted section ([#16197](https://github.com/RocketChat/Rocket.Chat/pull/16197))

- Result of get avatar from url can be null ([#16123](https://github.com/RocketChat/Rocket.Chat/pull/16123) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Role tags missing - Description field explanation ([#16356](https://github.com/RocketChat/Rocket.Chat/pull/16356) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Rooms not being marked as read sometimes ([#16397](https://github.com/RocketChat/Rocket.Chat/pull/16397))

- SafePorts: Ports 80, 8080 & 443 linked to respective protocols (#16108) ([#16108](https://github.com/RocketChat/Rocket.Chat/pull/16108))

- Save password without confirmation ([#16060](https://github.com/RocketChat/Rocket.Chat/pull/16060) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Send message with pending messages ([#16474](https://github.com/RocketChat/Rocket.Chat/pull/16474))

- Setup Wizard inputs and Admin Settings ([#16147](https://github.com/RocketChat/Rocket.Chat/pull/16147))

- Slack CSV User Importer ([#16253](https://github.com/RocketChat/Rocket.Chat/pull/16253))

- The "click to load" text is hard-coded and not translated. ([#16142](https://github.com/RocketChat/Rocket.Chat/pull/16142) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Thread message icon overlapping text ([#16083](https://github.com/RocketChat/Rocket.Chat/pull/16083))

- Unknown error when sending message if 'Set a User Name to Alias in Message' setting is enabled ([#16347](https://github.com/RocketChat/Rocket.Chat/pull/16347))

- User stuck after reset password ([#16184](https://github.com/RocketChat/Rocket.Chat/pull/16184))

- Video message sent to wrong room ([#16113](https://github.com/RocketChat/Rocket.Chat/pull/16113))

- When copying invite links, multiple toastr messages ([#16578](https://github.com/RocketChat/Rocket.Chat/pull/16578))

<details>
<summary>🔍 Minor changes</summary>


- Add breaking notice regarding TLS ([#16575](https://github.com/RocketChat/Rocket.Chat/pull/16575))

- Add Cloud Info to translation dictionary ([#16122](https://github.com/RocketChat/Rocket.Chat/pull/16122) by [@aviral243](https://github.com/aviral243))

- Add missing translations ([#16150](https://github.com/RocketChat/Rocket.Chat/pull/16150) by [@ritwizsinha](https://github.com/ritwizsinha))

- Add Ui Kit container ([#16503](https://github.com/RocketChat/Rocket.Chat/pull/16503))

- Catch zip errors on import file load ([#16494](https://github.com/RocketChat/Rocket.Chat/pull/16494))

- Disable PR Docker image build ([#16141](https://github.com/RocketChat/Rocket.Chat/pull/16141))

- Exclude federated and app users from active user count ([#16489](https://github.com/RocketChat/Rocket.Chat/pull/16489))

- Fix assets download on CI ([#16352](https://github.com/RocketChat/Rocket.Chat/pull/16352))

- Fix github actions accessing the github registry ([#16521](https://github.com/RocketChat/Rocket.Chat/pull/16521) by [@mrsimpson](https://github.com/mrsimpson))

- Fix index creation for apps_logs collection ([#16401](https://github.com/RocketChat/Rocket.Chat/pull/16401))

- Fix Preview Docker image build ([#16379](https://github.com/RocketChat/Rocket.Chat/pull/16379))

- Fix tests ([#16469](https://github.com/RocketChat/Rocket.Chat/pull/16469))

- Fix: License missing from manual register handler ([#16505](https://github.com/RocketChat/Rocket.Chat/pull/16505))

- LingoHub based on develop ([#16450](https://github.com/RocketChat/Rocket.Chat/pull/16450))

- Lint: Resolve complexity warnings ([#16114](https://github.com/RocketChat/Rocket.Chat/pull/16114))

- Merge master into develop & Set version to 2.5.0-develop ([#16107](https://github.com/RocketChat/Rocket.Chat/pull/16107))

- Regression: allow private channels to hide system messages ([#16483](https://github.com/RocketChat/Rocket.Chat/pull/16483))

- Regression: App deletion wasn’t returning the correct information ([#16360](https://github.com/RocketChat/Rocket.Chat/pull/16360))

- Regression: Fix app user status change for non-existing user ([#16458](https://github.com/RocketChat/Rocket.Chat/pull/16458))

- Regression: fix read unread messages ([#16562](https://github.com/RocketChat/Rocket.Chat/pull/16562))

- Regression: Fix sending a message not scrolling to bottom ([#16451](https://github.com/RocketChat/Rocket.Chat/pull/16451))

- Regression: Fix sequential messages grouping ([#16386](https://github.com/RocketChat/Rocket.Chat/pull/16386))

- Regression: Fix status bar margins ([#16438](https://github.com/RocketChat/Rocket.Chat/pull/16438))

- Regression: Fix uikit modal closing on click ([#16475](https://github.com/RocketChat/Rocket.Chat/pull/16475))

- Regression: Fix undefined presence after reconnect ([#16477](https://github.com/RocketChat/Rocket.Chat/pull/16477))

- Regression: Modal onSubmit ([#16556](https://github.com/RocketChat/Rocket.Chat/pull/16556))

- Regression: prevent submit modal ([#16488](https://github.com/RocketChat/Rocket.Chat/pull/16488))

- Regression: Rate limiter was not working due to Meteor internal changes ([#16361](https://github.com/RocketChat/Rocket.Chat/pull/16361))

- Regression: recent opened rooms being marked as read ([#16442](https://github.com/RocketChat/Rocket.Chat/pull/16442))

- Regression: Send app info along with interaction payload to the UI ([#16511](https://github.com/RocketChat/Rocket.Chat/pull/16511))

- Regression: send file modal not working via keyboard ([#16607](https://github.com/RocketChat/Rocket.Chat/pull/16607))

- Regression: Ui Kit messaging issues (#16513) ([#16513](https://github.com/RocketChat/Rocket.Chat/pull/16513))

- Regression: UIKit - Send container info on block actions triggered on a message ([#16514](https://github.com/RocketChat/Rocket.Chat/pull/16514))

- Regression: UIkit input states ([#16552](https://github.com/RocketChat/Rocket.Chat/pull/16552))

- Regression: UIKit missing select states: error/disabled ([#16540](https://github.com/RocketChat/Rocket.Chat/pull/16540))

- Regression: UIKit update modal actions ([#16570](https://github.com/RocketChat/Rocket.Chat/pull/16570))

- Regression: update package-lock ([#16528](https://github.com/RocketChat/Rocket.Chat/pull/16528))

- Regression: Update Uikit ([#16515](https://github.com/RocketChat/Rocket.Chat/pull/16515))

- Release 2.4.7 ([#16444](https://github.com/RocketChat/Rocket.Chat/pull/16444) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Release 2.4.9 ([#16544](https://github.com/RocketChat/Rocket.Chat/pull/16544))

- Remove users.info being called without need ([#16504](https://github.com/RocketChat/Rocket.Chat/pull/16504))

- Revert importer streamed uploads ([#16465](https://github.com/RocketChat/Rocket.Chat/pull/16465))

- Revert message properties validation ([#16395](https://github.com/RocketChat/Rocket.Chat/pull/16395) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Send build artifacts to S3 ([#16237](https://github.com/RocketChat/Rocket.Chat/pull/16237))

- Update apps engine to 1.12.0-beta.2496 ([#16398](https://github.com/RocketChat/Rocket.Chat/pull/16398))

- Update Apps-Engine version ([#16584](https://github.com/RocketChat/Rocket.Chat/pull/16584))

- Update presence package to 2.6.1 ([#16486](https://github.com/RocketChat/Rocket.Chat/pull/16486))

- Use base64 for import files upload to prevent file corruption ([#16516](https://github.com/RocketChat/Rocket.Chat/pull/16516))

- Use GitHub Actions to store builds ([#16443](https://github.com/RocketChat/Rocket.Chat/pull/16443))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Cool-fire](https://github.com/Cool-fire)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@antkaz](https://github.com/antkaz)
- [@ashwaniYDV](https://github.com/ashwaniYDV)
- [@aviral243](https://github.com/aviral243)
- [@mariaeduardacunha](https://github.com/mariaeduardacunha)
- [@mrsimpson](https://github.com/mrsimpson)
- [@ritwizsinha](https://github.com/ritwizsinha)
- [@vickyokrm](https://github.com/vickyokrm)

### 👩‍💻👨‍💻 Core Team 🤓

- [@LuluGO](https://github.com/LuluGO)
- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 2.4.14
`2020-12-18  ·  2 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`
- Apps-Engine: `1.11.2`

### 🐛 Bug fixes


- Issue with special message rendering ([#19817](https://github.com/RocketChat/Rocket.Chat/pull/19817))

- Problem with attachment render ([#19854](https://github.com/RocketChat/Rocket.Chat/pull/19854))

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)

# 2.4.12
`2020-05-11  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Email configs not updating after setting changes ([#17578](https://github.com/RocketChat/Rocket.Chat/pull/17578))

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)

# 2.4.10
`2020-02-20  ·  1 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- users.info endpoint not handling the error if the user does not exist ([#16495](https://github.com/RocketChat/Rocket.Chat/pull/16495) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.4.9
`2020-02-10  ·  1 🐛  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- `stdout` streamer infinite loop ([#16452](https://github.com/RocketChat/Rocket.Chat/pull/16452))

<details>
<summary>🔍 Minor changes</summary>


- Release 2.4.9 ([#16544](https://github.com/RocketChat/Rocket.Chat/pull/16544))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.4.8
`2020-02-07  ·  2 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Release 2.4.8 ([#16506](https://github.com/RocketChat/Rocket.Chat/pull/16506))

- Update presence package to 2.6.1 ([#16486](https://github.com/RocketChat/Rocket.Chat/pull/16486))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.4.7
`2020-02-03  ·  1 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Option to make a channel default ([#16433](https://github.com/RocketChat/Rocket.Chat/pull/16433) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

<details>
<summary>🔍 Minor changes</summary>


- Release 2.4.7 ([#16444](https://github.com/RocketChat/Rocket.Chat/pull/16444) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)

# 2.4.6
`2020-01-31  ·  3 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Fix index creation for apps_logs collection ([#16401](https://github.com/RocketChat/Rocket.Chat/pull/16401))

- Release 2.4.6 ([#16402](https://github.com/RocketChat/Rocket.Chat/pull/16402) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Revert message properties validation ([#16395](https://github.com/RocketChat/Rocket.Chat/pull/16395) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.4.5
`2020-01-29  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Release 2.4.5 ([#16380](https://github.com/RocketChat/Rocket.Chat/pull/16380))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.4.4
`2020-01-29  ·  1 🐛  ·  2 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- App removal was moving logs to the trash collection ([#16362](https://github.com/RocketChat/Rocket.Chat/pull/16362))

<details>
<summary>🔍 Minor changes</summary>


- Regression: Rate limiter was not working due to Meteor internal changes ([#16361](https://github.com/RocketChat/Rocket.Chat/pull/16361))

- Release 2.4.4 ([#16377](https://github.com/RocketChat/Rocket.Chat/pull/16377))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.4.3
`2020-01-28  ·  2 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Invite links usage by channel owners/moderators ([#16176](https://github.com/RocketChat/Rocket.Chat/pull/16176))

- Unknown error when sending message if 'Set a User Name to Alias in Message' setting is enabled ([#16347](https://github.com/RocketChat/Rocket.Chat/pull/16347))

<details>
<summary>🔍 Minor changes</summary>


- Release 2.4.3 ([#16358](https://github.com/RocketChat/Rocket.Chat/pull/16358))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.4.2
`2020-01-17  ·  4 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Integrations list without pagination and outgoing integration creation ([#16233](https://github.com/RocketChat/Rocket.Chat/pull/16233) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Setup Wizard inputs and Admin Settings ([#16147](https://github.com/RocketChat/Rocket.Chat/pull/16147))

- Slack CSV User Importer ([#16253](https://github.com/RocketChat/Rocket.Chat/pull/16253))

- User stuck after reset password ([#16184](https://github.com/RocketChat/Rocket.Chat/pull/16184))

<details>
<summary>🔍 Minor changes</summary>


- Release 2.4.2 ([#16274](https://github.com/RocketChat/Rocket.Chat/pull/16274) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 2.4.1
`2020-01-10  ·  3 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Add missing password field back to administration area ([#16171](https://github.com/RocketChat/Rocket.Chat/pull/16171))

- Enable apps change properties of the sender on the message as before ([#16189](https://github.com/RocketChat/Rocket.Chat/pull/16189))

- JS errors on Administration page ([#16139](https://github.com/RocketChat/Rocket.Chat/pull/16139) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

<details>
<summary>🔍 Minor changes</summary>


- Release 2.4.1 ([#16195](https://github.com/RocketChat/Rocket.Chat/pull/16195) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@mariaeduardacunha](https://github.com/mariaeduardacunha)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.4.0
`2019-12-27  ·  4 🎉  ·  28 🚀  ·  29 🐛  ·  19 🔍  ·  22 👩‍💻👨‍💻`

### Engine versions
- Node: `8.17.0`
- NPM: `6.13.4`
- MongoDB: `3.4, 3.6, 4.0`

### 🎉 New features


- Apps-Engine event for when a livechat room is closed ([#15837](https://github.com/RocketChat/Rocket.Chat/pull/15837))

- Do not print emails in console on production mode ([#15928](https://github.com/RocketChat/Rocket.Chat/pull/15928))

- Invite links: share a link to invite users ([#15933](https://github.com/RocketChat/Rocket.Chat/pull/15933))

- Logout other clients when changing password ([#15927](https://github.com/RocketChat/Rocket.Chat/pull/15927))

### 🚀 Improvements


- Add deprecate warning in some unused publications ([#15935](https://github.com/RocketChat/Rocket.Chat/pull/15935) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Livechat realtime dashboard ([#15792](https://github.com/RocketChat/Rocket.Chat/pull/15792) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move 'Reply in Thread' button from menu to message actions ([#15685](https://github.com/RocketChat/Rocket.Chat/pull/15685) by [@antkaz](https://github.com/antkaz))

- Notify logged agents when their departments change ([#16033](https://github.com/RocketChat/Rocket.Chat/pull/16033))

- Replace adminRooms publication by REST ([#15948](https://github.com/RocketChat/Rocket.Chat/pull/15948) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace customSounds publication by REST ([#15907](https://github.com/RocketChat/Rocket.Chat/pull/15907) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace discussionsOfARoom publication by REST ([#15908](https://github.com/RocketChat/Rocket.Chat/pull/15908) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace forgotten livechat:departmentAgents subscriptions ([#15970](https://github.com/RocketChat/Rocket.Chat/pull/15970) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace fullEmojiData publication by REST ([#15901](https://github.com/RocketChat/Rocket.Chat/pull/15901) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace fullUserData publication by REST ([#15650](https://github.com/RocketChat/Rocket.Chat/pull/15650) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace fullUserStatusData publication by REST ([#15942](https://github.com/RocketChat/Rocket.Chat/pull/15942) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace integrations and integrationHistory publications by REST ([#15885](https://github.com/RocketChat/Rocket.Chat/pull/15885) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:customFields to REST ([#15496](https://github.com/RocketChat/Rocket.Chat/pull/15496) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:inquiry publication by REST and Streamer ([#15977](https://github.com/RocketChat/Rocket.Chat/pull/15977) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:managers publication by REST ([#15944](https://github.com/RocketChat/Rocket.Chat/pull/15944) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:officeHour publication to REST ([#15503](https://github.com/RocketChat/Rocket.Chat/pull/15503) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:queue subscription ([#15612](https://github.com/RocketChat/Rocket.Chat/pull/15612) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:rooms publication by REST ([#15968](https://github.com/RocketChat/Rocket.Chat/pull/15968) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:visitorHistory publication by REST ([#15943](https://github.com/RocketChat/Rocket.Chat/pull/15943) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace oauth publications by REST ([#15878](https://github.com/RocketChat/Rocket.Chat/pull/15878) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace roles publication by REST ([#15910](https://github.com/RocketChat/Rocket.Chat/pull/15910) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace stdout publication by REST ([#16004](https://github.com/RocketChat/Rocket.Chat/pull/16004) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace userAutocomplete publication by REST ([#15956](https://github.com/RocketChat/Rocket.Chat/pull/15956) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace userData subscriptions by REST ([#15916](https://github.com/RocketChat/Rocket.Chat/pull/15916) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace webdavAccounts publication by REST ([#15926](https://github.com/RocketChat/Rocket.Chat/pull/15926) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Sorting on livechat analytics queries were wrong ([#16021](https://github.com/RocketChat/Rocket.Chat/pull/16021) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Update ui for Roles field ([#15888](https://github.com/RocketChat/Rocket.Chat/pull/15888) by [@antkaz](https://github.com/antkaz))

- Validate user identity on send message process ([#15887](https://github.com/RocketChat/Rocket.Chat/pull/15887) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 🐛 Bug fixes


- Add time format for latest message on the sidebar ([#15930](https://github.com/RocketChat/Rocket.Chat/pull/15930) by [@ritwizsinha](https://github.com/ritwizsinha))

- Added Join button to Read Only rooms. ([#16016](https://github.com/RocketChat/Rocket.Chat/pull/16016))

- Admin menu not showing after renamed integration permissions ([#15937](https://github.com/RocketChat/Rocket.Chat/pull/15937) by [@n-se](https://github.com/n-se))

- Admin Setting descriptions and Storybook ([#15994](https://github.com/RocketChat/Rocket.Chat/pull/15994))

- Administration UI issues ([#15934](https://github.com/RocketChat/Rocket.Chat/pull/15934))

- Auto load image user preference ([#15895](https://github.com/RocketChat/Rocket.Chat/pull/15895))

- Changed renderMessage priority, fixed Katex on/off setting ([#16012](https://github.com/RocketChat/Rocket.Chat/pull/16012))

- Default value of the Livechat WebhookUrl setting ([#15898](https://github.com/RocketChat/Rocket.Chat/pull/15898))

- Don't throw an error when a message is prevented from apps engine ([#15850](https://github.com/RocketChat/Rocket.Chat/pull/15850) by [@wreiske](https://github.com/wreiske))

- Dropzone being stuck when dragging to thread ([#16006](https://github.com/RocketChat/Rocket.Chat/pull/16006))

- Empty security section when 2fa is disabled ([#16009](https://github.com/RocketChat/Rocket.Chat/pull/16009))

- Error of bind environment on user data export ([#15985](https://github.com/RocketChat/Rocket.Chat/pull/15985))

- Fix sort livechat rooms ([#16001](https://github.com/RocketChat/Rocket.Chat/pull/16001) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Guest's name field missing when forwarding livechat rooms ([#15991](https://github.com/RocketChat/Rocket.Chat/pull/15991))

- Importer: Variable name appearing instead of it's value ([#16010](https://github.com/RocketChat/Rocket.Chat/pull/16010) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Incorrect translation key on Livechat Appearance template ([#15975](https://github.com/RocketChat/Rocket.Chat/pull/15975) by [@ritwizsinha](https://github.com/ritwizsinha))

- Invalid Redirect URI on Custom OAuth ([#15957](https://github.com/RocketChat/Rocket.Chat/pull/15957))

- Livechat build without NodeJS installed ([#15903](https://github.com/RocketChat/Rocket.Chat/pull/15903) by [@localguru](https://github.com/localguru))

- Livechat permissions being overwrite on server restart ([#15915](https://github.com/RocketChat/Rocket.Chat/pull/15915))

- Livechat triggers not firing ([#15897](https://github.com/RocketChat/Rocket.Chat/pull/15897))

- Livechat Widget version 1.3.0 ([#15966](https://github.com/RocketChat/Rocket.Chat/pull/15966))

- Message list scrolling to bottom on reactions ([#16018](https://github.com/RocketChat/Rocket.Chat/pull/16018) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- new message popup ([#16017](https://github.com/RocketChat/Rocket.Chat/pull/16017) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- Registration form was hidden when login form was disabled ([#16062](https://github.com/RocketChat/Rocket.Chat/pull/16062))

- SAML logout error ([#15978](https://github.com/RocketChat/Rocket.Chat/pull/15978))

- Server crash on sync with no response ([#15919](https://github.com/RocketChat/Rocket.Chat/pull/15919))

- Thread Replies in Search ([#15841](https://github.com/RocketChat/Rocket.Chat/pull/15841))

- width of upload-progress-text ([#16023](https://github.com/RocketChat/Rocket.Chat/pull/16023) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- z-index of new message button ([#16013](https://github.com/RocketChat/Rocket.Chat/pull/16013) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

<details>
<summary>🔍 Minor changes</summary>


- [CHORE] Replace findOne with findOneById methods (Omnichannel) ([#15894](https://github.com/RocketChat/Rocket.Chat/pull/15894))

- Change migration number 169 <-> 170 ([#15940](https://github.com/RocketChat/Rocket.Chat/pull/15940))

- Check package-lock consistency with package.json on CI ([#15961](https://github.com/RocketChat/Rocket.Chat/pull/15961))

- Enable typescript lint ([#15979](https://github.com/RocketChat/Rocket.Chat/pull/15979))

- Fix 'How it all started' link on README ([#15962](https://github.com/RocketChat/Rocket.Chat/pull/15962) by [@zdumitru](https://github.com/zdumitru))

- Fix typo in Italian translation ([#15998](https://github.com/RocketChat/Rocket.Chat/pull/15998) by [@iannuzzelli](https://github.com/iannuzzelli))

- Fixed Grammatical Mistakes. ([#15570](https://github.com/RocketChat/Rocket.Chat/pull/15570) by [@breaking-let](https://github.com/breaking-let))

- GitHub CI ([#15918](https://github.com/RocketChat/Rocket.Chat/pull/15918))

- LingoHub based on develop ([#15988](https://github.com/RocketChat/Rocket.Chat/pull/15988))

- LingoHub based on develop ([#15939](https://github.com/RocketChat/Rocket.Chat/pull/15939))

- Merge master into develop & Set version to 3.0.0-develop ([#15872](https://github.com/RocketChat/Rocket.Chat/pull/15872))

- Meteor update to 1.8.2 ([#15873](https://github.com/RocketChat/Rocket.Chat/pull/15873))

- Regression: Missing button to copy Invite links ([#16084](https://github.com/RocketChat/Rocket.Chat/pull/16084))

- Regression: Update components ([#16053](https://github.com/RocketChat/Rocket.Chat/pull/16053))

- Remove unnecessary cron starts ([#15989](https://github.com/RocketChat/Rocket.Chat/pull/15989))

- Some performance improvements ([#15886](https://github.com/RocketChat/Rocket.Chat/pull/15886))

- Update Meteor to 1.8.3 ([#16037](https://github.com/RocketChat/Rocket.Chat/pull/16037))

- Update NodeJS to 8.17.0 ([#16043](https://github.com/RocketChat/Rocket.Chat/pull/16043))

- Upgrade limax to 2.0.0 ([#16020](https://github.com/RocketChat/Rocket.Chat/pull/16020))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@antkaz](https://github.com/antkaz)
- [@ashwaniYDV](https://github.com/ashwaniYDV)
- [@breaking-let](https://github.com/breaking-let)
- [@iannuzzelli](https://github.com/iannuzzelli)
- [@localguru](https://github.com/localguru)
- [@mariaeduardacunha](https://github.com/mariaeduardacunha)
- [@n-se](https://github.com/n-se)
- [@ritwizsinha](https://github.com/ritwizsinha)
- [@wreiske](https://github.com/wreiske)
- [@zdumitru](https://github.com/zdumitru)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 2.3.3
`2020-01-10  ·  1 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`

### 🐛 Bug fixes


- Add missing password field back to administration area ([#16171](https://github.com/RocketChat/Rocket.Chat/pull/16171))

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 2.3.2
`2019-12-12  ·  2 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Invalid Redirect URI on Custom OAuth ([#15957](https://github.com/RocketChat/Rocket.Chat/pull/15957))

- Livechat Widget version 1.3.0 ([#15966](https://github.com/RocketChat/Rocket.Chat/pull/15966))

### 👩‍💻👨‍💻 Core Team 🤓

- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)

# 2.3.1
`2019-12-09  ·  6 🐛  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Admin menu not showing after renamed integration permissions ([#15937](https://github.com/RocketChat/Rocket.Chat/pull/15937) by [@n-se](https://github.com/n-se))

- Administration UI issues ([#15934](https://github.com/RocketChat/Rocket.Chat/pull/15934))

- Auto load image user preference ([#15895](https://github.com/RocketChat/Rocket.Chat/pull/15895))

- Default value of the Livechat WebhookUrl setting ([#15898](https://github.com/RocketChat/Rocket.Chat/pull/15898))

- Livechat permissions being overwrite on server restart ([#15915](https://github.com/RocketChat/Rocket.Chat/pull/15915))

- Livechat triggers not firing ([#15897](https://github.com/RocketChat/Rocket.Chat/pull/15897))

### 👩‍💻👨‍💻 Contributors 😍

- [@n-se](https://github.com/n-se)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@tassoevan](https://github.com/tassoevan)

# 2.3.0
`2019-11-27  ·  13 🎉  ·  17 🚀  ·  26 🐛  ·  17 🔍  ·  17 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🎉 New features


- Add a new stream to emit and listen room data events ([#15770](https://github.com/RocketChat/Rocket.Chat/pull/15770))

- Add ability to users reset their own E2E key ([#15777](https://github.com/RocketChat/Rocket.Chat/pull/15777))

- add delete-own-message permission ([#15512](https://github.com/RocketChat/Rocket.Chat/pull/15512))

- Add forms to view and edit Livechat agents info ([#15703](https://github.com/RocketChat/Rocket.Chat/pull/15703))

- Allow Regexes on SAML user field mapping ([#15743](https://github.com/RocketChat/Rocket.Chat/pull/15743))

- Livechat analytics ([#15230](https://github.com/RocketChat/Rocket.Chat/pull/15230) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Livechat analytics functions ([#15666](https://github.com/RocketChat/Rocket.Chat/pull/15666) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Notify users when their email address change ([#15828](https://github.com/RocketChat/Rocket.Chat/pull/15828))

- Option for admins to set a random password to a user ([#15818](https://github.com/RocketChat/Rocket.Chat/pull/15818))

- Option on livechat departments to ensure a chat has tags before closing ([#15752](https://github.com/RocketChat/Rocket.Chat/pull/15752))

- SAML login without popup windows ([#15836](https://github.com/RocketChat/Rocket.Chat/pull/15836))

- Setting to dismiss desktop notification only after interaction ([#14807](https://github.com/RocketChat/Rocket.Chat/pull/14807) by [@mpdbl](https://github.com/mpdbl))

- Workspace Manual Registration ([#15442](https://github.com/RocketChat/Rocket.Chat/pull/15442))

### 🚀 Improvements


- Add more fields to iframe integration event `unread-changed-by-subscription` ([#15786](https://github.com/RocketChat/Rocket.Chat/pull/15786) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Administration UI - React and Fuselage components ([#15452](https://github.com/RocketChat/Rocket.Chat/pull/15452))

- Allow dragging of images and text from browsers ([#15691](https://github.com/RocketChat/Rocket.Chat/pull/15691))

- dynamic import  livechat views ([#15775](https://github.com/RocketChat/Rocket.Chat/pull/15775))

- Lazyload Chart.js ([#15764](https://github.com/RocketChat/Rocket.Chat/pull/15764))

- Lazyload qrcode lib ([#15741](https://github.com/RocketChat/Rocket.Chat/pull/15741))

- Make push notification batchsize and interval configurable ([#15804](https://github.com/RocketChat/Rocket.Chat/pull/15804) by [@Exordian](https://github.com/Exordian))

- Remove "EmojiCustom" unused subscription ([#15658](https://github.com/RocketChat/Rocket.Chat/pull/15658) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- remove computations inside messageAttachment ([#15716](https://github.com/RocketChat/Rocket.Chat/pull/15716))

- Replace livechat:departmentAgents subscription to REST ([#15529](https://github.com/RocketChat/Rocket.Chat/pull/15529) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:externalMessages publication by REST ([#15643](https://github.com/RocketChat/Rocket.Chat/pull/15643) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:pagesvisited publication by REST ([#15629](https://github.com/RocketChat/Rocket.Chat/pull/15629) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:visitorInfo publication by REST ([#15639](https://github.com/RocketChat/Rocket.Chat/pull/15639) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace personalAccessTokens publication by REST ([#15644](https://github.com/RocketChat/Rocket.Chat/pull/15644) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace snippetedMessage publication by REST ([#15679](https://github.com/RocketChat/Rocket.Chat/pull/15679) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace snipptedMessages publication by REST ([#15678](https://github.com/RocketChat/Rocket.Chat/pull/15678) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Unfollow own threads ([#15740](https://github.com/RocketChat/Rocket.Chat/pull/15740))

### 🐛 Bug fixes


- Add button to reset.css ([#15773](https://github.com/RocketChat/Rocket.Chat/pull/15773))

- Add livechat agents into departments ([#15732](https://github.com/RocketChat/Rocket.Chat/pull/15732))

- Apply server side filters on Livechat lists ([#15717](https://github.com/RocketChat/Rocket.Chat/pull/15717))

- Block Show_Setup_Wizard Option ([#15623](https://github.com/RocketChat/Rocket.Chat/pull/15623))

- Changed cmsPage Style ([#15632](https://github.com/RocketChat/Rocket.Chat/pull/15632))

- Channel notification audio preferences ([#15771](https://github.com/RocketChat/Rocket.Chat/pull/15771))

- Duplicate label 'Hide Avatars' in accounts ([#15694](https://github.com/RocketChat/Rocket.Chat/pull/15694) by [@rajvaibhavdubey](https://github.com/rajvaibhavdubey))

- Edit in thread ([#15640](https://github.com/RocketChat/Rocket.Chat/pull/15640))

- Error when exporting user data ([#15654](https://github.com/RocketChat/Rocket.Chat/pull/15654))

- Forward Livechat UI and the related permissions  ([#15718](https://github.com/RocketChat/Rocket.Chat/pull/15718))

- Ignore file uploads from message box if text/plain content is being pasted ([#15631](https://github.com/RocketChat/Rocket.Chat/pull/15631))

- line-height to show entire letters ([#15581](https://github.com/RocketChat/Rocket.Chat/pull/15581) by [@nstseek](https://github.com/nstseek))

- Livechat transfer history messages ([#15780](https://github.com/RocketChat/Rocket.Chat/pull/15780))

- Livechat webhook broken when sending an image ([#15699](https://github.com/RocketChat/Rocket.Chat/pull/15699) by [@tatosjb](https://github.com/tatosjb))

- Mentions before blockquote ([#15774](https://github.com/RocketChat/Rocket.Chat/pull/15774))

- Missing Privacy Policy Agree on register ([#15832](https://github.com/RocketChat/Rocket.Chat/pull/15832))

- Not valid relative URLs on message attachments ([#15651](https://github.com/RocketChat/Rocket.Chat/pull/15651) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Null value at Notifications Preferences tab ([#15638](https://github.com/RocketChat/Rocket.Chat/pull/15638))

- Pasting images on reply as thread ([#15811](https://github.com/RocketChat/Rocket.Chat/pull/15811))

- Prevent agent last message undefined ([#15809](https://github.com/RocketChat/Rocket.Chat/pull/15809) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Push: fix notification priority for google (FCM) ([#15803](https://github.com/RocketChat/Rocket.Chat/pull/15803) by [@Exordian](https://github.com/Exordian))

- REST endpoint `chat.syncMessages` returning an error with deleted messages ([#15824](https://github.com/RocketChat/Rocket.Chat/pull/15824) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Sending messages to livechat rooms without a subscription ([#15707](https://github.com/RocketChat/Rocket.Chat/pull/15707))

- Sidebar font color was not respecting theming ([#15745](https://github.com/RocketChat/Rocket.Chat/pull/15745) by [@mariaeduardacunha](https://github.com/mariaeduardacunha))

- typo on PT-BR translation ([#15645](https://github.com/RocketChat/Rocket.Chat/pull/15645))

- Use Media Devices API to guess if a microphone is not available ([#15636](https://github.com/RocketChat/Rocket.Chat/pull/15636))

<details>
<summary>🔍 Minor changes</summary>


- [CHORE] Add lingohub to readme ([#15849](https://github.com/RocketChat/Rocket.Chat/pull/15849))

- [REGRESSION] Add livechat room type to the room's file list ([#15795](https://github.com/RocketChat/Rocket.Chat/pull/15795) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix Livechat duplicated templates error ([#15869](https://github.com/RocketChat/Rocket.Chat/pull/15869))

- Fix notification migration ([#15783](https://github.com/RocketChat/Rocket.Chat/pull/15783))

- Improve LDAP Login Fallback setting description in portuguese ([#15655](https://github.com/RocketChat/Rocket.Chat/pull/15655))

- Improvements to random password field on user edit/creation ([#15870](https://github.com/RocketChat/Rocket.Chat/pull/15870))

- LingoHub based on develop ([#15822](https://github.com/RocketChat/Rocket.Chat/pull/15822))

- LingoHub based on develop ([#15763](https://github.com/RocketChat/Rocket.Chat/pull/15763))

- LingoHub based on develop ([#15728](https://github.com/RocketChat/Rocket.Chat/pull/15728))

- LingoHub based on develop ([#15688](https://github.com/RocketChat/Rocket.Chat/pull/15688))

- Merge master into develop & Set version to 2.3.0-develop ([#15683](https://github.com/RocketChat/Rocket.Chat/pull/15683))

- Regression: fix admin instances info page ([#15772](https://github.com/RocketChat/Rocket.Chat/pull/15772))

- Regression: Fix hide avatars in side bar preference ([#15709](https://github.com/RocketChat/Rocket.Chat/pull/15709))

- Regression: messageAttachments inside messageAttachments not receiving settings ([#15733](https://github.com/RocketChat/Rocket.Chat/pull/15733))

- Remove unused permission to reset users' E2E key ([#15860](https://github.com/RocketChat/Rocket.Chat/pull/15860))

- Remove yarn.lock ([#15689](https://github.com/RocketChat/Rocket.Chat/pull/15689))

- Update moment-timezone ([#15729](https://github.com/RocketChat/Rocket.Chat/pull/15729))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Exordian](https://github.com/Exordian)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@mariaeduardacunha](https://github.com/mariaeduardacunha)
- [@mpdbl](https://github.com/mpdbl)
- [@nstseek](https://github.com/nstseek)
- [@rajvaibhavdubey](https://github.com/rajvaibhavdubey)
- [@tatosjb](https://github.com/tatosjb)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@gabriellsh](https://github.com/gabriellsh)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 2.2.1
`2019-11-19  ·  2 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Markdown link parser ([#15794](https://github.com/RocketChat/Rocket.Chat/pull/15794))

- Updating an app via "Update" button errors out with "App already exists" ([#15814](https://github.com/RocketChat/Rocket.Chat/pull/15814))

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@ggazzo](https://github.com/ggazzo)

# 2.2.0
`2019-10-27  ·  14 🎉  ·  16 🚀  ·  24 🐛  ·  28 🔍  ·  27 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🎉 New features


- Accept GIFs and SVGs for Avatars converting them to PNG and keep transparency of PNGs ([#11385](https://github.com/RocketChat/Rocket.Chat/pull/11385))

- Add new Livechat appearance setting to set the conversation finished message ([#15577](https://github.com/RocketChat/Rocket.Chat/pull/15577))

- Add option to enable X-Frame-options header to avoid loading inside any Iframe ([#14698](https://github.com/RocketChat/Rocket.Chat/pull/14698) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add users.requestDataDownload API endpoint ([#14428](https://github.com/RocketChat/Rocket.Chat/pull/14428) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@ubarsaiyan](https://github.com/ubarsaiyan))

- Added file type filter to RoomFiles ([#15289](https://github.com/RocketChat/Rocket.Chat/pull/15289) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@juanpetterson](https://github.com/juanpetterson))

- Assign new Livechat conversations to bot agents first ([#15317](https://github.com/RocketChat/Rocket.Chat/pull/15317))

- Check if agent can receive new livechat conversations when its status is away/idle ([#15451](https://github.com/RocketChat/Rocket.Chat/pull/15451))

- close emoji box using Keyboard Escape key ([#13956](https://github.com/RocketChat/Rocket.Chat/pull/13956) by [@mohamedar97](https://github.com/mohamedar97))

- Import DMs from CSV files ([#15534](https://github.com/RocketChat/Rocket.Chat/pull/15534))

- Import SAML language and auto join SAML channels ([#14203](https://github.com/RocketChat/Rocket.Chat/pull/14203) by [@Hudell](https://github.com/Hudell) & [@unixtam](https://github.com/unixtam))

- Remove all closed Livechat chats ([#13991](https://github.com/RocketChat/Rocket.Chat/pull/13991) by [@knrt10](https://github.com/knrt10))

- Separate integration roles ([#13902](https://github.com/RocketChat/Rocket.Chat/pull/13902) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Thread support to apps slashcommands and slashcommand previews ([#15574](https://github.com/RocketChat/Rocket.Chat/pull/15574))

- Update livechat widget version to 1.2.5 ([#15600](https://github.com/RocketChat/Rocket.Chat/pull/15600))

### 🚀 Improvements


- Cache hasPermissions ([#15589](https://github.com/RocketChat/Rocket.Chat/pull/15589))

- Detach React components from Meteor API ([#15482](https://github.com/RocketChat/Rocket.Chat/pull/15482))

- Disable edit visitor's phone number in SMS conversations ([#15593](https://github.com/RocketChat/Rocket.Chat/pull/15593))

- Lazyload Katex Package ([#15398](https://github.com/RocketChat/Rocket.Chat/pull/15398))

- Replace `livechat:departments` publication by REST Calls ([#15478](https://github.com/RocketChat/Rocket.Chat/pull/15478) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace `livechat:triggers` publication by REST calls ([#15507](https://github.com/RocketChat/Rocket.Chat/pull/15507) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:agents pub by REST calls ([#15490](https://github.com/RocketChat/Rocket.Chat/pull/15490) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:appearance pub to REST ([#15510](https://github.com/RocketChat/Rocket.Chat/pull/15510) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace livechat:integration publication by REST ([#15607](https://github.com/RocketChat/Rocket.Chat/pull/15607) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace mentionedMessages publication to REST ([#15540](https://github.com/RocketChat/Rocket.Chat/pull/15540) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace pinned messages subscription ([#15544](https://github.com/RocketChat/Rocket.Chat/pull/15544) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace roomFilesWithSearchText subscription ([#15550](https://github.com/RocketChat/Rocket.Chat/pull/15550) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace some livechat:rooms subscriptions ([#15532](https://github.com/RocketChat/Rocket.Chat/pull/15532) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Replace starred messages subscription ([#15548](https://github.com/RocketChat/Rocket.Chat/pull/15548) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Secure cookies when using HTTPS connection ([#15500](https://github.com/RocketChat/Rocket.Chat/pull/15500))

- Update Fuselage components on SetupWizard ([#15457](https://github.com/RocketChat/Rocket.Chat/pull/15457))

### 🐛 Bug fixes


- Add a header for the createAt column in the Directory ([#15556](https://github.com/RocketChat/Rocket.Chat/pull/15556) by [@antkaz](https://github.com/antkaz))

- Add permissions for slashCommands ([#15525](https://github.com/RocketChat/Rocket.Chat/pull/15525) by [@antkaz](https://github.com/antkaz))

- Adding "Promise.await" in "livechat/message" endpoint ([#15541](https://github.com/RocketChat/Rocket.Chat/pull/15541) by [@rodrigokamada](https://github.com/rodrigokamada))

- adjustments for tooltips to show room name instead of id ([#14084](https://github.com/RocketChat/Rocket.Chat/pull/14084) by [@mohamedar97](https://github.com/mohamedar97))

- Compact view ([#15416](https://github.com/RocketChat/Rocket.Chat/pull/15416))

- Deny editing visitor's phone number in SMS conversations ([#15602](https://github.com/RocketChat/Rocket.Chat/pull/15602))

- Dynamic import of JS files were not working correctly ([#15598](https://github.com/RocketChat/Rocket.Chat/pull/15598))

- Emoji are rendered in URL ([#15516](https://github.com/RocketChat/Rocket.Chat/pull/15516) by [@oguhpereira](https://github.com/oguhpereira))

- Exposing some fields on server logs at debug level ([#15514](https://github.com/RocketChat/Rocket.Chat/pull/15514))

- Fix a typo on Alpha API `e2e.setUserPublicAndPivateKeys` renaming to `e2e.setUserPublicAndPrivateKeys` ([#13334](https://github.com/RocketChat/Rocket.Chat/pull/13334))

- Incorrect display of the button "Invite users" ([#15594](https://github.com/RocketChat/Rocket.Chat/pull/15594))

- Issues saving audio notifications ([#15428](https://github.com/RocketChat/Rocket.Chat/pull/15428) by [@scrivna](https://github.com/scrivna))

- Japanese translation for run import ([#15515](https://github.com/RocketChat/Rocket.Chat/pull/15515) by [@yusukeh0710](https://github.com/yusukeh0710))

- leak on stdout listeners ([#15586](https://github.com/RocketChat/Rocket.Chat/pull/15586))

- Method saveUser is not using password policy ([#15445](https://github.com/RocketChat/Rocket.Chat/pull/15445))

- Missing ending slash on publicFilePath of fileUpload ([#15506](https://github.com/RocketChat/Rocket.Chat/pull/15506))

- Promise await for sendMessage in livechat/messages endpoint ([#15460](https://github.com/RocketChat/Rocket.Chat/pull/15460) by [@hmagarotto](https://github.com/hmagarotto))

- Read Recepts was not working ([#15603](https://github.com/RocketChat/Rocket.Chat/pull/15603))

- Registration/login page now mobile friendly (#15422) ([#15520](https://github.com/RocketChat/Rocket.Chat/pull/15520) by [@nstseek](https://github.com/nstseek))

- Reset password was allowing empty values leading to an impossibility to login ([#15444](https://github.com/RocketChat/Rocket.Chat/pull/15444))

- Self-XSS in validation functionality ([#15564](https://github.com/RocketChat/Rocket.Chat/pull/15564))

- Showing announcement back ([#15615](https://github.com/RocketChat/Rocket.Chat/pull/15615))

- Typo in autotranslate method ([#15344](https://github.com/RocketChat/Rocket.Chat/pull/15344) by [@Montel](https://github.com/Montel))

- Update apps engine rooms converter to use transformMappedData ([#15546](https://github.com/RocketChat/Rocket.Chat/pull/15546))

<details>
<summary>🔍 Minor changes</summary>


- [CHORE] remove 'bulk-create-c' permission ([#15517](https://github.com/RocketChat/Rocket.Chat/pull/15517) by [@antkaz](https://github.com/antkaz))

- [CHORE] Split logger classes to avoid cyclic dependencies ([#15559](https://github.com/RocketChat/Rocket.Chat/pull/15559) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- [CHORE] Update latest Livechat widget version to 1.2.2 ([#15592](https://github.com/RocketChat/Rocket.Chat/pull/15592))

- [CHORE] Update latest Livechat widget version to 1.2.4 ([#15596](https://github.com/RocketChat/Rocket.Chat/pull/15596))

- [FEATURE] Rest API upload file returns message object ([#13821](https://github.com/RocketChat/Rocket.Chat/pull/13821) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@knrt10](https://github.com/knrt10))

- [REGRESSION] Fix remove department from list ([#15591](https://github.com/RocketChat/Rocket.Chat/pull/15591) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Chore: Add Client Setup Information to Issue Template ([#15625](https://github.com/RocketChat/Rocket.Chat/pull/15625))

- docs: remove rocket chat launcher link ([#15477](https://github.com/RocketChat/Rocket.Chat/pull/15477) by [@RafaelGSS](https://github.com/RafaelGSS))

- LingoHub based on develop ([#15487](https://github.com/RocketChat/Rocket.Chat/pull/15487))

- Livechat Issues ([#15473](https://github.com/RocketChat/Rocket.Chat/pull/15473))

- Merge master into develop ([#15680](https://github.com/RocketChat/Rocket.Chat/pull/15680) by [@knrt10](https://github.com/knrt10))

- Merge master into develop & Set version to 2.2.0-develop ([#15622](https://github.com/RocketChat/Rocket.Chat/pull/15622))

- Merge master into develop & Set version to 2.2.0-develop ([#15469](https://github.com/RocketChat/Rocket.Chat/pull/15469))

- Move publication deprecation warnings ([#15676](https://github.com/RocketChat/Rocket.Chat/pull/15676) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- New: Add dev dependency david badge to README ([#9058](https://github.com/RocketChat/Rocket.Chat/pull/9058) by [@robbyoconnor](https://github.com/robbyoconnor))

- Regression: add stdout publication back ([#15614](https://github.com/RocketChat/Rocket.Chat/pull/15614))

- Regression: AppRoomsConverter on Livechat rooms ([#15646](https://github.com/RocketChat/Rocket.Chat/pull/15646))

- Regression: Fix broken message formatting box ([#15599](https://github.com/RocketChat/Rocket.Chat/pull/15599))

- Regression: Fix package-lock.json ([#15561](https://github.com/RocketChat/Rocket.Chat/pull/15561))

- Regression: fix unknown role breaking hasPermission ([#15641](https://github.com/RocketChat/Rocket.Chat/pull/15641))

- Regression: hasPermission ignoring subscription roles ([#15652](https://github.com/RocketChat/Rocket.Chat/pull/15652))

- Regression: Move import to avoid circular dependencies ([#15628](https://github.com/RocketChat/Rocket.Chat/pull/15628) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Remove reference to obsolete template helper ([#15675](https://github.com/RocketChat/Rocket.Chat/pull/15675))

- Release 2.1.2 ([#15667](https://github.com/RocketChat/Rocket.Chat/pull/15667) by [@knrt10](https://github.com/knrt10))

- Remove unneeded nginx file ([#15483](https://github.com/RocketChat/Rocket.Chat/pull/15483))

- Reply HTTP requests with `X-XSS-Protection: 1` header ([#15498](https://github.com/RocketChat/Rocket.Chat/pull/15498))

- Revert fix package-lock.json ([#15563](https://github.com/RocketChat/Rocket.Chat/pull/15563))

- Updating license term ([#15476](https://github.com/RocketChat/Rocket.Chat/pull/15476))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Montel](https://github.com/Montel)
- [@RafaelGSS](https://github.com/RafaelGSS)
- [@antkaz](https://github.com/antkaz)
- [@hmagarotto](https://github.com/hmagarotto)
- [@juanpetterson](https://github.com/juanpetterson)
- [@knrt10](https://github.com/knrt10)
- [@mohamedar97](https://github.com/mohamedar97)
- [@nstseek](https://github.com/nstseek)
- [@oguhpereira](https://github.com/oguhpereira)
- [@robbyoconnor](https://github.com/robbyoconnor)
- [@rodrigokamada](https://github.com/rodrigokamada)
- [@scrivna](https://github.com/scrivna)
- [@ubarsaiyan](https://github.com/ubarsaiyan)
- [@unixtam](https://github.com/unixtam)
- [@yusukeh0710](https://github.com/yusukeh0710)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@d-gubert](https://github.com/d-gubert)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@mar-v](https://github.com/mar-v)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 2.1.3
`2019-11-19  ·  2 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Markdown link parser ([#15794](https://github.com/RocketChat/Rocket.Chat/pull/15794))

- Updating an app via "Update" button errors out with "App already exists" ([#15814](https://github.com/RocketChat/Rocket.Chat/pull/15814))

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@ggazzo](https://github.com/ggazzo)

# 2.1.2
`2019-10-25  ·  3 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Channel Announcements not working ([#14635](https://github.com/RocketChat/Rocket.Chat/pull/14635) by [@knrt10](https://github.com/knrt10))

- Exception when sending email of messages attachments undefined ([#15657](https://github.com/RocketChat/Rocket.Chat/pull/15657))

- Read Receipts were not working properly with subscriptions without ls ([#15656](https://github.com/RocketChat/Rocket.Chat/pull/15656))

<details>
<summary>🔍 Minor changes</summary>


- Release 2.1.2 ([#15667](https://github.com/RocketChat/Rocket.Chat/pull/15667) by [@knrt10](https://github.com/knrt10))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@knrt10](https://github.com/knrt10)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@tassoevan](https://github.com/tassoevan)

# 2.1.1
`2019-10-17  ·  2 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Dynamic import of JS files were not working correctly ([#15598](https://github.com/RocketChat/Rocket.Chat/pull/15598))

- Read Recepts was not working ([#15603](https://github.com/RocketChat/Rocket.Chat/pull/15603))

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)

# 2.1.0
`2019-09-27  ·  1 ️️️⚠️  ·  13 🎉  ·  12 🚀  ·  22 🐛  ·  22 🔍  ·  20 👩‍💻👨‍💻`

### Engine versions
- Node: `8.15.1`
- NPM: `6.9.0`
- MongoDB: `3.4, 3.6, 4.0`

### ⚠️ BREAKING CHANGES


- Deprecate old CORS API access for Cordova mobile app ([#15322](https://github.com/RocketChat/Rocket.Chat/pull/15322))

### 🎉 New features


- Add ability to disable email notifications globally ([#9667](https://github.com/RocketChat/Rocket.Chat/pull/9667) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@ferdifly](https://github.com/ferdifly))

- Add JWT to uploaded files urls ([#15297](https://github.com/RocketChat/Rocket.Chat/pull/15297) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Allow file sharing through Twilio(WhatsApp) integration ([#15415](https://github.com/RocketChat/Rocket.Chat/pull/15415))

- Apps engine Livechat ([#14626](https://github.com/RocketChat/Rocket.Chat/pull/14626))

- Expand SAML Users Role Settings ([#15277](https://github.com/RocketChat/Rocket.Chat/pull/15277) by [@Hudell](https://github.com/Hudell))

- Guess a user's name from SAML credentials ([#15240](https://github.com/RocketChat/Rocket.Chat/pull/15240) by [@mrsimpson](https://github.com/mrsimpson))

- Livechat setting to show/hide Agent Information on the widget ([#15216](https://github.com/RocketChat/Rocket.Chat/pull/15216))

- Only Load CodeMirror code when it is needed ([#15351](https://github.com/RocketChat/Rocket.Chat/pull/15351))

- Provide site-url to outgoing integrations ([#15238](https://github.com/RocketChat/Rocket.Chat/pull/15238) by [@mrsimpson](https://github.com/mrsimpson))

- SAML User Data Mapping ([#15404](https://github.com/RocketChat/Rocket.Chat/pull/15404))

- Setting to configure SAML context comparison ([#15229](https://github.com/RocketChat/Rocket.Chat/pull/15229))

- Setting to remove message contents from email notifications ([#15406](https://github.com/RocketChat/Rocket.Chat/pull/15406))

- Validate NotBefore and NotOnOrAfter SAML assertions ([#15226](https://github.com/RocketChat/Rocket.Chat/pull/15226))

### 🚀 Improvements


- A11y: Buttons, Images, Popups ([#15405](https://github.com/RocketChat/Rocket.Chat/pull/15405))

- Add CustomSounds.play() helper ([#15256](https://github.com/RocketChat/Rocket.Chat/pull/15256))

- Add missing indices used by read receipts ([#15316](https://github.com/RocketChat/Rocket.Chat/pull/15316))

- Add possibility of renaming a discussion ([#15122](https://github.com/RocketChat/Rocket.Chat/pull/15122) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Administration UI ([#15401](https://github.com/RocketChat/Rocket.Chat/pull/15401))

- AvatarBlockUnauthenticatedAccess do not call user.find if you dont have to ([#15355](https://github.com/RocketChat/Rocket.Chat/pull/15355))

- Change default user's preference for notifications to 'All messages' ([#15420](https://github.com/RocketChat/Rocket.Chat/pull/15420))

- improve autolinker flow ([#15340](https://github.com/RocketChat/Rocket.Chat/pull/15340))

- Make the agents field optional when updating Livechat departments ([#15400](https://github.com/RocketChat/Rocket.Chat/pull/15400))

- Remove global Blaze helpers ([#15414](https://github.com/RocketChat/Rocket.Chat/pull/15414))

- Replace LESS autoprefixer plugin ([#15260](https://github.com/RocketChat/Rocket.Chat/pull/15260))

- User data export ([#15294](https://github.com/RocketChat/Rocket.Chat/pull/15294) by [@Hudell](https://github.com/Hudell))

### 🐛 Bug fixes


- Add ENV VAR to enable users create token feature ([#15334](https://github.com/RocketChat/Rocket.Chat/pull/15334) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- CAS users can take control of Rocket.Chat accounts ([#15346](https://github.com/RocketChat/Rocket.Chat/pull/15346))

- Delivering real-time messages to users that left a room ([#15389](https://github.com/RocketChat/Rocket.Chat/pull/15389) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Don't allow email violating whitelist addresses ([#15339](https://github.com/RocketChat/Rocket.Chat/pull/15339) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Double send bug on message box ([#15409](https://github.com/RocketChat/Rocket.Chat/pull/15409))

- Duplicate Channels in Search-bar ([#15056](https://github.com/RocketChat/Rocket.Chat/pull/15056))

- Empty custom emojis on emoji picker ([#15392](https://github.com/RocketChat/Rocket.Chat/pull/15392))

- Federation messages notifications ([#15418](https://github.com/RocketChat/Rocket.Chat/pull/15418))

- Fix file uploads JWT ([#15412](https://github.com/RocketChat/Rocket.Chat/pull/15412) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Grammatical error in Not Found page ([#15382](https://github.com/RocketChat/Rocket.Chat/pull/15382))

- LDAP usernames get additional '.' if they contain numbers ([#14644](https://github.com/RocketChat/Rocket.Chat/pull/14644) by [@Hudell](https://github.com/Hudell))

- Limit exposed fields on some users. endpoints ([#15327](https://github.com/RocketChat/Rocket.Chat/pull/15327) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Message box not centered ([#15367](https://github.com/RocketChat/Rocket.Chat/pull/15367))

- Notify admin was generating errors when Rocket.Cat user was edited or deleted ([#15387](https://github.com/RocketChat/Rocket.Chat/pull/15387))

- Property "permission" in slash commands of custom apps (#14739) ([#14741](https://github.com/RocketChat/Rocket.Chat/pull/14741) by [@ifantom](https://github.com/ifantom))

- Prune messages by cron if room not updated ([#15252](https://github.com/RocketChat/Rocket.Chat/pull/15252))

- Reduce Message cache time to 500ms ([#15295](https://github.com/RocketChat/Rocket.Chat/pull/15295) by [@vickyokrm](https://github.com/vickyokrm))

- REST API to return only public custom fields ([#15292](https://github.com/RocketChat/Rocket.Chat/pull/15292) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST endpoint `users.setPreferences` to not override all user's preferences ([#15288](https://github.com/RocketChat/Rocket.Chat/pull/15288) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Set the DEFAULT_ECDH_CURVE to auto (#15245) ([#15365](https://github.com/RocketChat/Rocket.Chat/pull/15365) by [@dlundgren](https://github.com/dlundgren))

- Subscription record not having the `ls` field ([#14544](https://github.com/RocketChat/Rocket.Chat/pull/14544) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- User Profile Time Format ([#15385](https://github.com/RocketChat/Rocket.Chat/pull/15385))

<details>
<summary>🔍 Minor changes</summary>


- [CHORE] Move pathFor helper to templateHelpers directory ([#15255](https://github.com/RocketChat/Rocket.Chat/pull/15255))

- [CHORE] Remove obsolete modal template ([#15257](https://github.com/RocketChat/Rocket.Chat/pull/15257))

- [Fix] Missing space between last username & 'and' word in react notification ([#15384](https://github.com/RocketChat/Rocket.Chat/pull/15384) by [@zdumitru](https://github.com/zdumitru))

- Add a missing 'Discussion' translation key ([#14029](https://github.com/RocketChat/Rocket.Chat/pull/14029) by [@ura14h](https://github.com/ura14h))

- Fix typo in LDAP User Search setting description ([#15228](https://github.com/RocketChat/Rocket.Chat/pull/15228))

- Improve Polish translation ([#14060](https://github.com/RocketChat/Rocket.Chat/pull/14060) by [@stepek](https://github.com/stepek))

- Improve text of the search bar description ([#15353](https://github.com/RocketChat/Rocket.Chat/pull/15353))

- LingoHub based on develop ([#15377](https://github.com/RocketChat/Rocket.Chat/pull/15377))

- Merge master into develop & Set version to 2.1.0-develop ([#15357](https://github.com/RocketChat/Rocket.Chat/pull/15357) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: API CORS not working after Cordova being disabled by default ([#15443](https://github.com/RocketChat/Rocket.Chat/pull/15443))

- Regression: Favorite room button ([#15426](https://github.com/RocketChat/Rocket.Chat/pull/15426))

- Regression: Fix Commit Section when there is no commit info ([#15436](https://github.com/RocketChat/Rocket.Chat/pull/15436))

- Regression: Fix DDP metrics ([#15368](https://github.com/RocketChat/Rocket.Chat/pull/15368))

- Regression: Fix invalid version string error on marketplace screen ([#15437](https://github.com/RocketChat/Rocket.Chat/pull/15437))

- Regression: Messagebox height changing when typing ([#15380](https://github.com/RocketChat/Rocket.Chat/pull/15380))

- Regression: Prevent parsing empty custom field setting ([#15413](https://github.com/RocketChat/Rocket.Chat/pull/15413) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: setup wizard dynamic import using relative url ([#15432](https://github.com/RocketChat/Rocket.Chat/pull/15432))

- Remove GraphQL dependencies left ([#15356](https://github.com/RocketChat/Rocket.Chat/pull/15356) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove log ADMIN_PASS environment variable ([#15307](https://github.com/RocketChat/Rocket.Chat/pull/15307))

- Update Apps-Engine version to final version ([#15458](https://github.com/RocketChat/Rocket.Chat/pull/15458))

- Update Meteor to 1.8.1 ([#15358](https://github.com/RocketChat/Rocket.Chat/pull/15358))

- Use version 2 of the DeepL API ([#15364](https://github.com/RocketChat/Rocket.Chat/pull/15364) by [@vickyokrm](https://github.com/vickyokrm))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@dlundgren](https://github.com/dlundgren)
- [@ferdifly](https://github.com/ferdifly)
- [@ifantom](https://github.com/ifantom)
- [@mrsimpson](https://github.com/mrsimpson)
- [@stepek](https://github.com/stepek)
- [@ura14h](https://github.com/ura14h)
- [@vickyokrm](https://github.com/vickyokrm)
- [@zdumitru](https://github.com/zdumitru)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@alansikora](https://github.com/alansikora)
- [@d-gubert](https://github.com/d-gubert)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@pierre-lehnen-rc](https://github.com/pierre-lehnen-rc)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 2.0.1
`2019-11-19  ·  2 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Markdown link parser ([#15794](https://github.com/RocketChat/Rocket.Chat/pull/15794))

- Updating an app via "Update" button errors out with "App already exists" ([#15814](https://github.com/RocketChat/Rocket.Chat/pull/15814))

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@ggazzo](https://github.com/ggazzo)

# 2.0.0
`2019-09-12  ·  7 ️️️⚠️  ·  14 🎉  ·  6 🚀  ·  19 🐛  ·  39 🔍  ·  26 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.4, 3.6, 4.0`

### ⚠️ BREAKING CHANGES


- Federation refactor with addition of chained events ([#15206](https://github.com/RocketChat/Rocket.Chat/pull/15206))

- Remove GraphQL and grant packages ([#15192](https://github.com/RocketChat/Rocket.Chat/pull/15192))

- Remove old livechat client ([#15133](https://github.com/RocketChat/Rocket.Chat/pull/15133))

- Remove publication `roomSubscriptionsByRole` ([#15193](https://github.com/RocketChat/Rocket.Chat/pull/15193))

- Remove publication `usersInRole` ([#15194](https://github.com/RocketChat/Rocket.Chat/pull/15194))

- Remove support of MongoDB 3.2 and deprecate MongoDB 3.4 ([#15199](https://github.com/RocketChat/Rocket.Chat/pull/15199))

- Replace tap:i18n to add support to 3-digit locales ([#15109](https://github.com/RocketChat/Rocket.Chat/pull/15109))

### 🎉 New features


- Add autotranslate Rest endpoints ([#14885](https://github.com/RocketChat/Rocket.Chat/pull/14885) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add Mobex to the list of SMS service providers ([#14655](https://github.com/RocketChat/Rocket.Chat/pull/14655) by [@zolbayars](https://github.com/zolbayars))

- Assume that Rocket.Chat runs behind one proxy by default (HTTP_FORWARDED_COUNT=1) ([#15214](https://github.com/RocketChat/Rocket.Chat/pull/15214))

- Custom message popups ([#15117](https://github.com/RocketChat/Rocket.Chat/pull/15117) by [@Hudell](https://github.com/Hudell))

- Endpoint to fetch livechat rooms with several filters ([#15155](https://github.com/RocketChat/Rocket.Chat/pull/15155) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Granular permissions for settings ([#8942](https://github.com/RocketChat/Rocket.Chat/pull/8942) by [@mrsimpson](https://github.com/mrsimpson))

- Integrate DEEPL translation service to RC core ([#12174](https://github.com/RocketChat/Rocket.Chat/pull/12174) by [@mrsimpson](https://github.com/mrsimpson) & [@vickyokrm](https://github.com/vickyokrm))

- Jitsi meet room access via a token ([#12259](https://github.com/RocketChat/Rocket.Chat/pull/12259) by [@rrzharikov](https://github.com/rrzharikov))

- LDAP User Groups, Roles, and Channel Synchronization ([#14278](https://github.com/RocketChat/Rocket.Chat/pull/14278) by [@Hudell](https://github.com/Hudell) & [@wreiske](https://github.com/wreiske))

- Option to hide the button of Custom OAuth on login screen ([#15053](https://github.com/RocketChat/Rocket.Chat/pull/15053))

- Options for SAML auth for individual organizations needs ([#14275](https://github.com/RocketChat/Rocket.Chat/pull/14275) by [@Deltachaos](https://github.com/Deltachaos) & [@Hudell](https://github.com/Hudell))

- Rest API Endpoint to get pinned messages from a room  ([#13864](https://github.com/RocketChat/Rocket.Chat/pull/13864) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@thayannevls](https://github.com/thayannevls))

- Setup Wizard and Page not found, using React components ([#15204](https://github.com/RocketChat/Rocket.Chat/pull/15204))

- Support multiple push gateways ([#14902](https://github.com/RocketChat/Rocket.Chat/pull/14902) by [@cardoso](https://github.com/cardoso))

### 🚀 Improvements


- Add asset extension validation ([#15088](https://github.com/RocketChat/Rocket.Chat/pull/15088) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add limit of 50 user's resume tokens ([#15102](https://github.com/RocketChat/Rocket.Chat/pull/15102) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add possibility to use commands inside threads through Rest API ([#15167](https://github.com/RocketChat/Rocket.Chat/pull/15167) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Livechat User Management Improvements ([#14736](https://github.com/RocketChat/Rocket.Chat/pull/14736) by [@Hudell](https://github.com/Hudell))

- Message tooltips as everyone else ([#15135](https://github.com/RocketChat/Rocket.Chat/pull/15135))

- Refactoring the queuing and routing processes of new livechats ([#15003](https://github.com/RocketChat/Rocket.Chat/pull/15003))

### 🐛 Bug fixes


- "Discussion" label in Sidebar not hidden, when Discussions are disabled (#14660) ([#14682](https://github.com/RocketChat/Rocket.Chat/pull/14682) by [@ifantom](https://github.com/ifantom))

- Attachment download button behavior ([#15172](https://github.com/RocketChat/Rocket.Chat/pull/15172))

- cachedcollection calling multiple times SYNC ([#15104](https://github.com/RocketChat/Rocket.Chat/pull/15104))

- Forget user session on window close ([#15205](https://github.com/RocketChat/Rocket.Chat/pull/15205))

- IE11 -  callback createTreeWalker doesnt accept acceptNode ([#15157](https://github.com/RocketChat/Rocket.Chat/pull/15157))

- IE11 baseURI  ([#15319](https://github.com/RocketChat/Rocket.Chat/pull/15319))

- IE11 modal, menu action and edit user page ([#15201](https://github.com/RocketChat/Rocket.Chat/pull/15201))

- Mark room as read logic ([#15174](https://github.com/RocketChat/Rocket.Chat/pull/15174))

- Messages search scroll ([#15175](https://github.com/RocketChat/Rocket.Chat/pull/15175))

- Prevent to create discussion with empty name ([#14507](https://github.com/RocketChat/Rocket.Chat/pull/14507) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Rate limit incoming integrations (webhooks) ([#15038](https://github.com/RocketChat/Rocket.Chat/pull/15038) by [@mrsimpson](https://github.com/mrsimpson))

- Redirect on app manual install ([#15306](https://github.com/RocketChat/Rocket.Chat/pull/15306))

- Remove new hidden file and fix for .env files for Snap ([#15120](https://github.com/RocketChat/Rocket.Chat/pull/15120))

- Search message wrongly grouping messages ([#15094](https://github.com/RocketChat/Rocket.Chat/pull/15094))

- TabBar not loading template titles ([#15177](https://github.com/RocketChat/Rocket.Chat/pull/15177) by [@Hudell](https://github.com/Hudell))

- Threads contextual bar button visible even with threads disabled ([#14956](https://github.com/RocketChat/Rocket.Chat/pull/14956) by [@cesarmal](https://github.com/cesarmal))

- Typo in 'access-permissions_description' ja translation ([#15162](https://github.com/RocketChat/Rocket.Chat/pull/15162) by [@NatsumiKubo](https://github.com/NatsumiKubo))

- User's auto complete showing everyone on the server ([#15212](https://github.com/RocketChat/Rocket.Chat/pull/15212))

- Webdav crash ([#14918](https://github.com/RocketChat/Rocket.Chat/pull/14918) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

<details>
<summary>🔍 Minor changes</summary>


- Add new step to build Docker image from PRs for production again ([#15124](https://github.com/RocketChat/Rocket.Chat/pull/15124))

- Add oplog events metrics ([#15249](https://github.com/RocketChat/Rocket.Chat/pull/15249))

- Add wreiske to authorized users in catbot ([#15147](https://github.com/RocketChat/Rocket.Chat/pull/15147))

- Allow file upload paths on attachments URLs ([#15121](https://github.com/RocketChat/Rocket.Chat/pull/15121) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Change notifications file imports to server ([#15184](https://github.com/RocketChat/Rocket.Chat/pull/15184))

- Federation improvements ([#15234](https://github.com/RocketChat/Rocket.Chat/pull/15234))

- Federation migration and additional improvements ([#15336](https://github.com/RocketChat/Rocket.Chat/pull/15336))

- Fix apps list error ([#15258](https://github.com/RocketChat/Rocket.Chat/pull/15258))

- Fix automated test for manual user activation ([#14978](https://github.com/RocketChat/Rocket.Chat/pull/14978) by [@mrsimpson](https://github.com/mrsimpson))

- Fix get IP for rate limiter ([#15262](https://github.com/RocketChat/Rocket.Chat/pull/15262))

- Fix v148 migration ([#15285](https://github.com/RocketChat/Rocket.Chat/pull/15285))

- Improve url validation inside message object ([#15074](https://github.com/RocketChat/Rocket.Chat/pull/15074) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- LingoHub based on develop ([#15218](https://github.com/RocketChat/Rocket.Chat/pull/15218))

- LingoHub based on develop ([#15166](https://github.com/RocketChat/Rocket.Chat/pull/15166))

- LingoHub based on develop ([#15115](https://github.com/RocketChat/Rocket.Chat/pull/15115))

- Merge master into develop & Set version to 1.4.0-develop ([#15097](https://github.com/RocketChat/Rocket.Chat/pull/15097))

- NEW: Apps enable after app installed ([#15202](https://github.com/RocketChat/Rocket.Chat/pull/15202))

- Regression: addPermissionToRole argument as string ([#15267](https://github.com/RocketChat/Rocket.Chat/pull/15267))

- Regression: cachedCollection wrong callback parameters ([#15136](https://github.com/RocketChat/Rocket.Chat/pull/15136))

- Regression: Double error toast on Setup Wizard ([#15268](https://github.com/RocketChat/Rocket.Chat/pull/15268))

- Regression: Errors on the console preventing some settings to be saved ([#15310](https://github.com/RocketChat/Rocket.Chat/pull/15310))

- Regression: Fix assets extension detection ([#15231](https://github.com/RocketChat/Rocket.Chat/pull/15231))

- Regression: fix typo permisson to permission ([#15217](https://github.com/RocketChat/Rocket.Chat/pull/15217))

- Regression: Fix wrong import and minor code improvements ([#15352](https://github.com/RocketChat/Rocket.Chat/pull/15352))

- Regression: last message doesn't update after reconnect ([#15329](https://github.com/RocketChat/Rocket.Chat/pull/15329))

- Regression: New Livechat methods and processes ([#15242](https://github.com/RocketChat/Rocket.Chat/pull/15242))

- Regression: Remove duplicated permission changes emitter ([#15321](https://github.com/RocketChat/Rocket.Chat/pull/15321))

- Regression: remove livechat cache from circle ci ([#15183](https://github.com/RocketChat/Rocket.Chat/pull/15183))

- Regression: Remove old scripts of Setup Wizard ([#15263](https://github.com/RocketChat/Rocket.Chat/pull/15263))

- Release 1.3.2 ([#15176](https://github.com/RocketChat/Rocket.Chat/pull/15176))

- Remove GPG file ([#15146](https://github.com/RocketChat/Rocket.Chat/pull/15146))

- removed unwanted code ([#15078](https://github.com/RocketChat/Rocket.Chat/pull/15078) by [@httpsOmkar](https://github.com/httpsOmkar))

- Switch outdated roadmap to point to milestones ([#15156](https://github.com/RocketChat/Rocket.Chat/pull/15156))

- Update latest Livechat widget version to 1.1.4 ([#15173](https://github.com/RocketChat/Rocket.Chat/pull/15173))

- Update latest Livechat widget version(1.1.3) ([#15154](https://github.com/RocketChat/Rocket.Chat/pull/15154))

- Update Livechat to 1.1.6 ([#15186](https://github.com/RocketChat/Rocket.Chat/pull/15186))

- Update presence package ([#15178](https://github.com/RocketChat/Rocket.Chat/pull/15178))

- Update pt-BR.i18n.json ([#15083](https://github.com/RocketChat/Rocket.Chat/pull/15083) by [@lucassmacedo](https://github.com/lucassmacedo))

- Update to version 2.0.0-develop ([#15142](https://github.com/RocketChat/Rocket.Chat/pull/15142))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Deltachaos](https://github.com/Deltachaos)
- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@NatsumiKubo](https://github.com/NatsumiKubo)
- [@cardoso](https://github.com/cardoso)
- [@cesarmal](https://github.com/cesarmal)
- [@httpsOmkar](https://github.com/httpsOmkar)
- [@ifantom](https://github.com/ifantom)
- [@lucassmacedo](https://github.com/lucassmacedo)
- [@mrsimpson](https://github.com/mrsimpson)
- [@rrzharikov](https://github.com/rrzharikov)
- [@thayannevls](https://github.com/thayannevls)
- [@vickyokrm](https://github.com/vickyokrm)
- [@wreiske](https://github.com/wreiske)
- [@zolbayars](https://github.com/zolbayars)

### 👩‍💻👨‍💻 Core Team 🤓

- [@LuluGO](https://github.com/LuluGO)
- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@alansikora](https://github.com/alansikora)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 1.3.5
`2020-12-18  ·  2 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- Apps-Engine: `1.5.1`

### 🐛 Bug fixes


- Issue with special message rendering ([#19817](https://github.com/RocketChat/Rocket.Chat/pull/19817))

- Problem with attachment render ([#19854](https://github.com/RocketChat/Rocket.Chat/pull/19854))

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)

# 1.3.3
`2019-11-19  ·  2 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Markdown link parser ([#15794](https://github.com/RocketChat/Rocket.Chat/pull/15794))

- Updating an app via "Update" button errors out with "App already exists" ([#15814](https://github.com/RocketChat/Rocket.Chat/pull/15814))

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@ggazzo](https://github.com/ggazzo)

# 1.3.2
`2019-08-14  ·  3 🐛  ·  3 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Attachment download button behavior ([#15172](https://github.com/RocketChat/Rocket.Chat/pull/15172))

- IE11 -  callback createTreeWalker doesnt accept acceptNode ([#15157](https://github.com/RocketChat/Rocket.Chat/pull/15157))

- Messages search scroll ([#15175](https://github.com/RocketChat/Rocket.Chat/pull/15175))

<details>
<summary>🔍 Minor changes</summary>


- Release 1.3.2 ([#15176](https://github.com/RocketChat/Rocket.Chat/pull/15176))

- Update latest Livechat widget version to 1.1.4 ([#15173](https://github.com/RocketChat/Rocket.Chat/pull/15173))

- Update latest Livechat widget version(1.1.3) ([#15154](https://github.com/RocketChat/Rocket.Chat/pull/15154))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 1.3.1
`2019-08-08  ·  2 🐛  ·  2 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Custom emoji table scroll ([#15119](https://github.com/RocketChat/Rocket.Chat/pull/15119))

- Direct Message names not visible on Admin panel ([#15114](https://github.com/RocketChat/Rocket.Chat/pull/15114))

<details>
<summary>🔍 Minor changes</summary>


- Fix custom auth ([#15141](https://github.com/RocketChat/Rocket.Chat/pull/15141) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Release 1.3.1 ([#15148](https://github.com/RocketChat/Rocket.Chat/pull/15148) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 1.3.0
`2019-08-02  ·  9 🎉  ·  6 🚀  ·  32 🐛  ·  32 🔍  ·  29 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🎉 New features


- Accept multiple redirect URIs on OAuth Apps ([#14935](https://github.com/RocketChat/Rocket.Chat/pull/14935) by [@Hudell](https://github.com/Hudell))

- Deprecate MongoDB version 3.2 ([#15025](https://github.com/RocketChat/Rocket.Chat/pull/15025))

- Options to filter discussion and livechat on Admin > Rooms ([#15019](https://github.com/RocketChat/Rocket.Chat/pull/15019))

- Setting to configure custom authn context on SAML requests ([#14675](https://github.com/RocketChat/Rocket.Chat/pull/14675) by [@Hudell](https://github.com/Hudell))

- Setting to prevent Livechat agents online when Office Hours are closed ([#14921](https://github.com/RocketChat/Rocket.Chat/pull/14921))

- Settings to further customize GitLab OAuth ([#15014](https://github.com/RocketChat/Rocket.Chat/pull/15014) by [@Hudell](https://github.com/Hudell))

- Show helpful error when oplog is missing ([#14954](https://github.com/RocketChat/Rocket.Chat/pull/14954) by [@justinr1234](https://github.com/justinr1234))

- Subscription enabled marketplace ([#14948](https://github.com/RocketChat/Rocket.Chat/pull/14948))

- Webdav File Picker ([#14879](https://github.com/RocketChat/Rocket.Chat/pull/14879) by [@ubarsaiyan](https://github.com/ubarsaiyan))

### 🚀 Improvements


- Add descriptions on user data download buttons and popup info ([#14852](https://github.com/RocketChat/Rocket.Chat/pull/14852) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add flag to identify remote federation users ([#15004](https://github.com/RocketChat/Rocket.Chat/pull/15004))

- Connectivity Services License Sync ([#15022](https://github.com/RocketChat/Rocket.Chat/pull/15022))

- Extract federation config to its own file ([#14992](https://github.com/RocketChat/Rocket.Chat/pull/14992))

- Remove too specific helpers isFirefox() and isChrome() ([#14963](https://github.com/RocketChat/Rocket.Chat/pull/14963))

- Update tabs markup ([#14964](https://github.com/RocketChat/Rocket.Chat/pull/14964))

### 🐛 Bug fixes


- 50 custom emoji limit ([#14951](https://github.com/RocketChat/Rocket.Chat/pull/14951))

- Allow storing the navigation history of unregistered Livechat visitors ([#14970](https://github.com/RocketChat/Rocket.Chat/pull/14970))

- Always displaying jumbomojis when using "marked" markdown ([#14861](https://github.com/RocketChat/Rocket.Chat/pull/14861) by [@brakhane](https://github.com/brakhane))

- Chrome doesn't load additional search results when bottom is reached ([#14965](https://github.com/RocketChat/Rocket.Chat/pull/14965))

- Custom User Status throttled by rate limiter ([#15001](https://github.com/RocketChat/Rocket.Chat/pull/15001) by [@Hudell](https://github.com/Hudell))

- CustomOauth Identity Step errors displayed in HTML format ([#15000](https://github.com/RocketChat/Rocket.Chat/pull/15000) by [@Hudell](https://github.com/Hudell))

- Edit message with arrow up key if not last message ([#15021](https://github.com/RocketChat/Rocket.Chat/pull/15021))

- Edit permissions screen ([#14950](https://github.com/RocketChat/Rocket.Chat/pull/14950) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- eternal loading file list ([#14952](https://github.com/RocketChat/Rocket.Chat/pull/14952))

- Invite users auto complete cropping results ([#15020](https://github.com/RocketChat/Rocket.Chat/pull/15020))

- Jump to message missing in Starred Messages ([#14949](https://github.com/RocketChat/Rocket.Chat/pull/14949))

- LDAP login with customField sync ([#14808](https://github.com/RocketChat/Rocket.Chat/pull/14808) by [@magicbelette](https://github.com/magicbelette))

- Livechat dashboard average and reaction time labels ([#14845](https://github.com/RocketChat/Rocket.Chat/pull/14845) by [@anandpathak](https://github.com/anandpathak))

- load more messages ([#14967](https://github.com/RocketChat/Rocket.Chat/pull/14967))

- Loading indicator positioning ([#14968](https://github.com/RocketChat/Rocket.Chat/pull/14968))

- Message attachments not allowing float numbers ([#14412](https://github.com/RocketChat/Rocket.Chat/pull/14412) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Method `getUsersOfRoom` not returning offline users if limit is not defined ([#14753](https://github.com/RocketChat/Rocket.Chat/pull/14753) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Not being able to mention users with "all" and "here" usernames - do not allow users register that usernames ([#14468](https://github.com/RocketChat/Rocket.Chat/pull/14468) by [@hamidrezabstn](https://github.com/hamidrezabstn))

- Not sanitized message types ([#15054](https://github.com/RocketChat/Rocket.Chat/pull/15054))

- Opening Livechat messages on mobile apps ([#14785](https://github.com/RocketChat/Rocket.Chat/pull/14785) by [@zolbayars](https://github.com/zolbayars))

- OTR key icon missing on messages ([#14953](https://github.com/RocketChat/Rocket.Chat/pull/14953))

- Prevent error on trying insert message with duplicated id ([#14945](https://github.com/RocketChat/Rocket.Chat/pull/14945) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Russian grammatical errors ([#14622](https://github.com/RocketChat/Rocket.Chat/pull/14622) by [@BehindLoader](https://github.com/BehindLoader))

- SAML login by giving displayName priority over userName for fullName ([#14880](https://github.com/RocketChat/Rocket.Chat/pull/14880) by [@pkolmann](https://github.com/pkolmann))

- setupWizard calling multiple getSetupWizardParameters ([#15060](https://github.com/RocketChat/Rocket.Chat/pull/15060))

- SVG uploads crashing process ([#15006](https://github.com/RocketChat/Rocket.Chat/pull/15006) by [@snoopotic](https://github.com/snoopotic))

- Typo in german translation ([#14833](https://github.com/RocketChat/Rocket.Chat/pull/14833) by [@Le-onardo](https://github.com/Le-onardo))

- Users staying online after logout ([#14966](https://github.com/RocketChat/Rocket.Chat/pull/14966) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- users.setStatus REST endpoint not allowing reset status message ([#14916](https://github.com/RocketChat/Rocket.Chat/pull/14916) by [@cardoso](https://github.com/cardoso))

- Video recorder message echo ([#14671](https://github.com/RocketChat/Rocket.Chat/pull/14671) by [@vova-zush](https://github.com/vova-zush))

- Wrong custom status displayed on room leader panel ([#14958](https://github.com/RocketChat/Rocket.Chat/pull/14958) by [@Hudell](https://github.com/Hudell))

- Wrong label order on room settings ([#14960](https://github.com/RocketChat/Rocket.Chat/pull/14960) by [@Hudell](https://github.com/Hudell))

<details>
<summary>🔍 Minor changes</summary>


- [IMPROVEMENT] patch to improve emoji render ([#14722](https://github.com/RocketChat/Rocket.Chat/pull/14722))

- Add missing French translation ([#15013](https://github.com/RocketChat/Rocket.Chat/pull/15013) by [@commiaI](https://github.com/commiaI))

- Always convert the sha256 password to lowercase on checking ([#14941](https://github.com/RocketChat/Rocket.Chat/pull/14941) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Bump jquery from 3.3.1 to 3.4.0 in /packages/rocketchat-livechat/.app ([#14922](https://github.com/RocketChat/Rocket.Chat/pull/14922) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Bump juice version to 5.2.0 ([#14974](https://github.com/RocketChat/Rocket.Chat/pull/14974))

- Bump marked from 0.5.2 to 0.6.1 ([#14969](https://github.com/RocketChat/Rocket.Chat/pull/14969) by [@dependabot[bot]](https://github.com/dependabot[bot]))

- Bump node-rsa version to 1.0.5 ([#14976](https://github.com/RocketChat/Rocket.Chat/pull/14976))

- Bump photoswipe version to 4.1.3 ([#14977](https://github.com/RocketChat/Rocket.Chat/pull/14977))

- Callbacks perf ([#14915](https://github.com/RocketChat/Rocket.Chat/pull/14915))

- Extract canSendMessage function ([#14909](https://github.com/RocketChat/Rocket.Chat/pull/14909))

- Fix statistics error for apps on first load ([#15026](https://github.com/RocketChat/Rocket.Chat/pull/15026))

- Improve Docker compose readability ([#14457](https://github.com/RocketChat/Rocket.Chat/pull/14457) by [@NateScarlet](https://github.com/NateScarlet))

- Improve: Get public key for marketplace ([#14851](https://github.com/RocketChat/Rocket.Chat/pull/14851))

- improve: relocate some of wizard info to register ([#14884](https://github.com/RocketChat/Rocket.Chat/pull/14884))

- Merge master into develop & Set version to 1.3.0-develop ([#14889](https://github.com/RocketChat/Rocket.Chat/pull/14889) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- New: Apps and integrations statistics ([#14878](https://github.com/RocketChat/Rocket.Chat/pull/14878) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Apps and Marketplace UI issues ([#15045](https://github.com/RocketChat/Rocket.Chat/pull/15045))

- Regression: displaying errors for apps not installed from Marketplace ([#15075](https://github.com/RocketChat/Rocket.Chat/pull/15075))

- Regression: fix code style, setup wizard error and profile page header ([#15041](https://github.com/RocketChat/Rocket.Chat/pull/15041))

- Regression: Framework version being attached to a request that doesn't require it ([#15039](https://github.com/RocketChat/Rocket.Chat/pull/15039))

- Regression: getSetupWizardParameters ([#15067](https://github.com/RocketChat/Rocket.Chat/pull/15067))

- Regression: Improve apps bridges for HA setup ([#15080](https://github.com/RocketChat/Rocket.Chat/pull/15080))

- Regression: Marketplace app pricing plan description ([#15076](https://github.com/RocketChat/Rocket.Chat/pull/15076))

- Regression: patch to improve emoji render ([#14980](https://github.com/RocketChat/Rocket.Chat/pull/14980))

- Regression: uninstall subscribed app modal ([#15077](https://github.com/RocketChat/Rocket.Chat/pull/15077))

- Regression: Webdav File Picker search and fixed overflows ([#15027](https://github.com/RocketChat/Rocket.Chat/pull/15027) by [@ubarsaiyan](https://github.com/ubarsaiyan))

- Release 1.2.1 ([#14898](https://github.com/RocketChat/Rocket.Chat/pull/14898))

- Remove unused dependency (lokijs) ([#14973](https://github.com/RocketChat/Rocket.Chat/pull/14973))

- Remove unused Meteor dependency (yasinuslu:blaze-meta) ([#14971](https://github.com/RocketChat/Rocket.Chat/pull/14971))

- Split oplog emitters in files ([#14917](https://github.com/RocketChat/Rocket.Chat/pull/14917))

- Update Livechat widget ([#15046](https://github.com/RocketChat/Rocket.Chat/pull/15046))

- Wrong text when reporting a message ([#14515](https://github.com/RocketChat/Rocket.Chat/pull/14515) by [@zdumitru](https://github.com/zdumitru))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@BehindLoader](https://github.com/BehindLoader)
- [@Hudell](https://github.com/Hudell)
- [@Le-onardo](https://github.com/Le-onardo)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@NateScarlet](https://github.com/NateScarlet)
- [@anandpathak](https://github.com/anandpathak)
- [@brakhane](https://github.com/brakhane)
- [@cardoso](https://github.com/cardoso)
- [@commiaI](https://github.com/commiaI)
- [@dependabot[bot]](https://github.com/dependabot[bot])
- [@hamidrezabstn](https://github.com/hamidrezabstn)
- [@justinr1234](https://github.com/justinr1234)
- [@magicbelette](https://github.com/magicbelette)
- [@pkolmann](https://github.com/pkolmann)
- [@snoopotic](https://github.com/snoopotic)
- [@ubarsaiyan](https://github.com/ubarsaiyan)
- [@vova-zush](https://github.com/vova-zush)
- [@zdumitru](https://github.com/zdumitru)
- [@zolbayars](https://github.com/zolbayars)

### 👩‍💻👨‍💻 Core Team 🤓

- [@alansikora](https://github.com/alansikora)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 1.2.4
`2019-08-08  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Fix custom auth ([#15141](https://github.com/RocketChat/Rocket.Chat/pull/15141) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

# 1.2.2
`2019-07-29  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Not sanitized message types ([#15054](https://github.com/RocketChat/Rocket.Chat/pull/15054))

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)

# 1.2.1
`2019-06-28  ·  1 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Not showing local app on App Details ([#14894](https://github.com/RocketChat/Rocket.Chat/pull/14894))

<details>
<summary>🔍 Minor changes</summary>


- Release 1.2.1 ([#14898](https://github.com/RocketChat/Rocket.Chat/pull/14898))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 1.2.0
`2019-06-27  ·  8 🎉  ·  4 🚀  ·  13 🐛  ·  9 🔍  ·  21 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🎉 New features


- Add Livechat inquiries endpoints ([#14779](https://github.com/RocketChat/Rocket.Chat/pull/14779) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add loading animation to webdav file picker ([#14759](https://github.com/RocketChat/Rocket.Chat/pull/14759) by [@ubarsaiyan](https://github.com/ubarsaiyan))

- Add tmid property to outgoing integration ([#14699](https://github.com/RocketChat/Rocket.Chat/pull/14699) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- changed mongo version for snap from 3.2.7 to 3.4.20 ([#14838](https://github.com/RocketChat/Rocket.Chat/pull/14838))

- Configuration to limit amount of livechat inquiries displayed ([#14690](https://github.com/RocketChat/Rocket.Chat/pull/14690))

- Custom User Status ([#13933](https://github.com/RocketChat/Rocket.Chat/pull/13933) by [@Hudell](https://github.com/Hudell) & [@wreiske](https://github.com/wreiske))

- Endpoint to anonymously read channel's messages ([#14714](https://github.com/RocketChat/Rocket.Chat/pull/14714) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Show App bundles and its apps ([#14886](https://github.com/RocketChat/Rocket.Chat/pull/14886))

### 🚀 Improvements


- Add an optional rocketchat-protocol DNS entry for Federation ([#14589](https://github.com/RocketChat/Rocket.Chat/pull/14589))

- Adds link to download generated user data file ([#14175](https://github.com/RocketChat/Rocket.Chat/pull/14175) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Layout of livechat manager pages to new style ([#13900](https://github.com/RocketChat/Rocket.Chat/pull/13900))

- Use configurable colors on sidebar items ([#14624](https://github.com/RocketChat/Rocket.Chat/pull/14624))

### 🐛 Bug fixes


- Assume microphone is available ([#14710](https://github.com/RocketChat/Rocket.Chat/pull/14710))

- Custom status fixes ([#14853](https://github.com/RocketChat/Rocket.Chat/pull/14853) by [@Hudell](https://github.com/Hudell) & [@wreiske](https://github.com/wreiske))

- Direct reply delete config and description ([#14493](https://github.com/RocketChat/Rocket.Chat/pull/14493) by [@ruKurz](https://github.com/ruKurz))

- Error when using Download My Data or Export My Data ([#14645](https://github.com/RocketChat/Rocket.Chat/pull/14645) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Gap of messages when loading history when using threads ([#14837](https://github.com/RocketChat/Rocket.Chat/pull/14837))

- Import Chart.js error ([#14471](https://github.com/RocketChat/Rocket.Chat/pull/14471) by [@Hudell](https://github.com/Hudell) & [@sonbn0](https://github.com/sonbn0))

- Increasing time to rate limit in shield.svg endpoint and add a setting to disable API rate limiter ([#14709](https://github.com/RocketChat/Rocket.Chat/pull/14709) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- LinkedIn OAuth login ([#14887](https://github.com/RocketChat/Rocket.Chat/pull/14887) by [@Hudell](https://github.com/Hudell))

- Move the set Avatar call on user creation to make sure the user has username ([#14665](https://github.com/RocketChat/Rocket.Chat/pull/14665) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Name is undefined in some emails ([#14533](https://github.com/RocketChat/Rocket.Chat/pull/14533) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Removes E2E action button, icon and banner when E2E is disabled. ([#14810](https://github.com/RocketChat/Rocket.Chat/pull/14810))

- users typing forever ([#14724](https://github.com/RocketChat/Rocket.Chat/pull/14724))

- Wrong filter field when filtering current Livechats ([#14569](https://github.com/RocketChat/Rocket.Chat/pull/14569))

<details>
<summary>🔍 Minor changes</summary>


- Add custom fileupload whitelist property ([#14754](https://github.com/RocketChat/Rocket.Chat/pull/14754))

- Allow debugging of cached collections by name ([#14859](https://github.com/RocketChat/Rocket.Chat/pull/14859))

- Extract permissions functions ([#14777](https://github.com/RocketChat/Rocket.Chat/pull/14777))

- Fix not fully extracted pieces ([#14805](https://github.com/RocketChat/Rocket.Chat/pull/14805))

- Merge master into develop & Set version to 1.2.0-develop ([#14656](https://github.com/RocketChat/Rocket.Chat/pull/14656) by [@AnBo83](https://github.com/AnBo83) & [@knrt10](https://github.com/knrt10) & [@mohamedar97](https://github.com/mohamedar97) & [@thaiphv](https://github.com/thaiphv))

- Regression: Allow debugging of cached collections by name ([#14862](https://github.com/RocketChat/Rocket.Chat/pull/14862))

- Regression: Fix desktop notifications not being sent ([#14860](https://github.com/RocketChat/Rocket.Chat/pull/14860))

- Regression: Fix file upload ([#14804](https://github.com/RocketChat/Rocket.Chat/pull/14804))

- Regression: thread loading parent msg if is not loaded ([#14839](https://github.com/RocketChat/Rocket.Chat/pull/14839))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AnBo83](https://github.com/AnBo83)
- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@knrt10](https://github.com/knrt10)
- [@mohamedar97](https://github.com/mohamedar97)
- [@ruKurz](https://github.com/ruKurz)
- [@sonbn0](https://github.com/sonbn0)
- [@thaiphv](https://github.com/thaiphv)
- [@ubarsaiyan](https://github.com/ubarsaiyan)
- [@wreiske](https://github.com/wreiske)

### 👩‍💻👨‍💻 Core Team 🤓

- [@LuluGO](https://github.com/LuluGO)
- [@PrajvalRaval](https://github.com/PrajvalRaval)
- [@alansikora](https://github.com/alansikora)
- [@engelgabriel](https://github.com/engelgabriel)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 1.1.5
`2019-08-08  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Fix custom auth ([#15141](https://github.com/RocketChat/Rocket.Chat/pull/15141) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

# 1.1.4
`2019-07-29  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Not sanitized message types ([#15054](https://github.com/RocketChat/Rocket.Chat/pull/15054))

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)

# 1.1.3
`2019-06-21  ·  1 🐛  ·  2 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Gap of messages when loading history when using threads ([#14837](https://github.com/RocketChat/Rocket.Chat/pull/14837))

<details>
<summary>🔍 Minor changes</summary>


- Regression: thread loading parent msg if is not loaded ([#14839](https://github.com/RocketChat/Rocket.Chat/pull/14839))

- Release 1.1.3 ([#14850](https://github.com/RocketChat/Rocket.Chat/pull/14850))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 1.1.2
`2019-06-17  ·  3 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Anonymous chat read ([#14717](https://github.com/RocketChat/Rocket.Chat/pull/14717) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- User Real Name being erased when not modified ([#14711](https://github.com/RocketChat/Rocket.Chat/pull/14711) by [@Hudell](https://github.com/Hudell))

- User status information on User Info panel ([#14763](https://github.com/RocketChat/Rocket.Chat/pull/14763) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

<details>
<summary>🔍 Minor changes</summary>


- Release 1.1.2 ([#14823](https://github.com/RocketChat/Rocket.Chat/pull/14823) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 1.1.1
`2019-05-30  ·  2 🐛  ·  1 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Load messages after disconnect and message box scroll missing ([#14668](https://github.com/RocketChat/Rocket.Chat/pull/14668))

- SAML login error. ([#14686](https://github.com/RocketChat/Rocket.Chat/pull/14686) by [@Hudell](https://github.com/Hudell))

<details>
<summary>🔍 Minor changes</summary>


- Removing unnecesary federation configs ([#14674](https://github.com/RocketChat/Rocket.Chat/pull/14674))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)

### 👩‍💻👨‍💻 Core Team 🤓

- [@alansikora](https://github.com/alansikora)
- [@ggazzo](https://github.com/ggazzo)

# 1.1.0
`2019-05-27  ·  5 🎉  ·  10 🚀  ·  59 🐛  ·  35 🔍  ·  28 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🎉 New features


- Add pause and reset button when adding custom sound   ([#13615](https://github.com/RocketChat/Rocket.Chat/pull/13615) by [@knrt10](https://github.com/knrt10))

- Custom user name field from Custom OAuth ([#14381](https://github.com/RocketChat/Rocket.Chat/pull/14381) by [@mjovanovic0](https://github.com/mjovanovic0))

- Missing "view-outside-room_description" translation key ([#13680](https://github.com/RocketChat/Rocket.Chat/pull/13680) by [@bhardwajaditya](https://github.com/bhardwajaditya))

- Returns custom emojis through the Livechat REST API ([#14370](https://github.com/RocketChat/Rocket.Chat/pull/14370))

- Setting option to mark as containing a secret/password ([#10273](https://github.com/RocketChat/Rocket.Chat/pull/10273) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 🚀 Improvements


- Added flag `skipActiveUsersToBeReady` to not wait the load of `active users` to present the Web interface ([#14431](https://github.com/RocketChat/Rocket.Chat/pull/14431))

- Allow change Discussion's properties ([#14389](https://github.com/RocketChat/Rocket.Chat/pull/14389))

- Change user presence events to Meteor Streams ([#14488](https://github.com/RocketChat/Rocket.Chat/pull/14488))

- Don't show unread count badge in burger menu if it is from the opened room ([#12971](https://github.com/RocketChat/Rocket.Chat/pull/12971))

- Don't use regex to find users ([#14397](https://github.com/RocketChat/Rocket.Chat/pull/14397))

- jump to selected message on open thread ([#14460](https://github.com/RocketChat/Rocket.Chat/pull/14460))

- Livechat CRM secret token optional ([#14022](https://github.com/RocketChat/Rocket.Chat/pull/14022))

- Message rendering time ([#14252](https://github.com/RocketChat/Rocket.Chat/pull/14252))

- SAML login process refactoring ([#12891](https://github.com/RocketChat/Rocket.Chat/pull/12891) by [@kukkjanos](https://github.com/kukkjanos))

- Upgrade EmojiOne to JoyPixels 4.5.0 ([#13807](https://github.com/RocketChat/Rocket.Chat/pull/13807) by [@wreiske](https://github.com/wreiske))

### 🐛 Bug fixes


- "Blank page" on safari 10.x ([#14651](https://github.com/RocketChat/Rocket.Chat/pull/14651))

- `Alphabetical` translation in DE ([#14490](https://github.com/RocketChat/Rocket.Chat/pull/14490) by [@AnBo83](https://github.com/AnBo83))

- Allow data URLs in isURL/getURL helpers ([#14464](https://github.com/RocketChat/Rocket.Chat/pull/14464))

- Avatar images on old Livechat client ([#14590](https://github.com/RocketChat/Rocket.Chat/pull/14590) by [@arminfelder](https://github.com/arminfelder))

- Bell was too small on threads ([#14394](https://github.com/RocketChat/Rocket.Chat/pull/14394))

- Broken layout when sidebar is open on IE/Edge ([#14567](https://github.com/RocketChat/Rocket.Chat/pull/14567))

- Channel Leader Bar is in the way of Thread Header  ([#14443](https://github.com/RocketChat/Rocket.Chat/pull/14443))

- Channel names on Directory got cut on small screens ([#14542](https://github.com/RocketChat/Rocket.Chat/pull/14542))

- Channel settings form to textarea for Topic and Description ([#13328](https://github.com/RocketChat/Rocket.Chat/pull/13328) by [@supra08](https://github.com/supra08))

- Custom scripts descriptions were not clear enough  ([#14516](https://github.com/RocketChat/Rocket.Chat/pull/14516) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Discussion name being invalid ([#14442](https://github.com/RocketChat/Rocket.Chat/pull/14442))

- Downloading files when running in sub directory ([#14485](https://github.com/RocketChat/Rocket.Chat/pull/14485) by [@miolane](https://github.com/miolane))

- Duplicated link to jump to message  ([#14505](https://github.com/RocketChat/Rocket.Chat/pull/14505) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- E2E messages not decrypting in message threads ([#14580](https://github.com/RocketChat/Rocket.Chat/pull/14580))

- Edit Message when down arrow is pressed. ([#14369](https://github.com/RocketChat/Rocket.Chat/pull/14369) by [@Kailash0311](https://github.com/Kailash0311))

- Elements in User Info require some padding ([#13640](https://github.com/RocketChat/Rocket.Chat/pull/13640) by [@mushroomgenie](https://github.com/mushroomgenie))

- Error 400 on send a reply to an old thread ([#14402](https://github.com/RocketChat/Rocket.Chat/pull/14402))

- Error when accessing an invalid file upload url ([#14282](https://github.com/RocketChat/Rocket.Chat/pull/14282) by [@wreiske](https://github.com/wreiske))

- Error when accessing avatar with no token ([#14293](https://github.com/RocketChat/Rocket.Chat/pull/14293))

- Escape unrecognized slash command message ([#14432](https://github.com/RocketChat/Rocket.Chat/pull/14432))

- Exception on crowd sync due to a wrong logging method ([#14405](https://github.com/RocketChat/Rocket.Chat/pull/14405))

- Fallback to mongo version that doesn't require clusterMonitor role ([#14403](https://github.com/RocketChat/Rocket.Chat/pull/14403))

- Fix redirect to First channel after login ([#14434](https://github.com/RocketChat/Rocket.Chat/pull/14434) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- IE11 support ([#14422](https://github.com/RocketChat/Rocket.Chat/pull/14422))

- Ignored messages ([#14465](https://github.com/RocketChat/Rocket.Chat/pull/14465))

- Inject code at the end of <head> tag ([#14623](https://github.com/RocketChat/Rocket.Chat/pull/14623))

- Mailer breaking if user doesn't have an email address ([#14614](https://github.com/RocketChat/Rocket.Chat/pull/14614) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Main thread title on replies ([#14372](https://github.com/RocketChat/Rocket.Chat/pull/14372))

- Mentions message missing 'jump to message' action ([#14430](https://github.com/RocketChat/Rocket.Chat/pull/14430))

- Messages on thread panel were receiving wrong context/subscription ([#14404](https://github.com/RocketChat/Rocket.Chat/pull/14404))

- Messages on threads disappearing ([#14393](https://github.com/RocketChat/Rocket.Chat/pull/14393))

- more message actions to threads context(follow, unfollow, copy, delete) ([#14387](https://github.com/RocketChat/Rocket.Chat/pull/14387))

- Multiple Slack Importer Bugs ([#12084](https://github.com/RocketChat/Rocket.Chat/pull/12084) by [@Hudell](https://github.com/Hudell))

- New day separator overlapping above system message ([#14362](https://github.com/RocketChat/Rocket.Chat/pull/14362))

- No feedback when adding users that already exists in a room ([#14534](https://github.com/RocketChat/Rocket.Chat/pull/14534) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@gsunit](https://github.com/gsunit))

- Optional exit on Unhandled Promise Rejection ([#14291](https://github.com/RocketChat/Rocket.Chat/pull/14291))

- Popup cloud console in new window ([#14296](https://github.com/RocketChat/Rocket.Chat/pull/14296))

- Pressing Enter in User Search field at channel causes reload ([#14388](https://github.com/RocketChat/Rocket.Chat/pull/14388) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- preview pdf its not working ([#14419](https://github.com/RocketChat/Rocket.Chat/pull/14419))

- Remove Livechat guest data was removing more rooms than expected ([#14509](https://github.com/RocketChat/Rocket.Chat/pull/14509))

- RocketChat client sending out video call requests unnecessarily ([#14496](https://github.com/RocketChat/Rocket.Chat/pull/14496))

- Role `user` has being added after email verification even for non anonymous users ([#14263](https://github.com/RocketChat/Rocket.Chat/pull/14263) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Role name spacing on Permissions page ([#14625](https://github.com/RocketChat/Rocket.Chat/pull/14625))

- Room name was undefined in some info dialogs ([#14415](https://github.com/RocketChat/Rocket.Chat/pull/14415) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- SAML credentialToken removal was preventing mobile from being able to authenticate ([#14345](https://github.com/RocketChat/Rocket.Chat/pull/14345))

- Save custom emoji with special characters causes some errors ([#14456](https://github.com/RocketChat/Rocket.Chat/pull/14456) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Send replyTo for livechat offline messages ([#14568](https://github.com/RocketChat/Rocket.Chat/pull/14568))

- Several problems with read-only rooms and muted users ([#11311](https://github.com/RocketChat/Rocket.Chat/pull/11311) by [@Hudell](https://github.com/Hudell))

- Showing the id instead of the name of custom notification sound ([#13660](https://github.com/RocketChat/Rocket.Chat/pull/13660) by [@knrt10](https://github.com/knrt10))

- Startup error in registration check ([#14286](https://github.com/RocketChat/Rocket.Chat/pull/14286))

- Stream not connecting connect when using subdir and multi-instance ([#14376](https://github.com/RocketChat/Rocket.Chat/pull/14376))

- Switch oplog required doc link to more accurate link ([#14288](https://github.com/RocketChat/Rocket.Chat/pull/14288))

- Unnecessary meteor.defer on openRoom ([#14396](https://github.com/RocketChat/Rocket.Chat/pull/14396))

- Unread property of the room's lastMessage object was being wrong some times ([#13919](https://github.com/RocketChat/Rocket.Chat/pull/13919) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Users actions in administration were returning error ([#14400](https://github.com/RocketChat/Rocket.Chat/pull/14400))

- Verify if the user is requesting your own information in users.info ([#14242](https://github.com/RocketChat/Rocket.Chat/pull/14242) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Wrong header at Apps admin section ([#14290](https://github.com/RocketChat/Rocket.Chat/pull/14290))

- Wrong token name was generating error on Gitlab OAuth login ([#14379](https://github.com/RocketChat/Rocket.Chat/pull/14379) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- You must join to view messages in this channel ([#14461](https://github.com/RocketChat/Rocket.Chat/pull/14461))

<details>
<summary>🔍 Minor changes</summary>


- [Fix] broken logo url in app.json ([#14572](https://github.com/RocketChat/Rocket.Chat/pull/14572) by [@jaredmoody](https://github.com/jaredmoody))

- [IMPROVEMENT] Add tooltip to to notify user the purpose of back button in discussion ([#13872](https://github.com/RocketChat/Rocket.Chat/pull/13872) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- [IMPROVEMENT] Don't group messages with different alias ([#14257](https://github.com/RocketChat/Rocket.Chat/pull/14257) by [@jungeonkim](https://github.com/jungeonkim))

- [REGRESSION] Fix Slack bridge channel owner on channel creation ([#14565](https://github.com/RocketChat/Rocket.Chat/pull/14565) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add digitalocean button to readme ([#14583](https://github.com/RocketChat/Rocket.Chat/pull/14583))

- Add missing german translations ([#14386](https://github.com/RocketChat/Rocket.Chat/pull/14386) by [@mrsimpson](https://github.com/mrsimpson))

- Allow removing description, topic and annoucement of rooms(set as empty string) ([#13682](https://github.com/RocketChat/Rocket.Chat/pull/13682) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Ci improvements ([#14600](https://github.com/RocketChat/Rocket.Chat/pull/14600))

- eslint errors currently on develop ([#14518](https://github.com/RocketChat/Rocket.Chat/pull/14518) by [@Hudell](https://github.com/Hudell))

- Federation i18n message changes ([#14595](https://github.com/RocketChat/Rocket.Chat/pull/14595))

- fix discussions: remove restriction for editing room info, server side ([#14039](https://github.com/RocketChat/Rocket.Chat/pull/14039) by [@mrsimpson](https://github.com/mrsimpson))

- Fix emoji replacing some chars ([#14570](https://github.com/RocketChat/Rocket.Chat/pull/14570))

- Fix i18n files keys sort ([#14433](https://github.com/RocketChat/Rocket.Chat/pull/14433))

- Fix thumbs up emoji shortname ([#14581](https://github.com/RocketChat/Rocket.Chat/pull/14581))

- Fix: Add emoji shortnames to emoji's list ([#14576](https://github.com/RocketChat/Rocket.Chat/pull/14576))

- Fix: emoji render performance for alias ([#14593](https://github.com/RocketChat/Rocket.Chat/pull/14593))

- Fix: Message body was not being updated when user disabled nrr message ([#14390](https://github.com/RocketChat/Rocket.Chat/pull/14390))

- Fixes on DAU and MAU aggregations ([#14418](https://github.com/RocketChat/Rocket.Chat/pull/14418))

- Google Plus account is no longer accessible ([#14503](https://github.com/RocketChat/Rocket.Chat/pull/14503) by [@zdumitru](https://github.com/zdumitru))

- Improve German translations ([#14351](https://github.com/RocketChat/Rocket.Chat/pull/14351) by [@mrsimpson](https://github.com/mrsimpson))

- Improvement: Permissions table ([#14646](https://github.com/RocketChat/Rocket.Chat/pull/14646))

- LingoHub based on develop ([#14561](https://github.com/RocketChat/Rocket.Chat/pull/14561))

- LingoHub based on develop ([#14478](https://github.com/RocketChat/Rocket.Chat/pull/14478))

- LingoHub based on develop ([#14426](https://github.com/RocketChat/Rocket.Chat/pull/14426))

- LingoHub based on develop ([#14643](https://github.com/RocketChat/Rocket.Chat/pull/14643))

- Merge master into develop & Set version to 1.1.0-develop ([#14317](https://github.com/RocketChat/Rocket.Chat/pull/14317) by [@wreiske](https://github.com/wreiske))

- Merge master into develop & Set version to 1.1.0-develop ([#14294](https://github.com/RocketChat/Rocket.Chat/pull/14294))

- MsgTyping refactor ([#14495](https://github.com/RocketChat/Rocket.Chat/pull/14495))

- New eslint rules ([#14332](https://github.com/RocketChat/Rocket.Chat/pull/14332))

- Refactor WebRTC class ([#13736](https://github.com/RocketChat/Rocket.Chat/pull/13736))

- Regression: Handle missing emojis ([#14641](https://github.com/RocketChat/Rocket.Chat/pull/14641))

- Regression: unit tests were being skipped ([#14543](https://github.com/RocketChat/Rocket.Chat/pull/14543))

- Remove specific eslint rules ([#14459](https://github.com/RocketChat/Rocket.Chat/pull/14459))

- Removed unnecessary DDP unblocks ([#13641](https://github.com/RocketChat/Rocket.Chat/pull/13641))

- Update Meteor Streamer package ([#14551](https://github.com/RocketChat/Rocket.Chat/pull/14551))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AnBo83](https://github.com/AnBo83)
- [@Hudell](https://github.com/Hudell)
- [@Kailash0311](https://github.com/Kailash0311)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@arminfelder](https://github.com/arminfelder)
- [@ashwaniYDV](https://github.com/ashwaniYDV)
- [@bhardwajaditya](https://github.com/bhardwajaditya)
- [@gsunit](https://github.com/gsunit)
- [@jaredmoody](https://github.com/jaredmoody)
- [@jungeonkim](https://github.com/jungeonkim)
- [@knrt10](https://github.com/knrt10)
- [@kukkjanos](https://github.com/kukkjanos)
- [@miolane](https://github.com/miolane)
- [@mjovanovic0](https://github.com/mjovanovic0)
- [@mrsimpson](https://github.com/mrsimpson)
- [@mushroomgenie](https://github.com/mushroomgenie)
- [@supra08](https://github.com/supra08)
- [@wreiske](https://github.com/wreiske)
- [@zdumitru](https://github.com/zdumitru)

### 👩‍💻👨‍💻 Core Team 🤓

- [@alansikora](https://github.com/alansikora)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 1.0.5
`2019-08-08  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Fix custom auth ([#15141](https://github.com/RocketChat/Rocket.Chat/pull/15141) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

# 1.0.4
`2019-07-29  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Not sanitized message types ([#15054](https://github.com/RocketChat/Rocket.Chat/pull/15054))

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)

# 1.0.3
`2019-05-09  ·  1 🔍  ·  8 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

<details>
<summary>🔍 Minor changes</summary>


- Release 1.0.3 ([#14446](https://github.com/RocketChat/Rocket.Chat/pull/14446) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@mrsimpson](https://github.com/mrsimpson))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@mrsimpson](https://github.com/mrsimpson)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 1.0.2
`2019-04-30  ·  2 🚀  ·  8 🐛  ·  6 🔍  ·  10 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🚀 Improvements


- Better error message when not able to get MongoDB Version ([#14320](https://github.com/RocketChat/Rocket.Chat/pull/14320))

- i18n of threads and discussion buttons ([#14334](https://github.com/RocketChat/Rocket.Chat/pull/14334))

### 🐛 Bug fixes


- Audio notification for messages on DM ([#14336](https://github.com/RocketChat/Rocket.Chat/pull/14336))

- Duplicate thread message after editing ([#14330](https://github.com/RocketChat/Rocket.Chat/pull/14330))

- Missing i18n for some new Permissions ([#14011](https://github.com/RocketChat/Rocket.Chat/pull/14011))

- New day separator rendered over thread reply ([#14328](https://github.com/RocketChat/Rocket.Chat/pull/14328))

- Remove reference to inexistent field when deleting message in thread ([#14311](https://github.com/RocketChat/Rocket.Chat/pull/14311))

- show roles on message ([#14313](https://github.com/RocketChat/Rocket.Chat/pull/14313))

- Unread line and new day separator were not aligned  ([#14338](https://github.com/RocketChat/Rocket.Chat/pull/14338))

- View Logs admin page was broken and not rendering color logs ([#14316](https://github.com/RocketChat/Rocket.Chat/pull/14316))

<details>
<summary>🔍 Minor changes</summary>


- [Fix] group name appears instead of the room id ([#14075](https://github.com/RocketChat/Rocket.Chat/pull/14075) by [@mohamedar97](https://github.com/mohamedar97))

- [Regression] Anonymous user fix ([#14301](https://github.com/RocketChat/Rocket.Chat/pull/14301) by [@knrt10](https://github.com/knrt10))

- Add cross-browser select arrow positioning ([#14318](https://github.com/RocketChat/Rocket.Chat/pull/14318))

- Coerces the MongoDB version string ([#14299](https://github.com/RocketChat/Rocket.Chat/pull/14299) by [@thaiphv](https://github.com/thaiphv))

- i18n: Update German strings ([#14182](https://github.com/RocketChat/Rocket.Chat/pull/14182) by [@AnBo83](https://github.com/AnBo83))

- Release 1.0.2 ([#14339](https://github.com/RocketChat/Rocket.Chat/pull/14339) by [@AnBo83](https://github.com/AnBo83) & [@knrt10](https://github.com/knrt10) & [@mohamedar97](https://github.com/mohamedar97) & [@thaiphv](https://github.com/thaiphv))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AnBo83](https://github.com/AnBo83)
- [@knrt10](https://github.com/knrt10)
- [@mohamedar97](https://github.com/mohamedar97)
- [@thaiphv](https://github.com/thaiphv)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@ggazzo](https://github.com/ggazzo)
- [@lolimay](https://github.com/lolimay)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 1.0.1
`2019-04-28  ·  7 🐛  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Error when accessing an invalid file upload url ([#14282](https://github.com/RocketChat/Rocket.Chat/pull/14282) by [@wreiske](https://github.com/wreiske))

- Error when accessing avatar with no token ([#14293](https://github.com/RocketChat/Rocket.Chat/pull/14293))

- Optional exit on Unhandled Promise Rejection ([#14291](https://github.com/RocketChat/Rocket.Chat/pull/14291))

- Popup cloud console in new window ([#14296](https://github.com/RocketChat/Rocket.Chat/pull/14296))

- Startup error in registration check ([#14286](https://github.com/RocketChat/Rocket.Chat/pull/14286))

- Switch oplog required doc link to more accurate link ([#14288](https://github.com/RocketChat/Rocket.Chat/pull/14288))

- Wrong header at Apps admin section ([#14290](https://github.com/RocketChat/Rocket.Chat/pull/14290))

### 👩‍💻👨‍💻 Contributors 😍

- [@wreiske](https://github.com/wreiske)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@rodrigok](https://github.com/rodrigok)

# 1.0.0
`2019-04-28  ·  4 ️️️⚠️  ·  34 🎉  ·  33 🚀  ·  107 🐛  ·  174 🔍  ·  60 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### ⚠️ BREAKING CHANGES


- Prevent start if incompatible mongo version ([#13927](https://github.com/RocketChat/Rocket.Chat/pull/13927))

- Remove deprecated file upload engine Slingshot ([#13724](https://github.com/RocketChat/Rocket.Chat/pull/13724))

- Remove internal hubot package ([#13522](https://github.com/RocketChat/Rocket.Chat/pull/13522) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Require OPLOG/REPLICASET to run Rocket.Chat ([#14227](https://github.com/RocketChat/Rocket.Chat/pull/14227))

### 🎉 New features


- - Add setting to request a comment when closing Livechat room ([#13983](https://github.com/RocketChat/Rocket.Chat/pull/13983) by [@knrt10](https://github.com/knrt10))

- Add an option to delete file in files list ([#13815](https://github.com/RocketChat/Rocket.Chat/pull/13815))

- Add e-mail field on Livechat Departments ([#13775](https://github.com/RocketChat/Rocket.Chat/pull/13775))

- Add GET method to fetch Livechat message through REST API ([#14147](https://github.com/RocketChat/Rocket.Chat/pull/14147))

- Add message action to copy message to input as reply ([#12626](https://github.com/RocketChat/Rocket.Chat/pull/12626) by [@mrsimpson](https://github.com/mrsimpson))

- Add missing remove add leader channel ([#13315](https://github.com/RocketChat/Rocket.Chat/pull/13315) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@Montel](https://github.com/Montel))

- Add offset parameter to channels.history, groups.history, dm.history ([#13310](https://github.com/RocketChat/Rocket.Chat/pull/13310) by [@xbolshe](https://github.com/xbolshe))

- Add parseUrls field to the apps message converter ([#13248](https://github.com/RocketChat/Rocket.Chat/pull/13248))

- Add support to updatedSince parameter in emoji-custom.list and deprecated old endpoint ([#13510](https://github.com/RocketChat/Rocket.Chat/pull/13510) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add Voxtelesys to list of SMS providers ([#13697](https://github.com/RocketChat/Rocket.Chat/pull/13697) by [@jhnburke8](https://github.com/jhnburke8) & [@john08burke](https://github.com/john08burke))

- allow drop files on thread ([#14214](https://github.com/RocketChat/Rocket.Chat/pull/14214))

- Allow sending long messages as attachments ([#13819](https://github.com/RocketChat/Rocket.Chat/pull/13819))

- Bosnian lang (BS) ([#13635](https://github.com/RocketChat/Rocket.Chat/pull/13635) by [@fliptrail](https://github.com/fliptrail))

- Chatpal: Enable custom search parameters ([#13829](https://github.com/RocketChat/Rocket.Chat/pull/13829) by [@Peym4n](https://github.com/Peym4n))

- Collect data for Monthly/Daily Active Users for a future dashboard ([#11525](https://github.com/RocketChat/Rocket.Chat/pull/11525))

- Discussions ([#13541](https://github.com/RocketChat/Rocket.Chat/pull/13541) by [@mrsimpson](https://github.com/mrsimpson) & [@vickyokrm](https://github.com/vickyokrm))

- Federation ([#12370](https://github.com/RocketChat/Rocket.Chat/pull/12370))

- legal notice page ([#12472](https://github.com/RocketChat/Rocket.Chat/pull/12472) by [@localguru](https://github.com/localguru))

- Limit all DDP/Websocket requests (configurable via admin panel) ([#13311](https://github.com/RocketChat/Rocket.Chat/pull/13311))

- Marketplace integration with Rocket.Chat Cloud ([#13809](https://github.com/RocketChat/Rocket.Chat/pull/13809))

- Multiple slackbridges ([#11346](https://github.com/RocketChat/Rocket.Chat/pull/11346) by [@Hudell](https://github.com/Hudell) & [@kable-wilmoth](https://github.com/kable-wilmoth))

- option to not use nrr (experimental) ([#14224](https://github.com/RocketChat/Rocket.Chat/pull/14224))

- Permission to assign roles ([#13597](https://github.com/RocketChat/Rocket.Chat/pull/13597) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Provide new Livechat client as community feature ([#13723](https://github.com/RocketChat/Rocket.Chat/pull/13723))

- reply with a file ([#12095](https://github.com/RocketChat/Rocket.Chat/pull/12095) by [@rssilva](https://github.com/rssilva))

- REST endpoint to forward livechat rooms ([#13308](https://github.com/RocketChat/Rocket.Chat/pull/13308))

- Rest endpoints of discussions ([#13987](https://github.com/RocketChat/Rocket.Chat/pull/13987) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Rest threads ([#14045](https://github.com/RocketChat/Rocket.Chat/pull/14045) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Set up livechat connections created from new client ([#14236](https://github.com/RocketChat/Rocket.Chat/pull/14236))

- Show department field on Livechat visitor panel ([#13530](https://github.com/RocketChat/Rocket.Chat/pull/13530))

- Threads V 1.0 ([#13996](https://github.com/RocketChat/Rocket.Chat/pull/13996))

- Update message actions ([#14268](https://github.com/RocketChat/Rocket.Chat/pull/14268) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- User avatars from external source ([#7929](https://github.com/RocketChat/Rocket.Chat/pull/7929) by [@mjovanovic0](https://github.com/mjovanovic0))

- users.setActiveStatus endpoint in rest api ([#13443](https://github.com/RocketChat/Rocket.Chat/pull/13443) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@thayannevls](https://github.com/thayannevls))

### 🚀 Improvements


- Add decoding for commonName (cn) and displayName attributes for SAML ([#12347](https://github.com/RocketChat/Rocket.Chat/pull/12347) by [@pkolmann](https://github.com/pkolmann))

- Add department field on find guest method ([#13491](https://github.com/RocketChat/Rocket.Chat/pull/13491) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add index for room's ts ([#13726](https://github.com/RocketChat/Rocket.Chat/pull/13726))

- Add permission to change other user profile avatar ([#13884](https://github.com/RocketChat/Rocket.Chat/pull/13884) by [@knrt10](https://github.com/knrt10))

- Admin ui ([#13393](https://github.com/RocketChat/Rocket.Chat/pull/13393))

- Allow custom rocketchat username for crowd users and enable login via email/crowd_username ([#12981](https://github.com/RocketChat/Rocket.Chat/pull/12981) by [@steerben](https://github.com/steerben))

- Attachment download caching ([#14137](https://github.com/RocketChat/Rocket.Chat/pull/14137) by [@wreiske](https://github.com/wreiske))

- Deprecate fixCordova helper ([#13598](https://github.com/RocketChat/Rocket.Chat/pull/13598))

- Disable X-Powered-By header in all known express middlewares ([#13388](https://github.com/RocketChat/Rocket.Chat/pull/13388))

- End to end tests ([#13401](https://github.com/RocketChat/Rocket.Chat/pull/13401))

- Filter agents with autocomplete input instead of select element ([#13730](https://github.com/RocketChat/Rocket.Chat/pull/13730))

- Get avatar from oauth ([#14131](https://github.com/RocketChat/Rocket.Chat/pull/14131))

- Ignore agent status when queuing incoming livechats via Guest Pool ([#13818](https://github.com/RocketChat/Rocket.Chat/pull/13818))

- Include more information to help with bug reports and debugging ([#14047](https://github.com/RocketChat/Rocket.Chat/pull/14047))

- Join channels by sending a message or join button (#13752) ([#13752](https://github.com/RocketChat/Rocket.Chat/pull/13752) by [@bhardwajaditya](https://github.com/bhardwajaditya))

- KaTeX and Autolinker message rendering ([#11698](https://github.com/RocketChat/Rocket.Chat/pull/11698))

- Line height on static content pages ([#11673](https://github.com/RocketChat/Rocket.Chat/pull/11673) by [@timkinnane](https://github.com/timkinnane))

- new icons ([#13289](https://github.com/RocketChat/Rocket.Chat/pull/13289))

- New sidebar item badges, mention links, and ticks ([#14030](https://github.com/RocketChat/Rocket.Chat/pull/14030))

- OAuth Role Sync ([#13761](https://github.com/RocketChat/Rocket.Chat/pull/13761) by [@hypery2k](https://github.com/hypery2k))

- Remove dangling side-nav styles ([#13584](https://github.com/RocketChat/Rocket.Chat/pull/13584))

- Remove setting to show a livechat is waiting ([#13992](https://github.com/RocketChat/Rocket.Chat/pull/13992))

- Remove unnecessary "File Upload". ([#13743](https://github.com/RocketChat/Rocket.Chat/pull/13743) by [@knrt10](https://github.com/knrt10))

- Replace livechat inquiry dialog with preview room ([#13986](https://github.com/RocketChat/Rocket.Chat/pull/13986))

- Replaces color #13679A to #1d74f5 ([#13796](https://github.com/RocketChat/Rocket.Chat/pull/13796) by [@fliptrail](https://github.com/fliptrail))

- Send `uniqueID` to all clients so Jitsi rooms can be created correctly ([#13342](https://github.com/RocketChat/Rocket.Chat/pull/13342))

- Show rooms with mentions on unread category even with hide counter ([#13948](https://github.com/RocketChat/Rocket.Chat/pull/13948))

- UI of page not found ([#13757](https://github.com/RocketChat/Rocket.Chat/pull/13757) by [@fliptrail](https://github.com/fliptrail))

- UI of Permissions page ([#13732](https://github.com/RocketChat/Rocket.Chat/pull/13732) by [@fliptrail](https://github.com/fliptrail))

- Update deleteUser errors to be more semantic ([#12380](https://github.com/RocketChat/Rocket.Chat/pull/12380) by [@timkinnane](https://github.com/timkinnane))

- Update the Apps Engine version to v1.4.1 ([#14072](https://github.com/RocketChat/Rocket.Chat/pull/14072))

- Update to MongoDB 4.0 in docker-compose file ([#13396](https://github.com/RocketChat/Rocket.Chat/pull/13396) by [@ngulden](https://github.com/ngulden))

- Use SessionId for credential token in SAML request ([#13791](https://github.com/RocketChat/Rocket.Chat/pull/13791) by [@MohammedEssehemy](https://github.com/MohammedEssehemy))

### 🐛 Bug fixes


- .bin extension added to attached file names ([#13468](https://github.com/RocketChat/Rocket.Chat/pull/13468) by [@Hudell](https://github.com/Hudell))

- Ability to activate an app installed by zip even offline ([#13563](https://github.com/RocketChat/Rocket.Chat/pull/13563) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add custom MIME types for *.ico extension ([#13969](https://github.com/RocketChat/Rocket.Chat/pull/13969))

- Add retries to docker-compose.yml, to wait for MongoDB to be ready ([#13199](https://github.com/RocketChat/Rocket.Chat/pull/13199) by [@tiangolo](https://github.com/tiangolo))

- Adds Proper Language display name for many languages ([#13714](https://github.com/RocketChat/Rocket.Chat/pull/13714) by [@fliptrail](https://github.com/fliptrail))

- Align burger menu in header with content matching room header ([#14265](https://github.com/RocketChat/Rocket.Chat/pull/14265))

- allow user to logout before set username ([#13439](https://github.com/RocketChat/Rocket.Chat/pull/13439))

- Apps converters delete fields on message attachments ([#14028](https://github.com/RocketChat/Rocket.Chat/pull/14028))

- Attachments without dates were showing December 31, 1970 ([#13428](https://github.com/RocketChat/Rocket.Chat/pull/13428) by [@wreiske](https://github.com/wreiske))

- Audio message recording ([#13727](https://github.com/RocketChat/Rocket.Chat/pull/13727))

- Audio message recording issues ([#13486](https://github.com/RocketChat/Rocket.Chat/pull/13486))

- Auto hide Livechat room from sidebar on close ([#13824](https://github.com/RocketChat/Rocket.Chat/pull/13824) by [@knrt10](https://github.com/knrt10))

- Auto-translate toggle not updating rendered messages ([#14262](https://github.com/RocketChat/Rocket.Chat/pull/14262))

- Autogrow not working properly for many message boxes ([#14163](https://github.com/RocketChat/Rocket.Chat/pull/14163))

- Avatar fonts for PNG and JPG ([#13681](https://github.com/RocketChat/Rocket.Chat/pull/13681))

- Avatar image being shrinked on autocomplete ([#13914](https://github.com/RocketChat/Rocket.Chat/pull/13914))

- Block User Icon ([#13630](https://github.com/RocketChat/Rocket.Chat/pull/13630) by [@knrt10](https://github.com/knrt10))

- Bugfix markdown Marked link new tab ([#13245](https://github.com/RocketChat/Rocket.Chat/pull/13245) by [@DeviaVir](https://github.com/DeviaVir))

- Change localStorage keys to work when server is running in a subdir ([#13968](https://github.com/RocketChat/Rocket.Chat/pull/13968) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Change userId of rate limiter, change to logged user ([#13442](https://github.com/RocketChat/Rocket.Chat/pull/13442) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Changing Room name updates the webhook ([#13672](https://github.com/RocketChat/Rocket.Chat/pull/13672) by [@knrt10](https://github.com/knrt10))

- Check settings for name requirement before validating ([#14021](https://github.com/RocketChat/Rocket.Chat/pull/14021))

- Closing sidebar when room menu is clicked. ([#13842](https://github.com/RocketChat/Rocket.Chat/pull/13842) by [@Kailash0311](https://github.com/Kailash0311))

- Corrects UI background of forced F2A Authentication ([#13670](https://github.com/RocketChat/Rocket.Chat/pull/13670) by [@fliptrail](https://github.com/fliptrail))

- Custom Oauth login not working with accessToken ([#14113](https://github.com/RocketChat/Rocket.Chat/pull/14113) by [@knrt10](https://github.com/knrt10))

- Custom Oauth store refresh and id tokens with expiresIn ([#14121](https://github.com/RocketChat/Rocket.Chat/pull/14121) by [@ralfbecker](https://github.com/ralfbecker))

- Directory and Apps logs page ([#13938](https://github.com/RocketChat/Rocket.Chat/pull/13938))

- Display first message when taking Livechat inquiry ([#13896](https://github.com/RocketChat/Rocket.Chat/pull/13896))

- Do not allow change avatars of another users without permission ([#13629](https://github.com/RocketChat/Rocket.Chat/pull/13629) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Emoji detection at line breaks ([#13447](https://github.com/RocketChat/Rocket.Chat/pull/13447) by [@savish28](https://github.com/savish28))

- Empty result when getting badge count notification ([#14244](https://github.com/RocketChat/Rocket.Chat/pull/14244))

- Error when recording data into the connection object ([#13553](https://github.com/RocketChat/Rocket.Chat/pull/13553) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix bug when user try recreate channel or group with same name and remove room from cache when user leaves room ([#12341](https://github.com/RocketChat/Rocket.Chat/pull/12341) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix issue cannot filter channels by name ([#12952](https://github.com/RocketChat/Rocket.Chat/pull/12952) by [@huydang284](https://github.com/huydang284))

- Fix rendering of links in the announcement modal ([#13250](https://github.com/RocketChat/Rocket.Chat/pull/13250) by [@supra08](https://github.com/supra08))

- Fix snap refresh hook ([#13702](https://github.com/RocketChat/Rocket.Chat/pull/13702))

- Fix wrong this scope in Notifications ([#13515](https://github.com/RocketChat/Rocket.Chat/pull/13515) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fixed grammatical error. ([#13559](https://github.com/RocketChat/Rocket.Chat/pull/13559) by [@gsunit](https://github.com/gsunit))

- Fixed rocketchat-oembed meta fragment pulling ([#13056](https://github.com/RocketChat/Rocket.Chat/pull/13056) by [@wreiske](https://github.com/wreiske))

- Fixed text for "bulk-register-user" ([#11558](https://github.com/RocketChat/Rocket.Chat/pull/11558) by [@the4ndy](https://github.com/the4ndy))

- Fixing rooms find by type and name ([#11451](https://github.com/RocketChat/Rocket.Chat/pull/11451) by [@hmagarotto](https://github.com/hmagarotto))

- Focus on input when emoji picker box is open was not working ([#13981](https://github.com/RocketChat/Rocket.Chat/pull/13981))

- Forwarded Livechat visitor name is not getting updated on the sidebar ([#13783](https://github.com/RocketChat/Rocket.Chat/pull/13783) by [@zolbayars](https://github.com/zolbayars))

- Get next Livechat agent endpoint ([#13485](https://github.com/RocketChat/Rocket.Chat/pull/13485))

- Groups endpoints permission validations ([#13994](https://github.com/RocketChat/Rocket.Chat/pull/13994) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Handle showing/hiding input in messageBox ([#13564](https://github.com/RocketChat/Rocket.Chat/pull/13564))

- HipChat Enterprise importer fails when importing a large amount of messages (millions) ([#13221](https://github.com/RocketChat/Rocket.Chat/pull/13221) by [@Hudell](https://github.com/Hudell))

- Hipchat Enterprise Importer not generating subscriptions ([#13293](https://github.com/RocketChat/Rocket.Chat/pull/13293) by [@Hudell](https://github.com/Hudell))

- Image attachment re-renders on message update ([#14207](https://github.com/RocketChat/Rocket.Chat/pull/14207) by [@Kailash0311](https://github.com/Kailash0311))

- Improve cloud section ([#13820](https://github.com/RocketChat/Rocket.Chat/pull/13820))

- In home screen Rocket.Chat+ is dispalyed as Rocket.Chat ([#13784](https://github.com/RocketChat/Rocket.Chat/pull/13784) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Legal pages' style ([#13677](https://github.com/RocketChat/Rocket.Chat/pull/13677))

- Limit App’s HTTP calls to 500ms ([#13949](https://github.com/RocketChat/Rocket.Chat/pull/13949))

- linear-gradient background on safari ([#13363](https://github.com/RocketChat/Rocket.Chat/pull/13363))

- link of k8s deploy ([#13612](https://github.com/RocketChat/Rocket.Chat/pull/13612) by [@Mr-Linus](https://github.com/Mr-Linus))

- Links and upload paths when running in a subdir ([#13982](https://github.com/RocketChat/Rocket.Chat/pull/13982) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Livechat office hours ([#14031](https://github.com/RocketChat/Rocket.Chat/pull/14031) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Livechat user registration in another department ([#10695](https://github.com/RocketChat/Rocket.Chat/pull/10695))

- Loading theme CSS on first server startup ([#13953](https://github.com/RocketChat/Rocket.Chat/pull/13953))

- Loading user list from room messages ([#13769](https://github.com/RocketChat/Rocket.Chat/pull/13769))

- mention-links not being always resolved ([#11745](https://github.com/RocketChat/Rocket.Chat/pull/11745) by [@mrsimpson](https://github.com/mrsimpson))

- Message updating by Apps ([#13294](https://github.com/RocketChat/Rocket.Chat/pull/13294))

- Minor issues detected after testing the new Livechat client ([#13521](https://github.com/RocketChat/Rocket.Chat/pull/13521))

- Missing connection headers on Livechat REST API ([#14130](https://github.com/RocketChat/Rocket.Chat/pull/14130))

- Mobile view and re-enable E2E tests ([#13322](https://github.com/RocketChat/Rocket.Chat/pull/13322))

- No new room created when conversation is closed ([#13753](https://github.com/RocketChat/Rocket.Chat/pull/13753) by [@knrt10](https://github.com/knrt10))

- Non-latin room names and other slugifications ([#13467](https://github.com/RocketChat/Rocket.Chat/pull/13467))

- Normalize TAPi18n language string on Livechat widget ([#14012](https://github.com/RocketChat/Rocket.Chat/pull/14012))

- Obey audio notification preferences ([#14188](https://github.com/RocketChat/Rocket.Chat/pull/14188))

- Opening a Livechat room from another agent ([#13951](https://github.com/RocketChat/Rocket.Chat/pull/13951))

- OTR dialog issue ([#13755](https://github.com/RocketChat/Rocket.Chat/pull/13755) by [@knrt10](https://github.com/knrt10))

- Partially messaging formatting for bold letters ([#13599](https://github.com/RocketChat/Rocket.Chat/pull/13599) by [@knrt10](https://github.com/knrt10))

- Pass token for cloud register ([#13350](https://github.com/RocketChat/Rocket.Chat/pull/13350))

- Preview of image uploads were not working when apps framework is enable ([#13303](https://github.com/RocketChat/Rocket.Chat/pull/13303))

- Race condition on the loading of Apps on the admin page ([#13587](https://github.com/RocketChat/Rocket.Chat/pull/13587))

- Rate Limiter was limiting communication between instances ([#13326](https://github.com/RocketChat/Rocket.Chat/pull/13326))

- Read Receipt for Livechat Messages fixed ([#13832](https://github.com/RocketChat/Rocket.Chat/pull/13832) by [@knrt10](https://github.com/knrt10))

- Real names were not displayed in the reactions (API/UI) ([#13495](https://github.com/RocketChat/Rocket.Chat/pull/13495) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Receiving agent for new livechats from REST API ([#14103](https://github.com/RocketChat/Rocket.Chat/pull/14103))

- Remove Room info for Direct Messages (#9383) ([#12429](https://github.com/RocketChat/Rocket.Chat/pull/12429) by [@vinade](https://github.com/vinade))

- Remove spaces in some i18n files ([#13801](https://github.com/RocketChat/Rocket.Chat/pull/13801) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- renderField template to correct short property usage ([#14148](https://github.com/RocketChat/Rocket.Chat/pull/14148))

- REST endpoint for creating custom emojis ([#13306](https://github.com/RocketChat/Rocket.Chat/pull/13306))

- Restart required to apply changes in API Rate Limiter settings ([#13451](https://github.com/RocketChat/Rocket.Chat/pull/13451) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Right arrows in default HTML content ([#13502](https://github.com/RocketChat/Rocket.Chat/pull/13502))

- SAML certificate settings don't follow a pattern ([#14179](https://github.com/RocketChat/Rocket.Chat/pull/14179) by [@Hudell](https://github.com/Hudell))

- Setup wizard calling 'saveSetting' for each field/setting ([#13349](https://github.com/RocketChat/Rocket.Chat/pull/13349))

- Sidenav does not open on some admin pages ([#14010](https://github.com/RocketChat/Rocket.Chat/pull/14010) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Sidenav mouse hover was slow ([#13482](https://github.com/RocketChat/Rocket.Chat/pull/13482))

- Slackbridge private channels ([#14273](https://github.com/RocketChat/Rocket.Chat/pull/14273) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@nylen](https://github.com/nylen))

- Small improvements on message box ([#13444](https://github.com/RocketChat/Rocket.Chat/pull/13444))

- Some Safari bugs ([#13895](https://github.com/RocketChat/Rocket.Chat/pull/13895))

- Stop livestream ([#13676](https://github.com/RocketChat/Rocket.Chat/pull/13676))

- Support for handling SAML LogoutRequest SLO ([#14074](https://github.com/RocketChat/Rocket.Chat/pull/14074))

- Theme CSS loading in subdir env ([#14015](https://github.com/RocketChat/Rocket.Chat/pull/14015))

- Translation interpolations for many languages ([#13751](https://github.com/RocketChat/Rocket.Chat/pull/13751) by [@fliptrail](https://github.com/fliptrail))

- Typo in a referrer header in inject.js file ([#13469](https://github.com/RocketChat/Rocket.Chat/pull/13469) by [@algomaster99](https://github.com/algomaster99))

- Update bad-words to 3.0.2 ([#13705](https://github.com/RocketChat/Rocket.Chat/pull/13705) by [@trivoallan](https://github.com/trivoallan))

- Updating a message from apps if keep history is on ([#14129](https://github.com/RocketChat/Rocket.Chat/pull/14129))

- User is unable to enter multiple emojis by clicking on the emoji icon ([#13744](https://github.com/RocketChat/Rocket.Chat/pull/13744) by [@Kailash0311](https://github.com/Kailash0311))

- users.getPreferences when the user doesn't have any preferences ([#13532](https://github.com/RocketChat/Rocket.Chat/pull/13532) by [@thayannevls](https://github.com/thayannevls))

- VIDEO/JITSI multiple calls before video call ([#13855](https://github.com/RocketChat/Rocket.Chat/pull/13855))

- View All members button now not in direct room ([#14081](https://github.com/RocketChat/Rocket.Chat/pull/14081) by [@knrt10](https://github.com/knrt10))

- WebRTC wasn't working duo to design and browser's APIs changes ([#13675](https://github.com/RocketChat/Rocket.Chat/pull/13675))

- wrong importing of e2e ([#13863](https://github.com/RocketChat/Rocket.Chat/pull/13863))

- Wrong permalink when running in subdir ([#13746](https://github.com/RocketChat/Rocket.Chat/pull/13746) by [@ura14h](https://github.com/ura14h))

- wrong width/height for tile_70 (mstile 70x70 (png)) ([#13851](https://github.com/RocketChat/Rocket.Chat/pull/13851) by [@ulf-f](https://github.com/ulf-f))

<details>
<summary>🔍 Minor changes</summary>


-  Convert rocketchat-apps to main module structure ([#13409](https://github.com/RocketChat/Rocket.Chat/pull/13409) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-lib to main module structure ([#13415](https://github.com/RocketChat/Rocket.Chat/pull/13415) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Fix some imports from wrong packages, remove exports and files unused in rc-ui ([#13422](https://github.com/RocketChat/Rocket.Chat/pull/13422) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Import missed functions to remove dependency of RC namespace ([#13414](https://github.com/RocketChat/Rocket.Chat/pull/13414) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in livechat/client ([#13370](https://github.com/RocketChat/Rocket.Chat/pull/13370) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-integrations and importer-hipchat-enterprise ([#13386](https://github.com/RocketChat/Rocket.Chat/pull/13386) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-livechat/server/publications ([#13383](https://github.com/RocketChat/Rocket.Chat/pull/13383) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-message-pin and message-snippet ([#13343](https://github.com/RocketChat/Rocket.Chat/pull/13343) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-oembed and rc-otr ([#13345](https://github.com/RocketChat/Rocket.Chat/pull/13345) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-reactions, retention-policy and search ([#13347](https://github.com/RocketChat/Rocket.Chat/pull/13347) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-slash-archiveroom, create, help, hide, invite, inviteall and join ([#13356](https://github.com/RocketChat/Rocket.Chat/pull/13356) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-smarsh-connector, sms and spotify ([#13358](https://github.com/RocketChat/Rocket.Chat/pull/13358) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-statistics and tokenpass ([#13359](https://github.com/RocketChat/Rocket.Chat/pull/13359) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-ui-master, ui-message- user-data-download and version-check ([#13365](https://github.com/RocketChat/Rocket.Chat/pull/13365) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-ui, ui-account and ui-admin ([#13361](https://github.com/RocketChat/Rocket.Chat/pull/13361) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in rc-videobridge and webdav ([#13366](https://github.com/RocketChat/Rocket.Chat/pull/13366) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in root client folder, imports/message-read-receipt and imports/personal-access-tokens ([#13389](https://github.com/RocketChat/Rocket.Chat/pull/13389) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in root server folder - step 1 ([#13390](https://github.com/RocketChat/Rocket.Chat/pull/13390) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove dependency of RC namespace in root server folder - step 4 ([#13400](https://github.com/RocketChat/Rocket.Chat/pull/13400) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove functions from globals ([#13421](https://github.com/RocketChat/Rocket.Chat/pull/13421) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove LIvechat global variable from RC namespace ([#13378](https://github.com/RocketChat/Rocket.Chat/pull/13378) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove unused files and code in rc-lib - step 1 ([#13416](https://github.com/RocketChat/Rocket.Chat/pull/13416) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove unused files and code in rc-lib - step 3 ([#13420](https://github.com/RocketChat/Rocket.Chat/pull/13420) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Remove unused files in rc-lib - step 2 ([#13419](https://github.com/RocketChat/Rocket.Chat/pull/13419) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- [BUG] Icon Fixed for Knowledge base on Livechat  ([#13806](https://github.com/RocketChat/Rocket.Chat/pull/13806) by [@knrt10](https://github.com/knrt10))

- [New] Reply privately to group messages ([#14150](https://github.com/RocketChat/Rocket.Chat/pull/14150) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@bhardwajaditya](https://github.com/bhardwajaditya))

- [Regression] Fix integrations message example ([#14111](https://github.com/RocketChat/Rocket.Chat/pull/14111) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- [REGRESSION] Fix variable name references in message template ([#14184](https://github.com/RocketChat/Rocket.Chat/pull/14184))

- [REGRESSION] Messages sent by livechat's guests are losing sender info ([#14174](https://github.com/RocketChat/Rocket.Chat/pull/14174))

- [Regression] Personal Access Token list fixed ([#14216](https://github.com/RocketChat/Rocket.Chat/pull/14216) by [@knrt10](https://github.com/knrt10))

- Add better positioning for tooltips on edges ([#13472](https://github.com/RocketChat/Rocket.Chat/pull/13472))

- Add Houston config ([#13707](https://github.com/RocketChat/Rocket.Chat/pull/13707))

- Add pagination to getUsersOfRoom ([#12834](https://github.com/RocketChat/Rocket.Chat/pull/12834) by [@Hudell](https://github.com/Hudell))

- Add support to search for all users in directory ([#13803](https://github.com/RocketChat/Rocket.Chat/pull/13803))

- Added federation ping, loopback and dashboard ([#14007](https://github.com/RocketChat/Rocket.Chat/pull/14007))

- Adds French translation of Personal Access Token ([#13779](https://github.com/RocketChat/Rocket.Chat/pull/13779) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Allow set env var METEOR_OPLOG_TOO_FAR_BEHIND ([#14017](https://github.com/RocketChat/Rocket.Chat/pull/14017))

- Broken styles in Administration's contextual bar ([#14222](https://github.com/RocketChat/Rocket.Chat/pull/14222))

- Change dynamic dependency of FileUpload in Messages models ([#13776](https://github.com/RocketChat/Rocket.Chat/pull/13776) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Change the way to resolve DNS for Federation ([#13695](https://github.com/RocketChat/Rocket.Chat/pull/13695))

- Convert imports to relative paths ([#13740](https://github.com/RocketChat/Rocket.Chat/pull/13740))

- Convert rc-nrr and slashcommands open to main module structure ([#13520](https://github.com/RocketChat/Rocket.Chat/pull/13520) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- created function to allow change default values, fix loading search users ([#14177](https://github.com/RocketChat/Rocket.Chat/pull/14177))

- Depack: Use mainModule for root files ([#13508](https://github.com/RocketChat/Rocket.Chat/pull/13508))

- Depackaging ([#13483](https://github.com/RocketChat/Rocket.Chat/pull/13483) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Deprecate /api/v1/info in favor of /api/info ([#13798](https://github.com/RocketChat/Rocket.Chat/pull/13798) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- ESLint: Add more import rules ([#14226](https://github.com/RocketChat/Rocket.Chat/pull/14226))

- Exit process on unhandled rejection ([#14220](https://github.com/RocketChat/Rocket.Chat/pull/14220))

- Faster CI build for PR ([#14171](https://github.com/RocketChat/Rocket.Chat/pull/14171))

- Fix debug logging not being enabled by the setting ([#13979](https://github.com/RocketChat/Rocket.Chat/pull/13979))

- Fix discussions issues after room deletion and translation actions not being shown ([#14018](https://github.com/RocketChat/Rocket.Chat/pull/14018))

- Fix messages losing thread titles on editing or reaction and improve message actions ([#14051](https://github.com/RocketChat/Rocket.Chat/pull/14051))

- Fix missing dependencies on stretch CI image ([#13910](https://github.com/RocketChat/Rocket.Chat/pull/13910))

- Fix modal scroll ([#14052](https://github.com/RocketChat/Rocket.Chat/pull/14052))

- Fix race condition of lastMessage set ([#14041](https://github.com/RocketChat/Rocket.Chat/pull/14041))

- Fix room re-rendering ([#14044](https://github.com/RocketChat/Rocket.Chat/pull/14044))

- Fix sending message from action buttons in messages ([#14101](https://github.com/RocketChat/Rocket.Chat/pull/14101))

- Fix sending notifications to mentions on threads and discussion email sender ([#14043](https://github.com/RocketChat/Rocket.Chat/pull/14043))

- Fix shield indentation ([#14048](https://github.com/RocketChat/Rocket.Chat/pull/14048))

- Fix threads rendering performance ([#14059](https://github.com/RocketChat/Rocket.Chat/pull/14059))

- Fix threads tests ([#14180](https://github.com/RocketChat/Rocket.Chat/pull/14180))

- Fix top bar unread message counter ([#14102](https://github.com/RocketChat/Rocket.Chat/pull/14102))

- Fix update apps capability of updating messages ([#14118](https://github.com/RocketChat/Rocket.Chat/pull/14118))

- Fix wrong imports ([#13601](https://github.com/RocketChat/Rocket.Chat/pull/13601))

- Fix: addRoomAccessValidator method created for Threads ([#13789](https://github.com/RocketChat/Rocket.Chat/pull/13789))

- Fix: Error when version check endpoint was returning invalid data ([#14089](https://github.com/RocketChat/Rocket.Chat/pull/14089))

- Fix: Missing export in cloud package ([#13282](https://github.com/RocketChat/Rocket.Chat/pull/13282))

- Fix: Mongo.setConnectionOptions was not being set correctly ([#13586](https://github.com/RocketChat/Rocket.Chat/pull/13586))

- Fix: Remove message class `sequential` if `new-day` is present ([#14116](https://github.com/RocketChat/Rocket.Chat/pull/14116))

- Fix: Skip thread notifications on message edit ([#14100](https://github.com/RocketChat/Rocket.Chat/pull/14100))

- Fix: Some german translations ([#13299](https://github.com/RocketChat/Rocket.Chat/pull/13299) by [@soenkef](https://github.com/soenkef))

- Fix: Tests were not exiting RC instances ([#14054](https://github.com/RocketChat/Rocket.Chat/pull/14054))

- Force some words to translate in other languages ([#13367](https://github.com/RocketChat/Rocket.Chat/pull/13367) by [@soltanabadiyan](https://github.com/soltanabadiyan))

- Force unstyling of blockquote under .message-body--unstyled ([#14274](https://github.com/RocketChat/Rocket.Chat/pull/14274))

- Improve message validation ([#14266](https://github.com/RocketChat/Rocket.Chat/pull/14266))

- Improve: Decrease padding for app buy modal ([#13984](https://github.com/RocketChat/Rocket.Chat/pull/13984))

- Improve: Marketplace auth inside Rocket.Chat instead of inside the iframe.   ([#14258](https://github.com/RocketChat/Rocket.Chat/pull/14258))

- Improve: Send cloud token to Federation Hub ([#13651](https://github.com/RocketChat/Rocket.Chat/pull/13651))

- Improve: Support search and adding federated users through regular endpoints ([#13936](https://github.com/RocketChat/Rocket.Chat/pull/13936))

- Increment user counter on DMs ([#14185](https://github.com/RocketChat/Rocket.Chat/pull/14185))

- LingoHub based on develop ([#13964](https://github.com/RocketChat/Rocket.Chat/pull/13964))

- LingoHub based on develop ([#13891](https://github.com/RocketChat/Rocket.Chat/pull/13891))

- LingoHub based on develop ([#13839](https://github.com/RocketChat/Rocket.Chat/pull/13839))

- LingoHub based on develop ([#13623](https://github.com/RocketChat/Rocket.Chat/pull/13623))

- LingoHub based on develop ([#14046](https://github.com/RocketChat/Rocket.Chat/pull/14046))

- LingoHub based on develop ([#14178](https://github.com/RocketChat/Rocket.Chat/pull/14178))

- Lingohub sync and additional fixes ([#13825](https://github.com/RocketChat/Rocket.Chat/pull/13825))

- Merge master into develop & Set version to 1.0.0-develop ([#13435](https://github.com/RocketChat/Rocket.Chat/pull/13435) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@TkTech](https://github.com/TkTech) & [@theundefined](https://github.com/theundefined))

- Move LDAP Escape to login handler ([#14234](https://github.com/RocketChat/Rocket.Chat/pull/14234))

- Move mongo config away from cors package ([#13531](https://github.com/RocketChat/Rocket.Chat/pull/13531))

- Move rc-livechat server models to rc-models ([#13384](https://github.com/RocketChat/Rocket.Chat/pull/13384) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- New threads layout ([#14269](https://github.com/RocketChat/Rocket.Chat/pull/14269))

- OpenShift custom OAuth support ([#13925](https://github.com/RocketChat/Rocket.Chat/pull/13925) by [@bsharrow](https://github.com/bsharrow))

- Prevent click on reply thread to trigger flex tab closing ([#14215](https://github.com/RocketChat/Rocket.Chat/pull/14215))

- Prevent error for ldap login with invalid characters ([#14160](https://github.com/RocketChat/Rocket.Chat/pull/14160))

- Prevent error on normalize thread message for preview ([#14170](https://github.com/RocketChat/Rocket.Chat/pull/14170))

- Prioritize user-mentions badge ([#14057](https://github.com/RocketChat/Rocket.Chat/pull/14057))

- Proper thread quote, clear message box on send, and other nice things to have ([#14049](https://github.com/RocketChat/Rocket.Chat/pull/14049))

- Regression: Active room was not being marked ([#14276](https://github.com/RocketChat/Rocket.Chat/pull/14276))

- Regression: Add debounce on admin users search to avoid blocking by DDP Rate Limiter ([#13529](https://github.com/RocketChat/Rocket.Chat/pull/13529) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Add missing translations used in Apps pages ([#13674](https://github.com/RocketChat/Rocket.Chat/pull/13674))

- Regression: Admin embedded layout ([#14229](https://github.com/RocketChat/Rocket.Chat/pull/14229))

- Regression: Broken UI for messages ([#14223](https://github.com/RocketChat/Rocket.Chat/pull/14223))

- Regression: Cursor position set to beginning when editing a message ([#14245](https://github.com/RocketChat/Rocket.Chat/pull/14245))

- Regression: Discussions - Invite users and DM ([#13646](https://github.com/RocketChat/Rocket.Chat/pull/13646))

- Regression: Discussions were not showing on Tab Bar ([#14050](https://github.com/RocketChat/Rocket.Chat/pull/14050) by [@knrt10](https://github.com/knrt10))

- Regression: Exception on notification when adding someone in room via mention ([#14251](https://github.com/RocketChat/Rocket.Chat/pull/14251))

- Regression: fix app pages styles ([#13567](https://github.com/RocketChat/Rocket.Chat/pull/13567))

- Regression: Fix autolinker that was not parsing urls correctly ([#13497](https://github.com/RocketChat/Rocket.Chat/pull/13497) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: fix drop file ([#14225](https://github.com/RocketChat/Rocket.Chat/pull/14225))

- Regression: Fix embedded layout ([#13574](https://github.com/RocketChat/Rocket.Chat/pull/13574))

- Regression: fix grouping for reactive message ([#14246](https://github.com/RocketChat/Rocket.Chat/pull/14246))

- Regression: Fix icon for DMs ([#13679](https://github.com/RocketChat/Rocket.Chat/pull/13679))

- Regression: Fix wrong imports in rc-models ([#13516](https://github.com/RocketChat/Rocket.Chat/pull/13516) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: grouping messages on threads ([#14238](https://github.com/RocketChat/Rocket.Chat/pull/14238))

- Regression: Message box does not go back to initial state after sending a message ([#14161](https://github.com/RocketChat/Rocket.Chat/pull/14161))

- Regression: Message box geolocation was throwing error ([#13496](https://github.com/RocketChat/Rocket.Chat/pull/13496) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Missing settings import at `packages/rocketchat-livechat/server/methods/saveAppearance.js` ([#13573](https://github.com/RocketChat/Rocket.Chat/pull/13573) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Not updating subscriptions and not showing desktop notifcations ([#13509](https://github.com/RocketChat/Rocket.Chat/pull/13509))

- Regression: Prevent startup errors for mentions parsing ([#14219](https://github.com/RocketChat/Rocket.Chat/pull/14219))

- Regression: Prune Threads ([#13683](https://github.com/RocketChat/Rocket.Chat/pull/13683))

- Regression: Remove border from unstyled message body ([#14235](https://github.com/RocketChat/Rocket.Chat/pull/14235))

- Regression: removed backup files ([#13729](https://github.com/RocketChat/Rocket.Chat/pull/13729))

- Regression: Role creation and deletion error fixed ([#14097](https://github.com/RocketChat/Rocket.Chat/pull/14097) by [@knrt10](https://github.com/knrt10))

- Regression: Sidebar create new channel hover text ([#13658](https://github.com/RocketChat/Rocket.Chat/pull/13658) by [@bhardwajaditya](https://github.com/bhardwajaditya))

- Regression: System messages styling ([#14189](https://github.com/RocketChat/Rocket.Chat/pull/14189))

- Regression: Table admin pages ([#13411](https://github.com/RocketChat/Rocket.Chat/pull/13411))

- Regression: Template error ([#13410](https://github.com/RocketChat/Rocket.Chat/pull/13410))

- Regression: Threads styles improvement ([#13741](https://github.com/RocketChat/Rocket.Chat/pull/13741))

- Regression: User autocomplete was not listing users from correct room ([#14125](https://github.com/RocketChat/Rocket.Chat/pull/14125))

- Regression: User Discussions join message ([#13656](https://github.com/RocketChat/Rocket.Chat/pull/13656) by [@bhardwajaditya](https://github.com/bhardwajaditya))

- Regression: wrong expression at messageBox.actions.remove() ([#14192](https://github.com/RocketChat/Rocket.Chat/pull/14192))

- Remove bitcoin link in Readme.md since the link is broken ([#13935](https://github.com/RocketChat/Rocket.Chat/pull/13935) by [@ashwaniYDV](https://github.com/ashwaniYDV))

- Remove dependency of RC namespace in rc-livechat/imports, lib, server/api, server/hooks and server/lib ([#13379](https://github.com/RocketChat/Rocket.Chat/pull/13379) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in rc-livechat/server/methods ([#13382](https://github.com/RocketChat/Rocket.Chat/pull/13382) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in rc-livechat/server/models ([#13377](https://github.com/RocketChat/Rocket.Chat/pull/13377) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in rc-oauth2-server and message-star ([#13344](https://github.com/RocketChat/Rocket.Chat/pull/13344) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in rc-setup-wizard, slackbridge and asciiarts ([#13348](https://github.com/RocketChat/Rocket.Chat/pull/13348) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in rc-slash-kick, leave, me, msg, mute, open, topic and unarchiveroom ([#13357](https://github.com/RocketChat/Rocket.Chat/pull/13357) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in rc-ui-clean-history, ui-admin and ui-login ([#13362](https://github.com/RocketChat/Rocket.Chat/pull/13362) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in rc-wordpress, chatpal-search and irc ([#13492](https://github.com/RocketChat/Rocket.Chat/pull/13492) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in root server folder - step 2 ([#13397](https://github.com/RocketChat/Rocket.Chat/pull/13397) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in root server folder - step 3 ([#13398](https://github.com/RocketChat/Rocket.Chat/pull/13398) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in root server folder - step 5 ([#13402](https://github.com/RocketChat/Rocket.Chat/pull/13402) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RC namespace in root server folder - step 6 ([#13405](https://github.com/RocketChat/Rocket.Chat/pull/13405) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove Npm.depends and Npm.require except those that are inside package.js ([#13518](https://github.com/RocketChat/Rocket.Chat/pull/13518) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove Package references ([#13523](https://github.com/RocketChat/Rocket.Chat/pull/13523) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove Sandstorm support ([#13773](https://github.com/RocketChat/Rocket.Chat/pull/13773))

- Remove some bad references to messageBox ([#13954](https://github.com/RocketChat/Rocket.Chat/pull/13954))

- Remove some index.js files routing for server/client files ([#13772](https://github.com/RocketChat/Rocket.Chat/pull/13772))

- Remove unused files ([#13833](https://github.com/RocketChat/Rocket.Chat/pull/13833))

- Remove unused files ([#13725](https://github.com/RocketChat/Rocket.Chat/pull/13725))

- Remove unused style ([#13834](https://github.com/RocketChat/Rocket.Chat/pull/13834))

- Removed old templates ([#13406](https://github.com/RocketChat/Rocket.Chat/pull/13406))

- Removing (almost) every dynamic imports ([#13767](https://github.com/RocketChat/Rocket.Chat/pull/13767) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Rename Cloud to Connectivity Services & split Apps in Apps and Marketplace ([#14211](https://github.com/RocketChat/Rocket.Chat/pull/14211))

- Rename Threads to Discussion ([#13782](https://github.com/RocketChat/Rocket.Chat/pull/13782))

- Settings: disable reset button ([#14026](https://github.com/RocketChat/Rocket.Chat/pull/14026))

- Settings: hiding reset button for readonly fields ([#14025](https://github.com/RocketChat/Rocket.Chat/pull/14025))

- Show discussion avatar ([#14053](https://github.com/RocketChat/Rocket.Chat/pull/14053))

- Small improvements to federation callbacks/hooks ([#13946](https://github.com/RocketChat/Rocket.Chat/pull/13946))

- Smaller thread replies and system messages ([#14099](https://github.com/RocketChat/Rocket.Chat/pull/14099))

- Unify mime-type package configuration ([#14217](https://github.com/RocketChat/Rocket.Chat/pull/14217))

- Unstuck observers every minute ([#14076](https://github.com/RocketChat/Rocket.Chat/pull/14076))

- Update badges and mention links colors ([#14071](https://github.com/RocketChat/Rocket.Chat/pull/14071))

- Update eslint config ([#13966](https://github.com/RocketChat/Rocket.Chat/pull/13966))

- Update husky config ([#13687](https://github.com/RocketChat/Rocket.Chat/pull/13687))

- Update Meteor 1.8.0.2 ([#13519](https://github.com/RocketChat/Rocket.Chat/pull/13519))

- Update preview Dockerfile to use Stretch dependencies ([#13947](https://github.com/RocketChat/Rocket.Chat/pull/13947))

- Use CircleCI Debian Stretch images ([#13906](https://github.com/RocketChat/Rocket.Chat/pull/13906))

- Use main message as thread tab title ([#14213](https://github.com/RocketChat/Rocket.Chat/pull/14213))

- Use own logic to get thread infos via REST ([#14210](https://github.com/RocketChat/Rocket.Chat/pull/14210) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- User remove role dialog fixed ([#13874](https://github.com/RocketChat/Rocket.Chat/pull/13874) by [@bhardwajaditya](https://github.com/bhardwajaditya))

- Wait port release to finish tests ([#14066](https://github.com/RocketChat/Rocket.Chat/pull/14066))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@DeviaVir](https://github.com/DeviaVir)
- [@Hudell](https://github.com/Hudell)
- [@Kailash0311](https://github.com/Kailash0311)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@MohammedEssehemy](https://github.com/MohammedEssehemy)
- [@Montel](https://github.com/Montel)
- [@Mr-Linus](https://github.com/Mr-Linus)
- [@Peym4n](https://github.com/Peym4n)
- [@TkTech](https://github.com/TkTech)
- [@algomaster99](https://github.com/algomaster99)
- [@ashwaniYDV](https://github.com/ashwaniYDV)
- [@bhardwajaditya](https://github.com/bhardwajaditya)
- [@bsharrow](https://github.com/bsharrow)
- [@fliptrail](https://github.com/fliptrail)
- [@gsunit](https://github.com/gsunit)
- [@hmagarotto](https://github.com/hmagarotto)
- [@huydang284](https://github.com/huydang284)
- [@hypery2k](https://github.com/hypery2k)
- [@jhnburke8](https://github.com/jhnburke8)
- [@john08burke](https://github.com/john08burke)
- [@kable-wilmoth](https://github.com/kable-wilmoth)
- [@knrt10](https://github.com/knrt10)
- [@localguru](https://github.com/localguru)
- [@mjovanovic0](https://github.com/mjovanovic0)
- [@mrsimpson](https://github.com/mrsimpson)
- [@ngulden](https://github.com/ngulden)
- [@nylen](https://github.com/nylen)
- [@pkolmann](https://github.com/pkolmann)
- [@ralfbecker](https://github.com/ralfbecker)
- [@rssilva](https://github.com/rssilva)
- [@savish28](https://github.com/savish28)
- [@soenkef](https://github.com/soenkef)
- [@soltanabadiyan](https://github.com/soltanabadiyan)
- [@steerben](https://github.com/steerben)
- [@supra08](https://github.com/supra08)
- [@thayannevls](https://github.com/thayannevls)
- [@the4ndy](https://github.com/the4ndy)
- [@theundefined](https://github.com/theundefined)
- [@tiangolo](https://github.com/tiangolo)
- [@timkinnane](https://github.com/timkinnane)
- [@trivoallan](https://github.com/trivoallan)
- [@ulf-f](https://github.com/ulf-f)
- [@ura14h](https://github.com/ura14h)
- [@vickyokrm](https://github.com/vickyokrm)
- [@vinade](https://github.com/vinade)
- [@wreiske](https://github.com/wreiske)
- [@xbolshe](https://github.com/xbolshe)
- [@zolbayars](https://github.com/zolbayars)

### 👩‍💻👨‍💻 Core Team 🤓

- [@LuluGO](https://github.com/LuluGO)
- [@alansikora](https://github.com/alansikora)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.74.3
`2019-02-13  ·  3 🚀  ·  11 🐛  ·  3 🔍  ·  9 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🚀 Improvements


- Add API option "permissionsRequired" ([#13430](https://github.com/RocketChat/Rocket.Chat/pull/13430))

- Allow configure Prometheus port per process via Environment Variable ([#13436](https://github.com/RocketChat/Rocket.Chat/pull/13436))

- Open rooms quicker ([#13417](https://github.com/RocketChat/Rocket.Chat/pull/13417))

### 🐛 Bug fixes


- "Test Desktop Notifications" not triggering a notification ([#13457](https://github.com/RocketChat/Rocket.Chat/pull/13457))

- Invalid condition on getting next livechat agent over REST API endpoint ([#13360](https://github.com/RocketChat/Rocket.Chat/pull/13360))

- Invalid push gateway configuration, requires the uniqueId ([#13423](https://github.com/RocketChat/Rocket.Chat/pull/13423))

- Misaligned upload progress bar "cancel" button ([#13407](https://github.com/RocketChat/Rocket.Chat/pull/13407))

- Not translated emails ([#13452](https://github.com/RocketChat/Rocket.Chat/pull/13452))

- Notify private settings changes even on public settings changed ([#13369](https://github.com/RocketChat/Rocket.Chat/pull/13369))

- Properly escape custom emoji names for pattern matching ([#13408](https://github.com/RocketChat/Rocket.Chat/pull/13408))

- Several Problems on HipChat Importer ([#13336](https://github.com/RocketChat/Rocket.Chat/pull/13336) by [@Hudell](https://github.com/Hudell))

- Translated and incorrect i18n variables ([#13463](https://github.com/RocketChat/Rocket.Chat/pull/13463) by [@leonboot](https://github.com/leonboot))

- Update Russian localization ([#13244](https://github.com/RocketChat/Rocket.Chat/pull/13244) by [@BehindLoader](https://github.com/BehindLoader))

- XML-decryption module not found ([#13437](https://github.com/RocketChat/Rocket.Chat/pull/13437) by [@Hudell](https://github.com/Hudell))

<details>
<summary>🔍 Minor changes</summary>


- Regression: Remove console.log on email translations ([#13456](https://github.com/RocketChat/Rocket.Chat/pull/13456))

- Release 0.74.3 ([#13474](https://github.com/RocketChat/Rocket.Chat/pull/13474) by [@BehindLoader](https://github.com/BehindLoader) & [@Hudell](https://github.com/Hudell) & [@leonboot](https://github.com/leonboot))

- Room loading improvements ([#13471](https://github.com/RocketChat/Rocket.Chat/pull/13471))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@BehindLoader](https://github.com/BehindLoader)
- [@Hudell](https://github.com/Hudell)
- [@leonboot](https://github.com/leonboot)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.74.2
`2019-02-05  ·  1 🚀  ·  3 🐛  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🚀 Improvements


- Send `uniqueID` to all clients so Jitsi rooms can be created correctly ([#13342](https://github.com/RocketChat/Rocket.Chat/pull/13342))

### 🐛 Bug fixes


- Pass token for cloud register ([#13350](https://github.com/RocketChat/Rocket.Chat/pull/13350))

- Rate Limiter was limiting communication between instances ([#13326](https://github.com/RocketChat/Rocket.Chat/pull/13326))

- Setup wizard calling 'saveSetting' for each field/setting ([#13349](https://github.com/RocketChat/Rocket.Chat/pull/13349))

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.74.1
`2019-02-01  ·  4 🎉  ·  7 🐛  ·  1 🔍  ·  8 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🎉 New features


- Add parseUrls field to the apps message converter ([#13248](https://github.com/RocketChat/Rocket.Chat/pull/13248))

- Collect data for Monthly/Daily Active Users for a future dashboard ([#11525](https://github.com/RocketChat/Rocket.Chat/pull/11525))

- Limit all DDP/Websocket requests (configurable via admin panel) ([#13311](https://github.com/RocketChat/Rocket.Chat/pull/13311))

- REST endpoint to forward livechat rooms ([#13308](https://github.com/RocketChat/Rocket.Chat/pull/13308))

### 🐛 Bug fixes


- Fix bug when user try recreate channel or group with same name and remove room from cache when user leaves room ([#12341](https://github.com/RocketChat/Rocket.Chat/pull/12341) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- HipChat Enterprise importer fails when importing a large amount of messages (millions) ([#13221](https://github.com/RocketChat/Rocket.Chat/pull/13221) by [@Hudell](https://github.com/Hudell))

- Hipchat Enterprise Importer not generating subscriptions ([#13293](https://github.com/RocketChat/Rocket.Chat/pull/13293) by [@Hudell](https://github.com/Hudell))

- Message updating by Apps ([#13294](https://github.com/RocketChat/Rocket.Chat/pull/13294))

- Mobile view and re-enable E2E tests ([#13322](https://github.com/RocketChat/Rocket.Chat/pull/13322))

- Preview of image uploads were not working when apps framework is enable ([#13303](https://github.com/RocketChat/Rocket.Chat/pull/13303))

- REST endpoint for creating custom emojis ([#13306](https://github.com/RocketChat/Rocket.Chat/pull/13306))

<details>
<summary>🔍 Minor changes</summary>


- Fix: Missing export in cloud package ([#13282](https://github.com/RocketChat/Rocket.Chat/pull/13282))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.74.0
`2019-01-28  ·  11 🎉  ·  11 🚀  ·  15 🐛  ·  36 🔍  ·  22 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🎉 New features


- Add Allow Methods directive to CORS ([#13073](https://github.com/RocketChat/Rocket.Chat/pull/13073) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add create, update and delete endpoint for custom emojis ([#13160](https://github.com/RocketChat/Rocket.Chat/pull/13160) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add new Livechat REST endpoint to update the visitor's status ([#13108](https://github.com/RocketChat/Rocket.Chat/pull/13108))

- Add rate limiter to REST endpoints ([#11251](https://github.com/RocketChat/Rocket.Chat/pull/11251) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added an option to disable email when activate and deactivate users ([#13183](https://github.com/RocketChat/Rocket.Chat/pull/13183) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added endpoint to update timeout of the jitsi video conference ([#13167](https://github.com/RocketChat/Rocket.Chat/pull/13167) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added stream to notify when agent status change ([#13076](https://github.com/RocketChat/Rocket.Chat/pull/13076) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Cloud Integration ([#13013](https://github.com/RocketChat/Rocket.Chat/pull/13013))

- Display total number of files and total upload size in admin ([#13184](https://github.com/RocketChat/Rocket.Chat/pull/13184))

- Livechat GDPR compliance ([#12982](https://github.com/RocketChat/Rocket.Chat/pull/12982))

- SAML: Adds possibility to decrypt encrypted assertions ([#12153](https://github.com/RocketChat/Rocket.Chat/pull/12153) by [@gerbsen](https://github.com/gerbsen))

### 🚀 Improvements


- Add "Apps Engine Version" to Administration > Info ([#13169](https://github.com/RocketChat/Rocket.Chat/pull/13169))

- Adds history log for all Importers and improves HipChat import performance ([#13083](https://github.com/RocketChat/Rocket.Chat/pull/13083) by [@Hudell](https://github.com/Hudell))

- Adds the "showConnecting" property to Livechat Config payload ([#13158](https://github.com/RocketChat/Rocket.Chat/pull/13158))

- Change the way the app detail screen shows support link when it's an email ([#13129](https://github.com/RocketChat/Rocket.Chat/pull/13129))

- Dutch translations ([#12294](https://github.com/RocketChat/Rocket.Chat/pull/12294) by [@Jeroeny](https://github.com/Jeroeny))

- Inject metrics on callbacks ([#13266](https://github.com/RocketChat/Rocket.Chat/pull/13266))

- New Livechat statistics added to statistics collector ([#13168](https://github.com/RocketChat/Rocket.Chat/pull/13168))

- Persian translations ([#13114](https://github.com/RocketChat/Rocket.Chat/pull/13114) by [@behnejad](https://github.com/behnejad))

- Process alerts from update checking ([#13194](https://github.com/RocketChat/Rocket.Chat/pull/13194))

- Return room type field on Livechat findRoom method ([#13078](https://github.com/RocketChat/Rocket.Chat/pull/13078))

- Return visitorEmails field on Livechat findGuest method ([#13097](https://github.com/RocketChat/Rocket.Chat/pull/13097) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 🐛 Bug fixes


- #11692 - Suppress error when drop collection in migration to suit to … ([#13091](https://github.com/RocketChat/Rocket.Chat/pull/13091) by [@Xuhao](https://github.com/Xuhao))

- Avatars with transparency were being converted to black ([#13181](https://github.com/RocketChat/Rocket.Chat/pull/13181))

- Change input type of e2e to password ([#13077](https://github.com/RocketChat/Rocket.Chat/pull/13077) by [@supra08](https://github.com/supra08))

- Change webdav creation, due to changes in the npm lib after last update ([#13170](https://github.com/RocketChat/Rocket.Chat/pull/13170) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Emoticons not displayed in room topic ([#12858](https://github.com/RocketChat/Rocket.Chat/pull/12858) by [@alexbartsch](https://github.com/alexbartsch))

- Invite command was not accpeting @ in username ([#12927](https://github.com/RocketChat/Rocket.Chat/pull/12927) by [@piotrkochan](https://github.com/piotrkochan))

- LDAP login of new users overwriting `fname` from all subscriptions ([#13203](https://github.com/RocketChat/Rocket.Chat/pull/13203))

- Notifications for mentions not working on large rooms and don't emit desktop notifications for offline users ([#13067](https://github.com/RocketChat/Rocket.Chat/pull/13067))

- Remove ES6 code from Livechat widget script ([#13105](https://github.com/RocketChat/Rocket.Chat/pull/13105))

- Remove unused code for Cordova ([#13188](https://github.com/RocketChat/Rocket.Chat/pull/13188))

- REST api client base url on subdir ([#13180](https://github.com/RocketChat/Rocket.Chat/pull/13180))

- REST API endpoint `users.getPersonalAccessTokens` error when user has no access tokens ([#13150](https://github.com/RocketChat/Rocket.Chat/pull/13150) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Snap upgrade add post-refresh hook ([#13153](https://github.com/RocketChat/Rocket.Chat/pull/13153))

- Update Message: Does not show edited when message was not edited. ([#13053](https://github.com/RocketChat/Rocket.Chat/pull/13053) by [@Kailash0311](https://github.com/Kailash0311))

- User status on header and user info are not translated ([#13096](https://github.com/RocketChat/Rocket.Chat/pull/13096))

<details>
<summary>🔍 Minor changes</summary>


-  Remove dependency of RocketChat namespace and push-notifications ([#13137](https://github.com/RocketChat/Rocket.Chat/pull/13137) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Change apps engine persistence bridge method to updateByAssociations ([#13239](https://github.com/RocketChat/Rocket.Chat/pull/13239))

- Convert rocketchat-file-upload to main module structure ([#13094](https://github.com/RocketChat/Rocket.Chat/pull/13094) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ui-master to main module structure ([#13107](https://github.com/RocketChat/Rocket.Chat/pull/13107) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ui-sidenav to main module structure ([#13098](https://github.com/RocketChat/Rocket.Chat/pull/13098) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-webrtc to main module structure ([#13117](https://github.com/RocketChat/Rocket.Chat/pull/13117) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat:ui to main module structure ([#13132](https://github.com/RocketChat/Rocket.Chat/pull/13132) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Globals/main module custom oauth ([#13037](https://github.com/RocketChat/Rocket.Chat/pull/13037) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Globals/move rocketchat notifications ([#13035](https://github.com/RocketChat/Rocket.Chat/pull/13035) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Language: Edit typo "Обновлить" ([#13177](https://github.com/RocketChat/Rocket.Chat/pull/13177) by [@zpavlig](https://github.com/zpavlig))

- LingoHub based on develop ([#13201](https://github.com/RocketChat/Rocket.Chat/pull/13201))

- Merge master into develop & Set version to 0.74.0-develop ([#13050](https://github.com/RocketChat/Rocket.Chat/pull/13050) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@ohmonster](https://github.com/ohmonster) & [@piotrkochan](https://github.com/piotrkochan))

- Move rocketchat models ([#13027](https://github.com/RocketChat/Rocket.Chat/pull/13027) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move rocketchat promises ([#13039](https://github.com/RocketChat/Rocket.Chat/pull/13039) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move rocketchat settings to specific package ([#13026](https://github.com/RocketChat/Rocket.Chat/pull/13026) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move some function to utils ([#13122](https://github.com/RocketChat/Rocket.Chat/pull/13122) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move some ui function to ui-utils ([#13123](https://github.com/RocketChat/Rocket.Chat/pull/13123) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move UI Collections to rocketchat:models ([#13064](https://github.com/RocketChat/Rocket.Chat/pull/13064) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move/create rocketchat callbacks ([#13034](https://github.com/RocketChat/Rocket.Chat/pull/13034) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move/create rocketchat metrics ([#13032](https://github.com/RocketChat/Rocket.Chat/pull/13032) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Fix audio message upload ([#13224](https://github.com/RocketChat/Rocket.Chat/pull/13224))

- Regression: Fix emoji search ([#13207](https://github.com/RocketChat/Rocket.Chat/pull/13207))

- Regression: Fix export AudioRecorder ([#13192](https://github.com/RocketChat/Rocket.Chat/pull/13192) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: fix rooms model's collection name ([#13146](https://github.com/RocketChat/Rocket.Chat/pull/13146))

- Regression: fix upload permissions ([#13157](https://github.com/RocketChat/Rocket.Chat/pull/13157))

- Release 0.74.0 ([#13270](https://github.com/RocketChat/Rocket.Chat/pull/13270) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@Xuhao](https://github.com/Xuhao) & [@supra08](https://github.com/supra08))

- Remove dependency between lib and authz ([#13066](https://github.com/RocketChat/Rocket.Chat/pull/13066) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency between RocketChat namespace and migrations ([#13133](https://github.com/RocketChat/Rocket.Chat/pull/13133) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RocketChat namespace and custom-sounds ([#13136](https://github.com/RocketChat/Rocket.Chat/pull/13136) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RocketChat namespace and logger ([#13135](https://github.com/RocketChat/Rocket.Chat/pull/13135) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove dependency of RocketChat namespace inside rocketchat:ui ([#13131](https://github.com/RocketChat/Rocket.Chat/pull/13131) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove directly dependency between lib and e2e ([#13115](https://github.com/RocketChat/Rocket.Chat/pull/13115) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove directly dependency between rocketchat:lib and emoji ([#13118](https://github.com/RocketChat/Rocket.Chat/pull/13118) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove incorrect pt-BR translation ([#13074](https://github.com/RocketChat/Rocket.Chat/pull/13074))

- Rocketchat mailer ([#13036](https://github.com/RocketChat/Rocket.Chat/pull/13036) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Test only MongoDB with oplog versions 3.2 and 4.0 for PRs ([#13119](https://github.com/RocketChat/Rocket.Chat/pull/13119))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@Jeroeny](https://github.com/Jeroeny)
- [@Kailash0311](https://github.com/Kailash0311)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Xuhao](https://github.com/Xuhao)
- [@alexbartsch](https://github.com/alexbartsch)
- [@behnejad](https://github.com/behnejad)
- [@gerbsen](https://github.com/gerbsen)
- [@ohmonster](https://github.com/ohmonster)
- [@piotrkochan](https://github.com/piotrkochan)
- [@supra08](https://github.com/supra08)
- [@zpavlig](https://github.com/zpavlig)

### 👩‍💻👨‍💻 Core Team 🤓

- [@LuluGO](https://github.com/LuluGO)
- [@d-gubert](https://github.com/d-gubert)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.73.2
`2019-01-07  ·  1 🎉  ·  1 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🎉 New features


- Cloud Integration ([#13013](https://github.com/RocketChat/Rocket.Chat/pull/13013))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.73.2 ([#13086](https://github.com/RocketChat/Rocket.Chat/pull/13086))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@graywolf336](https://github.com/graywolf336)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.73.1
`2018-12-28  ·  1 🐛  ·  3 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`
- MongoDB: `3.2, 3.4, 3.6, 4.0`

### 🐛 Bug fixes


- Default importer path ([#13045](https://github.com/RocketChat/Rocket.Chat/pull/13045))

<details>
<summary>🔍 Minor changes</summary>


- Execute tests with versions 3.2, 3.4, 3.6 and 4.0 of MongoDB ([#13049](https://github.com/RocketChat/Rocket.Chat/pull/13049))

- Regression: Get room's members list not working on MongoDB 3.2 ([#13051](https://github.com/RocketChat/Rocket.Chat/pull/13051))

- Release 0.73.1 ([#13052](https://github.com/RocketChat/Rocket.Chat/pull/13052))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.73.0
`2018-12-28  ·  1 ️️️⚠️  ·  16 🎉  ·  25 🚀  ·  60 🐛  ·  165 🔍  ·  39 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.4`
- NPM: `6.4.1`

### ⚠️ BREAKING CHANGES


- Update to Meteor to 1.8 ([#12468](https://github.com/RocketChat/Rocket.Chat/pull/12468))

### 🎉 New features


- /api/v1/spotlight: return joinCodeRequired field for rooms ([#12651](https://github.com/RocketChat/Rocket.Chat/pull/12651) by [@cardoso](https://github.com/cardoso))

- Add permission to enable personal access token to specific roles ([#12309](https://github.com/RocketChat/Rocket.Chat/pull/12309) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add query parameter support to emoji-custom endpoint ([#12754](https://github.com/RocketChat/Rocket.Chat/pull/12754) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added a link to contributing.md ([#12856](https://github.com/RocketChat/Rocket.Chat/pull/12856) by [@sanketsingh24](https://github.com/sanketsingh24))

- Added chat.getDeletedMessages since specific date ([#13010](https://github.com/RocketChat/Rocket.Chat/pull/13010) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Config hooks for snap ([#12351](https://github.com/RocketChat/Rocket.Chat/pull/12351))

- Create new permission.listAll endpoint to be able to use updatedSince parameter ([#12748](https://github.com/RocketChat/Rocket.Chat/pull/12748) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Download button for each file in fileslist ([#12874](https://github.com/RocketChat/Rocket.Chat/pull/12874) by [@alexbartsch](https://github.com/alexbartsch))

- Include message type & id in push notification payload ([#12771](https://github.com/RocketChat/Rocket.Chat/pull/12771) by [@cardoso](https://github.com/cardoso))

- Livechat registration form message ([#12597](https://github.com/RocketChat/Rocket.Chat/pull/12597))

- Make Livechat's widget draggable ([#12378](https://github.com/RocketChat/Rocket.Chat/pull/12378))

- Mandatory 2fa for role ([#9748](https://github.com/RocketChat/Rocket.Chat/pull/9748) by [@Hudell](https://github.com/Hudell) & [@karlprieb](https://github.com/karlprieb))

- New API Endpoints for the new version of JS SDK ([#12623](https://github.com/RocketChat/Rocket.Chat/pull/12623) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Option to reset e2e key ([#12483](https://github.com/RocketChat/Rocket.Chat/pull/12483) by [@Hudell](https://github.com/Hudell))

- Setting to configure robots.txt content ([#12547](https://github.com/RocketChat/Rocket.Chat/pull/12547) by [@Hudell](https://github.com/Hudell))

- Syncloud deploy option ([#12867](https://github.com/RocketChat/Rocket.Chat/pull/12867) by [@cyberb](https://github.com/cyberb))

### 🚀 Improvements


- Accept Slash Commands via Action Buttons when `msg_in_chat_window: true` ([#13009](https://github.com/RocketChat/Rocket.Chat/pull/13009))

- Add CTRL modifier for keyboard shortcut ([#12525](https://github.com/RocketChat/Rocket.Chat/pull/12525) by [@nicolasbock](https://github.com/nicolasbock))

- Add missing translation keys. ([#12722](https://github.com/RocketChat/Rocket.Chat/pull/12722) by [@ura14h](https://github.com/ura14h))

- Add more methods to deal with rooms via Rocket.Chat.Apps ([#12680](https://github.com/RocketChat/Rocket.Chat/pull/12680))

- Add new acceptable header for Livechat REST requests ([#12561](https://github.com/RocketChat/Rocket.Chat/pull/12561))

- Add rooms property in user object, if the user has the permission, with rooms roles ([#12105](https://github.com/RocketChat/Rocket.Chat/pull/12105) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Adding debugging instructions in README ([#12989](https://github.com/RocketChat/Rocket.Chat/pull/12989) by [@hypery2k](https://github.com/hypery2k))

- Allow apps to update persistence by association ([#12714](https://github.com/RocketChat/Rocket.Chat/pull/12714))

- Allow transfer Livechats to online agents only ([#13008](https://github.com/RocketChat/Rocket.Chat/pull/13008))

- Atlassian Crowd settings and option to sync user data ([#12616](https://github.com/RocketChat/Rocket.Chat/pull/12616))

- Better query for finding subscriptions that need a new E2E Key ([#12692](https://github.com/RocketChat/Rocket.Chat/pull/12692) by [@Hudell](https://github.com/Hudell))

- border-radius to use --border-radius ([#12675](https://github.com/RocketChat/Rocket.Chat/pull/12675))

- CircleCI to use MongoDB 4.0 for testing ([#12618](https://github.com/RocketChat/Rocket.Chat/pull/12618))

- Do not emit settings if there are no changes ([#12904](https://github.com/RocketChat/Rocket.Chat/pull/12904))

- Emoji search on messageBox behaving like emojiPicker's search (#9607) ([#12452](https://github.com/RocketChat/Rocket.Chat/pull/12452) by [@vinade](https://github.com/vinade))

- German translations ([#12471](https://github.com/RocketChat/Rocket.Chat/pull/12471) by [@mrsimpson](https://github.com/mrsimpson))

- Hipchat Enterprise Importer ([#12985](https://github.com/RocketChat/Rocket.Chat/pull/12985) by [@Hudell](https://github.com/Hudell))

- Ignore non-existent Livechat custom fields on Livechat API ([#12522](https://github.com/RocketChat/Rocket.Chat/pull/12522))

- Improve unreads and unreadsFrom response, prevent it to be equal null ([#12563](https://github.com/RocketChat/Rocket.Chat/pull/12563) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Japanese translations ([#12382](https://github.com/RocketChat/Rocket.Chat/pull/12382) by [@ura14h](https://github.com/ura14h))

- Limit the number of typing users shown (#8722) ([#12400](https://github.com/RocketChat/Rocket.Chat/pull/12400) by [@vinade](https://github.com/vinade))

- Returning an open room object in the Livechat config endpoint ([#12865](https://github.com/RocketChat/Rocket.Chat/pull/12865))

- Update the 'keyboard shortcuts' documentation ([#12564](https://github.com/RocketChat/Rocket.Chat/pull/12564) by [@nicolasbock](https://github.com/nicolasbock))

- Use MongoBD aggregation to get users from a room ([#12566](https://github.com/RocketChat/Rocket.Chat/pull/12566))

- Username suggestion logic ([#12779](https://github.com/RocketChat/Rocket.Chat/pull/12779))

### 🐛 Bug fixes


- `Disabled` word translation to Chinese ([#12260](https://github.com/RocketChat/Rocket.Chat/pull/12260) by [@AndreamApp](https://github.com/AndreamApp))

- `Disabled` word translation to Spanish ([#12406](https://github.com/RocketChat/Rocket.Chat/pull/12406) by [@Ismaw34](https://github.com/Ismaw34))

- Admin styles ([#12614](https://github.com/RocketChat/Rocket.Chat/pull/12614))

- Admin styles ([#12602](https://github.com/RocketChat/Rocket.Chat/pull/12602))

- Autotranslate icon on message action menu ([#12585](https://github.com/RocketChat/Rocket.Chat/pull/12585))

- Avoiding links with highlighted words ([#12123](https://github.com/RocketChat/Rocket.Chat/pull/12123) by [@rssilva](https://github.com/rssilva))

- cannot reset password ([#12903](https://github.com/RocketChat/Rocket.Chat/pull/12903) by [@Hudell](https://github.com/Hudell))

- CAS Login not working with renamed users ([#12860](https://github.com/RocketChat/Rocket.Chat/pull/12860) by [@Hudell](https://github.com/Hudell))

- Change field checks in RocketChat.saveStreamingOptions ([#12973](https://github.com/RocketChat/Rocket.Chat/pull/12973))

- Change JSON to EJSON.parse query to support type Date ([#12706](https://github.com/RocketChat/Rocket.Chat/pull/12706) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Change registration message when user need to confirm email ([#9336](https://github.com/RocketChat/Rocket.Chat/pull/9336) by [@karlprieb](https://github.com/karlprieb))

- Check for object falsehood before referencing properties in saveRoomSettings ([#12972](https://github.com/RocketChat/Rocket.Chat/pull/12972))

- Condition to not render PDF preview ([#12632](https://github.com/RocketChat/Rocket.Chat/pull/12632))

- Correct roomName value in Mail Messages (#12363) ([#12453](https://github.com/RocketChat/Rocket.Chat/pull/12453) by [@vinade](https://github.com/vinade))

- Crowd sync was being stopped when a user was not found ([#12930](https://github.com/RocketChat/Rocket.Chat/pull/12930) by [@piotrkochan](https://github.com/piotrkochan))

- Data Import not working ([#12866](https://github.com/RocketChat/Rocket.Chat/pull/12866) by [@Hudell](https://github.com/Hudell))

- DE translation for idle-time-limit ([#12637](https://github.com/RocketChat/Rocket.Chat/pull/12637) by [@pfuender](https://github.com/pfuender))

- Download files without extension wasn't possible ([#13033](https://github.com/RocketChat/Rocket.Chat/pull/13033))

- E2E`s password reaveal text is always `>%S` when language is zh ([#12795](https://github.com/RocketChat/Rocket.Chat/pull/12795) by [@lvyue](https://github.com/lvyue))

- Email sending with GDPR user data ([#12487](https://github.com/RocketChat/Rocket.Chat/pull/12487))

- Emoji picker is not in viewport on small screens ([#12457](https://github.com/RocketChat/Rocket.Chat/pull/12457) by [@ramrami](https://github.com/ramrami))

- Exception in getSingleMessage ([#12970](https://github.com/RocketChat/Rocket.Chat/pull/12970) by [@tsukiRep](https://github.com/tsukiRep))

- Fix favico error ([#12643](https://github.com/RocketChat/Rocket.Chat/pull/12643) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix set avatar http call, to avoid SSL errors ([#12790](https://github.com/RocketChat/Rocket.Chat/pull/12790) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix users.setPreferences endpoint, set language correctly ([#12734](https://github.com/RocketChat/Rocket.Chat/pull/12734) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix wrong parameter in chat.delete endpoint and add some test cases ([#12408](https://github.com/RocketChat/Rocket.Chat/pull/12408) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fixed Anonymous Registration ([#12633](https://github.com/RocketChat/Rocket.Chat/pull/12633) by [@wreiske](https://github.com/wreiske))

- German translation for for API_EmbedIgnoredHosts label ([#12518](https://github.com/RocketChat/Rocket.Chat/pull/12518) by [@mbrodala](https://github.com/mbrodala))

- Google Cloud Storage storage provider ([#12843](https://github.com/RocketChat/Rocket.Chat/pull/12843))

- Handle all events for enter key in message box ([#12507](https://github.com/RocketChat/Rocket.Chat/pull/12507))

- high cpu usage ~ svg icon ([#12677](https://github.com/RocketChat/Rocket.Chat/pull/12677) by [@ph1p](https://github.com/ph1p))

- Import missed file in rocketchat-authorization ([#12570](https://github.com/RocketChat/Rocket.Chat/pull/12570) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Incorrect parameter name in Livechat stream ([#12851](https://github.com/RocketChat/Rocket.Chat/pull/12851))

- Inherit font family in message user card ([#13004](https://github.com/RocketChat/Rocket.Chat/pull/13004))

- line-height for unread bar buttons (jump to first and mark as read) ([#12900](https://github.com/RocketChat/Rocket.Chat/pull/12900))

- Manage own integrations permissions check ([#12397](https://github.com/RocketChat/Rocket.Chat/pull/12397))

- multiple rooms-changed ([#12940](https://github.com/RocketChat/Rocket.Chat/pull/12940))

- Nested Markdown blocks not parsed properly ([#12998](https://github.com/RocketChat/Rocket.Chat/pull/12998) by [@Hudell](https://github.com/Hudell))

- Padding for message box in embedded layout ([#12556](https://github.com/RocketChat/Rocket.Chat/pull/12556))

- PDF view loading indicator ([#12882](https://github.com/RocketChat/Rocket.Chat/pull/12882))

- Pin and unpin message were not checking permissions ([#12739](https://github.com/RocketChat/Rocket.Chat/pull/12739) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Prevent subscriptions and calls to rooms events that the user is not participating ([#12558](https://github.com/RocketChat/Rocket.Chat/pull/12558) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Provide better Dutch translations 🇳🇱 ([#12792](https://github.com/RocketChat/Rocket.Chat/pull/12792) by [@mathysie](https://github.com/mathysie))

- Readable validation on the apps engine environment bridge ([#12994](https://github.com/RocketChat/Rocket.Chat/pull/12994))

- Remove sharp's deprecation warnings on image upload ([#12980](https://github.com/RocketChat/Rocket.Chat/pull/12980))

- Reset password email ([#12898](https://github.com/RocketChat/Rocket.Chat/pull/12898))

- Revert Jitsi external API to an asset ([#12954](https://github.com/RocketChat/Rocket.Chat/pull/12954))

- Some deprecation issues for media recording ([#12948](https://github.com/RocketChat/Rocket.Chat/pull/12948))

- Some icons were missing ([#12913](https://github.com/RocketChat/Rocket.Chat/pull/12913))

- Spotlight being called while in background ([#12957](https://github.com/RocketChat/Rocket.Chat/pull/12957))

- Spotlight method being called multiple times ([#12536](https://github.com/RocketChat/Rocket.Chat/pull/12536))

- Stop click event propagation on mention link or user card ([#12983](https://github.com/RocketChat/Rocket.Chat/pull/12983))

- Stream of my_message wasn't sending the room information ([#12914](https://github.com/RocketChat/Rocket.Chat/pull/12914))

- stream room-changed ([#12411](https://github.com/RocketChat/Rocket.Chat/pull/12411))

- Update caret position on insert a new line in message box ([#12713](https://github.com/RocketChat/Rocket.Chat/pull/12713))

- Use web.browser.legacy bundle for Livechat script ([#12975](https://github.com/RocketChat/Rocket.Chat/pull/12975))

- User data download fails when a room has been deleted. ([#12829](https://github.com/RocketChat/Rocket.Chat/pull/12829) by [@Hudell](https://github.com/Hudell))

- Version check update notification ([#12905](https://github.com/RocketChat/Rocket.Chat/pull/12905))

- Webdav integration account settings were being shown even when Webdav was disabled ([#12569](https://github.com/RocketChat/Rocket.Chat/pull/12569) by [@karakayasemi](https://github.com/karakayasemi))

- Wrong test case for `users.setAvatar` endpoint ([#12539](https://github.com/RocketChat/Rocket.Chat/pull/12539) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

<details>
<summary>🔍 Minor changes</summary>


-  Convert rocketchat-channel-settings to main module structure ([#12594](https://github.com/RocketChat/Rocket.Chat/pull/12594) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-emoji-custom to main module structure ([#12604](https://github.com/RocketChat/Rocket.Chat/pull/12604) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-importer-slack to main module structure ([#12666](https://github.com/RocketChat/Rocket.Chat/pull/12666) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-livestream to main module structure ([#12679](https://github.com/RocketChat/Rocket.Chat/pull/12679) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-mentions-flextab to main module structure ([#12757](https://github.com/RocketChat/Rocket.Chat/pull/12757) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-reactions to main module structure ([#12888](https://github.com/RocketChat/Rocket.Chat/pull/12888) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-ui-account to main module structure ([#12842](https://github.com/RocketChat/Rocket.Chat/pull/12842) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-ui-flextab to main module structure ([#12859](https://github.com/RocketChat/Rocket.Chat/pull/12859) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- [DOCS] Remove Cordova links, include F-Droid download button and few other adjustments ([#12583](https://github.com/RocketChat/Rocket.Chat/pull/12583) by [@rafaelks](https://github.com/rafaelks))

- Add check to make sure releases was updated ([#12791](https://github.com/RocketChat/Rocket.Chat/pull/12791))

- Added "npm install" to quick start for developers ([#12374](https://github.com/RocketChat/Rocket.Chat/pull/12374) by [@wreiske](https://github.com/wreiske))

- Added imports for global variables in rocketchat-google-natural-language package ([#12647](https://github.com/RocketChat/Rocket.Chat/pull/12647) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Bump Apps Engine to 1.3.0 ([#12705](https://github.com/RocketChat/Rocket.Chat/pull/12705))

- Change `chat.getDeletedMessages` to get messages after informed date and return only message's _id ([#13021](https://github.com/RocketChat/Rocket.Chat/pull/13021) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- changed maxRoomsOpen ([#12949](https://github.com/RocketChat/Rocket.Chat/pull/12949))

- Convert chatpal search package to modular structure ([#12485](https://github.com/RocketChat/Rocket.Chat/pull/12485) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert emoji-emojione to main module structure ([#12605](https://github.com/RocketChat/Rocket.Chat/pull/12605) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert meteor-accounts-saml to main module structure ([#12486](https://github.com/RocketChat/Rocket.Chat/pull/12486) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert meteor-autocomplete package to main module structure ([#12491](https://github.com/RocketChat/Rocket.Chat/pull/12491) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert meteor-timesync to main module structure ([#12495](https://github.com/RocketChat/Rocket.Chat/pull/12495) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-2fa to main module structure ([#12501](https://github.com/RocketChat/Rocket.Chat/pull/12501) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-action-links to main module structure ([#12503](https://github.com/RocketChat/Rocket.Chat/pull/12503) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-analytics to main module structure ([#12506](https://github.com/RocketChat/Rocket.Chat/pull/12506) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-api to main module structure ([#12510](https://github.com/RocketChat/Rocket.Chat/pull/12510) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-assets to main module structure ([#12521](https://github.com/RocketChat/Rocket.Chat/pull/12521) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-authorization to main module structure ([#12523](https://github.com/RocketChat/Rocket.Chat/pull/12523) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-autolinker to main module structure ([#12529](https://github.com/RocketChat/Rocket.Chat/pull/12529) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-autotranslate to main module structure ([#12530](https://github.com/RocketChat/Rocket.Chat/pull/12530) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-bot-helpers to main module structure ([#12531](https://github.com/RocketChat/Rocket.Chat/pull/12531) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-cas to main module structure ([#12532](https://github.com/RocketChat/Rocket.Chat/pull/12532) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-channel-settings-mail-messages to main module structure ([#12537](https://github.com/RocketChat/Rocket.Chat/pull/12537) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-colors to main module structure ([#12538](https://github.com/RocketChat/Rocket.Chat/pull/12538) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-cors to main module structure ([#12595](https://github.com/RocketChat/Rocket.Chat/pull/12595) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-crowd to main module structure ([#12596](https://github.com/RocketChat/Rocket.Chat/pull/12596) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-custom-sounds to main module structure ([#12599](https://github.com/RocketChat/Rocket.Chat/pull/12599) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-dolphin to main module structure ([#12600](https://github.com/RocketChat/Rocket.Chat/pull/12600) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-drupal to main module structure ([#12601](https://github.com/RocketChat/Rocket.Chat/pull/12601) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-emoji to main module structure ([#12603](https://github.com/RocketChat/Rocket.Chat/pull/12603) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-error-handler to main module structure ([#12606](https://github.com/RocketChat/Rocket.Chat/pull/12606) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-favico to main module structure ([#12607](https://github.com/RocketChat/Rocket.Chat/pull/12607) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-file to main module structure ([#12644](https://github.com/RocketChat/Rocket.Chat/pull/12644) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-github-enterprise to main module structure ([#12642](https://github.com/RocketChat/Rocket.Chat/pull/12642) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-gitlab to main module structure ([#12646](https://github.com/RocketChat/Rocket.Chat/pull/12646) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-google-vision to main module structure ([#12649](https://github.com/RocketChat/Rocket.Chat/pull/12649) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-grant to main module structure ([#12657](https://github.com/RocketChat/Rocket.Chat/pull/12657) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-graphql to main module structure ([#12658](https://github.com/RocketChat/Rocket.Chat/pull/12658) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-highlight-words to main module structure ([#12659](https://github.com/RocketChat/Rocket.Chat/pull/12659) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-iframe-login to main module structure ([#12661](https://github.com/RocketChat/Rocket.Chat/pull/12661) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer to main module structure ([#12662](https://github.com/RocketChat/Rocket.Chat/pull/12662) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer-csv to main module structure ([#12663](https://github.com/RocketChat/Rocket.Chat/pull/12663) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer-hipchat to main module structure ([#12664](https://github.com/RocketChat/Rocket.Chat/pull/12664) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer-hipchat-enterprise to main module structure ([#12665](https://github.com/RocketChat/Rocket.Chat/pull/12665) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer-slack-users to main module structure ([#12669](https://github.com/RocketChat/Rocket.Chat/pull/12669) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-integrations to main module structure ([#12670](https://github.com/RocketChat/Rocket.Chat/pull/12670) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-internal-hubot to main module structure ([#12671](https://github.com/RocketChat/Rocket.Chat/pull/12671) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-irc to main module structure ([#12672](https://github.com/RocketChat/Rocket.Chat/pull/12672) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-issuelinks to main module structure ([#12674](https://github.com/RocketChat/Rocket.Chat/pull/12674) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-katex to main module structure ([#12895](https://github.com/RocketChat/Rocket.Chat/pull/12895) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ldap to main module structure ([#12678](https://github.com/RocketChat/Rocket.Chat/pull/12678) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-livechat to main module structure ([#12942](https://github.com/RocketChat/Rocket.Chat/pull/12942) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-logger to main module structure and remove Logger from eslintrc ([#12995](https://github.com/RocketChat/Rocket.Chat/pull/12995) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-mail-messages to main module structure ([#12682](https://github.com/RocketChat/Rocket.Chat/pull/12682) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-mapview to main module structure ([#12701](https://github.com/RocketChat/Rocket.Chat/pull/12701) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-markdown to main module structure ([#12755](https://github.com/RocketChat/Rocket.Chat/pull/12755) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-mentions to main module structure ([#12756](https://github.com/RocketChat/Rocket.Chat/pull/12756) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-message-action to main module structure ([#12759](https://github.com/RocketChat/Rocket.Chat/pull/12759) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-message-attachments to main module structure ([#12760](https://github.com/RocketChat/Rocket.Chat/pull/12760) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-message-mark-as-unread to main module structure ([#12766](https://github.com/RocketChat/Rocket.Chat/pull/12766) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-message-pin to main module structure ([#12767](https://github.com/RocketChat/Rocket.Chat/pull/12767) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-message-snippet to main module structure ([#12768](https://github.com/RocketChat/Rocket.Chat/pull/12768) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-message-star to main module structure ([#12770](https://github.com/RocketChat/Rocket.Chat/pull/12770) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-migrations to main-module structure ([#12772](https://github.com/RocketChat/Rocket.Chat/pull/12772) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-oauth2-server-config to main module structure ([#12773](https://github.com/RocketChat/Rocket.Chat/pull/12773) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-oembed to main module structure ([#12775](https://github.com/RocketChat/Rocket.Chat/pull/12775) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-otr to main module structure ([#12777](https://github.com/RocketChat/Rocket.Chat/pull/12777) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-push-notifications to main module structure ([#12778](https://github.com/RocketChat/Rocket.Chat/pull/12778) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-retention-policy to main module structure ([#12797](https://github.com/RocketChat/Rocket.Chat/pull/12797) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-sandstorm to main module structure ([#12799](https://github.com/RocketChat/Rocket.Chat/pull/12799) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-search to main module structure ([#12801](https://github.com/RocketChat/Rocket.Chat/pull/12801) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-setup-wizard to main module structure ([#12806](https://github.com/RocketChat/Rocket.Chat/pull/12806) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slackbridge to main module structure ([#12807](https://github.com/RocketChat/Rocket.Chat/pull/12807) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcomands-archiveroom to main module structure ([#12810](https://github.com/RocketChat/Rocket.Chat/pull/12810) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-asciiarts to main module structure ([#12808](https://github.com/RocketChat/Rocket.Chat/pull/12808) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-create to main module structure ([#12811](https://github.com/RocketChat/Rocket.Chat/pull/12811) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-help to main module structure ([#12812](https://github.com/RocketChat/Rocket.Chat/pull/12812) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-hide to main module structure ([#12813](https://github.com/RocketChat/Rocket.Chat/pull/12813) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-invite to main module structure ([#12814](https://github.com/RocketChat/Rocket.Chat/pull/12814) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-inviteall to main module structure ([#12815](https://github.com/RocketChat/Rocket.Chat/pull/12815) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-join to main module structure ([#12816](https://github.com/RocketChat/Rocket.Chat/pull/12816) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-kick to main module structure ([#12817](https://github.com/RocketChat/Rocket.Chat/pull/12817) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-leave to main module structure ([#12821](https://github.com/RocketChat/Rocket.Chat/pull/12821) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-me to main module structure ([#12822](https://github.com/RocketChat/Rocket.Chat/pull/12822) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-msg to main module structure ([#12823](https://github.com/RocketChat/Rocket.Chat/pull/12823) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-mute to main module structure ([#12824](https://github.com/RocketChat/Rocket.Chat/pull/12824) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-open to main module structure ([#12825](https://github.com/RocketChat/Rocket.Chat/pull/12825) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-topic to main module structure ([#12826](https://github.com/RocketChat/Rocket.Chat/pull/12826) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slashcommands-unarchiveroom to main module structure ([#12827](https://github.com/RocketChat/Rocket.Chat/pull/12827) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-slider to main module structure ([#12828](https://github.com/RocketChat/Rocket.Chat/pull/12828) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-smarsh-connector to main module structure ([#12830](https://github.com/RocketChat/Rocket.Chat/pull/12830) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-sms to main module structure ([#12831](https://github.com/RocketChat/Rocket.Chat/pull/12831) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-spotify to main module structure ([#12832](https://github.com/RocketChat/Rocket.Chat/pull/12832) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-statistics to main module structure ([#12833](https://github.com/RocketChat/Rocket.Chat/pull/12833) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-theme to main module structure ([#12896](https://github.com/RocketChat/Rocket.Chat/pull/12896) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-token-login to main module structure ([#12837](https://github.com/RocketChat/Rocket.Chat/pull/12837) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-tokenpass to main module structure ([#12838](https://github.com/RocketChat/Rocket.Chat/pull/12838) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-tooltip to main module structure ([#12839](https://github.com/RocketChat/Rocket.Chat/pull/12839) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ui-admin to main module structure ([#12844](https://github.com/RocketChat/Rocket.Chat/pull/12844) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ui-clean-history to main module structure ([#12846](https://github.com/RocketChat/Rocket.Chat/pull/12846) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ui-login to main module structure ([#12861](https://github.com/RocketChat/Rocket.Chat/pull/12861) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ui-message to main module structure ([#12871](https://github.com/RocketChat/Rocket.Chat/pull/12871) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ui-vrecord to main module structure ([#12875](https://github.com/RocketChat/Rocket.Chat/pull/12875) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-user-data-dowload to main module structure ([#12877](https://github.com/RocketChat/Rocket.Chat/pull/12877) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-version-check to main module structure ([#12879](https://github.com/RocketChat/Rocket.Chat/pull/12879) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-videobridge to main module structure ([#12881](https://github.com/RocketChat/Rocket.Chat/pull/12881) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-webdav to main module structure ([#12886](https://github.com/RocketChat/Rocket.Chat/pull/12886) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-wordpress to main module structure ([#12887](https://github.com/RocketChat/Rocket.Chat/pull/12887) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Dependencies update ([#12624](https://github.com/RocketChat/Rocket.Chat/pull/12624))

- Fix CI deploy job ([#12803](https://github.com/RocketChat/Rocket.Chat/pull/12803))

- Fix crowd error with import of SyncedCron ([#12641](https://github.com/RocketChat/Rocket.Chat/pull/12641))

- Fix CSS import order ([#12524](https://github.com/RocketChat/Rocket.Chat/pull/12524))

- Fix ES translation ([#12509](https://github.com/RocketChat/Rocket.Chat/pull/12509))

- Fix punctuation, spelling, and grammar ([#12451](https://github.com/RocketChat/Rocket.Chat/pull/12451) by [@imronras](https://github.com/imronras))

- Fix some Ukrainian translations ([#12712](https://github.com/RocketChat/Rocket.Chat/pull/12712) by [@zdumitru](https://github.com/zdumitru))

- Fix users.setAvatar endpoint tests and logic ([#12625](https://github.com/RocketChat/Rocket.Chat/pull/12625) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: Add email dependency in package.js ([#12645](https://github.com/RocketChat/Rocket.Chat/pull/12645) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: Developers not being able to debug root files in VSCode ([#12440](https://github.com/RocketChat/Rocket.Chat/pull/12440) by [@mrsimpson](https://github.com/mrsimpson))

- Fix: Exception when registering a user with gravatar ([#12699](https://github.com/RocketChat/Rocket.Chat/pull/12699) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: Fix tests by increasing window size ([#12707](https://github.com/RocketChat/Rocket.Chat/pull/12707) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: snap push from ci ([#12883](https://github.com/RocketChat/Rocket.Chat/pull/12883))

- German translation typo fix for Reacted_with ([#12761](https://github.com/RocketChat/Rocket.Chat/pull/12761) by [@localguru](https://github.com/localguru))

- Improve Importer code quality ([#13020](https://github.com/RocketChat/Rocket.Chat/pull/13020) by [@Hudell](https://github.com/Hudell))

- Improve: Add missing translation keys. ([#12708](https://github.com/RocketChat/Rocket.Chat/pull/12708) by [@ura14h](https://github.com/ura14h))

- LingoHub based on develop ([#13014](https://github.com/RocketChat/Rocket.Chat/pull/13014))

- LingoHub based on develop ([#12684](https://github.com/RocketChat/Rocket.Chat/pull/12684))

- LingoHub based on develop ([#12470](https://github.com/RocketChat/Rocket.Chat/pull/12470))

- Merge master into develop & Set version to 0.72.0-develop ([#12460](https://github.com/RocketChat/Rocket.Chat/pull/12460) by [@Hudell](https://github.com/Hudell))

- Merge master into develop & Set version to 0.73.0-develop ([#12776](https://github.com/RocketChat/Rocket.Chat/pull/12776))

- Move globals of test to a specific eslintrc file ([#12959](https://github.com/RocketChat/Rocket.Chat/pull/12959))

- Move isFirefox and isChrome functions to rocketchat-utils ([#13011](https://github.com/RocketChat/Rocket.Chat/pull/13011) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Move tapi18n t and isRtl functions from ui to utils ([#13005](https://github.com/RocketChat/Rocket.Chat/pull/13005) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Account pages layout ([#12735](https://github.com/RocketChat/Rocket.Chat/pull/12735))

- Regression: Expand Administration sections by toggling section title ([#12736](https://github.com/RocketChat/Rocket.Chat/pull/12736))

- Regression: Fix Safari detection in PDF previewing ([#12737](https://github.com/RocketChat/Rocket.Chat/pull/12737))

- Regression: Inherit font-family for message box ([#12729](https://github.com/RocketChat/Rocket.Chat/pull/12729))

- Regression: List of custom emojis wasn't working ([#13031](https://github.com/RocketChat/Rocket.Chat/pull/13031) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Release 0.72.2 ([#12901](https://github.com/RocketChat/Rocket.Chat/pull/12901))

- Release 0.72.3 ([#12932](https://github.com/RocketChat/Rocket.Chat/pull/12932) by [@Hudell](https://github.com/Hudell) & [@piotrkochan](https://github.com/piotrkochan))

- Removal of EJSON, Accounts, Email, HTTP, Random, ReactiveDict, ReactiveVar, SHA256 and WebApp global variables ([#12377](https://github.com/RocketChat/Rocket.Chat/pull/12377))

- Removal of Match, check, moment, Tracker and Mongo global variables ([#12410](https://github.com/RocketChat/Rocket.Chat/pull/12410) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Removal of Meteor global variable ([#12371](https://github.com/RocketChat/Rocket.Chat/pull/12371))

- Removal of TAPi18n and TAPi18next global variables ([#12467](https://github.com/RocketChat/Rocket.Chat/pull/12467) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Removal of Template, Blaze, BlazeLayout, FlowRouter, DDPRateLimiter, Session, UAParser, Promise, Reload and CryptoJS global variables ([#12433](https://github.com/RocketChat/Rocket.Chat/pull/12433) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove /* globals */ from files wave-1 ([#12984](https://github.com/RocketChat/Rocket.Chat/pull/12984) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove /* globals */ wave 2 ([#12988](https://github.com/RocketChat/Rocket.Chat/pull/12988) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove /* globals */ wave 3 ([#12997](https://github.com/RocketChat/Rocket.Chat/pull/12997) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove /* globals */ wave 4 ([#12999](https://github.com/RocketChat/Rocket.Chat/pull/12999) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove conventional changelog cli, we are using our own cli now (Houston) ([#12798](https://github.com/RocketChat/Rocket.Chat/pull/12798))

- Remove global ServiceConfiguration ([#12960](https://github.com/RocketChat/Rocket.Chat/pull/12960))

- Remove global toastr ([#12961](https://github.com/RocketChat/Rocket.Chat/pull/12961))

- Remove rocketchat-tutum package ([#12840](https://github.com/RocketChat/Rocket.Chat/pull/12840) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove template for feature requests as issues ([#12426](https://github.com/RocketChat/Rocket.Chat/pull/12426))

- Removed RocketChatFile from globals ([#12650](https://github.com/RocketChat/Rocket.Chat/pull/12650) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Revert imports of css, reAdd them to the addFiles function ([#12934](https://github.com/RocketChat/Rocket.Chat/pull/12934) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Update Apps Engine to 1.3.1 ([#12741](https://github.com/RocketChat/Rocket.Chat/pull/12741))

- Update npm dependencies ([#12465](https://github.com/RocketChat/Rocket.Chat/pull/12465))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AndreamApp](https://github.com/AndreamApp)
- [@Hudell](https://github.com/Hudell)
- [@Ismaw34](https://github.com/Ismaw34)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@alexbartsch](https://github.com/alexbartsch)
- [@cardoso](https://github.com/cardoso)
- [@cyberb](https://github.com/cyberb)
- [@hypery2k](https://github.com/hypery2k)
- [@imronras](https://github.com/imronras)
- [@karakayasemi](https://github.com/karakayasemi)
- [@karlprieb](https://github.com/karlprieb)
- [@localguru](https://github.com/localguru)
- [@lvyue](https://github.com/lvyue)
- [@mathysie](https://github.com/mathysie)
- [@mbrodala](https://github.com/mbrodala)
- [@mrsimpson](https://github.com/mrsimpson)
- [@nicolasbock](https://github.com/nicolasbock)
- [@pfuender](https://github.com/pfuender)
- [@ph1p](https://github.com/ph1p)
- [@piotrkochan](https://github.com/piotrkochan)
- [@rafaelks](https://github.com/rafaelks)
- [@ramrami](https://github.com/ramrami)
- [@rssilva](https://github.com/rssilva)
- [@sanketsingh24](https://github.com/sanketsingh24)
- [@tsukiRep](https://github.com/tsukiRep)
- [@ura14h](https://github.com/ura14h)
- [@vinade](https://github.com/vinade)
- [@wreiske](https://github.com/wreiske)
- [@zdumitru](https://github.com/zdumitru)

### 👩‍💻👨‍💻 Core Team 🤓

- [@LuluGO](https://github.com/LuluGO)
- [@d-gubert](https://github.com/d-gubert)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.72.3
`2018-12-12  ·  1 🔍  ·  5 👩‍💻👨‍💻`

<details>
<summary>🔍 Minor changes</summary>


- Release 0.72.3 ([#12932](https://github.com/RocketChat/Rocket.Chat/pull/12932) by [@Hudell](https://github.com/Hudell) & [@piotrkochan](https://github.com/piotrkochan))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@piotrkochan](https://github.com/piotrkochan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@tassoevan](https://github.com/tassoevan)

# 0.72.2
`2018-12-10  ·  3 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### 🐛 Bug fixes


- line-height for unread bar buttons (jump to first and mark as read) ([#12900](https://github.com/RocketChat/Rocket.Chat/pull/12900))

- PDF view loading indicator ([#12882](https://github.com/RocketChat/Rocket.Chat/pull/12882))

- Reset password email ([#12898](https://github.com/RocketChat/Rocket.Chat/pull/12898))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.72.2 ([#12901](https://github.com/RocketChat/Rocket.Chat/pull/12901))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.72.1
`2018-12-05  ·  4 🐛  ·  3 🔍  ·  8 👩‍💻👨‍💻`

### 🐛 Bug fixes


- API users.info returns caller rooms and not requested user ones ([#12727](https://github.com/RocketChat/Rocket.Chat/pull/12727) by [@piotrkochan](https://github.com/piotrkochan))

- Change spread operator to Array.from for Edge browser ([#12818](https://github.com/RocketChat/Rocket.Chat/pull/12818) by [@ohmonster](https://github.com/ohmonster))

- Emoji as avatar ([#12805](https://github.com/RocketChat/Rocket.Chat/pull/12805))

- Missing HipChat Enterprise Importer ([#12847](https://github.com/RocketChat/Rocket.Chat/pull/12847) by [@Hudell](https://github.com/Hudell))

<details>
<summary>🔍 Minor changes</summary>


- Bump Apps-Engine version ([#12848](https://github.com/RocketChat/Rocket.Chat/pull/12848))

- Change file order in rocketchat-cors ([#12804](https://github.com/RocketChat/Rocket.Chat/pull/12804) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Release 0.72.1 ([#12850](https://github.com/RocketChat/Rocket.Chat/pull/12850) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@ohmonster](https://github.com/ohmonster) & [@piotrkochan](https://github.com/piotrkochan))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@ohmonster](https://github.com/ohmonster)
- [@piotrkochan](https://github.com/piotrkochan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@d-gubert](https://github.com/d-gubert)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.72.0
`2018-11-28  ·  2 ️️️⚠️  ·  6 🎉  ·  16 🚀  ·  22 🐛  ·  79 🔍  ·  25 👩‍💻👨‍💻`

### ⚠️ BREAKING CHANGES


- Support for Cordova (Rocket.Chat Legacy app) has reached End-of-life, support has been discontinued

- Update to Meteor to 1.8 ([#12468](https://github.com/RocketChat/Rocket.Chat/pull/12468))

### 🎉 New features


- /api/v1/spotlight: return joinCodeRequired field for rooms ([#12651](https://github.com/RocketChat/Rocket.Chat/pull/12651) by [@cardoso](https://github.com/cardoso))

- Add permission to enable personal access token to specific roles ([#12309](https://github.com/RocketChat/Rocket.Chat/pull/12309) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Make Livechat's widget draggable ([#12378](https://github.com/RocketChat/Rocket.Chat/pull/12378))

- New API Endpoints for the new version of JS SDK ([#12623](https://github.com/RocketChat/Rocket.Chat/pull/12623) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Option to reset e2e key ([#12483](https://github.com/RocketChat/Rocket.Chat/pull/12483) by [@Hudell](https://github.com/Hudell))

- Setting to configure robots.txt content ([#12547](https://github.com/RocketChat/Rocket.Chat/pull/12547) by [@Hudell](https://github.com/Hudell))

### 🚀 Improvements


- Add CTRL modifier for keyboard shortcut ([#12525](https://github.com/RocketChat/Rocket.Chat/pull/12525) by [@nicolasbock](https://github.com/nicolasbock))

- Add more methods to deal with rooms via Rocket.Chat.Apps ([#12680](https://github.com/RocketChat/Rocket.Chat/pull/12680))

- Add new acceptable header for Livechat REST requests ([#12561](https://github.com/RocketChat/Rocket.Chat/pull/12561))

- Add rooms property in user object, if the user has the permission, with rooms roles ([#12105](https://github.com/RocketChat/Rocket.Chat/pull/12105) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Allow apps to update persistence by association ([#12714](https://github.com/RocketChat/Rocket.Chat/pull/12714))

- Atlassian Crowd settings and option to sync user data ([#12616](https://github.com/RocketChat/Rocket.Chat/pull/12616))

- Better query for finding subscriptions that need a new E2E Key ([#12692](https://github.com/RocketChat/Rocket.Chat/pull/12692) by [@Hudell](https://github.com/Hudell))

- border-radius to use --border-radius ([#12675](https://github.com/RocketChat/Rocket.Chat/pull/12675))

- CircleCI to use MongoDB 4.0 for testing ([#12618](https://github.com/RocketChat/Rocket.Chat/pull/12618))

- Emoji search on messageBox behaving like emojiPicker's search (#9607) ([#12452](https://github.com/RocketChat/Rocket.Chat/pull/12452) by [@vinade](https://github.com/vinade))

- German translations ([#12471](https://github.com/RocketChat/Rocket.Chat/pull/12471) by [@mrsimpson](https://github.com/mrsimpson))

- Ignore non-existent Livechat custom fields on Livechat API ([#12522](https://github.com/RocketChat/Rocket.Chat/pull/12522))

- Improve unreads and unreadsFrom response, prevent it to be equal null ([#12563](https://github.com/RocketChat/Rocket.Chat/pull/12563) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Japanese translations ([#12382](https://github.com/RocketChat/Rocket.Chat/pull/12382) by [@ura14h](https://github.com/ura14h))

- Limit the number of typing users shown (#8722) ([#12400](https://github.com/RocketChat/Rocket.Chat/pull/12400) by [@vinade](https://github.com/vinade))

- Update the 'keyboard shortcuts' documentation ([#12564](https://github.com/RocketChat/Rocket.Chat/pull/12564) by [@nicolasbock](https://github.com/nicolasbock))

### 🐛 Bug fixes


- `Disabled` word translation to Chinese ([#12260](https://github.com/RocketChat/Rocket.Chat/pull/12260) by [@AndreamApp](https://github.com/AndreamApp))

- `Disabled` word translation to Spanish ([#12406](https://github.com/RocketChat/Rocket.Chat/pull/12406) by [@Ismaw34](https://github.com/Ismaw34))

- Admin styles ([#12614](https://github.com/RocketChat/Rocket.Chat/pull/12614))

- Admin styles ([#12602](https://github.com/RocketChat/Rocket.Chat/pull/12602))

- Change registration message when user need to confirm email ([#9336](https://github.com/RocketChat/Rocket.Chat/pull/9336) by [@karlprieb](https://github.com/karlprieb))

- Condition to not render PDF preview ([#12632](https://github.com/RocketChat/Rocket.Chat/pull/12632))

- Correct roomName value in Mail Messages (#12363) ([#12453](https://github.com/RocketChat/Rocket.Chat/pull/12453) by [@vinade](https://github.com/vinade))

- DE translation for idle-time-limit ([#12637](https://github.com/RocketChat/Rocket.Chat/pull/12637) by [@pfuender](https://github.com/pfuender))

- Emoji picker is not in viewport on small screens ([#12457](https://github.com/RocketChat/Rocket.Chat/pull/12457) by [@ramrami](https://github.com/ramrami))

- Fix favico error ([#12643](https://github.com/RocketChat/Rocket.Chat/pull/12643) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix wrong parameter in chat.delete endpoint and add some test cases ([#12408](https://github.com/RocketChat/Rocket.Chat/pull/12408) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fixed Anonymous Registration ([#12633](https://github.com/RocketChat/Rocket.Chat/pull/12633) by [@wreiske](https://github.com/wreiske))

- German translation for for API_EmbedIgnoredHosts label ([#12518](https://github.com/RocketChat/Rocket.Chat/pull/12518) by [@mbrodala](https://github.com/mbrodala))

- Handle all events for enter key in message box ([#12507](https://github.com/RocketChat/Rocket.Chat/pull/12507))

- high cpu usage ~ svg icon ([#12677](https://github.com/RocketChat/Rocket.Chat/pull/12677) by [@ph1p](https://github.com/ph1p))

- Import missed file in rocketchat-authorization ([#12570](https://github.com/RocketChat/Rocket.Chat/pull/12570) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Manage own integrations permissions check ([#12397](https://github.com/RocketChat/Rocket.Chat/pull/12397))

- Prevent subscriptions and calls to rooms events that the user is not participating ([#12558](https://github.com/RocketChat/Rocket.Chat/pull/12558) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Spotlight method being called multiple times ([#12536](https://github.com/RocketChat/Rocket.Chat/pull/12536))

- stream room-changed ([#12411](https://github.com/RocketChat/Rocket.Chat/pull/12411))

- Update caret position on insert a new line in message box ([#12713](https://github.com/RocketChat/Rocket.Chat/pull/12713))

- Wrong test case for `users.setAvatar` endpoint ([#12539](https://github.com/RocketChat/Rocket.Chat/pull/12539) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

<details>
<summary>🔍 Minor changes</summary>


-  Convert rocketchat-channel-settings to main module structure ([#12594](https://github.com/RocketChat/Rocket.Chat/pull/12594) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-emoji-custom to main module structure ([#12604](https://github.com/RocketChat/Rocket.Chat/pull/12604) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-importer-slack to main module structure ([#12666](https://github.com/RocketChat/Rocket.Chat/pull/12666) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

-  Convert rocketchat-livestream to main module structure ([#12679](https://github.com/RocketChat/Rocket.Chat/pull/12679) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- [DOCS] Remove Cordova links, include F-Droid download button and few other adjustments ([#12583](https://github.com/RocketChat/Rocket.Chat/pull/12583) by [@rafaelks](https://github.com/rafaelks))

- Added "npm install" to quick start for developers ([#12374](https://github.com/RocketChat/Rocket.Chat/pull/12374) by [@wreiske](https://github.com/wreiske))

- Added imports for global variables in rocketchat-google-natural-language package ([#12647](https://github.com/RocketChat/Rocket.Chat/pull/12647) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Bump Apps Engine to 1.3.0 ([#12705](https://github.com/RocketChat/Rocket.Chat/pull/12705))

- Convert chatpal search package to modular structure ([#12485](https://github.com/RocketChat/Rocket.Chat/pull/12485) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert emoji-emojione to main module structure ([#12605](https://github.com/RocketChat/Rocket.Chat/pull/12605) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert meteor-accounts-saml to main module structure ([#12486](https://github.com/RocketChat/Rocket.Chat/pull/12486) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert meteor-autocomplete package to main module structure ([#12491](https://github.com/RocketChat/Rocket.Chat/pull/12491) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert meteor-timesync to main module structure ([#12495](https://github.com/RocketChat/Rocket.Chat/pull/12495) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-2fa to main module structure ([#12501](https://github.com/RocketChat/Rocket.Chat/pull/12501) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-action-links to main module structure ([#12503](https://github.com/RocketChat/Rocket.Chat/pull/12503) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-analytics to main module structure ([#12506](https://github.com/RocketChat/Rocket.Chat/pull/12506) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-api to main module structure ([#12510](https://github.com/RocketChat/Rocket.Chat/pull/12510) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-assets to main module structure ([#12521](https://github.com/RocketChat/Rocket.Chat/pull/12521) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-authorization to main module structure ([#12523](https://github.com/RocketChat/Rocket.Chat/pull/12523) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-autolinker to main module structure ([#12529](https://github.com/RocketChat/Rocket.Chat/pull/12529) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-autotranslate to main module structure ([#12530](https://github.com/RocketChat/Rocket.Chat/pull/12530) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-bot-helpers to main module structure ([#12531](https://github.com/RocketChat/Rocket.Chat/pull/12531) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-cas to main module structure ([#12532](https://github.com/RocketChat/Rocket.Chat/pull/12532) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-channel-settings-mail-messages to main module structure ([#12537](https://github.com/RocketChat/Rocket.Chat/pull/12537) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-colors to main module structure ([#12538](https://github.com/RocketChat/Rocket.Chat/pull/12538) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-cors to main module structure ([#12595](https://github.com/RocketChat/Rocket.Chat/pull/12595) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-crowd to main module structure ([#12596](https://github.com/RocketChat/Rocket.Chat/pull/12596) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-custom-sounds to main module structure ([#12599](https://github.com/RocketChat/Rocket.Chat/pull/12599) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-dolphin to main module structure ([#12600](https://github.com/RocketChat/Rocket.Chat/pull/12600) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-drupal to main module structure ([#12601](https://github.com/RocketChat/Rocket.Chat/pull/12601) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-emoji to main module structure ([#12603](https://github.com/RocketChat/Rocket.Chat/pull/12603) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-error-handler to main module structure ([#12606](https://github.com/RocketChat/Rocket.Chat/pull/12606) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-favico to main module structure ([#12607](https://github.com/RocketChat/Rocket.Chat/pull/12607) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-file to main module structure ([#12644](https://github.com/RocketChat/Rocket.Chat/pull/12644) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-github-enterprise to main module structure ([#12642](https://github.com/RocketChat/Rocket.Chat/pull/12642) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-gitlab to main module structure ([#12646](https://github.com/RocketChat/Rocket.Chat/pull/12646) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-google-vision to main module structure ([#12649](https://github.com/RocketChat/Rocket.Chat/pull/12649) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-grant to main module structure ([#12657](https://github.com/RocketChat/Rocket.Chat/pull/12657) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-graphql to main module structure ([#12658](https://github.com/RocketChat/Rocket.Chat/pull/12658) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-highlight-words to main module structure ([#12659](https://github.com/RocketChat/Rocket.Chat/pull/12659) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-iframe-login to main module structure ([#12661](https://github.com/RocketChat/Rocket.Chat/pull/12661) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer to main module structure ([#12662](https://github.com/RocketChat/Rocket.Chat/pull/12662) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer-csv to main module structure ([#12663](https://github.com/RocketChat/Rocket.Chat/pull/12663) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer-hipchat to main module structure ([#12664](https://github.com/RocketChat/Rocket.Chat/pull/12664) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer-hipchat-enterprise to main module structure ([#12665](https://github.com/RocketChat/Rocket.Chat/pull/12665) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-importer-slack-users to main module structure ([#12669](https://github.com/RocketChat/Rocket.Chat/pull/12669) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-integrations to main module structure ([#12670](https://github.com/RocketChat/Rocket.Chat/pull/12670) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-internal-hubot to main module structure ([#12671](https://github.com/RocketChat/Rocket.Chat/pull/12671) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-irc to main module structure ([#12672](https://github.com/RocketChat/Rocket.Chat/pull/12672) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-issuelinks to main module structure ([#12674](https://github.com/RocketChat/Rocket.Chat/pull/12674) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-ldap to main module structure ([#12678](https://github.com/RocketChat/Rocket.Chat/pull/12678) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Convert rocketchat-mail-messages to main module structure ([#12682](https://github.com/RocketChat/Rocket.Chat/pull/12682) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix crowd error with import of SyncedCron ([#12641](https://github.com/RocketChat/Rocket.Chat/pull/12641))

- Fix CSS import order ([#12524](https://github.com/RocketChat/Rocket.Chat/pull/12524))

- Fix ES translation ([#12509](https://github.com/RocketChat/Rocket.Chat/pull/12509))

- Fix punctuation, spelling, and grammar ([#12451](https://github.com/RocketChat/Rocket.Chat/pull/12451) by [@imronras](https://github.com/imronras))

- Fix some Ukrainian translations ([#12712](https://github.com/RocketChat/Rocket.Chat/pull/12712) by [@zdumitru](https://github.com/zdumitru))

- Fix users.setAvatar endpoint tests and logic ([#12625](https://github.com/RocketChat/Rocket.Chat/pull/12625) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: Add email dependency in package.js ([#12645](https://github.com/RocketChat/Rocket.Chat/pull/12645) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: Developers not being able to debug root files in VSCode ([#12440](https://github.com/RocketChat/Rocket.Chat/pull/12440) by [@mrsimpson](https://github.com/mrsimpson))

- Fix: Exception when registering a user with gravatar ([#12699](https://github.com/RocketChat/Rocket.Chat/pull/12699) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: Fix tests by increasing window size ([#12707](https://github.com/RocketChat/Rocket.Chat/pull/12707) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Improve: Add missing translation keys. ([#12708](https://github.com/RocketChat/Rocket.Chat/pull/12708) by [@ura14h](https://github.com/ura14h))

- LingoHub based on develop ([#12684](https://github.com/RocketChat/Rocket.Chat/pull/12684))

- LingoHub based on develop ([#12470](https://github.com/RocketChat/Rocket.Chat/pull/12470))

- Merge master into develop & Set version to 0.72.0-develop ([#12460](https://github.com/RocketChat/Rocket.Chat/pull/12460) by [@Hudell](https://github.com/Hudell))

- Regression: Account pages layout ([#12735](https://github.com/RocketChat/Rocket.Chat/pull/12735))

- Regression: Expand Administration sections by toggling section title ([#12736](https://github.com/RocketChat/Rocket.Chat/pull/12736))

- Regression: Fix Safari detection in PDF previewing ([#12737](https://github.com/RocketChat/Rocket.Chat/pull/12737))

- Regression: Inherit font-family for message box ([#12729](https://github.com/RocketChat/Rocket.Chat/pull/12729))

- Removal of EJSON, Accounts, Email, HTTP, Random, ReactiveDict, ReactiveVar, SHA256 and WebApp global variables ([#12377](https://github.com/RocketChat/Rocket.Chat/pull/12377))

- Removal of Match, check, moment, Tracker and Mongo global variables ([#12410](https://github.com/RocketChat/Rocket.Chat/pull/12410) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Removal of Meteor global variable ([#12371](https://github.com/RocketChat/Rocket.Chat/pull/12371))

- Removal of TAPi18n and TAPi18next global variables ([#12467](https://github.com/RocketChat/Rocket.Chat/pull/12467) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Removal of Template, Blaze, BlazeLayout, FlowRouter, DDPRateLimiter, Session, UAParser, Promise, Reload and CryptoJS global variables ([#12433](https://github.com/RocketChat/Rocket.Chat/pull/12433) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Remove template for feature requests as issues ([#12426](https://github.com/RocketChat/Rocket.Chat/pull/12426))

- Removed RocketChatFile from globals ([#12650](https://github.com/RocketChat/Rocket.Chat/pull/12650) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Update Apps Engine to 1.3.1 ([#12741](https://github.com/RocketChat/Rocket.Chat/pull/12741))

- Update npm dependencies ([#12465](https://github.com/RocketChat/Rocket.Chat/pull/12465))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AndreamApp](https://github.com/AndreamApp)
- [@Hudell](https://github.com/Hudell)
- [@Ismaw34](https://github.com/Ismaw34)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@cardoso](https://github.com/cardoso)
- [@imronras](https://github.com/imronras)
- [@karlprieb](https://github.com/karlprieb)
- [@mbrodala](https://github.com/mbrodala)
- [@mrsimpson](https://github.com/mrsimpson)
- [@nicolasbock](https://github.com/nicolasbock)
- [@pfuender](https://github.com/pfuender)
- [@ph1p](https://github.com/ph1p)
- [@rafaelks](https://github.com/rafaelks)
- [@ramrami](https://github.com/ramrami)
- [@ura14h](https://github.com/ura14h)
- [@vinade](https://github.com/vinade)
- [@wreiske](https://github.com/wreiske)
- [@zdumitru](https://github.com/zdumitru)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@ggazzo](https://github.com/ggazzo)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.71.2
`2018-12-10  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Reset password email ([#12898](https://github.com/RocketChat/Rocket.Chat/pull/12898))

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.71.1
`2018-10-31  ·  1 🐛  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Email sending with GDPR user data ([#12487](https://github.com/RocketChat/Rocket.Chat/pull/12487))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.71.1 ([#12499](https://github.com/RocketChat/Rocket.Chat/pull/12499))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.71.0
`2018-10-27  ·  2 ️️️⚠️  ·  5 🎉  ·  5 🚀  ·  23 🐛  ·  9 🔍  ·  20 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### ⚠️ BREAKING CHANGES


- Add expiration to API login tokens and fix duplicate login tokens created by LDAP ([#12186](https://github.com/RocketChat/Rocket.Chat/pull/12186) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Update `lastMessage` rooms property and convert the "starred" property, to the same format ([#12266](https://github.com/RocketChat/Rocket.Chat/pull/12266) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 🎉 New features


- Ability to disable user presence monitor ([#12353](https://github.com/RocketChat/Rocket.Chat/pull/12353))

- Add "help wanted" section to Readme ([#12432](https://github.com/RocketChat/Rocket.Chat/pull/12432) by [@isabellarussell](https://github.com/isabellarussell))

- Add delete channel mutation to GraphQL API ([#11860](https://github.com/RocketChat/Rocket.Chat/pull/11860) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- PDF message attachment preview (client side rendering) ([#10519](https://github.com/RocketChat/Rocket.Chat/pull/10519) by [@kb0304](https://github.com/kb0304))

- sidenav size on large screens ([#12372](https://github.com/RocketChat/Rocket.Chat/pull/12372))

### 🚀 Improvements


- Add missing livechat i18n keys ([#12330](https://github.com/RocketChat/Rocket.Chat/pull/12330) by [@MarcosEllys](https://github.com/MarcosEllys))

- Allow the imports to accept any file type ([#12425](https://github.com/RocketChat/Rocket.Chat/pull/12425))

- Avoid unnecessary calls to Meteor.user() on client ([#11212](https://github.com/RocketChat/Rocket.Chat/pull/11212))

- Livechat room closure endpoints ([#12360](https://github.com/RocketChat/Rocket.Chat/pull/12360))

- Set Livechat department before register guest ([#12161](https://github.com/RocketChat/Rocket.Chat/pull/12161))

### 🐛 Bug fixes


- Add image dimensions to attachment even when no reorientation is required ([#11521](https://github.com/RocketChat/Rocket.Chat/pull/11521))

- Apps not being able to state how the action buttons are aligned ([#12391](https://github.com/RocketChat/Rocket.Chat/pull/12391))

- Attachment actions not being collapsable ([#12436](https://github.com/RocketChat/Rocket.Chat/pull/12436))

- Attachment timestamp from and to Apps system not working ([#12445](https://github.com/RocketChat/Rocket.Chat/pull/12445))

- avatar?_dc=undefined ([#12365](https://github.com/RocketChat/Rocket.Chat/pull/12365))

- Blockstack errors in IE 11 ([#12338](https://github.com/RocketChat/Rocket.Chat/pull/12338))

- Cast env var setting to int based on option type ([#12194](https://github.com/RocketChat/Rocket.Chat/pull/12194) by [@crazy-max](https://github.com/crazy-max))

- Custom OAuth Configuration can't be removed ([#12256](https://github.com/RocketChat/Rocket.Chat/pull/12256) by [@Hudell](https://github.com/Hudell))

- Date range check on livechat analytics ([#12345](https://github.com/RocketChat/Rocket.Chat/pull/12345) by [@teresy](https://github.com/teresy))

- E2E alert shows up when encryption is disabled ([#12272](https://github.com/RocketChat/Rocket.Chat/pull/12272) by [@Hudell](https://github.com/Hudell))

- E2E: Decrypting UTF-8 encoded messages ([#12398](https://github.com/RocketChat/Rocket.Chat/pull/12398) by [@pmmaga](https://github.com/pmmaga))

- Edit room name with uppercase letters ([#12235](https://github.com/RocketChat/Rocket.Chat/pull/12235) by [@nikeee](https://github.com/nikeee))

- email api TAPi18n is undefined ([#12373](https://github.com/RocketChat/Rocket.Chat/pull/12373))

- iframe login token not checked ([#12158](https://github.com/RocketChat/Rocket.Chat/pull/12158) by [@nimetu](https://github.com/nimetu))

- Ignore errors when creating image preview for uploads ([#12424](https://github.com/RocketChat/Rocket.Chat/pull/12424))

- Invalid destructuring on Livechat API endpoint ([#12354](https://github.com/RocketChat/Rocket.Chat/pull/12354))

- Last message not updating after message delete if show deleted status is on ([#12350](https://github.com/RocketChat/Rocket.Chat/pull/12350))

- Links in home layout ([#12355](https://github.com/RocketChat/Rocket.Chat/pull/12355) by [@upiksaleh](https://github.com/upiksaleh))

- Modal confirm on enter ([#12283](https://github.com/RocketChat/Rocket.Chat/pull/12283))

- Remove e2e from users endpoint responses ([#12344](https://github.com/RocketChat/Rocket.Chat/pull/12344) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST `users.setAvatar` endpoint wasn't allowing update the avatar of other users even with correct permissions ([#11431](https://github.com/RocketChat/Rocket.Chat/pull/11431) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Slack importer: image previews not showing ([#11875](https://github.com/RocketChat/Rocket.Chat/pull/11875) by [@Hudell](https://github.com/Hudell) & [@madguy02](https://github.com/madguy02))

- users.register endpoint to not create an user if username already being used ([#12297](https://github.com/RocketChat/Rocket.Chat/pull/12297) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

<details>
<summary>🔍 Minor changes</summary>


- Apps: Room’s usernames was not working ([#12409](https://github.com/RocketChat/Rocket.Chat/pull/12409))

- Fix: Add wizard opt-in fields ([#12298](https://github.com/RocketChat/Rocket.Chat/pull/12298))

- Fix: update check on err.details ([#12346](https://github.com/RocketChat/Rocket.Chat/pull/12346) by [@teresy](https://github.com/teresy))

- Fix: wrong saveUser permission validations ([#12384](https://github.com/RocketChat/Rocket.Chat/pull/12384))

- Improve: Drop database between running tests on CI ([#12358](https://github.com/RocketChat/Rocket.Chat/pull/12358))

- Regression: Change `starred` message property from object to array ([#12405](https://github.com/RocketChat/Rocket.Chat/pull/12405) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: do not render pdf preview on safari <= 12 ([#12375](https://github.com/RocketChat/Rocket.Chat/pull/12375))

- Regression: Fix email headers not being used ([#12392](https://github.com/RocketChat/Rocket.Chat/pull/12392))

- Update Apps Framework to version 1.2.1 ([#12442](https://github.com/RocketChat/Rocket.Chat/pull/12442))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosEllys](https://github.com/MarcosEllys)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@crazy-max](https://github.com/crazy-max)
- [@isabellarussell](https://github.com/isabellarussell)
- [@kb0304](https://github.com/kb0304)
- [@madguy02](https://github.com/madguy02)
- [@nikeee](https://github.com/nikeee)
- [@nimetu](https://github.com/nimetu)
- [@pmmaga](https://github.com/pmmaga)
- [@teresy](https://github.com/teresy)
- [@upiksaleh](https://github.com/upiksaleh)

### 👩‍💻👨‍💻 Core Team 🤓

- [@Sing-Li](https://github.com/Sing-Li)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.70.5
`2018-12-10  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Reset password email ([#12898](https://github.com/RocketChat/Rocket.Chat/pull/12898))

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.70.4
`2018-10-09  ·  1 🐛  ·  2 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Modal confirm on enter ([#12283](https://github.com/RocketChat/Rocket.Chat/pull/12283))

<details>
<summary>🔍 Minor changes</summary>


- Fix: Add wizard opt-in fields ([#12298](https://github.com/RocketChat/Rocket.Chat/pull/12298))

- Release 0.70.4 ([#12299](https://github.com/RocketChat/Rocket.Chat/pull/12299))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.70.3
`2018-10-08  ·  1 🐛  ·  2 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- E2E alert shows up when encryption is disabled ([#12272](https://github.com/RocketChat/Rocket.Chat/pull/12272) by [@Hudell](https://github.com/Hudell))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.70.2 ([#12276](https://github.com/RocketChat/Rocket.Chat/pull/12276) by [@Hudell](https://github.com/Hudell))

- Release 0.70.3 ([#12281](https://github.com/RocketChat/Rocket.Chat/pull/12281))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.70.1
`2018-10-05  ·  8 🐛  ·  5 🔍  ·  11 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- E2E data not cleared on logout ([#12254](https://github.com/RocketChat/Rocket.Chat/pull/12254) by [@Hudell](https://github.com/Hudell))

- E2E password request not closing after entering password ([#12232](https://github.com/RocketChat/Rocket.Chat/pull/12232) by [@Hudell](https://github.com/Hudell))

- Emails' logo and links ([#12241](https://github.com/RocketChat/Rocket.Chat/pull/12241))

- Livechat CRM integration running when disabled  ([#12242](https://github.com/RocketChat/Rocket.Chat/pull/12242))

- Livechat integration with RDStation ([#12257](https://github.com/RocketChat/Rocket.Chat/pull/12257))

- Livechat triggers being registered twice after setting department via API ([#12255](https://github.com/RocketChat/Rocket.Chat/pull/12255) by [@edzluhan](https://github.com/edzluhan))

- Message editing was duplicating reply quotes ([#12263](https://github.com/RocketChat/Rocket.Chat/pull/12263))

- Set default action for Setup Wizard form submit ([#12240](https://github.com/RocketChat/Rocket.Chat/pull/12240))

<details>
<summary>🔍 Minor changes</summary>


- Add reetp to the issues' bot whitelist ([#12227](https://github.com/RocketChat/Rocket.Chat/pull/12227))

- Fix: Remove semver satisfies from Apps details that is already done my marketplace ([#12268](https://github.com/RocketChat/Rocket.Chat/pull/12268))

- Merge master into develop & Set version to 0.71.0-develop ([#12264](https://github.com/RocketChat/Rocket.Chat/pull/12264) by [@cardoso](https://github.com/cardoso) & [@kaiiiiiiiii](https://github.com/kaiiiiiiiii) & [@timkinnane](https://github.com/timkinnane))

- Regression: fix modal submit ([#12233](https://github.com/RocketChat/Rocket.Chat/pull/12233))

- Release 0.70.1 ([#12270](https://github.com/RocketChat/Rocket.Chat/pull/12270) by [@Hudell](https://github.com/Hudell) & [@edzluhan](https://github.com/edzluhan))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@cardoso](https://github.com/cardoso)
- [@edzluhan](https://github.com/edzluhan)
- [@kaiiiiiiiii](https://github.com/kaiiiiiiiii)
- [@timkinnane](https://github.com/timkinnane)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)
- [@theorenck](https://github.com/theorenck)

# 0.70.0
`2018-09-28  ·  2 ️️️⚠️  ·  18 🎉  ·  3 🚀  ·  35 🐛  ·  19 🔍  ·  32 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### ⚠️ BREAKING CHANGES


- **IMPROVE:** New emails design ([#12009](https://github.com/RocketChat/Rocket.Chat/pull/12009))

- Update the default port of the Prometheus exporter ([#11351](https://github.com/RocketChat/Rocket.Chat/pull/11351) by [@thaiphv](https://github.com/thaiphv))

### 🎉 New features


- Add Livechat Analytics permission ([#12184](https://github.com/RocketChat/Rocket.Chat/pull/12184))

- Allow multiple subcommands in MIGRATION_VERSION env variable ([#11184](https://github.com/RocketChat/Rocket.Chat/pull/11184) by [@arch119](https://github.com/arch119))

- Apps are enabled by default now ([#12189](https://github.com/RocketChat/Rocket.Chat/pull/12189))

- Apps: Add handlers for message updates ([#11993](https://github.com/RocketChat/Rocket.Chat/pull/11993) by [@cardoso](https://github.com/cardoso))

- Apps: API provider ([#11938](https://github.com/RocketChat/Rocket.Chat/pull/11938))

- Blockstack as decentralized auth provider ([#12047](https://github.com/RocketChat/Rocket.Chat/pull/12047) by [@timkinnane](https://github.com/timkinnane))

- Customizable default directory view ([#11965](https://github.com/RocketChat/Rocket.Chat/pull/11965) by [@ohmonster](https://github.com/ohmonster))

- Informal German translations ([#9984](https://github.com/RocketChat/Rocket.Chat/pull/9984) by [@mrsimpson](https://github.com/mrsimpson))

- Livechat Analytics and Reports ([#11238](https://github.com/RocketChat/Rocket.Chat/pull/11238) by [@pkgodara](https://github.com/pkgodara))

- Livechat notifications on new incoming inquiries for guest-pool ([#10588](https://github.com/RocketChat/Rocket.Chat/pull/10588) by [@mrsimpson](https://github.com/mrsimpson))

- Livechat REST endpoints ([#11900](https://github.com/RocketChat/Rocket.Chat/pull/11900))

- Livechat trigger option to run only once ([#12068](https://github.com/RocketChat/Rocket.Chat/pull/12068) by [@edzluhan](https://github.com/edzluhan))

- REST endpoint to set groups' announcement ([#11905](https://github.com/RocketChat/Rocket.Chat/pull/11905) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST endpoints to create roles and assign roles to users ([#11855](https://github.com/RocketChat/Rocket.Chat/pull/11855) by [@aferreira44](https://github.com/aferreira44))

- REST endpoints to get moderators from groups and channels ([#11909](https://github.com/RocketChat/Rocket.Chat/pull/11909) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Support for end to end encryption ([#10094](https://github.com/RocketChat/Rocket.Chat/pull/10094) by [@mrinaldhar](https://github.com/mrinaldhar))

- User preference for 24- or 12-hour clock ([#11169](https://github.com/RocketChat/Rocket.Chat/pull/11169) by [@vynmera](https://github.com/vynmera))

- WebDAV Integration (User file provider) ([#11679](https://github.com/RocketChat/Rocket.Chat/pull/11679) by [@karakayasemi](https://github.com/karakayasemi))

### 🚀 Improvements


- BigBlueButton joinViaHtml5 and video icon on sidebar ([#12107](https://github.com/RocketChat/Rocket.Chat/pull/12107))

- Cache livechat get agent trigger call ([#12083](https://github.com/RocketChat/Rocket.Chat/pull/12083))

- Use eslint-config package ([#12044](https://github.com/RocketChat/Rocket.Chat/pull/12044))

### 🐛 Bug fixes


- Adding scroll bar to read receipts modal ([#11919](https://github.com/RocketChat/Rocket.Chat/pull/11919) by [@rssilva](https://github.com/rssilva))

- Allow user with "bulk-register-user" permission to send invitations ([#12112](https://github.com/RocketChat/Rocket.Chat/pull/12112) by [@mrsimpson](https://github.com/mrsimpson))

- app engine verbose log typo ([#12126](https://github.com/RocketChat/Rocket.Chat/pull/12126) by [@williamriancho](https://github.com/williamriancho))

- App updates were not being shown correctly ([#11893](https://github.com/RocketChat/Rocket.Chat/pull/11893))

- Apps being able to see hidden settings ([#12159](https://github.com/RocketChat/Rocket.Chat/pull/12159))

- Apps: Add missing reactions and actions properties to app message object ([#11780](https://github.com/RocketChat/Rocket.Chat/pull/11780))

- Broken slack compatible webhook ([#11742](https://github.com/RocketChat/Rocket.Chat/pull/11742))

- Changing Mentions.userMentionRegex pattern to include <br> tag ([#12043](https://github.com/RocketChat/Rocket.Chat/pull/12043) by [@rssilva](https://github.com/rssilva))

- Close popover on shortcuts and writing ([#11562](https://github.com/RocketChat/Rocket.Chat/pull/11562))

- Direct messages leaking into logs ([#11863](https://github.com/RocketChat/Rocket.Chat/pull/11863) by [@Hudell](https://github.com/Hudell))

- Double output of message actions ([#11902](https://github.com/RocketChat/Rocket.Chat/pull/11902) by [@timkinnane](https://github.com/timkinnane))

- Duplicate email and auto-join on mentions ([#12168](https://github.com/RocketChat/Rocket.Chat/pull/12168))

- Duplicated message buttons ([#11853](https://github.com/RocketChat/Rocket.Chat/pull/11853) by [@ubarsaiyan](https://github.com/ubarsaiyan))

- Files list missing from popover menu when owner of room ([#11565](https://github.com/RocketChat/Rocket.Chat/pull/11565))

- Fixing spacement between tags and words on some labels ([#12018](https://github.com/RocketChat/Rocket.Chat/pull/12018) by [@rssilva](https://github.com/rssilva))

- Fixing translation on 'yesterday' word when calling timeAgo function ([#11946](https://github.com/RocketChat/Rocket.Chat/pull/11946) by [@rssilva](https://github.com/rssilva))

- Hipchat import was failing when importing messages from a non existent user ([#11892](https://github.com/RocketChat/Rocket.Chat/pull/11892))

- Hipchat importer was not importing users without emails and uploaded files ([#11910](https://github.com/RocketChat/Rocket.Chat/pull/11910))

- Horizontal scroll on user info tab ([#12102](https://github.com/RocketChat/Rocket.Chat/pull/12102) by [@rssilva](https://github.com/rssilva))

- Internal error when cross-origin with CORS is disabled ([#11953](https://github.com/RocketChat/Rocket.Chat/pull/11953) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- IRC Federation no longer working ([#11906](https://github.com/RocketChat/Rocket.Chat/pull/11906) by [@Hudell](https://github.com/Hudell))

- Livechat agent joining on pick from guest pool ([#12097](https://github.com/RocketChat/Rocket.Chat/pull/12097) by [@mrsimpson](https://github.com/mrsimpson))

- Login error message not obvious if user not activated ([#11785](https://github.com/RocketChat/Rocket.Chat/pull/11785) by [@crazy-max](https://github.com/crazy-max))

- Markdown ampersand escape on links ([#12140](https://github.com/RocketChat/Rocket.Chat/pull/12140) by [@rssilva](https://github.com/rssilva))

- Message reaction in GraphQL API ([#11967](https://github.com/RocketChat/Rocket.Chat/pull/11967) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Not able to set per-channel retention policies if no global policy is set for this channel type ([#11927](https://github.com/RocketChat/Rocket.Chat/pull/11927) by [@vynmera](https://github.com/vynmera))

- Permission check on joinRoom for private room ([#11857](https://github.com/RocketChat/Rocket.Chat/pull/11857) by [@timkinnane](https://github.com/timkinnane))

- Position of popover component on mobile ([#12038](https://github.com/RocketChat/Rocket.Chat/pull/12038))

- Prevent form submission in Files List search ([#11999](https://github.com/RocketChat/Rocket.Chat/pull/11999))

- Re-add the eye-off icon ([#12079](https://github.com/RocketChat/Rocket.Chat/pull/12079) by [@MIKI785](https://github.com/MIKI785))

- Real Name on Direct Messages  ([#12154](https://github.com/RocketChat/Rocket.Chat/pull/12154))

- Saving user preferences ([#12170](https://github.com/RocketChat/Rocket.Chat/pull/12170))

- Typo in a configuration key for SlackBridge excluded bot names ([#11872](https://github.com/RocketChat/Rocket.Chat/pull/11872) by [@TobiasKappe](https://github.com/TobiasKappe))

- video message recording, issue #11651 ([#12031](https://github.com/RocketChat/Rocket.Chat/pull/12031) by [@flaviogrossi](https://github.com/flaviogrossi))

- Wrong build path in install.sh ([#11879](https://github.com/RocketChat/Rocket.Chat/pull/11879))

<details>
<summary>🔍 Minor changes</summary>


- Better organize package.json ([#12115](https://github.com/RocketChat/Rocket.Chat/pull/12115))

- Fix the style lint ([#11991](https://github.com/RocketChat/Rocket.Chat/pull/11991))

- Fix using wrong variable ([#12114](https://github.com/RocketChat/Rocket.Chat/pull/12114))

- Fix: Add e2e doc to the alert ([#12187](https://github.com/RocketChat/Rocket.Chat/pull/12187))

- Fix: Change wording on e2e to make a little more clear ([#12124](https://github.com/RocketChat/Rocket.Chat/pull/12124))

- Fix: e2e password visible on always-on alert message. ([#12139](https://github.com/RocketChat/Rocket.Chat/pull/12139) by [@Hudell](https://github.com/Hudell))

- Fix: Message changing order when been edited with apps enabled ([#12188](https://github.com/RocketChat/Rocket.Chat/pull/12188))

- Improve: Decrypt last message ([#12173](https://github.com/RocketChat/Rocket.Chat/pull/12173))

- Improve: Do not start E2E Encryption when accessing admin as embedded ([#12192](https://github.com/RocketChat/Rocket.Chat/pull/12192))

- Improve: E2E setting description and alert ([#12191](https://github.com/RocketChat/Rocket.Chat/pull/12191))

- Improve: Expose apps enable setting at `General > Apps` ([#12196](https://github.com/RocketChat/Rocket.Chat/pull/12196))

- Improve: Moved the e2e password request to an alert instead of a popup ([#12172](https://github.com/RocketChat/Rocket.Chat/pull/12172) by [@Hudell](https://github.com/Hudell))

- Improve: Rename E2E methods ([#12175](https://github.com/RocketChat/Rocket.Chat/pull/12175) by [@Hudell](https://github.com/Hudell))

- Improve: Switch e2e doc to target _blank ([#12195](https://github.com/RocketChat/Rocket.Chat/pull/12195))

- LingoHub based on develop ([#11936](https://github.com/RocketChat/Rocket.Chat/pull/11936))

- Merge master into develop & Set version to 0.70.0-develop ([#11921](https://github.com/RocketChat/Rocket.Chat/pull/11921) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@c0dzilla](https://github.com/c0dzilla) & [@rndmh3ro](https://github.com/rndmh3ro) & [@ubarsaiyan](https://github.com/ubarsaiyan) & [@vynmera](https://github.com/vynmera))

- New: Option to change E2E key ([#12169](https://github.com/RocketChat/Rocket.Chat/pull/12169) by [@Hudell](https://github.com/Hudell))

- Regression: fix message box autogrow ([#12138](https://github.com/RocketChat/Rocket.Chat/pull/12138))

- Regression: Modal height ([#12122](https://github.com/RocketChat/Rocket.Chat/pull/12122))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MIKI785](https://github.com/MIKI785)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@TobiasKappe](https://github.com/TobiasKappe)
- [@aferreira44](https://github.com/aferreira44)
- [@arch119](https://github.com/arch119)
- [@c0dzilla](https://github.com/c0dzilla)
- [@cardoso](https://github.com/cardoso)
- [@crazy-max](https://github.com/crazy-max)
- [@edzluhan](https://github.com/edzluhan)
- [@flaviogrossi](https://github.com/flaviogrossi)
- [@karakayasemi](https://github.com/karakayasemi)
- [@mrinaldhar](https://github.com/mrinaldhar)
- [@mrsimpson](https://github.com/mrsimpson)
- [@ohmonster](https://github.com/ohmonster)
- [@pkgodara](https://github.com/pkgodara)
- [@rndmh3ro](https://github.com/rndmh3ro)
- [@rssilva](https://github.com/rssilva)
- [@thaiphv](https://github.com/thaiphv)
- [@timkinnane](https://github.com/timkinnane)
- [@ubarsaiyan](https://github.com/ubarsaiyan)
- [@vynmera](https://github.com/vynmera)
- [@williamriancho](https://github.com/williamriancho)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.69.2
`2018-09-11  ·  1 🎉  ·  4 🐛  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🎉 New features


- Include room name in stream for bots ([#11812](https://github.com/RocketChat/Rocket.Chat/pull/11812) by [@timkinnane](https://github.com/timkinnane))

### 🐛 Bug fixes


- Apps: setting with 'code' type only saving last line ([#11992](https://github.com/RocketChat/Rocket.Chat/pull/11992) by [@cardoso](https://github.com/cardoso))

- Hidden admin sidenav on embedded layout ([#12025](https://github.com/RocketChat/Rocket.Chat/pull/12025))

- Reset password link error if already logged in ([#12022](https://github.com/RocketChat/Rocket.Chat/pull/12022))

- Update user information not possible by admin if disabled to users ([#11955](https://github.com/RocketChat/Rocket.Chat/pull/11955) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

### 👩‍💻👨‍💻 Contributors 😍

- [@cardoso](https://github.com/cardoso)
- [@kaiiiiiiiii](https://github.com/kaiiiiiiiii)
- [@timkinnane](https://github.com/timkinnane)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.69.1
`2018-08-31  ·  4 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- App updates were not being shown correctly ([#11893](https://github.com/RocketChat/Rocket.Chat/pull/11893))

- Duplicated message buttons ([#11853](https://github.com/RocketChat/Rocket.Chat/pull/11853) by [@ubarsaiyan](https://github.com/ubarsaiyan))

- Hipchat import was failing when importing messages from a non existent user ([#11892](https://github.com/RocketChat/Rocket.Chat/pull/11892))

- Hipchat importer was not importing users without emails and uploaded files ([#11910](https://github.com/RocketChat/Rocket.Chat/pull/11910))

### 👩‍💻👨‍💻 Contributors 😍

- [@ubarsaiyan](https://github.com/ubarsaiyan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)

# 0.69.0
`2018-08-28  ·  10 🎉  ·  8 🚀  ·  45 🐛  ·  12 🔍  ·  27 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🎉 New features


- Beta support for Big Blue Button video conferencing system ([#11837](https://github.com/RocketChat/Rocket.Chat/pull/11837))

- Internal marketplace for apps ([#11864](https://github.com/RocketChat/Rocket.Chat/pull/11864) by [@gdelavald](https://github.com/gdelavald) & [@rssilva](https://github.com/rssilva))

- Make font of unread items bolder for better contrast ([#8602](https://github.com/RocketChat/Rocket.Chat/pull/8602) by [@ausminternet](https://github.com/ausminternet))

- Personal access tokens for users to create API tokens ([#11638](https://github.com/RocketChat/Rocket.Chat/pull/11638) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST endpoint to manage server assets ([#11697](https://github.com/RocketChat/Rocket.Chat/pull/11697) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Rich message text and image buttons ([#11473](https://github.com/RocketChat/Rocket.Chat/pull/11473) by [@ubarsaiyan](https://github.com/ubarsaiyan))

- Setting to block unauthenticated access to avatars ([#9749](https://github.com/RocketChat/Rocket.Chat/pull/9749) by [@Hudell](https://github.com/Hudell))

- Setting to enable/disable slack bridge reactions ([#10217](https://github.com/RocketChat/Rocket.Chat/pull/10217) by [@Hudell](https://github.com/Hudell) & [@kable-wilmoth](https://github.com/kable-wilmoth))

- Setting to set a JS/CSS CDN ([#11779](https://github.com/RocketChat/Rocket.Chat/pull/11779))

- Slackbridge: send attachment notifications ([#10269](https://github.com/RocketChat/Rocket.Chat/pull/10269) by [@Hudell](https://github.com/Hudell) & [@kable-wilmoth](https://github.com/kable-wilmoth))

### 🚀 Improvements


- Add nyan rocket on Rocket.Chat preview Docker image ([#11684](https://github.com/RocketChat/Rocket.Chat/pull/11684))

- Add template tag #{userdn} to filter LDAP group member format ([#11662](https://github.com/RocketChat/Rocket.Chat/pull/11662) by [@crazy-max](https://github.com/crazy-max))

- Escape parameters before send them to email template ([#11644](https://github.com/RocketChat/Rocket.Chat/pull/11644))

- Messagebox fix performance ([#11686](https://github.com/RocketChat/Rocket.Chat/pull/11686))

- Reducing `saveUser` code complexity ([#11645](https://github.com/RocketChat/Rocket.Chat/pull/11645) by [@Hudell](https://github.com/Hudell))

- Role tag UI ([#11674](https://github.com/RocketChat/Rocket.Chat/pull/11674) by [@timkinnane](https://github.com/timkinnane))

- Start storing Livechat department within rooms ([#11733](https://github.com/RocketChat/Rocket.Chat/pull/11733))

- Warn about push settings that need server restart ([#11784](https://github.com/RocketChat/Rocket.Chat/pull/11784))

### 🐛 Bug fixes


- "User is typing" not working in new Livechat session ([#11670](https://github.com/RocketChat/Rocket.Chat/pull/11670))

- App's i18nAlert is only being displayed as "i18nAlert" ([#11802](https://github.com/RocketChat/Rocket.Chat/pull/11802))

- Apply Cordova fix in lazy-loaded images sources ([#11807](https://github.com/RocketChat/Rocket.Chat/pull/11807))

- Broken logo on setup wizard ([#11708](https://github.com/RocketChat/Rocket.Chat/pull/11708))

- Cannot set property 'input' of undefined ([#11775](https://github.com/RocketChat/Rocket.Chat/pull/11775))

- Closed connections being storing on db ([#11709](https://github.com/RocketChat/Rocket.Chat/pull/11709))

- Code tag duplicating characters ([#11467](https://github.com/RocketChat/Rocket.Chat/pull/11467) by [@vynmera](https://github.com/vynmera))

- Custom sound uploader not working in Firefox and IE ([#11139](https://github.com/RocketChat/Rocket.Chat/pull/11139) by [@vynmera](https://github.com/vynmera))

- Default server language not being applied ([#11719](https://github.com/RocketChat/Rocket.Chat/pull/11719))

- Delete removed user's subscriptions ([#10700](https://github.com/RocketChat/Rocket.Chat/pull/10700) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- directory search table not clickable lines ([#11809](https://github.com/RocketChat/Rocket.Chat/pull/11809))

- Escape meta data before inject in head tag ([#11730](https://github.com/RocketChat/Rocket.Chat/pull/11730))

- Fix links in `onTableItemClick` of the directroy page ([#11543](https://github.com/RocketChat/Rocket.Chat/pull/11543) by [@ura14h](https://github.com/ura14h))

- Fix permalink of message when running system with subdir ([#11781](https://github.com/RocketChat/Rocket.Chat/pull/11781) by [@ura14h](https://github.com/ura14h))

- Fixing timeAgo function on directory ([#11728](https://github.com/RocketChat/Rocket.Chat/pull/11728) by [@rssilva](https://github.com/rssilva))

- Incorrect migration version in v130.js ([#11544](https://github.com/RocketChat/Rocket.Chat/pull/11544) by [@c0dzilla](https://github.com/c0dzilla))

- Livechat open room method ([#11830](https://github.com/RocketChat/Rocket.Chat/pull/11830))

- Livechat rooms starting with two unread message counter ([#11834](https://github.com/RocketChat/Rocket.Chat/pull/11834))

- LiveChat switch department not working ([#11011](https://github.com/RocketChat/Rocket.Chat/pull/11011))

- Login logo now centered on small screens ([#11626](https://github.com/RocketChat/Rocket.Chat/pull/11626) by [@wreiske](https://github.com/wreiske))

- Message attachments was not respecting sort and lost spacing ([#11740](https://github.com/RocketChat/Rocket.Chat/pull/11740))

- minor fixes in hungarian i18n  ([#11797](https://github.com/RocketChat/Rocket.Chat/pull/11797) by [@Atisom](https://github.com/Atisom))

- minor fixes in i18n ([#11761](https://github.com/RocketChat/Rocket.Chat/pull/11761) by [@Atisom](https://github.com/Atisom))

- Missing chat history for users without permission `preview-c-room` ([#11639](https://github.com/RocketChat/Rocket.Chat/pull/11639) by [@Hudell](https://github.com/Hudell))

- Missing twitter:image and og:image tags ([#11687](https://github.com/RocketChat/Rocket.Chat/pull/11687))

- permissions name no break ([#11836](https://github.com/RocketChat/Rocket.Chat/pull/11836))

- Prune translation on room info panel ([#11635](https://github.com/RocketChat/Rocket.Chat/pull/11635))

- Prune translations in German ([#11631](https://github.com/RocketChat/Rocket.Chat/pull/11631) by [@rndmh3ro](https://github.com/rndmh3ro))

- Push notifications stuck after db failure ([#11667](https://github.com/RocketChat/Rocket.Chat/pull/11667))

- re-adding margin to menu icon on header ([#11778](https://github.com/RocketChat/Rocket.Chat/pull/11778) by [@rssilva](https://github.com/rssilva))

- Regression in prune by user, and update lastMessage ([#11646](https://github.com/RocketChat/Rocket.Chat/pull/11646) by [@vynmera](https://github.com/vynmera))

- Removed hardcoded values. ([#11627](https://github.com/RocketChat/Rocket.Chat/pull/11627) by [@Hudell](https://github.com/Hudell))

- Render Attachment Pretext When Markdown Specified ([#11578](https://github.com/RocketChat/Rocket.Chat/pull/11578) by [@glstewart17](https://github.com/glstewart17))

- REST `im.members` endpoint not working without sort parameter ([#11821](https://github.com/RocketChat/Rocket.Chat/pull/11821) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST endpoints to update user not respecting some settings ([#11474](https://github.com/RocketChat/Rocket.Chat/pull/11474) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Results pagination on /directory REST endpoint ([#11551](https://github.com/RocketChat/Rocket.Chat/pull/11551) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Return room ID for groups where user joined ([#11703](https://github.com/RocketChat/Rocket.Chat/pull/11703) by [@timkinnane](https://github.com/timkinnane))

- Revoked `view-d-room` permission logics ([#11522](https://github.com/RocketChat/Rocket.Chat/pull/11522) by [@Hudell](https://github.com/Hudell))

- SAML is flooding logfile ([#11643](https://github.com/RocketChat/Rocket.Chat/pull/11643) by [@Hudell](https://github.com/Hudell))

- SAML login not working when user has multiple emails ([#11642](https://github.com/RocketChat/Rocket.Chat/pull/11642) by [@Hudell](https://github.com/Hudell))

- Searching by `undefined` via REST when using `query` param ([#11657](https://github.com/RocketChat/Rocket.Chat/pull/11657) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Some assets were pointing to nonexistent path ([#11796](https://github.com/RocketChat/Rocket.Chat/pull/11796))

- Translations were not unique per app allowing conflicts among apps ([#11878](https://github.com/RocketChat/Rocket.Chat/pull/11878))

- User info APIs not returning customFields correctly ([#11625](https://github.com/RocketChat/Rocket.Chat/pull/11625) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- wrong create date of channels and users on directory view ([#11682](https://github.com/RocketChat/Rocket.Chat/pull/11682) by [@gsperezb](https://github.com/gsperezb))

<details>
<summary>🔍 Minor changes</summary>


- Add new eslint rules (automatically fixed) ([#11800](https://github.com/RocketChat/Rocket.Chat/pull/11800))

- Additional eslint rules  ([#11804](https://github.com/RocketChat/Rocket.Chat/pull/11804))

- App engine merge ([#11835](https://github.com/RocketChat/Rocket.Chat/pull/11835))

- Do not remove package-lock.json of livechat package ([#11816](https://github.com/RocketChat/Rocket.Chat/pull/11816))

- Fixed deutsch message pruning translations ([#11691](https://github.com/RocketChat/Rocket.Chat/pull/11691) by [@TheReal1604](https://github.com/TheReal1604))

- Fixed the Finnish translation and removed some profanities ([#11794](https://github.com/RocketChat/Rocket.Chat/pull/11794) by [@jukper](https://github.com/jukper))

- LingoHub based on develop ([#11838](https://github.com/RocketChat/Rocket.Chat/pull/11838))

- Merge master into develop & Set version to 0.69.0-develop ([#11606](https://github.com/RocketChat/Rocket.Chat/pull/11606))

- Regression: Fix livechat code issues after new lint rules ([#11814](https://github.com/RocketChat/Rocket.Chat/pull/11814))

- Regression: Fix purge message's translations ([#11590](https://github.com/RocketChat/Rocket.Chat/pull/11590))

- Regression: role tag background, unread item font and message box autogrow ([#11861](https://github.com/RocketChat/Rocket.Chat/pull/11861))

- Run eslint and unit tests on pre-push hook ([#11815](https://github.com/RocketChat/Rocket.Chat/pull/11815))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Atisom](https://github.com/Atisom)
- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@TheReal1604](https://github.com/TheReal1604)
- [@ausminternet](https://github.com/ausminternet)
- [@c0dzilla](https://github.com/c0dzilla)
- [@crazy-max](https://github.com/crazy-max)
- [@gdelavald](https://github.com/gdelavald)
- [@glstewart17](https://github.com/glstewart17)
- [@gsperezb](https://github.com/gsperezb)
- [@jukper](https://github.com/jukper)
- [@kable-wilmoth](https://github.com/kable-wilmoth)
- [@rndmh3ro](https://github.com/rndmh3ro)
- [@rssilva](https://github.com/rssilva)
- [@timkinnane](https://github.com/timkinnane)
- [@ubarsaiyan](https://github.com/ubarsaiyan)
- [@ura14h](https://github.com/ura14h)
- [@vynmera](https://github.com/vynmera)
- [@wreiske](https://github.com/wreiske)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.68.5
`2018-08-23  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Livechat open room method ([#11830](https://github.com/RocketChat/Rocket.Chat/pull/11830))

### 👩‍💻👨‍💻 Core Team 🤓

- [@renatobecker](https://github.com/renatobecker)

# 0.68.4
`2018-08-10  ·  3 🐛  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Broken logo on setup wizard ([#11708](https://github.com/RocketChat/Rocket.Chat/pull/11708))

- Default server language not being applied ([#11719](https://github.com/RocketChat/Rocket.Chat/pull/11719))

- Regression in prune by user, and update lastMessage ([#11646](https://github.com/RocketChat/Rocket.Chat/pull/11646) by [@vynmera](https://github.com/vynmera))

### 👩‍💻👨‍💻 Contributors 😍

- [@vynmera](https://github.com/vynmera)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.68.3
`2018-08-01  ·  5 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Missing chat history for users without permission `preview-c-room` ([#11639](https://github.com/RocketChat/Rocket.Chat/pull/11639) by [@Hudell](https://github.com/Hudell))

- Prune translation on room info panel ([#11635](https://github.com/RocketChat/Rocket.Chat/pull/11635))

- Prune translations in German ([#11631](https://github.com/RocketChat/Rocket.Chat/pull/11631) by [@rndmh3ro](https://github.com/rndmh3ro))

- SAML login not working when user has multiple emails ([#11642](https://github.com/RocketChat/Rocket.Chat/pull/11642) by [@Hudell](https://github.com/Hudell))

- User info APIs not returning customFields correctly ([#11625](https://github.com/RocketChat/Rocket.Chat/pull/11625) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.68.3 ([#11650](https://github.com/RocketChat/Rocket.Chat/pull/11650) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@rndmh3ro](https://github.com/rndmh3ro))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@rndmh3ro](https://github.com/rndmh3ro)

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.68.2
`2018-07-31  ·  1 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Incorrect migration version in v130.js ([#11544](https://github.com/RocketChat/Rocket.Chat/pull/11544) by [@c0dzilla](https://github.com/c0dzilla))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.68.2 ([#11630](https://github.com/RocketChat/Rocket.Chat/pull/11630) by [@c0dzilla](https://github.com/c0dzilla))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@c0dzilla](https://github.com/c0dzilla)

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.68.1
`2018-07-31  ·  2 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- `Jump to message` search result action ([#11613](https://github.com/RocketChat/Rocket.Chat/pull/11613))

- HipChat importer wasn’t compatible with latest exports ([#11597](https://github.com/RocketChat/Rocket.Chat/pull/11597))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.68.1 ([#11616](https://github.com/RocketChat/Rocket.Chat/pull/11616))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.68.0
`2018-07-27  ·  2 ️️️⚠️  ·  13 🎉  ·  3 🚀  ·  23 🐛  ·  10 🔍  ·  21 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### ⚠️ BREAKING CHANGES


- Remove deprecated /user.roles endpoint ([#11493](https://github.com/RocketChat/Rocket.Chat/pull/11493) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Update GraphQL dependencies ([#11430](https://github.com/RocketChat/Rocket.Chat/pull/11430) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 🎉 New features


- Accept resumeToken as query param to log in ([#11443](https://github.com/RocketChat/Rocket.Chat/pull/11443))

- Add /roles.list REST endpoint to retrieve all server roles ([#11500](https://github.com/RocketChat/Rocket.Chat/pull/11500) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add /users.deleteOwnAccount REST endpoint to an user delete his own account ([#11488](https://github.com/RocketChat/Rocket.Chat/pull/11488) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Livechat File Upload ([#10514](https://github.com/RocketChat/Rocket.Chat/pull/10514))

- Make WebRTC not enabled by default ([#11489](https://github.com/RocketChat/Rocket.Chat/pull/11489))

- Message retention policy and pruning ([#11236](https://github.com/RocketChat/Rocket.Chat/pull/11236) by [@vynmera](https://github.com/vynmera))

- Privacy for custom user fields ([#11332](https://github.com/RocketChat/Rocket.Chat/pull/11332) by [@vynmera](https://github.com/vynmera))

- Replaced old logo with the new ones ([#11491](https://github.com/RocketChat/Rocket.Chat/pull/11491) by [@brunosquadros](https://github.com/brunosquadros))

- Room files search form ([#11486](https://github.com/RocketChat/Rocket.Chat/pull/11486))

- search only default tone emoji Popup search ([#10017](https://github.com/RocketChat/Rocket.Chat/pull/10017) by [@Joe-mcgee](https://github.com/Joe-mcgee))

- Send user status to client ([#11303](https://github.com/RocketChat/Rocket.Chat/pull/11303) by [@HappyTobi](https://github.com/HappyTobi))

- Setting to disable 2FA globally ([#11328](https://github.com/RocketChat/Rocket.Chat/pull/11328) by [@Hudell](https://github.com/Hudell))

- Sorting channels by number of users in directory ([#9972](https://github.com/RocketChat/Rocket.Chat/pull/9972) by [@arungalva](https://github.com/arungalva))

### 🚀 Improvements


- Allow markdown in room topic, announcement, and description including single quotes ([#11408](https://github.com/RocketChat/Rocket.Chat/pull/11408))

- Set default max upload size to 100mb ([#11327](https://github.com/RocketChat/Rocket.Chat/pull/11327) by [@cardoso](https://github.com/cardoso))

- Typing indicators now use Real Names ([#11164](https://github.com/RocketChat/Rocket.Chat/pull/11164) by [@vynmera](https://github.com/vynmera))

### 🐛 Bug fixes


- Add customFields property to /me REST endpoint response ([#11496](https://github.com/RocketChat/Rocket.Chat/pull/11496) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- broadcast channel reply ([#11462](https://github.com/RocketChat/Rocket.Chat/pull/11462))

- Check for channels property on message object before parsing mentions ([#11527](https://github.com/RocketChat/Rocket.Chat/pull/11527))

- Decrease room leader bar z-index ([#11450](https://github.com/RocketChat/Rocket.Chat/pull/11450))

- empty blockquote ([#11526](https://github.com/RocketChat/Rocket.Chat/pull/11526))

- Fixed svg for older chrome browsers bug #11414 ([#11416](https://github.com/RocketChat/Rocket.Chat/pull/11416) by [@tpDBL](https://github.com/tpDBL))

- Invalid permalink URLs for Direct Messages ([#11507](https://github.com/RocketChat/Rocket.Chat/pull/11507) by [@Hudell](https://github.com/Hudell))

- Loading and setting fixes for i18n and RTL ([#11363](https://github.com/RocketChat/Rocket.Chat/pull/11363))

- Marked parser breaking announcements and mentions at the start of messages ([#11357](https://github.com/RocketChat/Rocket.Chat/pull/11357) by [@vynmera](https://github.com/vynmera))

- Mixed case channel slugs ([#9449](https://github.com/RocketChat/Rocket.Chat/pull/9449) by [@soundstorm](https://github.com/soundstorm))

- New favicons size too small ([#11524](https://github.com/RocketChat/Rocket.Chat/pull/11524) by [@brunosquadros](https://github.com/brunosquadros))

- Only escape HTML from details in toast error messages ([#11459](https://github.com/RocketChat/Rocket.Chat/pull/11459))

- Record popup ([#11349](https://github.com/RocketChat/Rocket.Chat/pull/11349))

- Refinements in message popup mentions ([#11441](https://github.com/RocketChat/Rocket.Chat/pull/11441))

- Remove title attribute from sidebar items ([#11298](https://github.com/RocketChat/Rocket.Chat/pull/11298))

- Render reply preview with message as a common message ([#11534](https://github.com/RocketChat/Rocket.Chat/pull/11534))

- RocketChat.settings.get causing memory leak (sometimes) ([#11487](https://github.com/RocketChat/Rocket.Chat/pull/11487))

- SAML issues ([#11135](https://github.com/RocketChat/Rocket.Chat/pull/11135) by [@Hudell](https://github.com/Hudell) & [@arminfelder](https://github.com/arminfelder))

- Send Livechat back to Guest Pool ([#10731](https://github.com/RocketChat/Rocket.Chat/pull/10731))

- Snap font issue for sharp ([#11514](https://github.com/RocketChat/Rocket.Chat/pull/11514))

- Unlimited upload file size not working ([#11471](https://github.com/RocketChat/Rocket.Chat/pull/11471) by [@Hudell](https://github.com/Hudell))

- Unreads counter for new rooms on /channels.counters REST endpoint ([#11531](https://github.com/RocketChat/Rocket.Chat/pull/11531) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Wrap custom fields in user profile to new line ([#10119](https://github.com/RocketChat/Rocket.Chat/pull/10119) by [@PhpXp](https://github.com/PhpXp) & [@karlprieb](https://github.com/karlprieb))

<details>
<summary>🔍 Minor changes</summary>


- LingoHub based on develop ([#11587](https://github.com/RocketChat/Rocket.Chat/pull/11587))

- Merge master into develop & Set version to 0.68.0-develop ([#11536](https://github.com/RocketChat/Rocket.Chat/pull/11536))

- Regression: Add missing LiveChat permission to allow removing closed rooms ([#11423](https://github.com/RocketChat/Rocket.Chat/pull/11423))

- Regression: Fix purge message's translations ([#11590](https://github.com/RocketChat/Rocket.Chat/pull/11590))

- Regression: Make message popup user mentions reactive again ([#11567](https://github.com/RocketChat/Rocket.Chat/pull/11567))

- Regression: nonReactive to nonreactive ([#11550](https://github.com/RocketChat/Rocket.Chat/pull/11550))

- Regression: Remove safe area margins from logos ([#11508](https://github.com/RocketChat/Rocket.Chat/pull/11508) by [@brunosquadros](https://github.com/brunosquadros))

- Regression: Update cachedCollection version ([#11561](https://github.com/RocketChat/Rocket.Chat/pull/11561))

- Revert: Mixed case channel slugs #9449 ([#11537](https://github.com/RocketChat/Rocket.Chat/pull/11537))

- Update release issue template to use Houston CLI ([#11499](https://github.com/RocketChat/Rocket.Chat/pull/11499))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@HappyTobi](https://github.com/HappyTobi)
- [@Hudell](https://github.com/Hudell)
- [@Joe-mcgee](https://github.com/Joe-mcgee)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@PhpXp](https://github.com/PhpXp)
- [@arminfelder](https://github.com/arminfelder)
- [@arungalva](https://github.com/arungalva)
- [@brunosquadros](https://github.com/brunosquadros)
- [@cardoso](https://github.com/cardoso)
- [@karlprieb](https://github.com/karlprieb)
- [@soundstorm](https://github.com/soundstorm)
- [@tpDBL](https://github.com/tpDBL)
- [@vynmera](https://github.com/vynmera)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.67.0
`2018-07-20  ·  1 ️️️⚠️  ·  1 🎉  ·  2 🚀  ·  15 🐛  ·  7 🔍  ·  11 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### ⚠️ BREAKING CHANGES


- Remove cache layer and internal calculated property `room.usernames` ([#10749](https://github.com/RocketChat/Rocket.Chat/pull/10749))

### 🎉 New features


- Additional Livechat iFrame API's ([#10918](https://github.com/RocketChat/Rocket.Chat/pull/10918))

### 🚀 Improvements


- Setup Wizard username validation, step progress and optin/optout ([#11254](https://github.com/RocketChat/Rocket.Chat/pull/11254))

- Stop sort callbacks on run ([#11330](https://github.com/RocketChat/Rocket.Chat/pull/11330))

### 🐛 Bug fixes


- All messages notifications via email were sent as mention alert ([#11398](https://github.com/RocketChat/Rocket.Chat/pull/11398))

- Livechat not sending desktop notifications ([#11266](https://github.com/RocketChat/Rocket.Chat/pull/11266))

- Livechat taking inquiry leading to 404 page ([#11406](https://github.com/RocketChat/Rocket.Chat/pull/11406))

- Livestream muted when audio only option was enabled ([#11267](https://github.com/RocketChat/Rocket.Chat/pull/11267) by [@gdelavald](https://github.com/gdelavald))

- Message attachment's fields with different sizes ([#11342](https://github.com/RocketChat/Rocket.Chat/pull/11342))

- Message popup responsiveness in slash commands ([#11313](https://github.com/RocketChat/Rocket.Chat/pull/11313))

- Notification preferences being lost when switching view mode ([#11295](https://github.com/RocketChat/Rocket.Chat/pull/11295))

- Outgoing integrations were stopping the oplog tailing sometimes ([#11333](https://github.com/RocketChat/Rocket.Chat/pull/11333))

- Parse inline code without space before initial backtick ([#9754](https://github.com/RocketChat/Rocket.Chat/pull/9754) by [@c0dzilla](https://github.com/c0dzilla) & [@gdelavald](https://github.com/gdelavald))

- Remove file snap store doesn't like ([#11365](https://github.com/RocketChat/Rocket.Chat/pull/11365))

- SAML attributes with periods are not properly read. ([#11315](https://github.com/RocketChat/Rocket.Chat/pull/11315) by [@Hudell](https://github.com/Hudell))

- Some updates were returning errors when based on queries with position operators ([#11335](https://github.com/RocketChat/Rocket.Chat/pull/11335))

- sort fname sidenav ([#11358](https://github.com/RocketChat/Rocket.Chat/pull/11358))

- SVG icons code ([#11319](https://github.com/RocketChat/Rocket.Chat/pull/11319))

- web app manifest errors as reported by Chrome DevTools ([#9991](https://github.com/RocketChat/Rocket.Chat/pull/9991) by [@justinribeiro](https://github.com/justinribeiro))

<details>
<summary>🔍 Minor changes</summary>


- Fix dependency issue in redhat image ([#11497](https://github.com/RocketChat/Rocket.Chat/pull/11497))

- Merge master into develop & Set version to 0.67.0-develop ([#11417](https://github.com/RocketChat/Rocket.Chat/pull/11417))

- Merge master into develop & Set version to 0.67.0-develop ([#11399](https://github.com/RocketChat/Rocket.Chat/pull/11399))

- Merge master into develop & Set version to 0.67.0-develop ([#11348](https://github.com/RocketChat/Rocket.Chat/pull/11348) by [@Hudell](https://github.com/Hudell) & [@gdelavald](https://github.com/gdelavald))

- Merge master into develop & Set version to 0.67.0-develop ([#11290](https://github.com/RocketChat/Rocket.Chat/pull/11290))

- Regression: Fix migration 125 checking for settings field ([#11364](https://github.com/RocketChat/Rocket.Chat/pull/11364))

- Send setting Allow_Marketing_Emails to statistics collector ([#11359](https://github.com/RocketChat/Rocket.Chat/pull/11359))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@c0dzilla](https://github.com/c0dzilla)
- [@gdelavald](https://github.com/gdelavald)
- [@justinribeiro](https://github.com/justinribeiro)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.66.3
`2018-07-09  ·  2 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- All messages notifications via email were sent as mention alert ([#11398](https://github.com/RocketChat/Rocket.Chat/pull/11398))

- Livechat taking inquiry leading to 404 page ([#11406](https://github.com/RocketChat/Rocket.Chat/pull/11406))

### 👩‍💻👨‍💻 Core Team 🤓

- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)

# 0.66.2
`2018-07-06  ·  2 🐛  ·  2 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Livechat not sending desktop notifications ([#11266](https://github.com/RocketChat/Rocket.Chat/pull/11266))

- Remove file snap store doesn't like ([#11365](https://github.com/RocketChat/Rocket.Chat/pull/11365))

<details>
<summary>🔍 Minor changes</summary>


- Regression: Fix migration 125 checking for settings field ([#11364](https://github.com/RocketChat/Rocket.Chat/pull/11364))

- Send setting Allow_Marketing_Emails to statistics collector ([#11359](https://github.com/RocketChat/Rocket.Chat/pull/11359))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.66.1
`2018-07-04  ·  1 🚀  ·  5 🐛  ·  6 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### 🚀 Improvements


- Setup Wizard username validation, step progress and optin/optout ([#11254](https://github.com/RocketChat/Rocket.Chat/pull/11254))

### 🐛 Bug fixes


- Livestream muted when audio only option was enabled ([#11267](https://github.com/RocketChat/Rocket.Chat/pull/11267) by [@gdelavald](https://github.com/gdelavald))

- Notification preferences being lost when switching view mode ([#11295](https://github.com/RocketChat/Rocket.Chat/pull/11295))

- Outgoing integrations were stopping the oplog tailing sometimes ([#11333](https://github.com/RocketChat/Rocket.Chat/pull/11333))

- SAML attributes with periods are not properly read. ([#11315](https://github.com/RocketChat/Rocket.Chat/pull/11315) by [@Hudell](https://github.com/Hudell))

- Some updates were returning errors when based on queries with position operators ([#11335](https://github.com/RocketChat/Rocket.Chat/pull/11335))

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@gdelavald](https://github.com/gdelavald)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.66.0
`2018-06-27  ·  1 ️️️⚠️  ·  23 🎉  ·  3 🚀  ·  59 🐛  ·  47 🔍  ·  45 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.3`
- NPM: `5.6.0`

### ⚠️ BREAKING CHANGES


- Always remove the field `services` from user data responses in REST API ([#10799](https://github.com/RocketChat/Rocket.Chat/pull/10799) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 🎉 New features


- Add input to set time for avatar cache control ([#10958](https://github.com/RocketChat/Rocket.Chat/pull/10958) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add prometheus port config ([#11115](https://github.com/RocketChat/Rocket.Chat/pull/11115) by [@brylie](https://github.com/brylie) & [@stuartpb](https://github.com/stuartpb) & [@thaiphv](https://github.com/thaiphv))

- Button to remove closed LiveChat rooms ([#10301](https://github.com/RocketChat/Rocket.Chat/pull/10301))

- Changes all 'mergeChannels' to 'groupByType'. ([#10055](https://github.com/RocketChat/Rocket.Chat/pull/10055) by [@mikaelmello](https://github.com/mikaelmello))

- Command /hide to hide channels ([#10727](https://github.com/RocketChat/Rocket.Chat/pull/10727) by [@mikaelmello](https://github.com/mikaelmello))

- Custom login wallpapers ([#11025](https://github.com/RocketChat/Rocket.Chat/pull/11025) by [@vynmera](https://github.com/vynmera))

- Direct Reply: separate Reply-To email from account username field ([#10988](https://github.com/RocketChat/Rocket.Chat/pull/10988) by [@pkgodara](https://github.com/pkgodara))

- Disconnect users from websocket when away from the login screen for 10min ([#11086](https://github.com/RocketChat/Rocket.Chat/pull/11086))

- Do not wait method calls response on websocket before next method call ([#11087](https://github.com/RocketChat/Rocket.Chat/pull/11087))

- Don't ask me again checkbox on hide room modal ([#10973](https://github.com/RocketChat/Rocket.Chat/pull/10973) by [@karlprieb](https://github.com/karlprieb))

- Make supplying an AWS access key and secret optional for S3 uploads ([#10673](https://github.com/RocketChat/Rocket.Chat/pull/10673) by [@saplla](https://github.com/saplla))

- Option to trace Methods and Subscription calls ([#11085](https://github.com/RocketChat/Rocket.Chat/pull/11085))

- Reduce the amount of DDP API calls on login screen ([#11083](https://github.com/RocketChat/Rocket.Chat/pull/11083))

- Replace variable 'mergeChannels' with 'groupByType'. ([#10954](https://github.com/RocketChat/Rocket.Chat/pull/10954) by [@mikaelmello](https://github.com/mikaelmello))

- REST API endpoint `channels.setDefault` ([#10941](https://github.com/RocketChat/Rocket.Chat/pull/10941) by [@vynmera](https://github.com/vynmera))

- REST API endpoints `permissions.list` and `permissions.update`. Deprecated endpoint `permissions` ([#10975](https://github.com/RocketChat/Rocket.Chat/pull/10975) by [@vynmera](https://github.com/vynmera))

- Send LiveChat visitor navigation history as messages ([#10091](https://github.com/RocketChat/Rocket.Chat/pull/10091))

- Set Document Domain property in IFrame ([#9751](https://github.com/RocketChat/Rocket.Chat/pull/9751) by [@kb0304](https://github.com/kb0304))

- Support for dynamic slack and rocket.chat channels ([#10205](https://github.com/RocketChat/Rocket.Chat/pull/10205) by [@Hudell](https://github.com/Hudell) & [@kable-wilmoth](https://github.com/kable-wilmoth))

- Update katex to v0.9.0 ([#8402](https://github.com/RocketChat/Rocket.Chat/pull/8402) by [@pitamar](https://github.com/pitamar))

- Update WeDeploy deployment ([#10841](https://github.com/RocketChat/Rocket.Chat/pull/10841) by [@jonnilundy](https://github.com/jonnilundy))

- WebDAV(Nextcloud/ownCloud) Storage Server Option ([#11027](https://github.com/RocketChat/Rocket.Chat/pull/11027) by [@karakayasemi](https://github.com/karakayasemi))

- Youtube Broadcasting ([#10127](https://github.com/RocketChat/Rocket.Chat/pull/10127) by [@gdelavald](https://github.com/gdelavald))

### 🚀 Improvements


- Listing of apps in the admin page ([#11166](https://github.com/RocketChat/Rocket.Chat/pull/11166) by [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb))

- UI design for Tables and tabs component on Directory ([#11026](https://github.com/RocketChat/Rocket.Chat/pull/11026) by [@karlprieb](https://github.com/karlprieb))

- User mentions ([#11001](https://github.com/RocketChat/Rocket.Chat/pull/11001) by [@vynmera](https://github.com/vynmera))

### 🐛 Bug fixes


- "blank messages" on iOS < 11 ([#11221](https://github.com/RocketChat/Rocket.Chat/pull/11221))

- "blank" screen on iOS < 11 ([#11199](https://github.com/RocketChat/Rocket.Chat/pull/11199))

- /groups.invite not allow a user to invite even with permission ([#11010](https://github.com/RocketChat/Rocket.Chat/pull/11010) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add parameter to REST chat.react endpoint, to make it work like a setter ([#10447](https://github.com/RocketChat/Rocket.Chat/pull/10447) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Allow inviting livechat managers to the same LiveChat room ([#10956](https://github.com/RocketChat/Rocket.Chat/pull/10956))

- Application crashing on startup when trying to log errors to `exceptions` channel ([#10934](https://github.com/RocketChat/Rocket.Chat/pull/10934))

- Armhf snap build ([#11268](https://github.com/RocketChat/Rocket.Chat/pull/11268))

- avoid send presence without login ([#11074](https://github.com/RocketChat/Rocket.Chat/pull/11074))

- Build for Sandstorm missing dependence for capnp ([#11056](https://github.com/RocketChat/Rocket.Chat/pull/11056) by [@peterlee0127](https://github.com/peterlee0127))

- Can't access the `/account/profile` ([#11089](https://github.com/RocketChat/Rocket.Chat/pull/11089))

- Cannot read property 'debug' of undefined when trying to use REST API ([#10805](https://github.com/RocketChat/Rocket.Chat/pull/10805) by [@haffla](https://github.com/haffla))

- Confirm password on set new password user profile ([#11095](https://github.com/RocketChat/Rocket.Chat/pull/11095))

- Default selected language ([#11150](https://github.com/RocketChat/Rocket.Chat/pull/11150))

- Exception in metrics generation ([#11072](https://github.com/RocketChat/Rocket.Chat/pull/11072))

- Exception thrown on avatar validation ([#11009](https://github.com/RocketChat/Rocket.Chat/pull/11009) by [@Hudell](https://github.com/Hudell))

- Failure to download user data ([#11190](https://github.com/RocketChat/Rocket.Chat/pull/11190) by [@Hudell](https://github.com/Hudell))

- flex-tab icons missing ([#11049](https://github.com/RocketChat/Rocket.Chat/pull/11049))

- Generated random password visible to the user ([#11096](https://github.com/RocketChat/Rocket.Chat/pull/11096))

- HipChat Cloud import fails to import rooms ([#11188](https://github.com/RocketChat/Rocket.Chat/pull/11188) by [@Hudell](https://github.com/Hudell))

- Icons svg xml structure ([#10771](https://github.com/RocketChat/Rocket.Chat/pull/10771) by [@timkinnane](https://github.com/timkinnane))

- Idle time limit wasn’t working as expected ([#11084](https://github.com/RocketChat/Rocket.Chat/pull/11084))

- Image lazy load was breaking attachments ([#10904](https://github.com/RocketChat/Rocket.Chat/pull/10904))

- Incomplete email notification link ([#10928](https://github.com/RocketChat/Rocket.Chat/pull/10928))

- Internal Server Error on first login with CAS integration ([#11257](https://github.com/RocketChat/Rocket.Chat/pull/11257) by [@Hudell](https://github.com/Hudell))

- LDAP was accepting login with empty passwords for certain AD configurations ([#11264](https://github.com/RocketChat/Rocket.Chat/pull/11264))

- Leave room wasn't working as expected ([#10851](https://github.com/RocketChat/Rocket.Chat/pull/10851))

- Link previews not being removed from messages after removed on editing ([#11063](https://github.com/RocketChat/Rocket.Chat/pull/11063))

- LiveChat appearance changes not being saved ([#11111](https://github.com/RocketChat/Rocket.Chat/pull/11111))

- Livechat icon with status ([#11177](https://github.com/RocketChat/Rocket.Chat/pull/11177))

- Livechat visitor not being prompted for transcript when himself is closing the chat ([#10767](https://github.com/RocketChat/Rocket.Chat/pull/10767))

- Message_AllowedMaxSize fails for emoji sequences ([#10431](https://github.com/RocketChat/Rocket.Chat/pull/10431) by [@c0dzilla](https://github.com/c0dzilla))

- Missing language constants ([#11173](https://github.com/RocketChat/Rocket.Chat/pull/11173) by [@rw4lll](https://github.com/rw4lll))

- Notification not working for group mentions and not respecting ignored users ([#11024](https://github.com/RocketChat/Rocket.Chat/pull/11024))

- open conversation from room info ([#11050](https://github.com/RocketChat/Rocket.Chat/pull/11050))

- Overlapping of search text and cancel search icon (X) ([#10294](https://github.com/RocketChat/Rocket.Chat/pull/10294) by [@taeven](https://github.com/taeven))

- Popover position ([#11113](https://github.com/RocketChat/Rocket.Chat/pull/11113))

- Preview of large images not resizing to fit the area and having scrollbars ([#10998](https://github.com/RocketChat/Rocket.Chat/pull/10998) by [@vynmera](https://github.com/vynmera))

- Reaction Toggle was not working when omitting the last parameter from the API (DDP and REST) ([#11276](https://github.com/RocketChat/Rocket.Chat/pull/11276) by [@Hudell](https://github.com/Hudell))

- Remove failed upload messages when switching rooms ([#11132](https://github.com/RocketChat/Rocket.Chat/pull/11132))

- Remove outdated 2FA warning for mobile clients ([#10916](https://github.com/RocketChat/Rocket.Chat/pull/10916) by [@cardoso](https://github.com/cardoso))

- remove sidebar on embedded view ([#11183](https://github.com/RocketChat/Rocket.Chat/pull/11183))

- Rendering of emails and mentions in messages ([#11165](https://github.com/RocketChat/Rocket.Chat/pull/11165))

- REST API: Add more test cases for `/login` ([#10999](https://github.com/RocketChat/Rocket.Chat/pull/10999) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST endpoint `users.updateOwnBasicInfo` was not returning errors for invalid names and trying to save custom fields when empty ([#11204](https://github.com/RocketChat/Rocket.Chat/pull/11204) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Room creation error due absence of subscriptions ([#11178](https://github.com/RocketChat/Rocket.Chat/pull/11178))

- Rooms list sorting by activity multiple re-renders and case sensitive sorting alphabetically ([#9959](https://github.com/RocketChat/Rocket.Chat/pull/9959) by [@JoseRenan](https://github.com/JoseRenan) & [@karlprieb](https://github.com/karlprieb))

- set-toolbar-items postMessage ([#11109](https://github.com/RocketChat/Rocket.Chat/pull/11109))

- Some typos in the error message names ([#11136](https://github.com/RocketChat/Rocket.Chat/pull/11136) by [@vynmera](https://github.com/vynmera))

- Strange msg when setting room announcement, topic or description to be empty ([#11012](https://github.com/RocketChat/Rocket.Chat/pull/11012) by [@vynmera](https://github.com/vynmera))

- The process was freezing in some cases when HTTP calls exceeds timeout on integrations ([#11253](https://github.com/RocketChat/Rocket.Chat/pull/11253))

- title and value attachments are optionals on sendMessage method ([#11021](https://github.com/RocketChat/Rocket.Chat/pull/11021) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Update capnproto dependence for Sandstorm Build ([#11263](https://github.com/RocketChat/Rocket.Chat/pull/11263) by [@peterlee0127](https://github.com/peterlee0127))

- Update ja.i18n.json ([#11020](https://github.com/RocketChat/Rocket.Chat/pull/11020) by [@Hudell](https://github.com/Hudell) & [@noobbbbb](https://github.com/noobbbbb))

- Update Sandstorm build config ([#10867](https://github.com/RocketChat/Rocket.Chat/pull/10867) by [@ocdtrekkie](https://github.com/ocdtrekkie))

- Users model was not receiving options ([#11129](https://github.com/RocketChat/Rocket.Chat/pull/11129))

- Various lang fixes [RU] ([#10095](https://github.com/RocketChat/Rocket.Chat/pull/10095) by [@rw4lll](https://github.com/rw4lll))

- Wordpress oauth configuration not loading properly ([#11187](https://github.com/RocketChat/Rocket.Chat/pull/11187) by [@Hudell](https://github.com/Hudell))

- Wordpress OAuth not providing enough info to log in  ([#11152](https://github.com/RocketChat/Rocket.Chat/pull/11152) by [@Hudell](https://github.com/Hudell))

- Wrong font-family order ([#11191](https://github.com/RocketChat/Rocket.Chat/pull/11191) by [@Hudell](https://github.com/Hudell) & [@myfonj](https://github.com/myfonj))

<details>
<summary>🔍 Minor changes</summary>


- [FIX Readme] Nodejs + Python version spicifications ([#11181](https://github.com/RocketChat/Rocket.Chat/pull/11181) by [@mahdiyari](https://github.com/mahdiyari))

- Add Dockerfile with MongoDB ([#10971](https://github.com/RocketChat/Rocket.Chat/pull/10971))

- Add verification to make sure the user exists in REST  insert object helper ([#11008](https://github.com/RocketChat/Rocket.Chat/pull/11008) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Build Docker image on CI ([#11076](https://github.com/RocketChat/Rocket.Chat/pull/11076))

- Changed 'confirm password' placeholder text on user registration form ([#9969](https://github.com/RocketChat/Rocket.Chat/pull/9969) by [@kumarnitj](https://github.com/kumarnitj))

- Develop sync commits ([#10909](https://github.com/RocketChat/Rocket.Chat/pull/10909) by [@nsuchy](https://github.com/nsuchy) & [@rafaelks](https://github.com/rafaelks))

- Develop sync2 ([#10908](https://github.com/RocketChat/Rocket.Chat/pull/10908) by [@nsuchy](https://github.com/nsuchy) & [@rafaelks](https://github.com/rafaelks))

- Fix Docker image build on tags ([#11271](https://github.com/RocketChat/Rocket.Chat/pull/11271))

- Fix Docker image for develop commits ([#11093](https://github.com/RocketChat/Rocket.Chat/pull/11093))

- Fix PR Docker image creation by splitting in two build jobs ([#11107](https://github.com/RocketChat/Rocket.Chat/pull/11107))

- Fix readme typo ([#5](https://github.com/RocketChat/Rocket.Chat/pull/5) by [@filipealva](https://github.com/filipealva))

- IRC Federation: RFC2813 implementation (ngIRCd) ([#10113](https://github.com/RocketChat/Rocket.Chat/pull/10113) by [@Hudell](https://github.com/Hudell) & [@cpitman](https://github.com/cpitman) & [@lindoelio](https://github.com/lindoelio))

- LingoHub based on develop ([#11208](https://github.com/RocketChat/Rocket.Chat/pull/11208))

- LingoHub based on develop ([#11062](https://github.com/RocketChat/Rocket.Chat/pull/11062))

- LingoHub based on develop ([#11054](https://github.com/RocketChat/Rocket.Chat/pull/11054))

- LingoHub based on develop ([#11053](https://github.com/RocketChat/Rocket.Chat/pull/11053))

- LingoHub based on develop ([#11051](https://github.com/RocketChat/Rocket.Chat/pull/11051))

- LingoHub based on develop ([#11045](https://github.com/RocketChat/Rocket.Chat/pull/11045))

- LingoHub based on develop ([#11044](https://github.com/RocketChat/Rocket.Chat/pull/11044))

- LingoHub based on develop ([#11043](https://github.com/RocketChat/Rocket.Chat/pull/11043))

- LingoHub based on develop ([#11042](https://github.com/RocketChat/Rocket.Chat/pull/11042))

- LingoHub based on develop ([#11039](https://github.com/RocketChat/Rocket.Chat/pull/11039))

- LingoHub based on develop ([#11035](https://github.com/RocketChat/Rocket.Chat/pull/11035))

- LingoHub based on develop ([#11246](https://github.com/RocketChat/Rocket.Chat/pull/11246))

- Merge master into develop & Set version to 0.66.0-develop ([#11277](https://github.com/RocketChat/Rocket.Chat/pull/11277) by [@Hudell](https://github.com/Hudell) & [@brylie](https://github.com/brylie) & [@stuartpb](https://github.com/stuartpb))

- Merge master into develop & Set version to 0.66.0-develop ([#10903](https://github.com/RocketChat/Rocket.Chat/pull/10903) by [@nsuchy](https://github.com/nsuchy) & [@rafaelks](https://github.com/rafaelks))

- New history source format & add Node and NPM versions ([#11237](https://github.com/RocketChat/Rocket.Chat/pull/11237))

- NPM Dependencies Update ([#10913](https://github.com/RocketChat/Rocket.Chat/pull/10913))

- Regression: check username or usersCount on browseChannels ([#11216](https://github.com/RocketChat/Rocket.Chat/pull/11216))

- Regression: Directory css ([#11206](https://github.com/RocketChat/Rocket.Chat/pull/11206) by [@karlprieb](https://github.com/karlprieb))

- Regression: Directory sort users, fix null results, text for empty results ([#11224](https://github.com/RocketChat/Rocket.Chat/pull/11224))

- Regression: Directory user table infinite scroll doesn't working ([#11200](https://github.com/RocketChat/Rocket.Chat/pull/11200) by [@karlprieb](https://github.com/karlprieb))

- Regression: Fix directory table loading ([#11223](https://github.com/RocketChat/Rocket.Chat/pull/11223) by [@karlprieb](https://github.com/karlprieb))

- Regression: Fix latest and release-candidate docker images building ([#11215](https://github.com/RocketChat/Rocket.Chat/pull/11215))

- Regression: Prometheus was not being enabled in some cases ([#11249](https://github.com/RocketChat/Rocket.Chat/pull/11249))

- Regression: Sending message with a mention is not showing to sender ([#11211](https://github.com/RocketChat/Rocket.Chat/pull/11211))

- Regression: sidebar sorting was being wrong in some cases where the rooms records were returned before the subscriptions ([#11273](https://github.com/RocketChat/Rocket.Chat/pull/11273))

- Regression: Skip operations if no actions on livechat migration ([#11232](https://github.com/RocketChat/Rocket.Chat/pull/11232))

- Regression: sorting direct message by asc on favorites group ([#11090](https://github.com/RocketChat/Rocket.Chat/pull/11090))

- Remove wrong and not needed time unit ([#10807](https://github.com/RocketChat/Rocket.Chat/pull/10807) by [@cliffparnitzky](https://github.com/cliffparnitzky))

- Renaming username.username to username.value for clarity ([#10986](https://github.com/RocketChat/Rocket.Chat/pull/10986))

- Speed up the build time by removing JSON Minify from i18n package ([#11097](https://github.com/RocketChat/Rocket.Chat/pull/11097))

- Update Documentation: README.md ([#10207](https://github.com/RocketChat/Rocket.Chat/pull/10207) by [@rakhi2104](https://github.com/rakhi2104))

- Update issue templates ([#11070](https://github.com/RocketChat/Rocket.Chat/pull/11070))

- update meteor to 1.6.1 for sandstorm build ([#10131](https://github.com/RocketChat/Rocket.Chat/pull/10131) by [@peterlee0127](https://github.com/peterlee0127))

- Update Meteor to 1.6.1.3 ([#11247](https://github.com/RocketChat/Rocket.Chat/pull/11247))

- Update v126.js ([#11103](https://github.com/RocketChat/Rocket.Chat/pull/11103))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@JoseRenan](https://github.com/JoseRenan)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@brylie](https://github.com/brylie)
- [@c0dzilla](https://github.com/c0dzilla)
- [@cardoso](https://github.com/cardoso)
- [@cliffparnitzky](https://github.com/cliffparnitzky)
- [@cpitman](https://github.com/cpitman)
- [@filipealva](https://github.com/filipealva)
- [@gdelavald](https://github.com/gdelavald)
- [@haffla](https://github.com/haffla)
- [@jonnilundy](https://github.com/jonnilundy)
- [@kable-wilmoth](https://github.com/kable-wilmoth)
- [@karakayasemi](https://github.com/karakayasemi)
- [@karlprieb](https://github.com/karlprieb)
- [@kb0304](https://github.com/kb0304)
- [@kumarnitj](https://github.com/kumarnitj)
- [@lindoelio](https://github.com/lindoelio)
- [@mahdiyari](https://github.com/mahdiyari)
- [@mikaelmello](https://github.com/mikaelmello)
- [@myfonj](https://github.com/myfonj)
- [@noobbbbb](https://github.com/noobbbbb)
- [@nsuchy](https://github.com/nsuchy)
- [@ocdtrekkie](https://github.com/ocdtrekkie)
- [@peterlee0127](https://github.com/peterlee0127)
- [@pitamar](https://github.com/pitamar)
- [@pkgodara](https://github.com/pkgodara)
- [@rafaelks](https://github.com/rafaelks)
- [@rakhi2104](https://github.com/rakhi2104)
- [@rw4lll](https://github.com/rw4lll)
- [@saplla](https://github.com/saplla)
- [@stuartpb](https://github.com/stuartpb)
- [@taeven](https://github.com/taeven)
- [@thaiphv](https://github.com/thaiphv)
- [@timkinnane](https://github.com/timkinnane)
- [@vynmera](https://github.com/vynmera)

### 👩‍💻👨‍💻 Core Team 🤓

- [@alansikora](https://github.com/alansikora)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
- [@tassoevan](https://github.com/tassoevan)

# 0.65.2
`2018-06-16  ·  1 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

### 🐛 Bug fixes


- i18n - add semantic markup ([#9534](https://github.com/RocketChat/Rocket.Chat/pull/9534) by [@brylie](https://github.com/brylie))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.65.1 ([#10947](https://github.com/RocketChat/Rocket.Chat/pull/10947))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@brylie](https://github.com/brylie)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.65.1
`2018-05-30  ·  5 🐛  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Application crashing on startup when trying to log errors to `exceptions` channel ([#10934](https://github.com/RocketChat/Rocket.Chat/pull/10934))

- Image lazy load was breaking attachments ([#10904](https://github.com/RocketChat/Rocket.Chat/pull/10904))

- Incomplete email notification link ([#10928](https://github.com/RocketChat/Rocket.Chat/pull/10928))

- Leave room wasn't working as expected ([#10851](https://github.com/RocketChat/Rocket.Chat/pull/10851))

- Livechat not loading ([#10940](https://github.com/RocketChat/Rocket.Chat/pull/10940))

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.65.0
`2018-05-28  ·  17 🎉  ·  24 🐛  ·  30 🔍  ·  25 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

### 🎉 New features


- Add more options for Wordpress OAuth configuration ([#10724](https://github.com/RocketChat/Rocket.Chat/pull/10724) by [@Hudell](https://github.com/Hudell))

- Add permission `view-broadcast-member-list` ([#10753](https://github.com/RocketChat/Rocket.Chat/pull/10753) by [@cardoso](https://github.com/cardoso))

- Add REST API endpoint `users.getUsernameSuggestion` to get username suggestion ([#10702](https://github.com/RocketChat/Rocket.Chat/pull/10702) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add REST API endpoints `channels.counters`, `groups.counters and `im.counters` ([#9679](https://github.com/RocketChat/Rocket.Chat/pull/9679) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@xbolshe](https://github.com/xbolshe))

- Add REST API endpoints `channels.setCustomFields` and `groups.setCustomFields` ([#9733](https://github.com/RocketChat/Rocket.Chat/pull/9733) by [@xbolshe](https://github.com/xbolshe))

- Add REST endpoint `subscriptions.unread` to mark messages as unread ([#10778](https://github.com/RocketChat/Rocket.Chat/pull/10778) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add REST endpoints `channels.roles` & `groups.roles` ([#10607](https://github.com/RocketChat/Rocket.Chat/pull/10607) by [@cardoso](https://github.com/cardoso) & [@rafaelks](https://github.com/rafaelks))

- Implement a local password policy ([#9857](https://github.com/RocketChat/Rocket.Chat/pull/9857))

- Improvements to notifications logic ([#10686](https://github.com/RocketChat/Rocket.Chat/pull/10686))

- Lazy load image attachments ([#10608](https://github.com/RocketChat/Rocket.Chat/pull/10608) by [@karlprieb](https://github.com/karlprieb))

- Now is possible to access files using header authorization (`x-user-id` and `x-auth-token`) ([#10741](https://github.com/RocketChat/Rocket.Chat/pull/10741) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Options to enable/disable each Livechat registration form field ([#10584](https://github.com/RocketChat/Rocket.Chat/pull/10584))

- REST API endpoint `/me` now returns all the settings, including the default values ([#10662](https://github.com/RocketChat/Rocket.Chat/pull/10662) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST API endpoint `settings` now allow set colors and trigger actions ([#10488](https://github.com/RocketChat/Rocket.Chat/pull/10488) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@ThomasRoehl](https://github.com/ThomasRoehl))

- Return the result of the `/me` endpoint within the result of the `/login` endpoint ([#10677](https://github.com/RocketChat/Rocket.Chat/pull/10677) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Setup Wizard ([#10523](https://github.com/RocketChat/Rocket.Chat/pull/10523) by [@karlprieb](https://github.com/karlprieb))

- View pinned message's attachment ([#10214](https://github.com/RocketChat/Rocket.Chat/pull/10214) by [@c0dzilla](https://github.com/c0dzilla) & [@karlprieb](https://github.com/karlprieb))

### 🐛 Bug fixes


- Broadcast channels were showing reply button for deleted messages and generating wrong reply links some times ([#10835](https://github.com/RocketChat/Rocket.Chat/pull/10835))

- Cancel button wasn't working while uploading file ([#10715](https://github.com/RocketChat/Rocket.Chat/pull/10715) by [@Mr-Gryphon](https://github.com/Mr-Gryphon) & [@karlprieb](https://github.com/karlprieb))

- Channel owner was being set as muted when creating a read-only channel ([#10665](https://github.com/RocketChat/Rocket.Chat/pull/10665) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Enabling `Collapse Embedded Media by Default` was hiding replies and quotes ([#10427](https://github.com/RocketChat/Rocket.Chat/pull/10427) by [@c0dzilla](https://github.com/c0dzilla))

- Horizontally align items in preview message ([#10883](https://github.com/RocketChat/Rocket.Chat/pull/10883) by [@gdelavald](https://github.com/gdelavald))

- Improve desktop notification formatting ([#10445](https://github.com/RocketChat/Rocket.Chat/pull/10445) by [@Sameesunkaria](https://github.com/Sameesunkaria))

- Internal Error when requesting user data download ([#10837](https://github.com/RocketChat/Rocket.Chat/pull/10837) by [@Hudell](https://github.com/Hudell))

- Layout badge cutting on unread messages for long names ([#10846](https://github.com/RocketChat/Rocket.Chat/pull/10846) by [@kos4live](https://github.com/kos4live))

- Livechat managers were not being able to send messages in some cases ([#10663](https://github.com/RocketChat/Rocket.Chat/pull/10663))

- Livechat settings not appearing correctly ([#10612](https://github.com/RocketChat/Rocket.Chat/pull/10612))

- Message box emoji icon was flickering when typing a text ([#10678](https://github.com/RocketChat/Rocket.Chat/pull/10678) by [@gdelavald](https://github.com/gdelavald))

- Missing attachment description when Rocket.Chat Apps were enabled ([#10705](https://github.com/RocketChat/Rocket.Chat/pull/10705) by [@Hudell](https://github.com/Hudell))

- Missing option to disable/enable System Messages ([#10704](https://github.com/RocketChat/Rocket.Chat/pull/10704))

- Missing pagination fields in the response of REST /directory endpoint ([#10840](https://github.com/RocketChat/Rocket.Chat/pull/10840) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Not escaping special chars on mentions ([#10793](https://github.com/RocketChat/Rocket.Chat/pull/10793) by [@erhan-](https://github.com/erhan-))

- Private settings were not being cleared from client cache in some cases ([#10625](https://github.com/RocketChat/Rocket.Chat/pull/10625) by [@Hudell](https://github.com/Hudell))

- Regression: Empty content on announcement modal ([#10733](https://github.com/RocketChat/Rocket.Chat/pull/10733) by [@gdelavald](https://github.com/gdelavald))

- Remove outdated translations of Internal Hubot's description of Scripts to Load that were pointing to a non existent address ([#10448](https://github.com/RocketChat/Rocket.Chat/pull/10448) by [@Hudell](https://github.com/Hudell))

- SAML wasn't working correctly when running multiple instances ([#10681](https://github.com/RocketChat/Rocket.Chat/pull/10681) by [@Hudell](https://github.com/Hudell))

- Send a message when muted returns inconsistent result in chat.sendMessage ([#10720](https://github.com/RocketChat/Rocket.Chat/pull/10720) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Slack-Bridge bug when migrating to 0.64.1 ([#10875](https://github.com/RocketChat/Rocket.Chat/pull/10875))

- The first users was not set as admin some times ([#10878](https://github.com/RocketChat/Rocket.Chat/pull/10878))

- UI was not disabling the actions when users has had no permissions to create channels or add users to rooms ([#10564](https://github.com/RocketChat/Rocket.Chat/pull/10564) by [@cfunkles](https://github.com/cfunkles) & [@chuckAtCataworx](https://github.com/chuckAtCataworx))

- User's preference `Unread on Top` wasn't working for LiveChat rooms ([#10734](https://github.com/RocketChat/Rocket.Chat/pull/10734))

<details>
<summary>🔍 Minor changes</summary>


- Add `npm run postinstall` into example build script ([#10524](https://github.com/RocketChat/Rocket.Chat/pull/10524) by [@peccu](https://github.com/peccu))

- Add badge back to push notifications ([#10779](https://github.com/RocketChat/Rocket.Chat/pull/10779))

- Add setting and expose prometheus on port 9100 ([#10766](https://github.com/RocketChat/Rocket.Chat/pull/10766))

- Apps: Command previews are clickable & Apps Framework is controlled via a setting ([#10853](https://github.com/RocketChat/Rocket.Chat/pull/10853))

- Apps: Command Previews, Message and Room Removal Events ([#10822](https://github.com/RocketChat/Rocket.Chat/pull/10822))

- Better metric for notifications ([#10786](https://github.com/RocketChat/Rocket.Chat/pull/10786))

- Correct links in README file ([#10674](https://github.com/RocketChat/Rocket.Chat/pull/10674) by [@winterstefan](https://github.com/winterstefan))

- Develop sync ([#10815](https://github.com/RocketChat/Rocket.Chat/pull/10815) by [@nsuchy](https://github.com/nsuchy) & [@rafaelks](https://github.com/rafaelks))

- Fix: Clarify the wording of the release issue template ([#10520](https://github.com/RocketChat/Rocket.Chat/pull/10520))

- Fix: Manage apps layout was a bit confuse ([#10882](https://github.com/RocketChat/Rocket.Chat/pull/10882) by [@gdelavald](https://github.com/gdelavald))

- Fix: Regression in REST API endpoint `/me`  ([#10833](https://github.com/RocketChat/Rocket.Chat/pull/10833) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: Regression Lazyload fix shuffle avatars ([#10887](https://github.com/RocketChat/Rocket.Chat/pull/10887))

- Fix: Regression on users avatar in admin pages ([#10836](https://github.com/RocketChat/Rocket.Chat/pull/10836))

- Fix: typo on error message for push token API ([#10857](https://github.com/RocketChat/Rocket.Chat/pull/10857) by [@rafaelks](https://github.com/rafaelks))

- Improvement to push notifications on direct messages ([#10788](https://github.com/RocketChat/Rocket.Chat/pull/10788))

- LingoHub based on develop ([#10691](https://github.com/RocketChat/Rocket.Chat/pull/10691))

- LingoHub based on develop ([#10886](https://github.com/RocketChat/Rocket.Chat/pull/10886))

- Major dependencies update ([#10661](https://github.com/RocketChat/Rocket.Chat/pull/10661))

- More improvements on send notifications logic ([#10736](https://github.com/RocketChat/Rocket.Chat/pull/10736))

- Prevent setup wizard redirects ([#10811](https://github.com/RocketChat/Rocket.Chat/pull/10811))

- Prometheus: Add metric to track hooks time ([#10798](https://github.com/RocketChat/Rocket.Chat/pull/10798))

- Prometheus: Fix notification metric ([#10803](https://github.com/RocketChat/Rocket.Chat/pull/10803))

- Prometheus: Improve metric names ([#10789](https://github.com/RocketChat/Rocket.Chat/pull/10789))

- Regression: Autorun of wizard was not destroyed after completion ([#10802](https://github.com/RocketChat/Rocket.Chat/pull/10802))

- Regression: Fix email notification preference not showing correct selected value ([#10847](https://github.com/RocketChat/Rocket.Chat/pull/10847))

- Regression: Fix notifications for direct messages ([#10760](https://github.com/RocketChat/Rocket.Chat/pull/10760))

- Regression: Fix wrong wizard field name ([#10804](https://github.com/RocketChat/Rocket.Chat/pull/10804))

- Regression: Make settings `Site_Name` and `Language` public again ([#10848](https://github.com/RocketChat/Rocket.Chat/pull/10848))

- Release 0.65.0 ([#10893](https://github.com/RocketChat/Rocket.Chat/pull/10893) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@Sameesunkaria](https://github.com/Sameesunkaria) & [@cardoso](https://github.com/cardoso) & [@erhan-](https://github.com/erhan-) & [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb) & [@peccu](https://github.com/peccu) & [@winterstefan](https://github.com/winterstefan))

- Wizard improvements ([#10776](https://github.com/RocketChat/Rocket.Chat/pull/10776))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Mr-Gryphon](https://github.com/Mr-Gryphon)
- [@Sameesunkaria](https://github.com/Sameesunkaria)
- [@ThomasRoehl](https://github.com/ThomasRoehl)
- [@c0dzilla](https://github.com/c0dzilla)
- [@cardoso](https://github.com/cardoso)
- [@cfunkles](https://github.com/cfunkles)
- [@chuckAtCataworx](https://github.com/chuckAtCataworx)
- [@erhan-](https://github.com/erhan-)
- [@gdelavald](https://github.com/gdelavald)
- [@karlprieb](https://github.com/karlprieb)
- [@kos4live](https://github.com/kos4live)
- [@nsuchy](https://github.com/nsuchy)
- [@peccu](https://github.com/peccu)
- [@rafaelks](https://github.com/rafaelks)
- [@winterstefan](https://github.com/winterstefan)
- [@xbolshe](https://github.com/xbolshe)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.64.2
`2018-05-18  ·  1 🔍  ·  12 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

<details>
<summary>🔍 Minor changes</summary>


- Release 0.64.2 ([#10812](https://github.com/RocketChat/Rocket.Chat/pull/10812) by [@Hudell](https://github.com/Hudell) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@Sameesunkaria](https://github.com/Sameesunkaria) & [@cardoso](https://github.com/cardoso) & [@erhan-](https://github.com/erhan-) & [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb) & [@peccu](https://github.com/peccu) & [@winterstefan](https://github.com/winterstefan))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Sameesunkaria](https://github.com/Sameesunkaria)
- [@cardoso](https://github.com/cardoso)
- [@erhan-](https://github.com/erhan-)
- [@gdelavald](https://github.com/gdelavald)
- [@karlprieb](https://github.com/karlprieb)
- [@peccu](https://github.com/peccu)
- [@winterstefan](https://github.com/winterstefan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.64.1
`2018-05-03  ·  1 🎉  ·  2 🐛  ·  4 🔍  ·  5 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

### 🎉 New features


- Store the last sent message to show bellow the room's name by default ([#10597](https://github.com/RocketChat/Rocket.Chat/pull/10597))

### 🐛 Bug fixes


- E-mails were hidden some information ([#10615](https://github.com/RocketChat/Rocket.Chat/pull/10615))

- Regression on 0.64.0 was freezing the application when posting some URLs ([#10627](https://github.com/RocketChat/Rocket.Chat/pull/10627))

<details>
<summary>🔍 Minor changes</summary>


- Dependencies update ([#10648](https://github.com/RocketChat/Rocket.Chat/pull/10648))

- Regression: Updating an App on multi-instance servers wasn't working ([#10611](https://github.com/RocketChat/Rocket.Chat/pull/10611))

- Release 0.64.1 ([#10660](https://github.com/RocketChat/Rocket.Chat/pull/10660) by [@saplla](https://github.com/saplla))

- Support passing extra connection options to the Mongo driver ([#10529](https://github.com/RocketChat/Rocket.Chat/pull/10529) by [@saplla](https://github.com/saplla))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@saplla](https://github.com/saplla)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@graywolf336](https://github.com/graywolf336)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.64.0
`2018-04-28  ·  2 ️️️⚠️  ·  18 🎉  ·  44 🐛  ·  31 🔍  ·  30 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

### ⚠️ BREAKING CHANGES


- The property "settings" is no longer available to regular users via rest api ([#10411](https://github.com/RocketChat/Rocket.Chat/pull/10411))

- Validate incoming message schema ([#9922](https://github.com/RocketChat/Rocket.Chat/pull/9922) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

### 🎉 New features


- Add information regarding Zapier and Bots to the integrations page ([#10574](https://github.com/RocketChat/Rocket.Chat/pull/10574))

- Add internal API to handle room announcements ([#10396](https://github.com/RocketChat/Rocket.Chat/pull/10396) by [@gdelavald](https://github.com/gdelavald))

- Add message preview when quoting another message ([#10437](https://github.com/RocketChat/Rocket.Chat/pull/10437) by [@gdelavald](https://github.com/gdelavald))

- Automatically trigger Redhat registry build when tagging new release ([#10414](https://github.com/RocketChat/Rocket.Chat/pull/10414))

- Body of the payload on an incoming webhook is included on the request object ([#10259](https://github.com/RocketChat/Rocket.Chat/pull/10259) by [@Hudell](https://github.com/Hudell))

- Broadcast Channels ([#9950](https://github.com/RocketChat/Rocket.Chat/pull/9950))

- GDPR - Right to access and Data Portability ([#9906](https://github.com/RocketChat/Rocket.Chat/pull/9906) by [@Hudell](https://github.com/Hudell))

- Livechat setting to customize ended conversation message ([#10108](https://github.com/RocketChat/Rocket.Chat/pull/10108))

- Option to ignore users on channels ([#10517](https://github.com/RocketChat/Rocket.Chat/pull/10517) by [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb))

- Option to mute group mentions (@all and @here) ([#10502](https://github.com/RocketChat/Rocket.Chat/pull/10502) by [@Hudell](https://github.com/Hudell))

- Prevent the browser to autocomplete some setting fields ([#10439](https://github.com/RocketChat/Rocket.Chat/pull/10439) by [@gdelavald](https://github.com/gdelavald))

- REST API endpoint `/directory` ([#10442](https://github.com/RocketChat/Rocket.Chat/pull/10442) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST API endpoint `rooms.favorite` to favorite and unfavorite rooms ([#10342](https://github.com/RocketChat/Rocket.Chat/pull/10342) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST endpoint to recover forgotten password ([#10371](https://github.com/RocketChat/Rocket.Chat/pull/10371) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST endpoint to report messages ([#10354](https://github.com/RocketChat/Rocket.Chat/pull/10354) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Search Provider Framework ([#10110](https://github.com/RocketChat/Rocket.Chat/pull/10110) by [@tkurz](https://github.com/tkurz))

- Shows user's real name on autocomplete popup ([#10444](https://github.com/RocketChat/Rocket.Chat/pull/10444) by [@gdelavald](https://github.com/gdelavald))

- Twilio MMS support for LiveChat integration ([#7964](https://github.com/RocketChat/Rocket.Chat/pull/7964) by [@t3hchipmunk](https://github.com/t3hchipmunk))

### 🐛 Bug fixes


- "Highlight Words" wasn't working with more than one word ([#10083](https://github.com/RocketChat/Rocket.Chat/pull/10083) by [@gdelavald](https://github.com/gdelavald) & [@nemaniarjun](https://github.com/nemaniarjun))

- "Idle Time Limit" using milliseconds instead of seconds ([#9824](https://github.com/RocketChat/Rocket.Chat/pull/9824) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Add user object to responses in /*.files Rest endpoints ([#10480](https://github.com/RocketChat/Rocket.Chat/pull/10480) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Autocomplete list when inviting a user was partial hidden ([#10409](https://github.com/RocketChat/Rocket.Chat/pull/10409) by [@karlprieb](https://github.com/karlprieb))

- Button on user info contextual bar scrolling with the content ([#10358](https://github.com/RocketChat/Rocket.Chat/pull/10358) by [@karlprieb](https://github.com/karlprieb) & [@okaybroda](https://github.com/okaybroda))

- Button to delete rooms by the owners wasn't appearing ([#10438](https://github.com/RocketChat/Rocket.Chat/pull/10438) by [@karlprieb](https://github.com/karlprieb))

- Custom fields was misaligned in registration form ([#10463](https://github.com/RocketChat/Rocket.Chat/pull/10463) by [@dschuan](https://github.com/dschuan))

- Directory sort and column sizes were wrong ([#10403](https://github.com/RocketChat/Rocket.Chat/pull/10403) by [@karlprieb](https://github.com/karlprieb))

- Dropdown elements were using old styles ([#10482](https://github.com/RocketChat/Rocket.Chat/pull/10482) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Empty panel after changing a user's username ([#10404](https://github.com/RocketChat/Rocket.Chat/pull/10404) by [@Hudell](https://github.com/Hudell))

- Error messages weren't been displayed when email verification fails ([#10446](https://github.com/RocketChat/Rocket.Chat/pull/10446) by [@Hudell](https://github.com/Hudell) & [@karlprieb](https://github.com/karlprieb))

- GitLab authentication scope was too open, reduced to read only access ([#10225](https://github.com/RocketChat/Rocket.Chat/pull/10225) by [@rafaelks](https://github.com/rafaelks))

- Incoming integrations being able to trigger an empty message with a GET ([#9576](https://github.com/RocketChat/Rocket.Chat/pull/9576))

- Integrations with room data not having the usernames filled in ([#10576](https://github.com/RocketChat/Rocket.Chat/pull/10576))

- Links being embedded inside of blockquotes ([#10496](https://github.com/RocketChat/Rocket.Chat/pull/10496) by [@gdelavald](https://github.com/gdelavald))

- Livechat desktop notifications not being displayed ([#10221](https://github.com/RocketChat/Rocket.Chat/pull/10221))

- Livechat translation files being ignored ([#10369](https://github.com/RocketChat/Rocket.Chat/pull/10369))

- Member list search with no results ([#10599](https://github.com/RocketChat/Rocket.Chat/pull/10599))

- Message view mode setting was missing at user's preferences  ([#10395](https://github.com/RocketChat/Rocket.Chat/pull/10395) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii) & [@karlprieb](https://github.com/karlprieb))

- Messages was grouping wrong some times when server is slow ([#10472](https://github.com/RocketChat/Rocket.Chat/pull/10472) by [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb))

- Missing "Administration" menu for user with manage-emoji permission ([#10171](https://github.com/RocketChat/Rocket.Chat/pull/10171) by [@c0dzilla](https://github.com/c0dzilla) & [@karlprieb](https://github.com/karlprieb))

- Missing "Administration" menu for users with some administration permissions ([#10551](https://github.com/RocketChat/Rocket.Chat/pull/10551) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Missing i18n translation key for "Unread" ([#10387](https://github.com/RocketChat/Rocket.Chat/pull/10387) by [@Hudell](https://github.com/Hudell))

- Missing page "not found" ([#6673](https://github.com/RocketChat/Rocket.Chat/pull/6673) by [@Prakharsvnit](https://github.com/Prakharsvnit) & [@karlprieb](https://github.com/karlprieb))

- Missing RocketApps input types ([#10394](https://github.com/RocketChat/Rocket.Chat/pull/10394) by [@karlprieb](https://github.com/karlprieb))

- Missing user data on files uploaded through the API ([#10473](https://github.com/RocketChat/Rocket.Chat/pull/10473) by [@Hudell](https://github.com/Hudell))

- Owner unable to delete channel or group from APIs ([#9729](https://github.com/RocketChat/Rocket.Chat/pull/9729) by [@c0dzilla](https://github.com/c0dzilla))

- Profile image was not being shown in user's directory search ([#10399](https://github.com/RocketChat/Rocket.Chat/pull/10399) by [@karlprieb](https://github.com/karlprieb) & [@lunaticmonk](https://github.com/lunaticmonk))

- Remove a user from the user's list when creating a new channel removes the wrong user ([#10423](https://github.com/RocketChat/Rocket.Chat/pull/10423) by [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb))

- Rename method to clean history of messages ([#10498](https://github.com/RocketChat/Rocket.Chat/pull/10498) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Renaming agent's username within Livechat's department ([#10344](https://github.com/RocketChat/Rocket.Chat/pull/10344))

- REST API OAuth services endpoint were missing fields and flag to indicate custom services ([#10299](https://github.com/RocketChat/Rocket.Chat/pull/10299) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- REST spotlight API wasn't allowing searches with # and @ ([#10410](https://github.com/RocketChat/Rocket.Chat/pull/10410) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Room's name was cutting instead of having ellipses on sidebar ([#10430](https://github.com/RocketChat/Rocket.Chat/pull/10430))

- Russian translation of "False" ([#10418](https://github.com/RocketChat/Rocket.Chat/pull/10418) by [@strangerintheq](https://github.com/strangerintheq))

- Snaps installations are breaking on avatar requests ([#10390](https://github.com/RocketChat/Rocket.Chat/pull/10390))

- Stop Firefox announcement overflowing viewport ([#10503](https://github.com/RocketChat/Rocket.Chat/pull/10503) by [@brendangadd](https://github.com/brendangadd))

- Switch buttons were cutting in RTL mode ([#10558](https://github.com/RocketChat/Rocket.Chat/pull/10558))

- The 'channel.messages' REST API Endpoint error ([#10485](https://github.com/RocketChat/Rocket.Chat/pull/10485) by [@rafaelks](https://github.com/rafaelks))

- Unique identifier file not really being unique ([#10341](https://github.com/RocketChat/Rocket.Chat/pull/10341) by [@abernix](https://github.com/abernix))

- Updated OpenShift Template to take an Image as a Param ([#9946](https://github.com/RocketChat/Rocket.Chat/pull/9946) by [@christianh814](https://github.com/christianh814))

- Wordpress oAuth authentication wasn't behaving correctly ([#10550](https://github.com/RocketChat/Rocket.Chat/pull/10550) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Wrong column positions in the directory search for users ([#10454](https://github.com/RocketChat/Rocket.Chat/pull/10454) by [@karlprieb](https://github.com/karlprieb) & [@lunaticmonk](https://github.com/lunaticmonk))

- Wrong positioning of popover when using RTL languages ([#10428](https://github.com/RocketChat/Rocket.Chat/pull/10428) by [@karlprieb](https://github.com/karlprieb))

<details>
<summary>🔍 Minor changes</summary>


- [OTHER] Develop sync ([#10487](https://github.com/RocketChat/Rocket.Chat/pull/10487))

- [OTHER] More Listeners for Apps & Utilize Promises inside Apps ([#10335](https://github.com/RocketChat/Rocket.Chat/pull/10335))

- [OTHER] Removed the developer warning on the rest api ([#10441](https://github.com/RocketChat/Rocket.Chat/pull/10441))

- Add some missing translations ([#10435](https://github.com/RocketChat/Rocket.Chat/pull/10435) by [@gdelavald](https://github.com/gdelavald))

- Change Docker-Compose to use mmapv1 storage engine for mongo ([#10336](https://github.com/RocketChat/Rocket.Chat/pull/10336))

- Deps update ([#10549](https://github.com/RocketChat/Rocket.Chat/pull/10549))

- Develop sync ([#10505](https://github.com/RocketChat/Rocket.Chat/pull/10505) by [@nsuchy](https://github.com/nsuchy) & [@rafaelks](https://github.com/rafaelks))

- Development: Add Visual Studio Code debugging configuration ([#10586](https://github.com/RocketChat/Rocket.Chat/pull/10586))

- Fix and improve vietnamese translation ([#10397](https://github.com/RocketChat/Rocket.Chat/pull/10397) by [@TDiNguyen](https://github.com/TDiNguyen) & [@tttt-conan](https://github.com/tttt-conan))

- Fix: Remove "secret" from REST endpoint /settings.oauth response ([#10513](https://github.com/RocketChat/Rocket.Chat/pull/10513) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Included missing lib for migrations ([#10532](https://github.com/RocketChat/Rocket.Chat/pull/10532) by [@Hudell](https://github.com/Hudell))

- LingoHub based on develop ([#10545](https://github.com/RocketChat/Rocket.Chat/pull/10545))

- Master into Develop Branch Sync ([#10376](https://github.com/RocketChat/Rocket.Chat/pull/10376))

- New issue template for *Release Process* ([#10234](https://github.com/RocketChat/Rocket.Chat/pull/10234))

- Regression: /api/v1/settings.oauth not returning clientId for Twitter ([#10560](https://github.com/RocketChat/Rocket.Chat/pull/10560) by [@cardoso](https://github.com/cardoso))

- Regression: /api/v1/settings.oauth not sending needed info for SAML & CAS ([#10596](https://github.com/RocketChat/Rocket.Chat/pull/10596) by [@cardoso](https://github.com/cardoso))

- Regression: Apps and Livechats not getting along well with each other ([#10598](https://github.com/RocketChat/Rocket.Chat/pull/10598))

- Regression: Attachments and fields incorrectly failing on validation ([#10573](https://github.com/RocketChat/Rocket.Chat/pull/10573))

- Regression: Fix announcement bar being displayed without content ([#10554](https://github.com/RocketChat/Rocket.Chat/pull/10554) by [@gdelavald](https://github.com/gdelavald))

- Regression: Inconsistent response of settings.oauth endpoint ([#10553](https://github.com/RocketChat/Rocket.Chat/pull/10553) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Regression: Remove added mentions on quote/reply ([#10571](https://github.com/RocketChat/Rocket.Chat/pull/10571) by [@gdelavald](https://github.com/gdelavald))

- Regression: Revert announcement structure ([#10544](https://github.com/RocketChat/Rocket.Chat/pull/10544) by [@gdelavald](https://github.com/gdelavald))

- Regression: Rocket.Chat App author link opens in same window ([#10575](https://github.com/RocketChat/Rocket.Chat/pull/10575) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Regression: Rooms and Apps weren't playing nice with each other ([#10559](https://github.com/RocketChat/Rocket.Chat/pull/10559))

- Regression: Upload was not working ([#10543](https://github.com/RocketChat/Rocket.Chat/pull/10543))

- Regression: Various search provider fixes ([#10591](https://github.com/RocketChat/Rocket.Chat/pull/10591) by [@tkurz](https://github.com/tkurz))

- Regression: Webhooks breaking due to restricted test ([#10555](https://github.com/RocketChat/Rocket.Chat/pull/10555))

- Release 0.64.0 ([#10613](https://github.com/RocketChat/Rocket.Chat/pull/10613) by [@TwizzyDizzy](https://github.com/TwizzyDizzy) & [@christianh814](https://github.com/christianh814) & [@gdelavald](https://github.com/gdelavald) & [@tttt-conan](https://github.com/tttt-conan))

- Remove @core team mention from Pull Request template ([#10384](https://github.com/RocketChat/Rocket.Chat/pull/10384))

- Update allowed labels for bot ([#10360](https://github.com/RocketChat/Rocket.Chat/pull/10360) by [@TwizzyDizzy](https://github.com/TwizzyDizzy))

- Use Node 8.9 for CI build ([#10405](https://github.com/RocketChat/Rocket.Chat/pull/10405))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@Prakharsvnit](https://github.com/Prakharsvnit)
- [@TDiNguyen](https://github.com/TDiNguyen)
- [@TwizzyDizzy](https://github.com/TwizzyDizzy)
- [@abernix](https://github.com/abernix)
- [@brendangadd](https://github.com/brendangadd)
- [@c0dzilla](https://github.com/c0dzilla)
- [@cardoso](https://github.com/cardoso)
- [@christianh814](https://github.com/christianh814)
- [@dschuan](https://github.com/dschuan)
- [@gdelavald](https://github.com/gdelavald)
- [@kaiiiiiiiii](https://github.com/kaiiiiiiiii)
- [@karlprieb](https://github.com/karlprieb)
- [@lunaticmonk](https://github.com/lunaticmonk)
- [@nemaniarjun](https://github.com/nemaniarjun)
- [@nsuchy](https://github.com/nsuchy)
- [@okaybroda](https://github.com/okaybroda)
- [@rafaelks](https://github.com/rafaelks)
- [@strangerintheq](https://github.com/strangerintheq)
- [@t3hchipmunk](https://github.com/t3hchipmunk)
- [@tkurz](https://github.com/tkurz)
- [@tttt-conan](https://github.com/tttt-conan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.63.3
`2018-04-18  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

<details>
<summary>🔍 Minor changes</summary>


- Release 0.63.3 ([#10504](https://github.com/RocketChat/Rocket.Chat/pull/10504) by [@rafaelks](https://github.com/rafaelks))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@rafaelks](https://github.com/rafaelks)

### 👩‍💻👨‍💻 Core Team 🤓

- [@graywolf336](https://github.com/graywolf336)

# 0.63.2
`2018-04-17  ·  2 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

<details>
<summary>🔍 Minor changes</summary>


- add redhat dockerfile to master ([#10408](https://github.com/RocketChat/Rocket.Chat/pull/10408))

- Release 0.63.2 ([#10476](https://github.com/RocketChat/Rocket.Chat/pull/10476))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@graywolf336](https://github.com/graywolf336)

# 0.63.1
`2018-04-07  ·  1 🔍  ·  7 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

<details>
<summary>🔍 Minor changes</summary>


- Release 0.63.1 ([#10374](https://github.com/RocketChat/Rocket.Chat/pull/10374) by [@TechyPeople](https://github.com/TechyPeople) & [@kaiiiiiiiii](https://github.com/kaiiiiiiiii) & [@tttt-conan](https://github.com/tttt-conan))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@TechyPeople](https://github.com/TechyPeople)
- [@kaiiiiiiiii](https://github.com/kaiiiiiiiii)
- [@tttt-conan](https://github.com/tttt-conan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@graywolf336](https://github.com/graywolf336)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.63.0
`2018-04-04  ·  1 ️️️⚠️  ·  18 🎉  ·  36 🐛  ·  20 🔍  ·  25 👩‍💻👨‍💻`

### Engine versions
- Node: `8.11.1`
- NPM: `5.6.0`

### ⚠️ BREAKING CHANGES


- Removed Private History Route ([#10103](https://github.com/RocketChat/Rocket.Chat/pull/10103) by [@Hudell](https://github.com/Hudell))

### 🎉 New features


- Add leave public channel & leave private channel permissions ([#9584](https://github.com/RocketChat/Rocket.Chat/pull/9584) by [@kb0304](https://github.com/kb0304))

- Add option to login via REST using Facebook and Twitter tokens ([#9816](https://github.com/RocketChat/Rocket.Chat/pull/9816) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Add REST endpoint to get the list of custom emojis ([#9629](https://github.com/RocketChat/Rocket.Chat/pull/9629) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added endpoint to get the list of available oauth services ([#10144](https://github.com/RocketChat/Rocket.Chat/pull/10144) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added endpoint to retrieve mentions of a channel ([#10105](https://github.com/RocketChat/Rocket.Chat/pull/10105) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Added GET/POST channels.notifications ([#10128](https://github.com/RocketChat/Rocket.Chat/pull/10128) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Announcement bar color wasn't using color from theming variables ([#9367](https://github.com/RocketChat/Rocket.Chat/pull/9367) by [@cyclops24](https://github.com/cyclops24) & [@karlprieb](https://github.com/karlprieb))

- Audio recording as mp3 and better ui for recording ([#9726](https://github.com/RocketChat/Rocket.Chat/pull/9726) by [@kb0304](https://github.com/kb0304))

- Endpoint to retrieve message read receipts ([#9907](https://github.com/RocketChat/Rocket.Chat/pull/9907) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- GDPR Right to be forgotten/erased ([#9947](https://github.com/RocketChat/Rocket.Chat/pull/9947) by [@Hudell](https://github.com/Hudell))

- Improve history generation ([#10319](https://github.com/RocketChat/Rocket.Chat/pull/10319))

- Interface to install and manage RocketChat Apps (alpha) ([#10246](https://github.com/RocketChat/Rocket.Chat/pull/10246))

- Livechat messages rest APIs ([#10054](https://github.com/RocketChat/Rocket.Chat/pull/10054) by [@hmagarotto](https://github.com/hmagarotto))

- Livechat webhook request on message ([#9870](https://github.com/RocketChat/Rocket.Chat/pull/9870) by [@hmagarotto](https://github.com/hmagarotto))

- Reply preview ([#10086](https://github.com/RocketChat/Rocket.Chat/pull/10086) by [@ubarsaiyan](https://github.com/ubarsaiyan))

- REST API method to set room's announcement (channels.setAnnouncement) ([#9742](https://github.com/RocketChat/Rocket.Chat/pull/9742) by [@TopHattedCat](https://github.com/TopHattedCat))

- Setting to configure max delta for 2fa ([#9732](https://github.com/RocketChat/Rocket.Chat/pull/9732) by [@Hudell](https://github.com/Hudell))

- Support for agent's phone field ([#10123](https://github.com/RocketChat/Rocket.Chat/pull/10123))

### 🐛 Bug fixes


- "View All Members" button inside channel's "User Info" is over sized ([#10012](https://github.com/RocketChat/Rocket.Chat/pull/10012) by [@karlprieb](https://github.com/karlprieb))

- /me REST endpoint was missing user roles and preferences ([#10240](https://github.com/RocketChat/Rocket.Chat/pull/10240) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Able to react with invalid emoji ([#8667](https://github.com/RocketChat/Rocket.Chat/pull/8667) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@mutdmour](https://github.com/mutdmour))

- Apostrophe-containing URL misparsed ([#9739](https://github.com/RocketChat/Rocket.Chat/pull/9739) by [@lunaticmonk](https://github.com/lunaticmonk))

- Apostrophe-containing URL misparsed" ([#10242](https://github.com/RocketChat/Rocket.Chat/pull/10242))

- Audio Message UI fixes ([#10303](https://github.com/RocketChat/Rocket.Chat/pull/10303) by [@kb0304](https://github.com/kb0304))

- Avatar input was accepting not supported image types ([#10011](https://github.com/RocketChat/Rocket.Chat/pull/10011) by [@karlprieb](https://github.com/karlprieb))

- Broken video call accept dialog ([#9872](https://github.com/RocketChat/Rocket.Chat/pull/9872) by [@ramrami](https://github.com/ramrami))

- Browser was auto-filling values when editing another user profile ([#9932](https://github.com/RocketChat/Rocket.Chat/pull/9932) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Cannot answer to a livechat as a manager if agent has not answered yet ([#10082](https://github.com/RocketChat/Rocket.Chat/pull/10082) by [@kb0304](https://github.com/kb0304))

- Download links was duplicating Sub Paths ([#10029](https://github.com/RocketChat/Rocket.Chat/pull/10029))

- Dynamic CSS script isn't working on older browsers ([#10152](https://github.com/RocketChat/Rocket.Chat/pull/10152) by [@karlprieb](https://github.com/karlprieb))

- Extended view mode on sidebar ([#10160](https://github.com/RocketChat/Rocket.Chat/pull/10160) by [@karlprieb](https://github.com/karlprieb))

- File had redirect delay when using external storage services and no option to proxy only avatars ([#10272](https://github.com/RocketChat/Rocket.Chat/pull/10272))

- Incoming Webhooks were missing the raw content ([#10258](https://github.com/RocketChat/Rocket.Chat/pull/10258) by [@Hudell](https://github.com/Hudell))

- Initial loading feedback was missing ([#10028](https://github.com/RocketChat/Rocket.Chat/pull/10028) by [@karlprieb](https://github.com/karlprieb))

- Inline code following a url leads to autolinking of code with url ([#10163](https://github.com/RocketChat/Rocket.Chat/pull/10163) by [@c0dzilla](https://github.com/c0dzilla))

- Message editing is crashing the server when read receipts are enabled ([#10061](https://github.com/RocketChat/Rocket.Chat/pull/10061))

- Missing pt-BR translations ([#10262](https://github.com/RocketChat/Rocket.Chat/pull/10262))

- Missing sidebar default options on admin ([#10016](https://github.com/RocketChat/Rocket.Chat/pull/10016) by [@karlprieb](https://github.com/karlprieb))

- Missing Translation Key on Reactions ([#10270](https://github.com/RocketChat/Rocket.Chat/pull/10270) by [@bernardoetrevisan](https://github.com/bernardoetrevisan))

- Name of files in file upload list cuts down at bottom due to overflow ([#9672](https://github.com/RocketChat/Rocket.Chat/pull/9672) by [@lunaticmonk](https://github.com/lunaticmonk))

- Nextcloud as custom oauth provider wasn't mapping data correctly ([#10090](https://github.com/RocketChat/Rocket.Chat/pull/10090) by [@pierreozoux](https://github.com/pierreozoux))

- No pattern for user's status text capitalization ([#9783](https://github.com/RocketChat/Rocket.Chat/pull/9783) by [@lunaticmonk](https://github.com/lunaticmonk))

- Popover divs don't scroll if they overflow the viewport ([#9860](https://github.com/RocketChat/Rocket.Chat/pull/9860) by [@Joe-mcgee](https://github.com/Joe-mcgee))

- Reactions not working on mobile ([#10104](https://github.com/RocketChat/Rocket.Chat/pull/10104))

- REST API: Can't list all public channels when user has permission `view-joined-room` ([#10009](https://github.com/RocketChat/Rocket.Chat/pull/10009) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Slack Import reports `invalid import file type` due to a call to BSON.native() which is now doesn't exist ([#10071](https://github.com/RocketChat/Rocket.Chat/pull/10071) by [@trongthanh](https://github.com/trongthanh))

- Unable to mention after newline in message ([#10078](https://github.com/RocketChat/Rocket.Chat/pull/10078) by [@c0dzilla](https://github.com/c0dzilla))

- Update preferences of users with settings: null was crashing the server ([#10076](https://github.com/RocketChat/Rocket.Chat/pull/10076))

- User preferences can't be saved when roles are hidden in admin settings ([#10051](https://github.com/RocketChat/Rocket.Chat/pull/10051) by [@Hudell](https://github.com/Hudell))

- User status missing on user info ([#9866](https://github.com/RocketChat/Rocket.Chat/pull/9866) by [@lunaticmonk](https://github.com/lunaticmonk))

- user status on sidenav ([#10222](https://github.com/RocketChat/Rocket.Chat/pull/10222))

- Verified property of user is always set to false if not supplied ([#9719](https://github.com/RocketChat/Rocket.Chat/pull/9719) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Wrong pagination information on /api/v1/channels.members ([#10224](https://github.com/RocketChat/Rocket.Chat/pull/10224) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Wrong switch button border color ([#10081](https://github.com/RocketChat/Rocket.Chat/pull/10081) by [@kb0304](https://github.com/kb0304))

<details>
<summary>🔍 Minor changes</summary>


- [OTHER] Reactivate all tests ([#10036](https://github.com/RocketChat/Rocket.Chat/pull/10036))

- [OTHER] Reactivate API tests ([#9844](https://github.com/RocketChat/Rocket.Chat/pull/9844) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@karlprieb](https://github.com/karlprieb))

- Add a few listener supports for the Rocket.Chat Apps ([#10154](https://github.com/RocketChat/Rocket.Chat/pull/10154))

- Add forums as a place to suggest, discuss and upvote features ([#10148](https://github.com/RocketChat/Rocket.Chat/pull/10148) by [@SeanPackham](https://github.com/SeanPackham))

- Bump snap version to include security fix ([#10313](https://github.com/RocketChat/Rocket.Chat/pull/10313))

- Fix caddy download link to pull from github ([#10260](https://github.com/RocketChat/Rocket.Chat/pull/10260))

- Fix snap install. Remove execstack from sharp, and bypass grpc error ([#10015](https://github.com/RocketChat/Rocket.Chat/pull/10015))

- Fix tests breaking randomly ([#10065](https://github.com/RocketChat/Rocket.Chat/pull/10065))

- Fix typo for Nextcloud login ([#10159](https://github.com/RocketChat/Rocket.Chat/pull/10159) by [@pierreozoux](https://github.com/pierreozoux))

- Fix: chat.react api not accepting previous emojis ([#10290](https://github.com/RocketChat/Rocket.Chat/pull/10290))

- Fix: inputs for rocketchat apps ([#10274](https://github.com/RocketChat/Rocket.Chat/pull/10274))

- Fix: possible errors on rocket.chat side of the apps ([#10252](https://github.com/RocketChat/Rocket.Chat/pull/10252))

- Fix: Reaction endpoint/api only working with regular emojis ([#10323](https://github.com/RocketChat/Rocket.Chat/pull/10323))

- Fix: Renaming channels.notifications Get/Post endpoints ([#10257](https://github.com/RocketChat/Rocket.Chat/pull/10257) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Fix: Scroll on content page ([#10300](https://github.com/RocketChat/Rocket.Chat/pull/10300))

- LingoHub based on develop ([#10243](https://github.com/RocketChat/Rocket.Chat/pull/10243))

- Release 0.63.0 ([#10324](https://github.com/RocketChat/Rocket.Chat/pull/10324) by [@Hudell](https://github.com/Hudell) & [@Joe-mcgee](https://github.com/Joe-mcgee) & [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@TopHattedCat](https://github.com/TopHattedCat) & [@hmagarotto](https://github.com/hmagarotto) & [@kaiiiiiiiii](https://github.com/kaiiiiiiiii) & [@karlprieb](https://github.com/karlprieb) & [@kb0304](https://github.com/kb0304) & [@lunaticmonk](https://github.com/lunaticmonk) & [@ramrami](https://github.com/ramrami))

- Rename migration name on 108 to match file name ([#10237](https://github.com/RocketChat/Rocket.Chat/pull/10237))

- Start 0.63.0-develop / develop sync from master ([#9985](https://github.com/RocketChat/Rocket.Chat/pull/9985))

- Update Meteor to 1.6.1.1 ([#10314](https://github.com/RocketChat/Rocket.Chat/pull/10314))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Hudell](https://github.com/Hudell)
- [@Joe-mcgee](https://github.com/Joe-mcgee)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@SeanPackham](https://github.com/SeanPackham)
- [@TopHattedCat](https://github.com/TopHattedCat)
- [@bernardoetrevisan](https://github.com/bernardoetrevisan)
- [@c0dzilla](https://github.com/c0dzilla)
- [@cyclops24](https://github.com/cyclops24)
- [@hmagarotto](https://github.com/hmagarotto)
- [@kaiiiiiiiii](https://github.com/kaiiiiiiiii)
- [@karlprieb](https://github.com/karlprieb)
- [@kb0304](https://github.com/kb0304)
- [@lunaticmonk](https://github.com/lunaticmonk)
- [@mutdmour](https://github.com/mutdmour)
- [@pierreozoux](https://github.com/pierreozoux)
- [@ramrami](https://github.com/ramrami)
- [@trongthanh](https://github.com/trongthanh)
- [@ubarsaiyan](https://github.com/ubarsaiyan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.62.2
`2018-03-09  ·  6 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.4`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Download links was duplicating Sub Paths ([#10029](https://github.com/RocketChat/Rocket.Chat/pull/10029))

- Message editing is crashing the server when read receipts are enabled ([#10061](https://github.com/RocketChat/Rocket.Chat/pull/10061))

- REST API: Can't list all public channels when user has permission `view-joined-room` ([#10009](https://github.com/RocketChat/Rocket.Chat/pull/10009) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

- Slack Import reports `invalid import file type` due to a call to BSON.native() which is now doesn't exist ([#10071](https://github.com/RocketChat/Rocket.Chat/pull/10071) by [@trongthanh](https://github.com/trongthanh))

- Update preferences of users with settings: null was crashing the server ([#10076](https://github.com/RocketChat/Rocket.Chat/pull/10076))

- Verified property of user is always set to false if not supplied ([#9719](https://github.com/RocketChat/Rocket.Chat/pull/9719) by [@MarcosSpessatto](https://github.com/MarcosSpessatto))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.62.2 ([#10087](https://github.com/RocketChat/Rocket.Chat/pull/10087))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@trongthanh](https://github.com/trongthanh)

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.62.1
`2018-03-03  ·  4 🐛  ·  1 🔍  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.4`
- NPM: `5.6.0`

### 🐛 Bug fixes


- Delete user without username was removing direct rooms of all users ([#9986](https://github.com/RocketChat/Rocket.Chat/pull/9986))

- Empty sidenav when sorting by activity and there is a subscription without room ([#9960](https://github.com/RocketChat/Rocket.Chat/pull/9960))

- New channel page on medium size screens ([#9988](https://github.com/RocketChat/Rocket.Chat/pull/9988) by [@karlprieb](https://github.com/karlprieb))

- Two factor authentication modal was not showing ([#9982](https://github.com/RocketChat/Rocket.Chat/pull/9982))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.62.1 ([#9989](https://github.com/RocketChat/Rocket.Chat/pull/9989))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@karlprieb](https://github.com/karlprieb)

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.62.0
`2018-02-27  ·  1 ️️️⚠️  ·  24 🎉  ·  32 🐛  ·  26 🔍  ·  39 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.4`
- NPM: `5.6.0`

### ⚠️ BREAKING CHANGES


- Remove Graphics/Image Magick support ([#9711](https://github.com/RocketChat/Rocket.Chat/pull/9711))

### 🎉 New features


- Add documentation requirement to PRs ([#9658](https://github.com/RocketChat/Rocket.Chat/pull/9658) by [@SeanPackham](https://github.com/SeanPackham))

- Add route to get user shield/badge ([#9549](https://github.com/RocketChat/Rocket.Chat/pull/9549) by [@kb0304](https://github.com/kb0304))

- Add user settings / preferences API endpoint ([#9457](https://github.com/RocketChat/Rocket.Chat/pull/9457) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@jgtoriginal](https://github.com/jgtoriginal))

- Alert admins when user requires approval & alert users when the account is approved/activated/deactivated ([#7098](https://github.com/RocketChat/Rocket.Chat/pull/7098) by [@luisfn](https://github.com/luisfn))

- Allow configuration of SAML logout behavior ([#9527](https://github.com/RocketChat/Rocket.Chat/pull/9527) by [@mrsimpson](https://github.com/mrsimpson))

- Allow request avatar placeholders as PNG or JPG instead of SVG ([#8193](https://github.com/RocketChat/Rocket.Chat/pull/8193) by [@lindoelio](https://github.com/lindoelio))

- Allow sounds when conversation is focused ([#9312](https://github.com/RocketChat/Rocket.Chat/pull/9312) by [@RationalCoding](https://github.com/RationalCoding))

- API to fetch permissions & user roles ([#9519](https://github.com/RocketChat/Rocket.Chat/pull/9519) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@rafaelks](https://github.com/rafaelks))

- Browse more channels / Directory ([#9642](https://github.com/RocketChat/Rocket.Chat/pull/9642) by [@karlprieb](https://github.com/karlprieb))

- General alert banner ([#9778](https://github.com/RocketChat/Rocket.Chat/pull/9778))

- Global message search (beta: disabled by default) ([#9687](https://github.com/RocketChat/Rocket.Chat/pull/9687) by [@cyberhck](https://github.com/cyberhck) & [@savikko](https://github.com/savikko))

- GraphQL API ([#8158](https://github.com/RocketChat/Rocket.Chat/pull/8158) by [@kamilkisiela](https://github.com/kamilkisiela))

- Image preview as 32x32 base64 jpeg ([#9218](https://github.com/RocketChat/Rocket.Chat/pull/9218) by [@jorgeluisrezende](https://github.com/jorgeluisrezende))

- Improved default welcome message ([#9298](https://github.com/RocketChat/Rocket.Chat/pull/9298) by [@HammyHavoc](https://github.com/HammyHavoc))

- Internal hubot support for Direct Messages and Private Groups ([#8933](https://github.com/RocketChat/Rocket.Chat/pull/8933) by [@ramrami](https://github.com/ramrami))

- Livestream tab ([#9255](https://github.com/RocketChat/Rocket.Chat/pull/9255) by [@gdelavald](https://github.com/gdelavald))

- Makes shield icon configurable ([#9746](https://github.com/RocketChat/Rocket.Chat/pull/9746) by [@c0dzilla](https://github.com/c0dzilla))

- Message read receipts ([#9717](https://github.com/RocketChat/Rocket.Chat/pull/9717))

- New REST API to mark channel as read ([#9507](https://github.com/RocketChat/Rocket.Chat/pull/9507) by [@rafaelks](https://github.com/rafaelks))

- New sidebar layout ([#9608](https://github.com/RocketChat/Rocket.Chat/pull/9608) by [@karlprieb](https://github.com/karlprieb))

- Option to proxy files and avatars through the server ([#9699](https://github.com/RocketChat/Rocket.Chat/pull/9699))

- Request mongoDB version in github issue template ([#9807](https://github.com/RocketChat/Rocket.Chat/pull/9807) by [@TwizzyDizzy](https://github.com/TwizzyDizzy))

- REST API to use Spotlight ([#9509](https://github.com/RocketChat/Rocket.Chat/pull/9509) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@rafaelks](https://github.com/rafaelks))

- Version update check ([#9793](https://github.com/RocketChat/Rocket.Chat/pull/9793))

### 🐛 Bug fixes


- 'Query' support for channels.list.joined, groups.list, groups.listAll, im.list ([#9424](https://github.com/RocketChat/Rocket.Chat/pull/9424) by [@xbolshe](https://github.com/xbolshe))

- API to retrive rooms was returning empty objects ([#9737](https://github.com/RocketChat/Rocket.Chat/pull/9737))

- Chat Message Reactions REST API End Point ([#9487](https://github.com/RocketChat/Rocket.Chat/pull/9487) by [@MarcosSpessatto](https://github.com/MarcosSpessatto) & [@jgtoriginal](https://github.com/jgtoriginal))

- Chrome 64 breaks jitsi-meet iframe ([#9560](https://github.com/RocketChat/Rocket.Chat/pull/9560) by [@speedy01](https://github.com/speedy01))

- Close button on file upload bar was not working ([#9662](https://github.com/RocketChat/Rocket.Chat/pull/9662) by [@karlprieb](https://github.com/karlprieb))

- Close Livechat conversation by visitor not working in version 0.61.0 ([#9714](https://github.com/RocketChat/Rocket.Chat/pull/9714))

- Custom emoji was cropping sometimes ([#9676](https://github.com/RocketChat/Rocket.Chat/pull/9676) by [@anu-007](https://github.com/anu-007))

- DeprecationWarning: prom-client ... when starting Rocket Chat server ([#9747](https://github.com/RocketChat/Rocket.Chat/pull/9747) by [@jgtoriginal](https://github.com/jgtoriginal))

- Desktop notification not showing when avatar came from external storage service ([#9639](https://github.com/RocketChat/Rocket.Chat/pull/9639))

- Emoji rendering on last message ([#9776](https://github.com/RocketChat/Rocket.Chat/pull/9776))

- Facebook integration in livechat not working on version 0.61.0 ([#9640](https://github.com/RocketChat/Rocket.Chat/pull/9640))

- Formal pronouns and some small mistakes in German texts ([#9067](https://github.com/RocketChat/Rocket.Chat/pull/9067) by [@AmShaegar13](https://github.com/AmShaegar13))

- GitLab OAuth does not work when GitLab’s URL ends with slash ([#9716](https://github.com/RocketChat/Rocket.Chat/pull/9716))

- Harmonize channel-related actions ([#9697](https://github.com/RocketChat/Rocket.Chat/pull/9697) by [@mrsimpson](https://github.com/mrsimpson))

- Importers no longer working due to the FileUpload changes ([#9850](https://github.com/RocketChat/Rocket.Chat/pull/9850))

- Livechat conversation not receiving messages when start without form ([#9772](https://github.com/RocketChat/Rocket.Chat/pull/9772))

- Livechat is not working when running in a sub path ([#9599](https://github.com/RocketChat/Rocket.Chat/pull/9599))

- Livechat issues on external queue and lead capture ([#9750](https://github.com/RocketChat/Rocket.Chat/pull/9750))

- Messages can't be quoted sometimes ([#9720](https://github.com/RocketChat/Rocket.Chat/pull/9720))

- Misplaced "Save Changes" button in user account panel ([#9888](https://github.com/RocketChat/Rocket.Chat/pull/9888) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Missing link Site URLs in enrollment e-mails ([#9454](https://github.com/RocketChat/Rocket.Chat/pull/9454) by [@kemitchell](https://github.com/kemitchell))

- Missing string 'Username_already_exist' on the accountProfile page ([#9610](https://github.com/RocketChat/Rocket.Chat/pull/9610) by [@lunaticmonk](https://github.com/lunaticmonk))

- Not receiving sound notifications in rooms created by new LiveChats ([#9802](https://github.com/RocketChat/Rocket.Chat/pull/9802))

- Parsing messages with multiple markdown matches ignore some tokens ([#9884](https://github.com/RocketChat/Rocket.Chat/pull/9884) by [@c0dzilla](https://github.com/c0dzilla))

- Rest API helpers only applying to v1 ([#9520](https://github.com/RocketChat/Rocket.Chat/pull/9520))

- Show custom room types icon in channel header ([#9696](https://github.com/RocketChat/Rocket.Chat/pull/9696) by [@mrsimpson](https://github.com/mrsimpson))

- Silence the update check error message ([#9858](https://github.com/RocketChat/Rocket.Chat/pull/9858))

- Snap build was failing ([#9879](https://github.com/RocketChat/Rocket.Chat/pull/9879))

- SVG avatars are not been displayed correctly when load in non HTML containers ([#9570](https://github.com/RocketChat/Rocket.Chat/pull/9570) by [@filipedelimabrito](https://github.com/filipedelimabrito))

- Typo on french translation for "Open" ([#9934](https://github.com/RocketChat/Rocket.Chat/pull/9934) by [@sizrar](https://github.com/sizrar))

- Weird rendering of emojis at sidebar when `last message` is activated ([#9623](https://github.com/RocketChat/Rocket.Chat/pull/9623))

- Wrong behavior of rooms info's *Read Only* and *Collaborative* buttons ([#9665](https://github.com/RocketChat/Rocket.Chat/pull/9665) by [@karlprieb](https://github.com/karlprieb))

<details>
<summary>🔍 Minor changes</summary>


- [Fix] Not Translated Phrases ([#9877](https://github.com/RocketChat/Rocket.Chat/pull/9877) by [@bernardoetrevisan](https://github.com/bernardoetrevisan))

- [OTHER] Fix Apps not working on multi-instance deployments ([#9902](https://github.com/RocketChat/Rocket.Chat/pull/9902))

- [OTHER] Rocket.Chat Apps ([#9666](https://github.com/RocketChat/Rocket.Chat/pull/9666))

- Dependencies update ([#9811](https://github.com/RocketChat/Rocket.Chat/pull/9811))

- Develop fix sync from master ([#9797](https://github.com/RocketChat/Rocket.Chat/pull/9797))

- Fix RHCC image path for OpenShift and default to the current namespace. ([#9901](https://github.com/RocketChat/Rocket.Chat/pull/9901) by [@jsm84](https://github.com/jsm84))

- Fix: Custom fields not showing on user info panel ([#9821](https://github.com/RocketChat/Rocket.Chat/pull/9821))

- Improve link handling for attachments ([#9908](https://github.com/RocketChat/Rocket.Chat/pull/9908))

- Move NRR package to inside the project and convert from CoffeeScript ([#9753](https://github.com/RocketChat/Rocket.Chat/pull/9753))

- Regression: Avatar now open account related options ([#9843](https://github.com/RocketChat/Rocket.Chat/pull/9843) by [@karlprieb](https://github.com/karlprieb))

- Regression: Change create channel icon ([#9851](https://github.com/RocketChat/Rocket.Chat/pull/9851) by [@karlprieb](https://github.com/karlprieb))

- Regression: Directory now list default channel ([#9931](https://github.com/RocketChat/Rocket.Chat/pull/9931) by [@karlprieb](https://github.com/karlprieb))

- Regression: Fix admin/user settings item text ([#9845](https://github.com/RocketChat/Rocket.Chat/pull/9845) by [@karlprieb](https://github.com/karlprieb))

- Regression: Fix channel icons on safari ([#9852](https://github.com/RocketChat/Rocket.Chat/pull/9852) by [@karlprieb](https://github.com/karlprieb))

- Regression: Fix livechat queue link ([#9928](https://github.com/RocketChat/Rocket.Chat/pull/9928) by [@karlprieb](https://github.com/karlprieb))

- Regression: Improve sidebar filter ([#9905](https://github.com/RocketChat/Rocket.Chat/pull/9905) by [@karlprieb](https://github.com/karlprieb))

- Regression: Misplaced language dropdown in user preferences panel ([#9883](https://github.com/RocketChat/Rocket.Chat/pull/9883) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Regression: Open search using ctrl/cmd + p and ctrl/cmd + k ([#9837](https://github.com/RocketChat/Rocket.Chat/pull/9837) by [@karlprieb](https://github.com/karlprieb))

- Regression: Overlapping header in user profile panel ([#9889](https://github.com/RocketChat/Rocket.Chat/pull/9889) by [@kaiiiiiiiii](https://github.com/kaiiiiiiiii))

- Regression: Page was not respecting the window height on Firefox ([#9804](https://github.com/RocketChat/Rocket.Chat/pull/9804))

- Regression: Search bar is now full width ([#9839](https://github.com/RocketChat/Rocket.Chat/pull/9839) by [@karlprieb](https://github.com/karlprieb))

- Regression: sort on room's list not working correctly ([#9897](https://github.com/RocketChat/Rocket.Chat/pull/9897))

- Release 0.62.0 ([#9935](https://github.com/RocketChat/Rocket.Chat/pull/9935))

- Sync from Master ([#9796](https://github.com/RocketChat/Rocket.Chat/pull/9796) by [@HammyHavoc](https://github.com/HammyHavoc))

- Update bot-config.yml ([#9784](https://github.com/RocketChat/Rocket.Chat/pull/9784) by [@JSzaszvari](https://github.com/JSzaszvari))

- Update to meteor 1.6.1 ([#9546](https://github.com/RocketChat/Rocket.Chat/pull/9546))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AmShaegar13](https://github.com/AmShaegar13)
- [@HammyHavoc](https://github.com/HammyHavoc)
- [@JSzaszvari](https://github.com/JSzaszvari)
- [@MarcosSpessatto](https://github.com/MarcosSpessatto)
- [@RationalCoding](https://github.com/RationalCoding)
- [@SeanPackham](https://github.com/SeanPackham)
- [@TwizzyDizzy](https://github.com/TwizzyDizzy)
- [@anu-007](https://github.com/anu-007)
- [@bernardoetrevisan](https://github.com/bernardoetrevisan)
- [@c0dzilla](https://github.com/c0dzilla)
- [@cyberhck](https://github.com/cyberhck)
- [@filipedelimabrito](https://github.com/filipedelimabrito)
- [@gdelavald](https://github.com/gdelavald)
- [@jgtoriginal](https://github.com/jgtoriginal)
- [@jorgeluisrezende](https://github.com/jorgeluisrezende)
- [@jsm84](https://github.com/jsm84)
- [@kaiiiiiiiii](https://github.com/kaiiiiiiiii)
- [@kamilkisiela](https://github.com/kamilkisiela)
- [@karlprieb](https://github.com/karlprieb)
- [@kb0304](https://github.com/kb0304)
- [@kemitchell](https://github.com/kemitchell)
- [@lindoelio](https://github.com/lindoelio)
- [@luisfn](https://github.com/luisfn)
- [@lunaticmonk](https://github.com/lunaticmonk)
- [@mrsimpson](https://github.com/mrsimpson)
- [@rafaelks](https://github.com/rafaelks)
- [@ramrami](https://github.com/ramrami)
- [@savikko](https://github.com/savikko)
- [@sizrar](https://github.com/sizrar)
- [@speedy01](https://github.com/speedy01)
- [@xbolshe](https://github.com/xbolshe)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@renatobecker](https://github.com/renatobecker)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.61.2
`2018-02-20  ·  3 🐛  ·  1 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.3`
- NPM: `5.5.1`

### 🐛 Bug fixes


- Emoji rendering on last message ([#9776](https://github.com/RocketChat/Rocket.Chat/pull/9776))

- Livechat conversation not receiving messages when start without form ([#9772](https://github.com/RocketChat/Rocket.Chat/pull/9772))

- Livechat issues on external queue and lead capture ([#9750](https://github.com/RocketChat/Rocket.Chat/pull/9750))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.61.2 ([#9786](https://github.com/RocketChat/Rocket.Chat/pull/9786))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@ggazzo](https://github.com/ggazzo)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.61.1
`2018-02-14  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.3`
- NPM: `5.5.1`

<details>
<summary>🔍 Minor changes</summary>


- Release 0.61.1 ([#9721](https://github.com/RocketChat/Rocket.Chat/pull/9721))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)

# 0.61.0
`2018-01-27  ·  1 ️️️⚠️  ·  12 🎉  ·  44 🐛  ·  39 🔍  ·  23 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.3`
- NPM: `5.5.1`

### ⚠️ BREAKING CHANGES


- Decouple livechat visitors from regular users ([#9048](https://github.com/RocketChat/Rocket.Chat/pull/9048))

### 🎉 New features


- add /home link to sidenav footer logo ([#9366](https://github.com/RocketChat/Rocket.Chat/pull/9366) by [@cyclops24](https://github.com/cyclops24))

- Add impersonate option for livechat triggers ([#9107](https://github.com/RocketChat/Rocket.Chat/pull/9107))

- Add mention-here permission #7631 ([#9228](https://github.com/RocketChat/Rocket.Chat/pull/9228) by [@ryjones](https://github.com/ryjones))

- Add support to external livechat queue service provider ([#9053](https://github.com/RocketChat/Rocket.Chat/pull/9053))

- Contextual bar mail messages ([#9510](https://github.com/RocketChat/Rocket.Chat/pull/9510) by [@karlprieb](https://github.com/karlprieb))

- Contextual Bar Redesign ([#8411](https://github.com/RocketChat/Rocket.Chat/pull/8411) by [@karlprieb](https://github.com/karlprieb))

- Indicate the Self DM room ([#9234](https://github.com/RocketChat/Rocket.Chat/pull/9234))

- Livechat extract lead data from message ([#9135](https://github.com/RocketChat/Rocket.Chat/pull/9135))

- Make Custom oauth accept nested usernameField ([#9066](https://github.com/RocketChat/Rocket.Chat/pull/9066) by [@pierreozoux](https://github.com/pierreozoux))

- new layout for emojipicker ([#9245](https://github.com/RocketChat/Rocket.Chat/pull/9245) by [@karlprieb](https://github.com/karlprieb))

- Sidebar menu option to mark room as unread ([#9216](https://github.com/RocketChat/Rocket.Chat/pull/9216) by [@karlprieb](https://github.com/karlprieb))

- Update documentation: provide example for multiple basedn ([#9442](https://github.com/RocketChat/Rocket.Chat/pull/9442) by [@rndmh3ro](https://github.com/rndmh3ro))

### 🐛 Bug fixes


- "Enter usernames" placeholder is cutting in "create channel" view ([#9194](https://github.com/RocketChat/Rocket.Chat/pull/9194) by [@TheReal1604](https://github.com/TheReal1604))

- "Use Emoji" preference not working ([#9182](https://github.com/RocketChat/Rocket.Chat/pull/9182) by [@karlprieb](https://github.com/karlprieb))

- **i18n:** add room type translation support for room-changed-privacy message ([#9369](https://github.com/RocketChat/Rocket.Chat/pull/9369) by [@cyclops24](https://github.com/cyclops24))

- announcement hyperlink color ([#9330](https://github.com/RocketChat/Rocket.Chat/pull/9330) by [@karlprieb](https://github.com/karlprieb))

- channel create scroll on small screens ([#9168](https://github.com/RocketChat/Rocket.Chat/pull/9168) by [@karlprieb](https://github.com/karlprieb))

- Channel page error ([#9091](https://github.com/RocketChat/Rocket.Chat/pull/9091) by [@ggrish](https://github.com/ggrish))

- Contextual bar redesign ([#9481](https://github.com/RocketChat/Rocket.Chat/pull/9481) by [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb))

- Cursor position when reply on safari ([#9185](https://github.com/RocketChat/Rocket.Chat/pull/9185) by [@karlprieb](https://github.com/karlprieb))

- custom emoji size on sidebar item ([#9314](https://github.com/RocketChat/Rocket.Chat/pull/9314) by [@karlprieb](https://github.com/karlprieb))

- Deleting message with store last message not removing ([#9335](https://github.com/RocketChat/Rocket.Chat/pull/9335))

- Do not block room while loading history ([#9121](https://github.com/RocketChat/Rocket.Chat/pull/9121))

- Emoji size on last message preview ([#9186](https://github.com/RocketChat/Rocket.Chat/pull/9186) by [@karlprieb](https://github.com/karlprieb))

- English Typos ([#9285](https://github.com/RocketChat/Rocket.Chat/pull/9285) by [@HammyHavoc](https://github.com/HammyHavoc))

- Error when user roles is missing or is invalid ([#9040](https://github.com/RocketChat/Rocket.Chat/pull/9040) by [@paulovitin](https://github.com/paulovitin))

- File access not working when passing credentials via querystring ([#9264](https://github.com/RocketChat/Rocket.Chat/pull/9264))

- File upload not working on IE and weird on Chrome ([#9206](https://github.com/RocketChat/Rocket.Chat/pull/9206) by [@karlprieb](https://github.com/karlprieb))

- Fix closing livechat inquiry ([#9164](https://github.com/RocketChat/Rocket.Chat/pull/9164))

- Fix livechat build ([#9451](https://github.com/RocketChat/Rocket.Chat/pull/9451))

- Fix livechat register form ([#9452](https://github.com/RocketChat/Rocket.Chat/pull/9452))

- Fix livechat visitor edit ([#9506](https://github.com/RocketChat/Rocket.Chat/pull/9506))

- go to replied message ([#9172](https://github.com/RocketChat/Rocket.Chat/pull/9172) by [@karlprieb](https://github.com/karlprieb))

- Highlight setting not working correctly ([#9364](https://github.com/RocketChat/Rocket.Chat/pull/9364) by [@cyclops24](https://github.com/cyclops24))

- Importers not recovering when an error occurs ([#9134](https://github.com/RocketChat/Rocket.Chat/pull/9134))

- large names on userinfo, and admin user bug on users with no usernames ([#9493](https://github.com/RocketChat/Rocket.Chat/pull/9493) by [@gdelavald](https://github.com/gdelavald))

- last message cutting on bottom ([#9345](https://github.com/RocketChat/Rocket.Chat/pull/9345) by [@karlprieb](https://github.com/karlprieb))

- Last sent message reoccurs in textbox ([#9169](https://github.com/RocketChat/Rocket.Chat/pull/9169))

- LDAP/AD is not importing all users ([#9309](https://github.com/RocketChat/Rocket.Chat/pull/9309))

- Made welcome emails more readable ([#9193](https://github.com/RocketChat/Rocket.Chat/pull/9193) by [@HammyHavoc](https://github.com/HammyHavoc))

- Make mentions and menu icons color darker ([#8922](https://github.com/RocketChat/Rocket.Chat/pull/8922) by [@karlprieb](https://github.com/karlprieb))

- make the cross icon on user selection at channel creation page work ([#9176](https://github.com/RocketChat/Rocket.Chat/pull/9176) by [@karlprieb](https://github.com/karlprieb) & [@vitor-nagao](https://github.com/vitor-nagao))

- mention-here is missing i18n text #9455 ([#9456](https://github.com/RocketChat/Rocket.Chat/pull/9456) by [@ryjones](https://github.com/ryjones))

- modal data on enter and modal style for file preview ([#9171](https://github.com/RocketChat/Rocket.Chat/pull/9171) by [@karlprieb](https://github.com/karlprieb))

- Move emojipicker css to theme package ([#9243](https://github.com/RocketChat/Rocket.Chat/pull/9243) by [@karlprieb](https://github.com/karlprieb))

- popover on safari for iOS ([#9328](https://github.com/RocketChat/Rocket.Chat/pull/9328) by [@karlprieb](https://github.com/karlprieb))

- Show modal with announcement ([#9241](https://github.com/RocketChat/Rocket.Chat/pull/9241) by [@karlprieb](https://github.com/karlprieb))

- show oauth logins when adblock is used ([#9170](https://github.com/RocketChat/Rocket.Chat/pull/9170) by [@karlprieb](https://github.com/karlprieb))

- sidebar footer padding ([#9249](https://github.com/RocketChat/Rocket.Chat/pull/9249) by [@karlprieb](https://github.com/karlprieb))

- Slash command 'archive' throws exception if the channel does not exist ([#9428](https://github.com/RocketChat/Rocket.Chat/pull/9428) by [@ramrami](https://github.com/ramrami))

- Slash command 'unarchive' throws exception if the channel does not exist  ([#9435](https://github.com/RocketChat/Rocket.Chat/pull/9435) by [@ramrami](https://github.com/ramrami))

- Subscriptions not removed when removing user ([#9432](https://github.com/RocketChat/Rocket.Chat/pull/9432))

- svg render on firefox ([#9311](https://github.com/RocketChat/Rocket.Chat/pull/9311) by [@karlprieb](https://github.com/karlprieb))

- Unread bar position when room have announcement ([#9188](https://github.com/RocketChat/Rocket.Chat/pull/9188) by [@karlprieb](https://github.com/karlprieb))

- Update Rocket.Chat for sandstorm ([#9062](https://github.com/RocketChat/Rocket.Chat/pull/9062) by [@peterlee0127](https://github.com/peterlee0127))

- Wrong position of notifications alert in accounts preference page ([#9289](https://github.com/RocketChat/Rocket.Chat/pull/9289) by [@HammyHavoc](https://github.com/HammyHavoc))

<details>
<summary>🔍 Minor changes</summary>


- [DOCS] Update the links of our Mobile Apps in Features topic ([#9469](https://github.com/RocketChat/Rocket.Chat/pull/9469) by [@rafaelks](https://github.com/rafaelks))

- [Fix] oauth not working because of email array ([#9173](https://github.com/RocketChat/Rocket.Chat/pull/9173))

- Add community bot ([#9439](https://github.com/RocketChat/Rocket.Chat/pull/9439))

- Add curl, its missing on worker nodes so has to be explicitly added ([#9248](https://github.com/RocketChat/Rocket.Chat/pull/9248))

- Dependencies Update ([#9197](https://github.com/RocketChat/Rocket.Chat/pull/9197))

- Develop sync - Bump version to 0.61.0-develop ([#9260](https://github.com/RocketChat/Rocket.Chat/pull/9260) by [@cpitman](https://github.com/cpitman) & [@karlprieb](https://github.com/karlprieb))

- Do not change room icon color when room is unread ([#9257](https://github.com/RocketChat/Rocket.Chat/pull/9257))

- Fix test without oplog by waiting a successful login on changing users ([#9146](https://github.com/RocketChat/Rocket.Chat/pull/9146))

- Fix: Can’t login using LDAP via REST ([#9162](https://github.com/RocketChat/Rocket.Chat/pull/9162))

- Fix: Change 'Wordpress' to 'WordPress ([#9291](https://github.com/RocketChat/Rocket.Chat/pull/9291) by [@HammyHavoc](https://github.com/HammyHavoc))

- Fix: Clear all unreads modal not closing after confirming ([#9137](https://github.com/RocketChat/Rocket.Chat/pull/9137))

- Fix: Click on channel name - hover area bigger than link area ([#9165](https://github.com/RocketChat/Rocket.Chat/pull/9165))

- Fix: Confirmation modals showing `Send` button ([#9136](https://github.com/RocketChat/Rocket.Chat/pull/9136))

- Fix: English language improvements ([#9299](https://github.com/RocketChat/Rocket.Chat/pull/9299) by [@HammyHavoc](https://github.com/HammyHavoc))

- Fix: Improved README.md ([#9290](https://github.com/RocketChat/Rocket.Chat/pull/9290) by [@HammyHavoc](https://github.com/HammyHavoc))

- Fix: Message action quick buttons drops if "new message" divider is being shown ([#9138](https://github.com/RocketChat/Rocket.Chat/pull/9138))

- Fix: Messages being displayed in reverse order ([#9144](https://github.com/RocketChat/Rocket.Chat/pull/9144))

- Fix: Missing option to set user's avatar from a url ([#9229](https://github.com/RocketChat/Rocket.Chat/pull/9229))

- Fix: Multiple unread indicators ([#9120](https://github.com/RocketChat/Rocket.Chat/pull/9120))

- Fix: README typo ([#9286](https://github.com/RocketChat/Rocket.Chat/pull/9286) by [@HammyHavoc](https://github.com/HammyHavoc))

- Fix: Rooms and users are using different avatar style ([#9196](https://github.com/RocketChat/Rocket.Chat/pull/9196))

- Fix: Sidebar item on rtl and small devices ([#9247](https://github.com/RocketChat/Rocket.Chat/pull/9247) by [@karlprieb](https://github.com/karlprieb))

- Fix: Snippet name to not showing in snippet list ([#9184](https://github.com/RocketChat/Rocket.Chat/pull/9184) by [@karlprieb](https://github.com/karlprieb))

- Fix: UI: Descenders of glyphs are cut off ([#9181](https://github.com/RocketChat/Rocket.Chat/pull/9181))

- Fix: UI: Descenders of glyphs are cut off ([#9166](https://github.com/RocketChat/Rocket.Chat/pull/9166))

- Fix: Unneeded warning in payload of REST API calls ([#9240](https://github.com/RocketChat/Rocket.Chat/pull/9240))

- Fix: Unread line ([#9149](https://github.com/RocketChat/Rocket.Chat/pull/9149))

- Fix: updating last message on message edit or delete ([#9227](https://github.com/RocketChat/Rocket.Chat/pull/9227))

- Fix: Upload access control too distributed ([#9215](https://github.com/RocketChat/Rocket.Chat/pull/9215))

- Fix: Username find is matching partially ([#9217](https://github.com/RocketChat/Rocket.Chat/pull/9217))

- Fix/api me only return verified ([#9183](https://github.com/RocketChat/Rocket.Chat/pull/9183))

- LingoHub based on develop ([#9256](https://github.com/RocketChat/Rocket.Chat/pull/9256))

- Prevent NPM package-lock inside livechat ([#9504](https://github.com/RocketChat/Rocket.Chat/pull/9504))

- Release 0.61.0 ([#9533](https://github.com/RocketChat/Rocket.Chat/pull/9533) by [@karlprieb](https://github.com/karlprieb) & [@ryjones](https://github.com/ryjones))

- Replace postcss-nesting with postcss-nested ([#9200](https://github.com/RocketChat/Rocket.Chat/pull/9200))

- Typo: German language file ([#9190](https://github.com/RocketChat/Rocket.Chat/pull/9190) by [@TheReal1604](https://github.com/TheReal1604))

- Update license ([#9490](https://github.com/RocketChat/Rocket.Chat/pull/9490))

- Update Marked dependecy to 0.3.9 ([#9346](https://github.com/RocketChat/Rocket.Chat/pull/9346))

- Use correct version of Mailparser module ([#9356](https://github.com/RocketChat/Rocket.Chat/pull/9356))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@HammyHavoc](https://github.com/HammyHavoc)
- [@TheReal1604](https://github.com/TheReal1604)
- [@cpitman](https://github.com/cpitman)
- [@cyclops24](https://github.com/cyclops24)
- [@gdelavald](https://github.com/gdelavald)
- [@ggrish](https://github.com/ggrish)
- [@karlprieb](https://github.com/karlprieb)
- [@paulovitin](https://github.com/paulovitin)
- [@peterlee0127](https://github.com/peterlee0127)
- [@pierreozoux](https://github.com/pierreozoux)
- [@rafaelks](https://github.com/rafaelks)
- [@ramrami](https://github.com/ramrami)
- [@rndmh3ro](https://github.com/rndmh3ro)
- [@ryjones](https://github.com/ryjones)
- [@vitor-nagao](https://github.com/vitor-nagao)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@frdmn](https://github.com/frdmn)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.60.4
`2018-01-10  ·  5 🐛  ·  2 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.3`
- NPM: `5.5.1`

### 🐛 Bug fixes


- announcement hyperlink color ([#9330](https://github.com/RocketChat/Rocket.Chat/pull/9330) by [@karlprieb](https://github.com/karlprieb))

- Deleting message with store last message not removing ([#9335](https://github.com/RocketChat/Rocket.Chat/pull/9335))

- last message cutting on bottom ([#9345](https://github.com/RocketChat/Rocket.Chat/pull/9345) by [@karlprieb](https://github.com/karlprieb))

- LDAP TLS not working in some cases ([#9343](https://github.com/RocketChat/Rocket.Chat/pull/9343))

- popover on safari for iOS ([#9328](https://github.com/RocketChat/Rocket.Chat/pull/9328) by [@karlprieb](https://github.com/karlprieb))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.60.4 ([#9377](https://github.com/RocketChat/Rocket.Chat/pull/9377))

- Update Marked dependecy to 0.3.9 ([#9346](https://github.com/RocketChat/Rocket.Chat/pull/9346))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@karlprieb](https://github.com/karlprieb)

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.60.3
`2018-01-03  ·  6 🐛  ·  5 🔍  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.3`
- NPM: `5.5.1`

### 🐛 Bug fixes


- custom emoji size on sidebar item ([#9314](https://github.com/RocketChat/Rocket.Chat/pull/9314) by [@karlprieb](https://github.com/karlprieb))

- English Typos ([#9285](https://github.com/RocketChat/Rocket.Chat/pull/9285) by [@HammyHavoc](https://github.com/HammyHavoc))

- LDAP/AD is not importing all users ([#9309](https://github.com/RocketChat/Rocket.Chat/pull/9309))

- sidebar footer padding ([#9249](https://github.com/RocketChat/Rocket.Chat/pull/9249) by [@karlprieb](https://github.com/karlprieb))

- svg render on firefox ([#9311](https://github.com/RocketChat/Rocket.Chat/pull/9311) by [@karlprieb](https://github.com/karlprieb))

- Wrong position of notifications alert in accounts preference page ([#9289](https://github.com/RocketChat/Rocket.Chat/pull/9289) by [@HammyHavoc](https://github.com/HammyHavoc))

<details>
<summary>🔍 Minor changes</summary>


- Fix: Change 'Wordpress' to 'WordPress ([#9291](https://github.com/RocketChat/Rocket.Chat/pull/9291) by [@HammyHavoc](https://github.com/HammyHavoc))

- Fix: English language improvements ([#9299](https://github.com/RocketChat/Rocket.Chat/pull/9299) by [@HammyHavoc](https://github.com/HammyHavoc))

- Fix: Improved README.md ([#9290](https://github.com/RocketChat/Rocket.Chat/pull/9290) by [@HammyHavoc](https://github.com/HammyHavoc))

- Fix: README typo ([#9286](https://github.com/RocketChat/Rocket.Chat/pull/9286) by [@HammyHavoc](https://github.com/HammyHavoc))

- Release 0.60.3 ([#9320](https://github.com/RocketChat/Rocket.Chat/pull/9320) by [@HammyHavoc](https://github.com/HammyHavoc))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@HammyHavoc](https://github.com/HammyHavoc)
- [@karlprieb](https://github.com/karlprieb)

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)

# 0.60.2
`2017-12-29  ·  3 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.3`
- NPM: `5.5.1`

### 🐛 Bug fixes


- Missing translations ([#9272](https://github.com/RocketChat/Rocket.Chat/pull/9272))

- Remove sweetalert from livechat facebook integration page ([#9274](https://github.com/RocketChat/Rocket.Chat/pull/9274))

- Restore translations from other languages ([#9277](https://github.com/RocketChat/Rocket.Chat/pull/9277))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.60.2 ([#9280](https://github.com/RocketChat/Rocket.Chat/pull/9280))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.60.1
`2017-12-27  ·  1 🐛  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.3`
- NPM: `5.5.1`

### 🐛 Bug fixes


- File access not working when passing credentials via querystring ([#9262](https://github.com/RocketChat/Rocket.Chat/pull/9262))

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)

# 0.60.0
`2017-12-27  ·  33 🎉  ·  171 🐛  ·  99 🔍  ·  71 👩‍💻👨‍💻`

### Engine versions
- Node: `8.9.3`
- NPM: `5.5.1`

### 🎉 New features


- Add "Favorites" and "Mark as read" options to the room list ([#8915](https://github.com/RocketChat/Rocket.Chat/pull/8915) by [@karlprieb](https://github.com/karlprieb))

- Add "real name change" setting ([#8739](https://github.com/RocketChat/Rocket.Chat/pull/8739) by [@AmShaegar13](https://github.com/AmShaegar13))

- Add new API endpoints ([#8947](https://github.com/RocketChat/Rocket.Chat/pull/8947))

- Add RD Station integration to livechat ([#8304](https://github.com/RocketChat/Rocket.Chat/pull/8304))

- Add settings for allow user direct messages to yourself ([#8066](https://github.com/RocketChat/Rocket.Chat/pull/8066) by [@lindoelio](https://github.com/lindoelio))

- Add sweet alert to video call tab ([#8108](https://github.com/RocketChat/Rocket.Chat/pull/8108))

- Add yunohost.org installation method to Readme.md ([#8037](https://github.com/RocketChat/Rocket.Chat/pull/8037) by [@selamanse](https://github.com/selamanse))

- Added support for Dataporten's userid-feide scope ([#8902](https://github.com/RocketChat/Rocket.Chat/pull/8902) by [@torgeirl](https://github.com/torgeirl))

- Adds admin option to globally set mobile devices to always be notified regardless of presence status. ([#7641](https://github.com/RocketChat/Rocket.Chat/pull/7641) by [@stalley](https://github.com/stalley))

- Allow user's default preferences configuration ([#7285](https://github.com/RocketChat/Rocket.Chat/pull/7285) by [@goiaba](https://github.com/goiaba))

- code to get the updated messages ([#8857](https://github.com/RocketChat/Rocket.Chat/pull/8857))

- Describe file uploads when notifying by email ([#8924](https://github.com/RocketChat/Rocket.Chat/pull/8924))

- Displays QR code for manually entering when enabling 2fa ([#8143](https://github.com/RocketChat/Rocket.Chat/pull/8143))

- Facebook livechat integration ([#8807](https://github.com/RocketChat/Rocket.Chat/pull/8807))

- Feature/livechat hide email ([#8149](https://github.com/RocketChat/Rocket.Chat/pull/8149) by [@icosamuel](https://github.com/icosamuel) & [@sarbasamuel](https://github.com/sarbasamuel))

- Improve room types API and usages ([#9009](https://github.com/RocketChat/Rocket.Chat/pull/9009) by [@mrsimpson](https://github.com/mrsimpson))

- Make Custom oauth accept nested usernameField ([#9066](https://github.com/RocketChat/Rocket.Chat/pull/9066) by [@pierreozoux](https://github.com/pierreozoux))

- make sidebar item width 100% ([#8362](https://github.com/RocketChat/Rocket.Chat/pull/8362) by [@karlprieb](https://github.com/karlprieb))

- Modal ([#9092](https://github.com/RocketChat/Rocket.Chat/pull/9092) by [@karlprieb](https://github.com/karlprieb))

- New Modal component ([#8882](https://github.com/RocketChat/Rocket.Chat/pull/8882) by [@karlprieb](https://github.com/karlprieb))

- Option to enable/disable auto away and configure timer ([#8029](https://github.com/RocketChat/Rocket.Chat/pull/8029) by [@armand1m](https://github.com/armand1m))

- Rest API endpoints to list, get, and run commands ([#8531](https://github.com/RocketChat/Rocket.Chat/pull/8531))

- Room counter sidebar preference ([#8866](https://github.com/RocketChat/Rocket.Chat/pull/8866) by [@karlprieb](https://github.com/karlprieb))

- Save room's last message ([#8979](https://github.com/RocketChat/Rocket.Chat/pull/8979) by [@karlprieb](https://github.com/karlprieb))

- Send category and title fields to iOS push notification ([#8905](https://github.com/RocketChat/Rocket.Chat/pull/8905))

- Sender's name in email notifications. ([#7999](https://github.com/RocketChat/Rocket.Chat/pull/7999) by [@pkgodara](https://github.com/pkgodara))

- Setting to disable MarkDown and enable AutoLinker ([#8459](https://github.com/RocketChat/Rocket.Chat/pull/8459))

- Smaller accountBox ([#8360](https://github.com/RocketChat/Rocket.Chat/pull/8360) by [@karlprieb](https://github.com/karlprieb))

- Token Controlled Access channels ([#8060](https://github.com/RocketChat/Rocket.Chat/pull/8060) by [@karlprieb](https://github.com/karlprieb) & [@lindoelio](https://github.com/lindoelio))

- Unify unread and mentions badge ([#8361](https://github.com/RocketChat/Rocket.Chat/pull/8361) by [@karlprieb](https://github.com/karlprieb))

- Upgrade Meteor to 1.6 ([#8715](https://github.com/RocketChat/Rocket.Chat/pull/8715) by [@karlprieb](https://github.com/karlprieb))

- Upgrade to meteor 1.5.2 ([#8073](https://github.com/RocketChat/Rocket.Chat/pull/8073))

- Use enter separator rather than comma in highlight preferences + Auto refresh after change highlighted words ([#8433](https://github.com/RocketChat/Rocket.Chat/pull/8433) by [@cyclops24](https://github.com/cyclops24))

### 🐛 Bug fixes


- "*.members" rest api being useless and only returning usernames ([#8147](https://github.com/RocketChat/Rocket.Chat/pull/8147))

- "Cancel button" on modal in RTL in Firefox 55 ([#8278](https://github.com/RocketChat/Rocket.Chat/pull/8278) by [@cyclops24](https://github.com/cyclops24))

- "Enter usernames" placeholder is cutting in "create channel" view ([#9194](https://github.com/RocketChat/Rocket.Chat/pull/9194) by [@TheReal1604](https://github.com/TheReal1604))

- "Use Emoji" preference not working ([#9182](https://github.com/RocketChat/Rocket.Chat/pull/9182) by [@karlprieb](https://github.com/karlprieb))

- **i18n:** My Profile & README.md links ([#8270](https://github.com/RocketChat/Rocket.Chat/pull/8270) by [@Rzeszow](https://github.com/Rzeszow))

- **PL:** Polish translation ([#7989](https://github.com/RocketChat/Rocket.Chat/pull/7989) by [@Rzeszow](https://github.com/Rzeszow))

- Add admin audio preferences translations ([#8094](https://github.com/RocketChat/Rocket.Chat/pull/8094))

- Add historic chats icon in Livechat ([#8708](https://github.com/RocketChat/Rocket.Chat/pull/8708) by [@mrsimpson](https://github.com/mrsimpson))

- Add needed dependency for snaps ([#8389](https://github.com/RocketChat/Rocket.Chat/pull/8389))

- Add padding on messages to allow space to the action buttons ([#7971](https://github.com/RocketChat/Rocket.Chat/pull/7971))

- Added afterUserCreated trigger after first CAS login ([#9022](https://github.com/RocketChat/Rocket.Chat/pull/9022) by [@AmShaegar13](https://github.com/AmShaegar13))

- Adds default search text padding for emoji search ([#7878](https://github.com/RocketChat/Rocket.Chat/pull/7878) by [@gdelavald](https://github.com/gdelavald))

- After deleting the room, cache is not synchronizing ([#8314](https://github.com/RocketChat/Rocket.Chat/pull/8314) by [@szluohua](https://github.com/szluohua))

- AmazonS3: Quote file.name for ContentDisposition for files with commas ([#8593](https://github.com/RocketChat/Rocket.Chat/pull/8593) by [@xenithorb](https://github.com/xenithorb))

- Amin menu not showing all items & File list breaking line ([#8299](https://github.com/RocketChat/Rocket.Chat/pull/8299) by [@karlprieb](https://github.com/karlprieb))

- API channel/group.members not sorting ([#8635](https://github.com/RocketChat/Rocket.Chat/pull/8635))

- Attachment icons alignment in LTR and RTL ([#8271](https://github.com/RocketChat/Rocket.Chat/pull/8271) by [@cyclops24](https://github.com/cyclops24))

- Audio message icon ([#8648](https://github.com/RocketChat/Rocket.Chat/pull/8648) by [@karlprieb](https://github.com/karlprieb))

- Autoupdate of CSS does not work when using a prefix ([#8107](https://github.com/RocketChat/Rocket.Chat/pull/8107) by [@Darkneon](https://github.com/Darkneon))

- Broken embedded view layout ([#7944](https://github.com/RocketChat/Rocket.Chat/pull/7944) by [@karlprieb](https://github.com/karlprieb))

- Broken emoji picker on firefox ([#7943](https://github.com/RocketChat/Rocket.Chat/pull/7943) by [@karlprieb](https://github.com/karlprieb))

- Call buttons with wrong margin on RTL ([#8307](https://github.com/RocketChat/Rocket.Chat/pull/8307) by [@karlprieb](https://github.com/karlprieb))

- Can't react on Read Only rooms even when enabled ([#8925](https://github.com/RocketChat/Rocket.Chat/pull/8925) by [@karlprieb](https://github.com/karlprieb))

- Can't use OAuth login against a Rocket.Chat OAuth server ([#9044](https://github.com/RocketChat/Rocket.Chat/pull/9044))

- Cannot edit or delete custom sounds ([#8889](https://github.com/RocketChat/Rocket.Chat/pull/8889) by [@ccfang](https://github.com/ccfang))

- CAS does not share secrets when operating multiple server instances ([#8654](https://github.com/RocketChat/Rocket.Chat/pull/8654) by [@AmShaegar13](https://github.com/AmShaegar13))

- Change old 'rocketbot' username to 'InternalHubot_Username' setting ([#8928](https://github.com/RocketChat/Rocket.Chat/pull/8928) by [@ramrami](https://github.com/ramrami))

- Change the unread messages style ([#8883](https://github.com/RocketChat/Rocket.Chat/pull/8883) by [@karlprieb](https://github.com/karlprieb))

- Changed all rocket.chat/docs/ to docs.rocket.chat/ ([#8588](https://github.com/RocketChat/Rocket.Chat/pull/8588) by [@RekkyRek](https://github.com/RekkyRek))

- Changed oembedUrlWidget to prefer og:image and twitter:image over msapplication-TileImage ([#9012](https://github.com/RocketChat/Rocket.Chat/pull/9012) by [@wferris722](https://github.com/wferris722))

- channel create scroll on small screens ([#9168](https://github.com/RocketChat/Rocket.Chat/pull/9168) by [@karlprieb](https://github.com/karlprieb))

- Channel page error ([#9091](https://github.com/RocketChat/Rocket.Chat/pull/9091) by [@ggrish](https://github.com/ggrish))

- Chat box no longer auto-focuses when typing ([#7984](https://github.com/RocketChat/Rocket.Chat/pull/7984))

- Check attachments is defined before accessing first element ([#8295](https://github.com/RocketChat/Rocket.Chat/pull/8295) by [@Darkneon](https://github.com/Darkneon))

- Check for mention-all permission in room scope ([#8931](https://github.com/RocketChat/Rocket.Chat/pull/8931))

- Color reset when default value editor is different ([#8543](https://github.com/RocketChat/Rocket.Chat/pull/8543))

- Contextual errors for this and RegExp declarations in IRC module ([#8656](https://github.com/RocketChat/Rocket.Chat/pull/8656) by [@Pharserror](https://github.com/Pharserror))

- copy to clipboard and update clipboard.js library ([#8039](https://github.com/RocketChat/Rocket.Chat/pull/8039) by [@karlprieb](https://github.com/karlprieb))

- Creating channels on Firefox ([#9109](https://github.com/RocketChat/Rocket.Chat/pull/9109) by [@karlprieb](https://github.com/karlprieb))

- Cursor position when reply on safari ([#9185](https://github.com/RocketChat/Rocket.Chat/pull/9185) by [@karlprieb](https://github.com/karlprieb))

- Custom OAuth: Not able to set different token place for routes ([#9034](https://github.com/RocketChat/Rocket.Chat/pull/9034))

- disabled katex tooltip on messageBox ([#8386](https://github.com/RocketChat/Rocket.Chat/pull/8386))

- DM email notifications always being sent regardless of account setting ([#8917](https://github.com/RocketChat/Rocket.Chat/pull/8917) by [@ashward](https://github.com/ashward))

- Do not block room while loading history ([#9121](https://github.com/RocketChat/Rocket.Chat/pull/9121))

- Do not send joinCode field to clients ([#8527](https://github.com/RocketChat/Rocket.Chat/pull/8527))

- Document README.md. Drupal repo out of date ([#7948](https://github.com/RocketChat/Rocket.Chat/pull/7948) by [@Lawri-van-Buel](https://github.com/Lawri-van-Buel))

- Don't strip trailing slash on autolinker urls ([#8812](https://github.com/RocketChat/Rocket.Chat/pull/8812) by [@jwilkins](https://github.com/jwilkins))

- Double scroll on 'keyboard shortcuts' menu in sidepanel ([#7927](https://github.com/RocketChat/Rocket.Chat/pull/7927) by [@aditya19496](https://github.com/aditya19496))

- Duplicate code in rest api letting in a few bugs with the rest api ([#8408](https://github.com/RocketChat/Rocket.Chat/pull/8408))

- Dynamic popover ([#8101](https://github.com/RocketChat/Rocket.Chat/pull/8101) by [@karlprieb](https://github.com/karlprieb))

- Email Subjects not being sent ([#8317](https://github.com/RocketChat/Rocket.Chat/pull/8317))

- Email verification indicator added ([#7923](https://github.com/RocketChat/Rocket.Chat/pull/7923) by [@aditya19496](https://github.com/aditya19496))

- Emoji Picker hidden for reactions in RTL ([#8300](https://github.com/RocketChat/Rocket.Chat/pull/8300) by [@karlprieb](https://github.com/karlprieb))

- Emoji size on last message preview ([#9186](https://github.com/RocketChat/Rocket.Chat/pull/9186) by [@karlprieb](https://github.com/karlprieb))

- Enable CORS for Restivus ([#8671](https://github.com/RocketChat/Rocket.Chat/pull/8671) by [@mrsimpson](https://github.com/mrsimpson))

- encode filename in url to prevent links breaking ([#8551](https://github.com/RocketChat/Rocket.Chat/pull/8551) by [@joesitton](https://github.com/joesitton))

- Error when saving integration with symbol as only trigger ([#9023](https://github.com/RocketChat/Rocket.Chat/pull/9023))

- Error when user roles is missing or is invalid ([#9040](https://github.com/RocketChat/Rocket.Chat/pull/9040) by [@paulovitin](https://github.com/paulovitin))

- Execute meteor reset on TRAVIS_TAG builds ([#8310](https://github.com/RocketChat/Rocket.Chat/pull/8310))

- File upload not working on IE and weird on Chrome ([#9206](https://github.com/RocketChat/Rocket.Chat/pull/9206) by [@karlprieb](https://github.com/karlprieb))

- fix color on unread messages ([#8282](https://github.com/RocketChat/Rocket.Chat/pull/8282))

- Fix e-mail message forward ([#8645](https://github.com/RocketChat/Rocket.Chat/pull/8645))

- Fix email on mention ([#7754](https://github.com/RocketChat/Rocket.Chat/pull/7754))

- fix emoji package path so they show up correctly in browser ([#8822](https://github.com/RocketChat/Rocket.Chat/pull/8822) by [@ryoshimizu](https://github.com/ryoshimizu))

- Fix google play logo on repo README ([#7912](https://github.com/RocketChat/Rocket.Chat/pull/7912) by [@luizbills](https://github.com/luizbills))

- Fix guest pool inquiry taking ([#8577](https://github.com/RocketChat/Rocket.Chat/pull/8577))

- Fix iframe login API response (issue #8145) ([#8146](https://github.com/RocketChat/Rocket.Chat/pull/8146) by [@astax-t](https://github.com/astax-t))

- Fix livechat toggle UI issue ([#7904](https://github.com/RocketChat/Rocket.Chat/pull/7904))

- Fix placeholders in account profile ([#7945](https://github.com/RocketChat/Rocket.Chat/pull/7945) by [@josiasds](https://github.com/josiasds))

- Fix setting user avatar on LDAP login ([#8099](https://github.com/RocketChat/Rocket.Chat/pull/8099))

- Fix the status on the members list ([#7963](https://github.com/RocketChat/Rocket.Chat/pull/7963))

- Fix typos ([#8679](https://github.com/RocketChat/Rocket.Chat/pull/8679))

- fixed some typos ([#8787](https://github.com/RocketChat/Rocket.Chat/pull/8787) by [@TheReal1604](https://github.com/TheReal1604))

- flextab height on smaller screens ([#8994](https://github.com/RocketChat/Rocket.Chat/pull/8994) by [@karlprieb](https://github.com/karlprieb))

- go to replied message ([#9172](https://github.com/RocketChat/Rocket.Chat/pull/9172) by [@karlprieb](https://github.com/karlprieb))

- Highlighted color height issue ([#8431](https://github.com/RocketChat/Rocket.Chat/pull/8431) by [@cyclops24](https://github.com/cyclops24))

- hyperlink style on sidebar footer ([#7882](https://github.com/RocketChat/Rocket.Chat/pull/7882) by [@karlprieb](https://github.com/karlprieb))

- i18n'd Resend_verification_mail, username_initials, upload avatar ([#8721](https://github.com/RocketChat/Rocket.Chat/pull/8721) by [@arungalva](https://github.com/arungalva))

- if ogImage exists use it over image in oembedUrlWidget ([#9000](https://github.com/RocketChat/Rocket.Chat/pull/9000) by [@satyapramodh](https://github.com/satyapramodh))

- Importers failing when usernames exists but cases don't match and improve the importer framework's performance ([#8966](https://github.com/RocketChat/Rocket.Chat/pull/8966))

- Importers not recovering when an error occurs ([#9134](https://github.com/RocketChat/Rocket.Chat/pull/9134))

- Improved grammar and made it clearer to the user ([#8795](https://github.com/RocketChat/Rocket.Chat/pull/8795) by [@HammyHavoc](https://github.com/HammyHavoc))

- Improving consistency of UX ([#8796](https://github.com/RocketChat/Rocket.Chat/pull/8796) by [@HammyHavoc](https://github.com/HammyHavoc))

- Incorrect URL for login terms when using prefix ([#8211](https://github.com/RocketChat/Rocket.Chat/pull/8211) by [@Darkneon](https://github.com/Darkneon))

- Invalid Code message for password protected channel ([#8491](https://github.com/RocketChat/Rocket.Chat/pull/8491))

- Invisible leader bar on hover ([#8048](https://github.com/RocketChat/Rocket.Chat/pull/8048))

- Issue #8166 where empty analytics setting breaks to load Piwik script ([#8167](https://github.com/RocketChat/Rocket.Chat/pull/8167) by [@ruKurz](https://github.com/ruKurz))

- Katex markdown link changed ([#8948](https://github.com/RocketChat/Rocket.Chat/pull/8948) by [@mritunjaygoutam12](https://github.com/mritunjaygoutam12))

- Last sent message reoccurs in textbox ([#9169](https://github.com/RocketChat/Rocket.Chat/pull/9169))

- LDAP login error regression at 0.59.0 ([#8541](https://github.com/RocketChat/Rocket.Chat/pull/8541))

- LDAP memory issues when pagination is not available ([#8457](https://github.com/RocketChat/Rocket.Chat/pull/8457))

- LDAP not merging existent users && Wrong id link generation ([#8613](https://github.com/RocketChat/Rocket.Chat/pull/8613))

- LDAP not respecting UTF8 characters & Sync Interval not working ([#8691](https://github.com/RocketChat/Rocket.Chat/pull/8691))

- Link for channels are not rendering correctly ([#8985](https://github.com/RocketChat/Rocket.Chat/pull/8985) by [@karlprieb](https://github.com/karlprieb))

- livechat icon ([#7886](https://github.com/RocketChat/Rocket.Chat/pull/7886) by [@karlprieb](https://github.com/karlprieb))

- long filename overlaps cancel button in progress bar ([#8868](https://github.com/RocketChat/Rocket.Chat/pull/8868) by [@joesitton](https://github.com/joesitton))

- Long room announcement cut off ([#8907](https://github.com/RocketChat/Rocket.Chat/pull/8907) by [@karlprieb](https://github.com/karlprieb))

- Made welcome emails more readable ([#9193](https://github.com/RocketChat/Rocket.Chat/pull/9193) by [@HammyHavoc](https://github.com/HammyHavoc))

- Make mentions and menu icons color darker ([#8922](https://github.com/RocketChat/Rocket.Chat/pull/8922) by [@karlprieb](https://github.com/karlprieb))

- make the cross icon on user selection at channel creation page work ([#9176](https://github.com/RocketChat/Rocket.Chat/pull/9176) by [@karlprieb](https://github.com/karlprieb) & [@vitor-nagao](https://github.com/vitor-nagao))

- Makes text action menu width based on content size ([#7887](https://github.com/RocketChat/Rocket.Chat/pull/7887) by [@gdelavald](https://github.com/gdelavald))

- Markdown being rendered in code tags ([#7965](https://github.com/RocketChat/Rocket.Chat/pull/7965))

- Mention unread indicator was removed ([#8316](https://github.com/RocketChat/Rocket.Chat/pull/8316))

- message actions over unread bar ([#7885](https://github.com/RocketChat/Rocket.Chat/pull/7885) by [@karlprieb](https://github.com/karlprieb))

- Message popup menu on mobile/cordova ([#8634](https://github.com/RocketChat/Rocket.Chat/pull/8634) by [@karlprieb](https://github.com/karlprieb))

- message-box autogrow ([#8019](https://github.com/RocketChat/Rocket.Chat/pull/8019) by [@karlprieb](https://github.com/karlprieb))

- Message-box autogrow flick ([#8932](https://github.com/RocketChat/Rocket.Chat/pull/8932) by [@karlprieb](https://github.com/karlprieb))

- Migration 103 wrong converting primrary colors ([#8544](https://github.com/RocketChat/Rocket.Chat/pull/8544))

- Missing i18n translations ([#8357](https://github.com/RocketChat/Rocket.Chat/pull/8357))

- Missing placeholder translations ([#8286](https://github.com/RocketChat/Rocket.Chat/pull/8286))

- Missing scroll at create channel page ([#8637](https://github.com/RocketChat/Rocket.Chat/pull/8637) by [@karlprieb](https://github.com/karlprieb))

- Missing sidebar footer padding ([#8884](https://github.com/RocketChat/Rocket.Chat/pull/8884) by [@karlprieb](https://github.com/karlprieb))

- modal data on enter and modal style for file preview ([#9171](https://github.com/RocketChat/Rocket.Chat/pull/9171) by [@karlprieb](https://github.com/karlprieb))

- Move emojipicker css to theme package ([#9243](https://github.com/RocketChat/Rocket.Chat/pull/9243) by [@karlprieb](https://github.com/karlprieb))

- Not sending email to mentioned users with unchanged preference ([#8059](https://github.com/RocketChat/Rocket.Chat/pull/8059))

- Notification is not sent when a video conference start ([#8828](https://github.com/RocketChat/Rocket.Chat/pull/8828) by [@deepseainside75](https://github.com/deepseainside75) & [@stefanoverducci](https://github.com/stefanoverducci))

- Notification sound is not disabling when busy ([#9042](https://github.com/RocketChat/Rocket.Chat/pull/9042))

- OTR buttons padding ([#7954](https://github.com/RocketChat/Rocket.Chat/pull/7954) by [@karlprieb](https://github.com/karlprieb))

- popover position on mobile ([#7883](https://github.com/RocketChat/Rocket.Chat/pull/7883) by [@karlprieb](https://github.com/karlprieb))

- Prevent autotranslate tokens race condition ([#8046](https://github.com/RocketChat/Rocket.Chat/pull/8046))

- Put delete action on another popover group ([#8315](https://github.com/RocketChat/Rocket.Chat/pull/8315) by [@karlprieb](https://github.com/karlprieb))

- Range Slider Value label has bug in RTL ([#8441](https://github.com/RocketChat/Rocket.Chat/pull/8441) by [@cyclops24](https://github.com/cyclops24))

- Recent emojis not updated when adding via text ([#7998](https://github.com/RocketChat/Rocket.Chat/pull/7998))

- remove accountBox from admin menu ([#8358](https://github.com/RocketChat/Rocket.Chat/pull/8358) by [@karlprieb](https://github.com/karlprieb))

- Remove break change in Realtime API ([#7895](https://github.com/RocketChat/Rocket.Chat/pull/7895))

- Remove sidebar header on admin embedded version ([#8334](https://github.com/RocketChat/Rocket.Chat/pull/8334) by [@karlprieb](https://github.com/karlprieb))

- REST API file upload not respecting size limit ([#9108](https://github.com/RocketChat/Rocket.Chat/pull/9108))

- RTL ([#8112](https://github.com/RocketChat/Rocket.Chat/pull/8112))

- Scroll on messagebox ([#8047](https://github.com/RocketChat/Rocket.Chat/pull/8047))

- Scrollbar not using new style ([#8190](https://github.com/RocketChat/Rocket.Chat/pull/8190))

- search results position on sidebar ([#7881](https://github.com/RocketChat/Rocket.Chat/pull/7881) by [@karlprieb](https://github.com/karlprieb))

- Set correct Twitter link ([#8830](https://github.com/RocketChat/Rocket.Chat/pull/8830) by [@jotafeldmann](https://github.com/jotafeldmann))

- Settings description not showing ([#8122](https://github.com/RocketChat/Rocket.Chat/pull/8122))

- Show leader on first load ([#7712](https://github.com/RocketChat/Rocket.Chat/pull/7712) by [@danischreiber](https://github.com/danischreiber))

- Show modal with announcement ([#9241](https://github.com/RocketChat/Rocket.Chat/pull/9241) by [@karlprieb](https://github.com/karlprieb))

- show oauth logins when adblock is used ([#9170](https://github.com/RocketChat/Rocket.Chat/pull/9170) by [@karlprieb](https://github.com/karlprieb))

- Show real name of current user at top of side nav if setting enabled ([#8718](https://github.com/RocketChat/Rocket.Chat/pull/8718) by [@alexbrazier](https://github.com/alexbrazier))

- Sidebar and RTL alignments ([#8154](https://github.com/RocketChat/Rocket.Chat/pull/8154) by [@karlprieb](https://github.com/karlprieb))

- sidebar buttons and badge paddings ([#7888](https://github.com/RocketChat/Rocket.Chat/pull/7888) by [@karlprieb](https://github.com/karlprieb))

- Sidebar item menu position in RTL ([#8397](https://github.com/RocketChat/Rocket.Chat/pull/8397) by [@cyclops24](https://github.com/cyclops24))

- sidebar paddings ([#7880](https://github.com/RocketChat/Rocket.Chat/pull/7880) by [@karlprieb](https://github.com/karlprieb))

- Slack import failing and not being able to be restarted ([#8390](https://github.com/RocketChat/Rocket.Chat/pull/8390))

- Small alignment fixes ([#7970](https://github.com/RocketChat/Rocket.Chat/pull/7970))

- snap install by setting grpc package used by google/vision to 1.6.6 ([#9029](https://github.com/RocketChat/Rocket.Chat/pull/9029))

- Snippetted messages not working ([#8937](https://github.com/RocketChat/Rocket.Chat/pull/8937) by [@karlprieb](https://github.com/karlprieb))

- Some UI problems on 0.60 ([#9095](https://github.com/RocketChat/Rocket.Chat/pull/9095) by [@karlprieb](https://github.com/karlprieb))

- Sort direct messages by full name if show real names setting enabled ([#8717](https://github.com/RocketChat/Rocket.Chat/pull/8717) by [@alexbrazier](https://github.com/alexbrazier))

- status and active room colors on sidebar ([#7960](https://github.com/RocketChat/Rocket.Chat/pull/7960) by [@karlprieb](https://github.com/karlprieb))

- Sync of non existent field throws exception ([#8006](https://github.com/RocketChat/Rocket.Chat/pull/8006) by [@goiaba](https://github.com/goiaba))

- Text area lost text when page reloads ([#8159](https://github.com/RocketChat/Rocket.Chat/pull/8159))

- TypeError: Cannot read property 't' of undefined ([#8298](https://github.com/RocketChat/Rocket.Chat/pull/8298))

- Typo Fix ([#8938](https://github.com/RocketChat/Rocket.Chat/pull/8938) by [@seangeleno](https://github.com/seangeleno))

- Uncessary route reload break some routes ([#8514](https://github.com/RocketChat/Rocket.Chat/pull/8514))

- Unread bar position when room have announcement ([#9188](https://github.com/RocketChat/Rocket.Chat/pull/9188) by [@karlprieb](https://github.com/karlprieb))

- Update insecure moment.js dependency ([#9046](https://github.com/RocketChat/Rocket.Chat/pull/9046) by [@robbyoconnor](https://github.com/robbyoconnor))

- Update pt-BR translation ([#8655](https://github.com/RocketChat/Rocket.Chat/pull/8655) by [@rodorgas](https://github.com/rodorgas))

- Update Rocket.Chat for sandstorm ([#9062](https://github.com/RocketChat/Rocket.Chat/pull/9062) by [@peterlee0127](https://github.com/peterlee0127))

- Update rocketchat:streamer to be compatible with previous version ([#9094](https://github.com/RocketChat/Rocket.Chat/pull/9094))

- Use encodeURI in AmazonS3 contentDisposition file.name to prevent fail ([#9024](https://github.com/RocketChat/Rocket.Chat/pull/9024) by [@paulovitin](https://github.com/paulovitin))

- User avatar in DM list. ([#8210](https://github.com/RocketChat/Rocket.Chat/pull/8210))

- User email settings on DM ([#8810](https://github.com/RocketChat/Rocket.Chat/pull/8810) by [@karlprieb](https://github.com/karlprieb))

- Username clipping on firefox ([#8716](https://github.com/RocketChat/Rocket.Chat/pull/8716) by [@karlprieb](https://github.com/karlprieb))

- username ellipsis on firefox ([#7953](https://github.com/RocketChat/Rocket.Chat/pull/7953) by [@karlprieb](https://github.com/karlprieb))

- Various LDAP issues & Missing pagination ([#8372](https://github.com/RocketChat/Rocket.Chat/pull/8372))

- Vertical menu on flex-tab ([#7988](https://github.com/RocketChat/Rocket.Chat/pull/7988) by [@karlprieb](https://github.com/karlprieb))

- Window exception when parsing Markdown on server ([#7893](https://github.com/RocketChat/Rocket.Chat/pull/7893))

- Wrong colors after migration 103 ([#8547](https://github.com/RocketChat/Rocket.Chat/pull/8547))

- Wrong file name when upload to AWS S3 ([#8296](https://github.com/RocketChat/Rocket.Chat/pull/8296))

- Wrong message when reseting password and 2FA is enabled ([#8489](https://github.com/RocketChat/Rocket.Chat/pull/8489))

- Wrong room counter name ([#9013](https://github.com/RocketChat/Rocket.Chat/pull/9013) by [@karlprieb](https://github.com/karlprieb))

- Xenforo [BD]API for 'user.user_id; instead of 'id' ([#8968](https://github.com/RocketChat/Rocket.Chat/pull/8968) by [@wesnspace](https://github.com/wesnspace))

<details>
<summary>🔍 Minor changes</summary>


- [DOCS] Add native mobile app links into README and update button images ([#7909](https://github.com/RocketChat/Rocket.Chat/pull/7909) by [@rafaelks](https://github.com/rafaelks))

- [FIX-RC] Mobile file upload not working ([#8331](https://github.com/RocketChat/Rocket.Chat/pull/8331) by [@karlprieb](https://github.com/karlprieb))

- [Fix] Store Outgoing Integration Result as String in Mongo ([#8413](https://github.com/RocketChat/Rocket.Chat/pull/8413) by [@cpitman](https://github.com/cpitman))

- [MOVE] Move archiveroom command to client/server folders ([#8140](https://github.com/RocketChat/Rocket.Chat/pull/8140) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move create command to client/server folder ([#8139](https://github.com/RocketChat/Rocket.Chat/pull/8139) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move favico to client folder ([#8077](https://github.com/RocketChat/Rocket.Chat/pull/8077) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move files from emojione to client/server folders ([#8078](https://github.com/RocketChat/Rocket.Chat/pull/8078) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move files from slashcommands-unarchive to client/server folders ([#8084](https://github.com/RocketChat/Rocket.Chat/pull/8084) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move invite command to client/server folder ([#8138](https://github.com/RocketChat/Rocket.Chat/pull/8138) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move inviteall command to client/server folder ([#8137](https://github.com/RocketChat/Rocket.Chat/pull/8137) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move join command to client/server folder ([#8136](https://github.com/RocketChat/Rocket.Chat/pull/8136) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move kick command to client/server folders ([#8135](https://github.com/RocketChat/Rocket.Chat/pull/8135) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move logger files to client/server folders ([#8150](https://github.com/RocketChat/Rocket.Chat/pull/8150) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move mentions files to client/server ([#8142](https://github.com/RocketChat/Rocket.Chat/pull/8142) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move slackbridge to client/server folders ([#8141](https://github.com/RocketChat/Rocket.Chat/pull/8141) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move slashcommands-open to client folder ([#8132](https://github.com/RocketChat/Rocket.Chat/pull/8132) by [@vcapretz](https://github.com/vcapretz))

- [MOVE] Move timesync files to client/server folders ([#8152](https://github.com/RocketChat/Rocket.Chat/pull/8152) by [@vcapretz](https://github.com/vcapretz))

- Add a few dots in readme.md ([#8906](https://github.com/RocketChat/Rocket.Chat/pull/8906) by [@dusta](https://github.com/dusta))

- Add curl, its missing on worker nodes so has to be explicitly added ([#9248](https://github.com/RocketChat/Rocket.Chat/pull/9248))

- Add i18n Title to snippet messages ([#8394](https://github.com/RocketChat/Rocket.Chat/pull/8394))

- Added d2c.io to deployment ([#8975](https://github.com/RocketChat/Rocket.Chat/pull/8975) by [@mastappl](https://github.com/mastappl))

- Added RocketChatLauncher (SaaS) ([#6606](https://github.com/RocketChat/Rocket.Chat/pull/6606) by [@designgurudotorg](https://github.com/designgurudotorg))

- Adding: How to Install in WeDeploy ([#8036](https://github.com/RocketChat/Rocket.Chat/pull/8036) by [@thompsonemerson](https://github.com/thompsonemerson))

- Bump version to 0.60.0-develop ([#8820](https://github.com/RocketChat/Rocket.Chat/pull/8820) by [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb))

- Change artifact path ([#8515](https://github.com/RocketChat/Rocket.Chat/pull/8515))

- Changed wording for "Maximum Allowed Message Size" ([#8872](https://github.com/RocketChat/Rocket.Chat/pull/8872) by [@HammyHavoc](https://github.com/HammyHavoc))

- Color variables migration ([#8463](https://github.com/RocketChat/Rocket.Chat/pull/8463) by [@karlprieb](https://github.com/karlprieb))

- Dependencies Update ([#9197](https://github.com/RocketChat/Rocket.Chat/pull/9197))

- Deps update ([#8273](https://github.com/RocketChat/Rocket.Chat/pull/8273))

- Develop sync ([#7866](https://github.com/RocketChat/Rocket.Chat/pull/7866))

- Do not change room icon color when room is unread ([#9257](https://github.com/RocketChat/Rocket.Chat/pull/9257))

- Enable AutoLinker back ([#8490](https://github.com/RocketChat/Rocket.Chat/pull/8490))

- Fix api regression (exception when deleting user) ([#9049](https://github.com/RocketChat/Rocket.Chat/pull/9049))

- Fix community links in readme ([#8589](https://github.com/RocketChat/Rocket.Chat/pull/8589))

- Fix Docker image build ([#8862](https://github.com/RocketChat/Rocket.Chat/pull/8862))

- Fix high CPU load when sending messages on large rooms (regression) ([#8520](https://github.com/RocketChat/Rocket.Chat/pull/8520))

- Fix link to .asc file on S3 ([#8829](https://github.com/RocketChat/Rocket.Chat/pull/8829))

- Fix more rtl issues ([#8194](https://github.com/RocketChat/Rocket.Chat/pull/8194) by [@karlprieb](https://github.com/karlprieb))

- Fix regression in api channels.members ([#9110](https://github.com/RocketChat/Rocket.Chat/pull/9110))

- Fix snap download url ([#8981](https://github.com/RocketChat/Rocket.Chat/pull/8981))

- Fix tag build ([#9084](https://github.com/RocketChat/Rocket.Chat/pull/9084))

- Fix test without oplog by waiting a successful login on changing users ([#9146](https://github.com/RocketChat/Rocket.Chat/pull/9146))

- Fix Travis CI build ([#8750](https://github.com/RocketChat/Rocket.Chat/pull/8750))

- Fix typo ([#8705](https://github.com/RocketChat/Rocket.Chat/pull/8705) by [@rmetzler](https://github.com/rmetzler))

- Fix: Account menu position on RTL ([#8416](https://github.com/RocketChat/Rocket.Chat/pull/8416) by [@karlprieb](https://github.com/karlprieb))

- Fix: Can’t login using LDAP via REST ([#9162](https://github.com/RocketChat/Rocket.Chat/pull/9162))

- Fix: Change password not working in new UI ([#8516](https://github.com/RocketChat/Rocket.Chat/pull/8516))

- Fix: Clear all unreads modal not closing after confirming ([#9137](https://github.com/RocketChat/Rocket.Chat/pull/9137))

- Fix: Click on channel name - hover area bigger than link area ([#9165](https://github.com/RocketChat/Rocket.Chat/pull/9165))

- Fix: Confirmation modals showing `Send` button ([#9136](https://github.com/RocketChat/Rocket.Chat/pull/9136))

- Fix: Message action quick buttons drops if "new message" divider is being shown ([#9138](https://github.com/RocketChat/Rocket.Chat/pull/9138))

- Fix: Messages being displayed in reverse order ([#9144](https://github.com/RocketChat/Rocket.Chat/pull/9144))

- Fix: Missing LDAP option to show internal logs ([#8417](https://github.com/RocketChat/Rocket.Chat/pull/8417))

- Fix: Missing LDAP reconnect setting ([#8414](https://github.com/RocketChat/Rocket.Chat/pull/8414))

- Fix: Missing option to set user's avatar from a url ([#9229](https://github.com/RocketChat/Rocket.Chat/pull/9229))

- Fix: Missing settings to configure LDAP size and page limits ([#8398](https://github.com/RocketChat/Rocket.Chat/pull/8398))

- Fix: Multiple unread indicators ([#9120](https://github.com/RocketChat/Rocket.Chat/pull/9120))

- Fix: Rooms and users are using different avatar style ([#9196](https://github.com/RocketChat/Rocket.Chat/pull/9196))

- Fix: Sidebar item on rtl and small devices ([#9247](https://github.com/RocketChat/Rocket.Chat/pull/9247) by [@karlprieb](https://github.com/karlprieb))

- Fix: Snippet name to not showing in snippet list ([#9184](https://github.com/RocketChat/Rocket.Chat/pull/9184) by [@karlprieb](https://github.com/karlprieb))

- Fix: UI: Descenders of glyphs are cut off ([#9166](https://github.com/RocketChat/Rocket.Chat/pull/9166))

- Fix: UI: Descenders of glyphs are cut off ([#9181](https://github.com/RocketChat/Rocket.Chat/pull/9181))

- Fix: Unneeded warning in payload of REST API calls ([#9240](https://github.com/RocketChat/Rocket.Chat/pull/9240))

- Fix: Unread line ([#9149](https://github.com/RocketChat/Rocket.Chat/pull/9149))

- Fix: updating last message on message edit or delete ([#9227](https://github.com/RocketChat/Rocket.Chat/pull/9227))

- Fix: Upload access control too distributed ([#9215](https://github.com/RocketChat/Rocket.Chat/pull/9215))

- Fix: Username find is matching partially ([#9217](https://github.com/RocketChat/Rocket.Chat/pull/9217))

- Fix: users listed as online after API login ([#9111](https://github.com/RocketChat/Rocket.Chat/pull/9111))

- Fix/api me only return verified ([#9183](https://github.com/RocketChat/Rocket.Chat/pull/9183))

- Hide flex-tab close button ([#7894](https://github.com/RocketChat/Rocket.Chat/pull/7894) by [@karlprieb](https://github.com/karlprieb))

- Improve markdown parser code ([#8451](https://github.com/RocketChat/Rocket.Chat/pull/8451))

- Improve room sync speed ([#8529](https://github.com/RocketChat/Rocket.Chat/pull/8529))

- install grpc package manually to fix snap armhf build ([#8653](https://github.com/RocketChat/Rocket.Chat/pull/8653))

- LingoHub based on develop ([#8831](https://github.com/RocketChat/Rocket.Chat/pull/8831))

- LingoHub based on develop ([#8375](https://github.com/RocketChat/Rocket.Chat/pull/8375))

- LingoHub based on develop ([#9256](https://github.com/RocketChat/Rocket.Chat/pull/9256))

- npm deps update ([#8197](https://github.com/RocketChat/Rocket.Chat/pull/8197))

- npm deps update ([#7969](https://github.com/RocketChat/Rocket.Chat/pull/7969))

- Release 0.60.0 ([#9259](https://github.com/RocketChat/Rocket.Chat/pull/9259))

- Remove chatops package ([#8742](https://github.com/RocketChat/Rocket.Chat/pull/8742))

- Remove field `lastActivity` from subscription data ([#8345](https://github.com/RocketChat/Rocket.Chat/pull/8345))

- Remove unnecessary returns in cors common ([#8054](https://github.com/RocketChat/Rocket.Chat/pull/8054) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- Removed tmeasday:crypto-md5 ([#8743](https://github.com/RocketChat/Rocket.Chat/pull/8743))

- removing a duplicate line ([#8434](https://github.com/RocketChat/Rocket.Chat/pull/8434) by [@vikaskedia](https://github.com/vikaskedia))

- Replace postcss-nesting with postcss-nested ([#9200](https://github.com/RocketChat/Rocket.Chat/pull/9200))

- Revert "npm deps update" ([#7983](https://github.com/RocketChat/Rocket.Chat/pull/7983))

- Sync translations from LingoHub ([#8363](https://github.com/RocketChat/Rocket.Chat/pull/8363))

- Turn off prettyJson if the node environment isn't development ([#9068](https://github.com/RocketChat/Rocket.Chat/pull/9068))

- Typo: German language file ([#9190](https://github.com/RocketChat/Rocket.Chat/pull/9190) by [@TheReal1604](https://github.com/TheReal1604))

- Update BlackDuck URL ([#7941](https://github.com/RocketChat/Rocket.Chat/pull/7941))

- Update DEMO to OPEN links ([#8793](https://github.com/RocketChat/Rocket.Chat/pull/8793))

- Update meteor package to 1.8.1 ([#8802](https://github.com/RocketChat/Rocket.Chat/pull/8802))

- Update Meteor to 1.5.2.2 ([#8364](https://github.com/RocketChat/Rocket.Chat/pull/8364))

- Update meteor to 1.5.2.2-rc.0 ([#8355](https://github.com/RocketChat/Rocket.Chat/pull/8355))

- Update multiple-instance-status package ([#9018](https://github.com/RocketChat/Rocket.Chat/pull/9018))

- Update path for s3 redirect in circle ci ([#8819](https://github.com/RocketChat/Rocket.Chat/pull/8819))

- Updated comments. ([#8719](https://github.com/RocketChat/Rocket.Chat/pull/8719) by [@jasonjyu](https://github.com/jasonjyu))

- Use real names for user and room in emails ([#7922](https://github.com/RocketChat/Rocket.Chat/pull/7922) by [@danischreiber](https://github.com/danischreiber))

- Use redhat official image with openshift ([#9007](https://github.com/RocketChat/Rocket.Chat/pull/9007))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AmShaegar13](https://github.com/AmShaegar13)
- [@Darkneon](https://github.com/Darkneon)
- [@HammyHavoc](https://github.com/HammyHavoc)
- [@Kiran-Rao](https://github.com/Kiran-Rao)
- [@Lawri-van-Buel](https://github.com/Lawri-van-Buel)
- [@Pharserror](https://github.com/Pharserror)
- [@RekkyRek](https://github.com/RekkyRek)
- [@Rzeszow](https://github.com/Rzeszow)
- [@TheReal1604](https://github.com/TheReal1604)
- [@aditya19496](https://github.com/aditya19496)
- [@alexbrazier](https://github.com/alexbrazier)
- [@armand1m](https://github.com/armand1m)
- [@arungalva](https://github.com/arungalva)
- [@ashward](https://github.com/ashward)
- [@astax-t](https://github.com/astax-t)
- [@ccfang](https://github.com/ccfang)
- [@cpitman](https://github.com/cpitman)
- [@cyclops24](https://github.com/cyclops24)
- [@danischreiber](https://github.com/danischreiber)
- [@deepseainside75](https://github.com/deepseainside75)
- [@designgurudotorg](https://github.com/designgurudotorg)
- [@dusta](https://github.com/dusta)
- [@gdelavald](https://github.com/gdelavald)
- [@ggrish](https://github.com/ggrish)
- [@goiaba](https://github.com/goiaba)
- [@icosamuel](https://github.com/icosamuel)
- [@jasonjyu](https://github.com/jasonjyu)
- [@joesitton](https://github.com/joesitton)
- [@josiasds](https://github.com/josiasds)
- [@jotafeldmann](https://github.com/jotafeldmann)
- [@jwilkins](https://github.com/jwilkins)
- [@karlprieb](https://github.com/karlprieb)
- [@lindoelio](https://github.com/lindoelio)
- [@luizbills](https://github.com/luizbills)
- [@mastappl](https://github.com/mastappl)
- [@mritunjaygoutam12](https://github.com/mritunjaygoutam12)
- [@mrsimpson](https://github.com/mrsimpson)
- [@paulovitin](https://github.com/paulovitin)
- [@peterlee0127](https://github.com/peterlee0127)
- [@pierreozoux](https://github.com/pierreozoux)
- [@pkgodara](https://github.com/pkgodara)
- [@rafaelks](https://github.com/rafaelks)
- [@ramrami](https://github.com/ramrami)
- [@rmetzler](https://github.com/rmetzler)
- [@robbyoconnor](https://github.com/robbyoconnor)
- [@rodorgas](https://github.com/rodorgas)
- [@ruKurz](https://github.com/ruKurz)
- [@ryoshimizu](https://github.com/ryoshimizu)
- [@sarbasamuel](https://github.com/sarbasamuel)
- [@satyapramodh](https://github.com/satyapramodh)
- [@seangeleno](https://github.com/seangeleno)
- [@selamanse](https://github.com/selamanse)
- [@stalley](https://github.com/stalley)
- [@stefanoverducci](https://github.com/stefanoverducci)
- [@szluohua](https://github.com/szluohua)
- [@thompsonemerson](https://github.com/thompsonemerson)
- [@torgeirl](https://github.com/torgeirl)
- [@vcapretz](https://github.com/vcapretz)
- [@vikaskedia](https://github.com/vikaskedia)
- [@vitor-nagao](https://github.com/vitor-nagao)
- [@wesnspace](https://github.com/wesnspace)
- [@wferris722](https://github.com/wferris722)
- [@xenithorb](https://github.com/xenithorb)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.59.6
`2017-11-29  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

<details>
<summary>🔍 Minor changes</summary>


- Fix tag build ([#8973](https://github.com/RocketChat/Rocket.Chat/pull/8973))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.59.5
`2017-11-29  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

<details>
<summary>🔍 Minor changes</summary>


- Fix CircleCI deploy filter ([#8972](https://github.com/RocketChat/Rocket.Chat/pull/8972))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.59.4
`2017-11-29  ·  1 🐛  ·  2 🔍  ·  5 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

### 🐛 Bug fixes


- Channel settings buttons ([#8753](https://github.com/RocketChat/Rocket.Chat/pull/8753) by [@karlprieb](https://github.com/karlprieb))

<details>
<summary>🔍 Minor changes</summary>


- Add CircleCI ([#8685](https://github.com/RocketChat/Rocket.Chat/pull/8685))

- Release/0.59.4 ([#8967](https://github.com/RocketChat/Rocket.Chat/pull/8967) by [@cpitman](https://github.com/cpitman) & [@karlprieb](https://github.com/karlprieb))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@cpitman](https://github.com/cpitman)
- [@karlprieb](https://github.com/karlprieb)

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.59.3
`2017-10-29  ·  7 🐛  ·  2 🔍  ·  8 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

### 🐛 Bug fixes


- AmazonS3: Quote file.name for ContentDisposition for files with commas ([#8593](https://github.com/RocketChat/Rocket.Chat/pull/8593) by [@xenithorb](https://github.com/xenithorb))

- Audio message icon ([#8648](https://github.com/RocketChat/Rocket.Chat/pull/8648) by [@karlprieb](https://github.com/karlprieb))

- Fix e-mail message forward ([#8645](https://github.com/RocketChat/Rocket.Chat/pull/8645))

- Fix typos ([#8679](https://github.com/RocketChat/Rocket.Chat/pull/8679))

- Highlighted color height issue ([#8431](https://github.com/RocketChat/Rocket.Chat/pull/8431) by [@cyclops24](https://github.com/cyclops24))

- LDAP not respecting UTF8 characters & Sync Interval not working ([#8691](https://github.com/RocketChat/Rocket.Chat/pull/8691))

- Update pt-BR translation ([#8655](https://github.com/RocketChat/Rocket.Chat/pull/8655) by [@rodorgas](https://github.com/rodorgas))

<details>
<summary>🔍 Minor changes</summary>


- install grpc package manually to fix snap armhf build ([#8653](https://github.com/RocketChat/Rocket.Chat/pull/8653))

- removing a duplicate line ([#8434](https://github.com/RocketChat/Rocket.Chat/pull/8434) by [@vikaskedia](https://github.com/vikaskedia))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@cyclops24](https://github.com/cyclops24)
- [@karlprieb](https://github.com/karlprieb)
- [@rodorgas](https://github.com/rodorgas)
- [@vikaskedia](https://github.com/vikaskedia)
- [@xenithorb](https://github.com/xenithorb)

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.59.2
`2017-10-25  ·  6 🐛  ·  4 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

### 🐛 Bug fixes


- API channel/group.members not sorting ([#8635](https://github.com/RocketChat/Rocket.Chat/pull/8635))

- encode filename in url to prevent links breaking ([#8551](https://github.com/RocketChat/Rocket.Chat/pull/8551) by [@joesitton](https://github.com/joesitton))

- Fix guest pool inquiry taking ([#8577](https://github.com/RocketChat/Rocket.Chat/pull/8577))

- LDAP not merging existent users && Wrong id link generation ([#8613](https://github.com/RocketChat/Rocket.Chat/pull/8613))

- Message popup menu on mobile/cordova ([#8634](https://github.com/RocketChat/Rocket.Chat/pull/8634) by [@karlprieb](https://github.com/karlprieb))

- Missing scroll at create channel page ([#8637](https://github.com/RocketChat/Rocket.Chat/pull/8637) by [@karlprieb](https://github.com/karlprieb))

### 👩‍💻👨‍💻 Contributors 😍

- [@joesitton](https://github.com/joesitton)
- [@karlprieb](https://github.com/karlprieb)

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.59.1
`2017-10-19  ·  4 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

### 🐛 Bug fixes


- Color reset when default value editor is different ([#8543](https://github.com/RocketChat/Rocket.Chat/pull/8543))

- LDAP login error regression at 0.59.0 ([#8541](https://github.com/RocketChat/Rocket.Chat/pull/8541))

- Migration 103 wrong converting primrary colors ([#8544](https://github.com/RocketChat/Rocket.Chat/pull/8544))

- Wrong colors after migration 103 ([#8547](https://github.com/RocketChat/Rocket.Chat/pull/8547))

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.59.0
`2017-10-18  ·  25 🎉  ·  122 🐛  ·  51 🔍  ·  46 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

### 🎉 New features


- Add classes to notification menu so they can be hidden in css ([#7636](https://github.com/RocketChat/Rocket.Chat/pull/7636) by [@danischreiber](https://github.com/danischreiber))

- Add markdown parser "marked" ([#7852](https://github.com/RocketChat/Rocket.Chat/pull/7852) by [@nishimaki10](https://github.com/nishimaki10))

- Add RD Station integration to livechat ([#8304](https://github.com/RocketChat/Rocket.Chat/pull/8304))

- Add room type as a class to the ul-group of rooms ([#7711](https://github.com/RocketChat/Rocket.Chat/pull/7711) by [@danischreiber](https://github.com/danischreiber))

- Add tags to uploaded images using Google Cloud Vision API ([#6301](https://github.com/RocketChat/Rocket.Chat/pull/6301) by [@karlprieb](https://github.com/karlprieb))

- Add unread options for direct messages ([#7658](https://github.com/RocketChat/Rocket.Chat/pull/7658))

- Adds a Keyboard Shortcut option to the flextab ([#5902](https://github.com/RocketChat/Rocket.Chat/pull/5902) by [@cnash](https://github.com/cnash) & [@karlprieb](https://github.com/karlprieb))

- Allow ldap mapping of customFields ([#7614](https://github.com/RocketChat/Rocket.Chat/pull/7614) by [@goiaba](https://github.com/goiaba))

- Allows admin to list all groups with API ([#7565](https://github.com/RocketChat/Rocket.Chat/pull/7565) by [@mboudet](https://github.com/mboudet))

- Audio Notification updated in sidebar ([#7817](https://github.com/RocketChat/Rocket.Chat/pull/7817) by [@aditya19496](https://github.com/aditya19496) & [@maarten-v](https://github.com/maarten-v))

- Automatically select the first channel ([#7350](https://github.com/RocketChat/Rocket.Chat/pull/7350) by [@antaryami-sahoo](https://github.com/antaryami-sahoo))

- block users to mention unknow users ([#7830](https://github.com/RocketChat/Rocket.Chat/pull/7830))

- Create a standard for our svg icons ([#7853](https://github.com/RocketChat/Rocket.Chat/pull/7853) by [@karlprieb](https://github.com/karlprieb))

- Enable read only channel creation ([#8260](https://github.com/RocketChat/Rocket.Chat/pull/8260) by [@karlprieb](https://github.com/karlprieb))

- Integrated personal email gateway (GSoC'17) ([#7342](https://github.com/RocketChat/Rocket.Chat/pull/7342) by [@pkgodara](https://github.com/pkgodara))

- make sidebar item width 100% ([#8362](https://github.com/RocketChat/Rocket.Chat/pull/8362) by [@karlprieb](https://github.com/karlprieb))

- Package to render issue numbers into links to an issue tracker. ([#6700](https://github.com/RocketChat/Rocket.Chat/pull/6700) by [@TAdeJong](https://github.com/TAdeJong) & [@TobiasKappe](https://github.com/TobiasKappe))

- Replace message cog for vertical menu ([#7864](https://github.com/RocketChat/Rocket.Chat/pull/7864) by [@karlprieb](https://github.com/karlprieb))

- Rocket.Chat UI Redesign ([#7643](https://github.com/RocketChat/Rocket.Chat/pull/7643))

- Search users by fields defined by admin ([#7612](https://github.com/RocketChat/Rocket.Chat/pull/7612) by [@goiaba](https://github.com/goiaba))

- Setting to disable MarkDown and enable AutoLinker ([#8459](https://github.com/RocketChat/Rocket.Chat/pull/8459))

- Smaller accountBox ([#8360](https://github.com/RocketChat/Rocket.Chat/pull/8360) by [@karlprieb](https://github.com/karlprieb))

- Template to show Custom Fields in user info view ([#7688](https://github.com/RocketChat/Rocket.Chat/pull/7688) by [@goiaba](https://github.com/goiaba))

- Unify unread and mentions badge ([#8361](https://github.com/RocketChat/Rocket.Chat/pull/8361) by [@karlprieb](https://github.com/karlprieb))

- Upgrade to meteor 1.5.2 ([#8073](https://github.com/RocketChat/Rocket.Chat/pull/8073))

### 🐛 Bug fixes


- "*.members" rest api being useless and only returning usernames ([#8147](https://github.com/RocketChat/Rocket.Chat/pull/8147))

- "Cancel button" on modal in RTL in Firefox 55 ([#8278](https://github.com/RocketChat/Rocket.Chat/pull/8278) by [@cyclops24](https://github.com/cyclops24))

- "Channel Setting" buttons alignment in RTL ([#8266](https://github.com/RocketChat/Rocket.Chat/pull/8266) by [@cyclops24](https://github.com/cyclops24))

- **i18n:** My Profile & README.md links ([#8270](https://github.com/RocketChat/Rocket.Chat/pull/8270) by [@Rzeszow](https://github.com/Rzeszow))

- **PL:** Polish translation ([#7989](https://github.com/RocketChat/Rocket.Chat/pull/7989) by [@Rzeszow](https://github.com/Rzeszow))

- Add admin audio preferences translations ([#8094](https://github.com/RocketChat/Rocket.Chat/pull/8094))

- Add CSS support for Safari versions > 7 ([#7854](https://github.com/RocketChat/Rocket.Chat/pull/7854))

- Add padding on messages to allow space to the action buttons ([#7971](https://github.com/RocketChat/Rocket.Chat/pull/7971))

- Adds default search text padding for emoji search ([#7878](https://github.com/RocketChat/Rocket.Chat/pull/7878) by [@gdelavald](https://github.com/gdelavald))

- After deleting the room, cache is not synchronizing ([#8314](https://github.com/RocketChat/Rocket.Chat/pull/8314) by [@szluohua](https://github.com/szluohua))

- Allow unknown file types if no allowed whitelist has been set (#7074) ([#8172](https://github.com/RocketChat/Rocket.Chat/pull/8172) by [@TriPhoenix](https://github.com/TriPhoenix))

- Amin menu not showing all items & File list breaking line ([#8299](https://github.com/RocketChat/Rocket.Chat/pull/8299) by [@karlprieb](https://github.com/karlprieb))

- Api groups.files is always returning empty ([#8241](https://github.com/RocketChat/Rocket.Chat/pull/8241))

- Attachment icons alignment in LTR and RTL ([#8271](https://github.com/RocketChat/Rocket.Chat/pull/8271) by [@cyclops24](https://github.com/cyclops24))

- Broken embedded view layout ([#7944](https://github.com/RocketChat/Rocket.Chat/pull/7944) by [@karlprieb](https://github.com/karlprieb))

- Broken emoji picker on firefox ([#7943](https://github.com/RocketChat/Rocket.Chat/pull/7943) by [@karlprieb](https://github.com/karlprieb))

- Call buttons with wrong margin on RTL ([#8307](https://github.com/RocketChat/Rocket.Chat/pull/8307) by [@karlprieb](https://github.com/karlprieb))

- Case insensitive SAML email check ([#8216](https://github.com/RocketChat/Rocket.Chat/pull/8216) by [@arminfelder](https://github.com/arminfelder))

- Chat box no longer auto-focuses when typing ([#7984](https://github.com/RocketChat/Rocket.Chat/pull/7984))

- Check attachments is defined before accessing first element ([#8295](https://github.com/RocketChat/Rocket.Chat/pull/8295) by [@Darkneon](https://github.com/Darkneon))

- clipboard and permalink on new popover ([#8259](https://github.com/RocketChat/Rocket.Chat/pull/8259) by [@karlprieb](https://github.com/karlprieb))

- copy to clipboard and update clipboard.js library ([#8039](https://github.com/RocketChat/Rocket.Chat/pull/8039) by [@karlprieb](https://github.com/karlprieb))

- Create channel button on Firefox ([#7942](https://github.com/RocketChat/Rocket.Chat/pull/7942) by [@karlprieb](https://github.com/karlprieb))

- Csv importer: work with more problematic data ([#7456](https://github.com/RocketChat/Rocket.Chat/pull/7456) by [@reist](https://github.com/reist))

- disabled katex tooltip on messageBox ([#8386](https://github.com/RocketChat/Rocket.Chat/pull/8386))

- Do not send joinCode field to clients ([#8527](https://github.com/RocketChat/Rocket.Chat/pull/8527))

- Document README.md. Drupal repo out of date ([#7948](https://github.com/RocketChat/Rocket.Chat/pull/7948) by [@Lawri-van-Buel](https://github.com/Lawri-van-Buel))

- Double scroll on 'keyboard shortcuts' menu in sidepanel ([#7927](https://github.com/RocketChat/Rocket.Chat/pull/7927) by [@aditya19496](https://github.com/aditya19496))

- Dutch translations ([#7815](https://github.com/RocketChat/Rocket.Chat/pull/7815) by [@maarten-v](https://github.com/maarten-v))

- Dynamic popover ([#8101](https://github.com/RocketChat/Rocket.Chat/pull/8101) by [@karlprieb](https://github.com/karlprieb))

- Email message forward error ([#7846](https://github.com/RocketChat/Rocket.Chat/pull/7846))

- Email Subjects not being sent ([#8317](https://github.com/RocketChat/Rocket.Chat/pull/8317))

- Emoji Picker hidden for reactions in RTL ([#8300](https://github.com/RocketChat/Rocket.Chat/pull/8300) by [@karlprieb](https://github.com/karlprieb))

- Error when translating message ([#8001](https://github.com/RocketChat/Rocket.Chat/pull/8001))

- Example usage of unsubscribe.js ([#7673](https://github.com/RocketChat/Rocket.Chat/pull/7673) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- Execute meteor reset on TRAVIS_TAG builds ([#8310](https://github.com/RocketChat/Rocket.Chat/pull/8310))

- File upload on multi-instances using a path prefix ([#7855](https://github.com/RocketChat/Rocket.Chat/pull/7855) by [@Darkneon](https://github.com/Darkneon))

- Fix avatar upload fail on Cordova app ([#7656](https://github.com/RocketChat/Rocket.Chat/pull/7656) by [@ccfang](https://github.com/ccfang))

- Fix black background on transparent avatars ([#7168](https://github.com/RocketChat/Rocket.Chat/pull/7168))

- fix color on unread messages ([#8282](https://github.com/RocketChat/Rocket.Chat/pull/8282))

- Fix Dutch translation ([#7814](https://github.com/RocketChat/Rocket.Chat/pull/7814) by [@maarten-v](https://github.com/maarten-v))

- Fix email on mention ([#7754](https://github.com/RocketChat/Rocket.Chat/pull/7754))

- Fix google play logo on repo README ([#7912](https://github.com/RocketChat/Rocket.Chat/pull/7912) by [@luizbills](https://github.com/luizbills))

- Fix iframe login API response (issue #8145) ([#8146](https://github.com/RocketChat/Rocket.Chat/pull/8146) by [@astax-t](https://github.com/astax-t))

- Fix livechat toggle UI issue ([#7904](https://github.com/RocketChat/Rocket.Chat/pull/7904))

- Fix messagebox growth ([#7629](https://github.com/RocketChat/Rocket.Chat/pull/7629))

- Fix migration 100 ([#7863](https://github.com/RocketChat/Rocket.Chat/pull/7863))

- Fix new room sound being played too much ([#8144](https://github.com/RocketChat/Rocket.Chat/pull/8144))

- Fix new-message button showing on search ([#7823](https://github.com/RocketChat/Rocket.Chat/pull/7823))

- Fix placeholders in account profile ([#7945](https://github.com/RocketChat/Rocket.Chat/pull/7945) by [@josiasds](https://github.com/josiasds))

- Fix room load on first hit ([#7687](https://github.com/RocketChat/Rocket.Chat/pull/7687))

- Fix setting user avatar on LDAP login ([#8099](https://github.com/RocketChat/Rocket.Chat/pull/8099))

- Fix the status on the members list ([#7963](https://github.com/RocketChat/Rocket.Chat/pull/7963))

- Fixed function closure syntax allowing validation emails to be sent. ([#7758](https://github.com/RocketChat/Rocket.Chat/pull/7758) by [@snoozan](https://github.com/snoozan))

- Google vision NSFW tag ([#7825](https://github.com/RocketChat/Rocket.Chat/pull/7825))

- Hide scrollbar on login page if not necessary ([#8014](https://github.com/RocketChat/Rocket.Chat/pull/8014) by [@alexbrazier](https://github.com/alexbrazier))

- hyperlink style on sidebar footer ([#7882](https://github.com/RocketChat/Rocket.Chat/pull/7882) by [@karlprieb](https://github.com/karlprieb))

- Incorrect URL for login terms when using prefix ([#8211](https://github.com/RocketChat/Rocket.Chat/pull/8211) by [@Darkneon](https://github.com/Darkneon))

- Invalid Code message for password protected channel ([#8491](https://github.com/RocketChat/Rocket.Chat/pull/8491))

- Invisible leader bar on hover ([#8048](https://github.com/RocketChat/Rocket.Chat/pull/8048))

- Issue #8166 where empty analytics setting breaks to load Piwik script ([#8167](https://github.com/RocketChat/Rocket.Chat/pull/8167) by [@ruKurz](https://github.com/ruKurz))

- LDAP memory issues when pagination is not available ([#8457](https://github.com/RocketChat/Rocket.Chat/pull/8457))

- Leave and hide buttons was removed ([#8213](https://github.com/RocketChat/Rocket.Chat/pull/8213) by [@karlprieb](https://github.com/karlprieb))

- livechat icon ([#7886](https://github.com/RocketChat/Rocket.Chat/pull/7886) by [@karlprieb](https://github.com/karlprieb))

- Make link inside YouTube preview open in new tab ([#7679](https://github.com/RocketChat/Rocket.Chat/pull/7679) by [@1lann](https://github.com/1lann))

- make sidebar item animation fast ([#8262](https://github.com/RocketChat/Rocket.Chat/pull/8262) by [@karlprieb](https://github.com/karlprieb))

- Makes text action menu width based on content size ([#7887](https://github.com/RocketChat/Rocket.Chat/pull/7887) by [@gdelavald](https://github.com/gdelavald))

- Markdown being rendered in code tags ([#7965](https://github.com/RocketChat/Rocket.Chat/pull/7965))

- Markdown noopener/noreferrer: use correct HTML attribute ([#7644](https://github.com/RocketChat/Rocket.Chat/pull/7644) by [@jangmarker](https://github.com/jangmarker))

- Mention unread indicator was removed ([#8316](https://github.com/RocketChat/Rocket.Chat/pull/8316))

- message actions over unread bar ([#7885](https://github.com/RocketChat/Rocket.Chat/pull/7885) by [@karlprieb](https://github.com/karlprieb))

- message-box autogrow ([#8019](https://github.com/RocketChat/Rocket.Chat/pull/8019) by [@karlprieb](https://github.com/karlprieb))

- meteor-accounts-saml issue with ns0,ns1 namespaces, makes it compatible with pysaml2 lib ([#7721](https://github.com/RocketChat/Rocket.Chat/pull/7721) by [@arminfelder](https://github.com/arminfelder))

- Missing i18n translations ([#8357](https://github.com/RocketChat/Rocket.Chat/pull/8357))

- Missing placeholder translations ([#8286](https://github.com/RocketChat/Rocket.Chat/pull/8286))

- Not sending email to mentioned users with unchanged preference ([#8059](https://github.com/RocketChat/Rocket.Chat/pull/8059))

- OTR buttons padding ([#7954](https://github.com/RocketChat/Rocket.Chat/pull/7954) by [@karlprieb](https://github.com/karlprieb))

- popover position on mobile ([#7883](https://github.com/RocketChat/Rocket.Chat/pull/7883) by [@karlprieb](https://github.com/karlprieb))

- Prevent autotranslate tokens race condition ([#8046](https://github.com/RocketChat/Rocket.Chat/pull/8046))

- Put delete action on another popover group ([#8315](https://github.com/RocketChat/Rocket.Chat/pull/8315) by [@karlprieb](https://github.com/karlprieb))

- Recent emojis not updated when adding via text ([#7998](https://github.com/RocketChat/Rocket.Chat/pull/7998))

- remove accountBox from admin menu ([#8358](https://github.com/RocketChat/Rocket.Chat/pull/8358) by [@karlprieb](https://github.com/karlprieb))

- Remove break change in Realtime API ([#7895](https://github.com/RocketChat/Rocket.Chat/pull/7895))

- Remove redundant "do" in "Are you sure ...?" messages. ([#7809](https://github.com/RocketChat/Rocket.Chat/pull/7809) by [@xurizaemon](https://github.com/xurizaemon))

- Remove references to non-existent tests ([#7672](https://github.com/RocketChat/Rocket.Chat/pull/7672) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- Remove sidebar header on admin embedded version ([#8334](https://github.com/RocketChat/Rocket.Chat/pull/8334) by [@karlprieb](https://github.com/karlprieb))

- Removing pipe and commas from custom emojis (#8168) ([#8237](https://github.com/RocketChat/Rocket.Chat/pull/8237) by [@matheusml](https://github.com/matheusml))

- room icon on header ([#8017](https://github.com/RocketChat/Rocket.Chat/pull/8017) by [@karlprieb](https://github.com/karlprieb))

- RTL ([#8112](https://github.com/RocketChat/Rocket.Chat/pull/8112))

- RTL on reply ([#8261](https://github.com/RocketChat/Rocket.Chat/pull/8261) by [@karlprieb](https://github.com/karlprieb))

- scroll on flex-tab ([#7748](https://github.com/RocketChat/Rocket.Chat/pull/7748))

- Scroll on messagebox ([#8047](https://github.com/RocketChat/Rocket.Chat/pull/8047))

- Scrollbar not using new style ([#8190](https://github.com/RocketChat/Rocket.Chat/pull/8190))

- search results height ([#8018](https://github.com/RocketChat/Rocket.Chat/pull/8018) by [@gdelavald](https://github.com/gdelavald) & [@karlprieb](https://github.com/karlprieb))

- search results position on sidebar ([#7881](https://github.com/RocketChat/Rocket.Chat/pull/7881) by [@karlprieb](https://github.com/karlprieb))

- Settings description not showing ([#8122](https://github.com/RocketChat/Rocket.Chat/pull/8122))

- Settings not getting applied from Meteor.settings and process.env  ([#7779](https://github.com/RocketChat/Rocket.Chat/pull/7779) by [@Darkneon](https://github.com/Darkneon))

- Show leader on first load ([#7712](https://github.com/RocketChat/Rocket.Chat/pull/7712) by [@danischreiber](https://github.com/danischreiber))

- Sidebar and RTL alignments ([#8154](https://github.com/RocketChat/Rocket.Chat/pull/8154) by [@karlprieb](https://github.com/karlprieb))

- sidebar buttons and badge paddings ([#7888](https://github.com/RocketChat/Rocket.Chat/pull/7888) by [@karlprieb](https://github.com/karlprieb))

- Sidebar item menu position in RTL ([#8397](https://github.com/RocketChat/Rocket.Chat/pull/8397) by [@cyclops24](https://github.com/cyclops24))

- sidebar paddings ([#7880](https://github.com/RocketChat/Rocket.Chat/pull/7880) by [@karlprieb](https://github.com/karlprieb))

- sidenav colors, hide and leave, create channel on safari ([#8257](https://github.com/RocketChat/Rocket.Chat/pull/8257) by [@karlprieb](https://github.com/karlprieb))

- sidenav mentions on hover ([#8252](https://github.com/RocketChat/Rocket.Chat/pull/8252) by [@karlprieb](https://github.com/karlprieb))

- Small alignment fixes ([#7970](https://github.com/RocketChat/Rocket.Chat/pull/7970))

- some placeholder and phrase traslation fix ([#8269](https://github.com/RocketChat/Rocket.Chat/pull/8269) by [@cyclops24](https://github.com/cyclops24))

- status and active room colors on sidebar ([#7960](https://github.com/RocketChat/Rocket.Chat/pull/7960) by [@karlprieb](https://github.com/karlprieb))

- Text area buttons and layout on mobile  ([#7985](https://github.com/RocketChat/Rocket.Chat/pull/7985))

- Text area lost text when page reloads ([#8159](https://github.com/RocketChat/Rocket.Chat/pull/8159))

- Textarea on firefox ([#7986](https://github.com/RocketChat/Rocket.Chat/pull/7986))

- TypeError: Cannot read property 't' of undefined ([#8298](https://github.com/RocketChat/Rocket.Chat/pull/8298))

- Uncessary route reload break some routes ([#8514](https://github.com/RocketChat/Rocket.Chat/pull/8514))

- Update Snap links ([#7778](https://github.com/RocketChat/Rocket.Chat/pull/7778) by [@MichaelGooden](https://github.com/MichaelGooden))

- User avatar in DM list. ([#8210](https://github.com/RocketChat/Rocket.Chat/pull/8210))

- username ellipsis on firefox ([#7953](https://github.com/RocketChat/Rocket.Chat/pull/7953) by [@karlprieb](https://github.com/karlprieb))

- Various LDAP issues & Missing pagination ([#8372](https://github.com/RocketChat/Rocket.Chat/pull/8372))

- Vertical menu on flex-tab ([#7988](https://github.com/RocketChat/Rocket.Chat/pull/7988) by [@karlprieb](https://github.com/karlprieb))

- Window exception when parsing Markdown on server ([#7893](https://github.com/RocketChat/Rocket.Chat/pull/7893))

- Wrong email subject when "All Messages" setting enabled ([#7639](https://github.com/RocketChat/Rocket.Chat/pull/7639))

- Wrong file name when upload to AWS S3 ([#8296](https://github.com/RocketChat/Rocket.Chat/pull/8296))

- Wrong message when reseting password and 2FA is enabled ([#8489](https://github.com/RocketChat/Rocket.Chat/pull/8489))

- Wrong render of snippet’s name ([#7630](https://github.com/RocketChat/Rocket.Chat/pull/7630))

<details>
<summary>🔍 Minor changes</summary>


- [DOCS] Add native mobile app links into README and update button images ([#7909](https://github.com/RocketChat/Rocket.Chat/pull/7909) by [@rafaelks](https://github.com/rafaelks))

- [FIX-RC] Mobile file upload not working ([#8331](https://github.com/RocketChat/Rocket.Chat/pull/8331) by [@karlprieb](https://github.com/karlprieb))

- [MOVE] Client folder rocketchat-autolinker ([#7667](https://github.com/RocketChat/Rocket.Chat/pull/7667) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- [MOVE] Client folder rocketchat-cas ([#7668](https://github.com/RocketChat/Rocket.Chat/pull/7668) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- [MOVE] Client folder rocketchat-colors ([#7664](https://github.com/RocketChat/Rocket.Chat/pull/7664) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- [MOVE] Client folder rocketchat-custom-oauth ([#7665](https://github.com/RocketChat/Rocket.Chat/pull/7665) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- [MOVE] Client folder rocketchat-custom-sounds ([#7670](https://github.com/RocketChat/Rocket.Chat/pull/7670) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- [MOVE] Client folder rocketchat-emoji ([#7671](https://github.com/RocketChat/Rocket.Chat/pull/7671) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- [MOVE] Client folder rocketchat-highlight-words ([#7669](https://github.com/RocketChat/Rocket.Chat/pull/7669) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- [MOVE] Client folder rocketchat-tooltip ([#7666](https://github.com/RocketChat/Rocket.Chat/pull/7666) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- 0.58.3 ([#8335](https://github.com/RocketChat/Rocket.Chat/pull/8335))

- Add i18n Title to snippet messages ([#8394](https://github.com/RocketChat/Rocket.Chat/pull/8394))

- Additions to the REST API ([#7793](https://github.com/RocketChat/Rocket.Chat/pull/7793))

- Bump version to 0.59.0-develop ([#7625](https://github.com/RocketChat/Rocket.Chat/pull/7625))

- Change artifact path ([#8515](https://github.com/RocketChat/Rocket.Chat/pull/8515))

- Color variables migration ([#8463](https://github.com/RocketChat/Rocket.Chat/pull/8463) by [@karlprieb](https://github.com/karlprieb))

- Deps update ([#8273](https://github.com/RocketChat/Rocket.Chat/pull/8273))

- Disable perfect scrollbar ([#8244](https://github.com/RocketChat/Rocket.Chat/pull/8244))

- Enable AutoLinker back ([#8490](https://github.com/RocketChat/Rocket.Chat/pull/8490))

- Fix `leave and hide` click, color and position ([#8243](https://github.com/RocketChat/Rocket.Chat/pull/8243) by [@karlprieb](https://github.com/karlprieb))

- Fix artifact path ([#8518](https://github.com/RocketChat/Rocket.Chat/pull/8518))

- Fix high CPU load when sending messages on large rooms (regression) ([#8520](https://github.com/RocketChat/Rocket.Chat/pull/8520))

- Fix more rtl issues ([#8194](https://github.com/RocketChat/Rocket.Chat/pull/8194) by [@karlprieb](https://github.com/karlprieb))

- Fix typo in generated URI ([#7661](https://github.com/RocketChat/Rocket.Chat/pull/7661) by [@Rohlik](https://github.com/Rohlik))

- Fix: Account menu position on RTL ([#8416](https://github.com/RocketChat/Rocket.Chat/pull/8416) by [@karlprieb](https://github.com/karlprieb))

- Fix: Change password not working in new UI ([#8516](https://github.com/RocketChat/Rocket.Chat/pull/8516))

- FIX: Error when starting local development environment ([#7728](https://github.com/RocketChat/Rocket.Chat/pull/7728) by [@rdebeasi](https://github.com/rdebeasi))

- Fix: Missing LDAP option to show internal logs ([#8417](https://github.com/RocketChat/Rocket.Chat/pull/8417))

- Fix: Missing LDAP reconnect setting ([#8414](https://github.com/RocketChat/Rocket.Chat/pull/8414))

- Fix: Missing settings to configure LDAP size and page limits ([#8398](https://github.com/RocketChat/Rocket.Chat/pull/8398))

- Hide flex-tab close button ([#7894](https://github.com/RocketChat/Rocket.Chat/pull/7894) by [@karlprieb](https://github.com/karlprieb))

- implemented new page-loader animated icon ([#2](https://github.com/RocketChat/Rocket.Chat/pull/2))

- Improve markdown parser code ([#8451](https://github.com/RocketChat/Rocket.Chat/pull/8451))

- Improve room sync speed ([#8529](https://github.com/RocketChat/Rocket.Chat/pull/8529))

- LingoHub based on develop ([#7803](https://github.com/RocketChat/Rocket.Chat/pull/7803))

- LingoHub based on develop ([#8375](https://github.com/RocketChat/Rocket.Chat/pull/8375))

- Merge 0.58.4 to master ([#8420](https://github.com/RocketChat/Rocket.Chat/pull/8420))

- Meteor packages and npm dependencies update ([#7677](https://github.com/RocketChat/Rocket.Chat/pull/7677))

- Mobile sidenav ([#7865](https://github.com/RocketChat/Rocket.Chat/pull/7865))

- npm deps update ([#7842](https://github.com/RocketChat/Rocket.Chat/pull/7842))

- npm deps update ([#7755](https://github.com/RocketChat/Rocket.Chat/pull/7755))

- npm deps update ([#8197](https://github.com/RocketChat/Rocket.Chat/pull/8197))

- Only use "File Uploaded" prefix on files ([#7652](https://github.com/RocketChat/Rocket.Chat/pull/7652))

- readme-file: fix broken link ([#8253](https://github.com/RocketChat/Rocket.Chat/pull/8253) by [@vcapretz](https://github.com/vcapretz))

- Remove CircleCI ([#7739](https://github.com/RocketChat/Rocket.Chat/pull/7739))

- Remove field `lastActivity` from subscription data ([#8345](https://github.com/RocketChat/Rocket.Chat/pull/8345))

- Remove unnecessary returns in cors common ([#8054](https://github.com/RocketChat/Rocket.Chat/pull/8054) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- Sync translations from LingoHub ([#8363](https://github.com/RocketChat/Rocket.Chat/pull/8363))

- Update BlackDuck URL ([#7941](https://github.com/RocketChat/Rocket.Chat/pull/7941))

- Update Meteor to 1.5.2.2 ([#8364](https://github.com/RocketChat/Rocket.Chat/pull/8364))

- Update meteor to 1.5.2.2-rc.0 ([#8355](https://github.com/RocketChat/Rocket.Chat/pull/8355))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@1lann](https://github.com/1lann)
- [@Darkneon](https://github.com/Darkneon)
- [@Kiran-Rao](https://github.com/Kiran-Rao)
- [@Lawri-van-Buel](https://github.com/Lawri-van-Buel)
- [@MichaelGooden](https://github.com/MichaelGooden)
- [@Rohlik](https://github.com/Rohlik)
- [@Rzeszow](https://github.com/Rzeszow)
- [@TAdeJong](https://github.com/TAdeJong)
- [@TobiasKappe](https://github.com/TobiasKappe)
- [@TriPhoenix](https://github.com/TriPhoenix)
- [@aditya19496](https://github.com/aditya19496)
- [@alexbrazier](https://github.com/alexbrazier)
- [@antaryami-sahoo](https://github.com/antaryami-sahoo)
- [@arminfelder](https://github.com/arminfelder)
- [@astax-t](https://github.com/astax-t)
- [@ccfang](https://github.com/ccfang)
- [@cnash](https://github.com/cnash)
- [@cyclops24](https://github.com/cyclops24)
- [@danischreiber](https://github.com/danischreiber)
- [@gdelavald](https://github.com/gdelavald)
- [@goiaba](https://github.com/goiaba)
- [@jangmarker](https://github.com/jangmarker)
- [@josiasds](https://github.com/josiasds)
- [@karlprieb](https://github.com/karlprieb)
- [@luizbills](https://github.com/luizbills)
- [@maarten-v](https://github.com/maarten-v)
- [@matheusml](https://github.com/matheusml)
- [@mboudet](https://github.com/mboudet)
- [@nishimaki10](https://github.com/nishimaki10)
- [@pkgodara](https://github.com/pkgodara)
- [@rafaelks](https://github.com/rafaelks)
- [@rdebeasi](https://github.com/rdebeasi)
- [@reist](https://github.com/reist)
- [@ruKurz](https://github.com/ruKurz)
- [@snoozan](https://github.com/snoozan)
- [@szluohua](https://github.com/szluohua)
- [@vcapretz](https://github.com/vcapretz)
- [@xurizaemon](https://github.com/xurizaemon)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.58.4
`2017-10-05  ·  3 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

### 🐛 Bug fixes


- Add needed dependency for snaps ([#8389](https://github.com/RocketChat/Rocket.Chat/pull/8389))

- Duplicate code in rest api letting in a few bugs with the rest api ([#8408](https://github.com/RocketChat/Rocket.Chat/pull/8408))

- Slack import failing and not being able to be restarted ([#8390](https://github.com/RocketChat/Rocket.Chat/pull/8390))

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@graywolf336](https://github.com/graywolf336)

# 0.58.2
`2017-08-22  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

<details>
<summary>🔍 Minor changes</summary>


- Release 0.58.2 ([#7841](https://github.com/RocketChat/Rocket.Chat/pull/7841) by [@snoozan](https://github.com/snoozan))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@snoozan](https://github.com/snoozan)

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)

# 0.58.1
`2017-08-17  ·  1 🐛  ·  1 🔍  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

### 🐛 Bug fixes


- Fix flex tab not opening and getting offscreen ([#7781](https://github.com/RocketChat/Rocket.Chat/pull/7781))

<details>
<summary>🔍 Minor changes</summary>


- Release 0.58.1 ([#7782](https://github.com/RocketChat/Rocket.Chat/pull/7782))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@rodrigok](https://github.com/rodrigok)

# 0.58.0
`2017-08-16  ·  1 ️️️⚠️  ·  27 🎉  ·  48 🐛  ·  19 🔍  ·  32 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.4`
- NPM: `4.6.1`

### ⚠️ BREAKING CHANGES


- Remove Sandstorm login method ([#7556](https://github.com/RocketChat/Rocket.Chat/pull/7556))

### 🎉 New features


- Add admin and user setting for notifications #4339 ([#7479](https://github.com/RocketChat/Rocket.Chat/pull/7479) by [@stalley](https://github.com/stalley))

- Add close button to flex tabs ([#7529](https://github.com/RocketChat/Rocket.Chat/pull/7529))

- Add customFields in rooms/get method ([#6564](https://github.com/RocketChat/Rocket.Chat/pull/6564) by [@borsden](https://github.com/borsden))

- Add healthchecks in OpenShift templates ([#7184](https://github.com/RocketChat/Rocket.Chat/pull/7184) by [@jfchevrette](https://github.com/jfchevrette))

- Add reaction to the last message when get the shortcut +: ([#7569](https://github.com/RocketChat/Rocket.Chat/pull/7569) by [@danilomiranda](https://github.com/danilomiranda))

- Add room type identifier to room list header ([#7520](https://github.com/RocketChat/Rocket.Chat/pull/7520) by [@danischreiber](https://github.com/danischreiber))

- Add setting to change User Agent of OEmbed calls ([#6753](https://github.com/RocketChat/Rocket.Chat/pull/6753) by [@AhmetS](https://github.com/AhmetS))

- Add toolbar buttons for iframe API ([#7525](https://github.com/RocketChat/Rocket.Chat/pull/7525))

- Add unread options for direct messages ([#7658](https://github.com/RocketChat/Rocket.Chat/pull/7658))

- Adding support for piwik sub domain settings ([#7324](https://github.com/RocketChat/Rocket.Chat/pull/7324) by [@ruKurz](https://github.com/ruKurz))

- Adds preference to one-click-to-direct-message and basic functionality ([#7564](https://github.com/RocketChat/Rocket.Chat/pull/7564) by [@gdelavald](https://github.com/gdelavald))

- Allow channel property in the integrations returned content ([#7214](https://github.com/RocketChat/Rocket.Chat/pull/7214))

- Allow special chars on room names ([#7595](https://github.com/RocketChat/Rocket.Chat/pull/7595))

- Closes tab bar on mobile when leaving room ([#7561](https://github.com/RocketChat/Rocket.Chat/pull/7561) by [@gdelavald](https://github.com/gdelavald))

- Configurable Volume for Notifications #6087 ([#7517](https://github.com/RocketChat/Rocket.Chat/pull/7517) by [@lindoelio](https://github.com/lindoelio))

- Do not rate limit bots on createDirectMessage ([#7326](https://github.com/RocketChat/Rocket.Chat/pull/7326) by [@jangmarker](https://github.com/jangmarker))

- Edit user permissions ([#7309](https://github.com/RocketChat/Rocket.Chat/pull/7309))

- flex-tab now is side by side with message list ([#7448](https://github.com/RocketChat/Rocket.Chat/pull/7448) by [@karlprieb](https://github.com/karlprieb))

- Force use of MongoDB for spotlight queries ([#7311](https://github.com/RocketChat/Rocket.Chat/pull/7311))

- Option to select unread count behavior ([#7477](https://github.com/RocketChat/Rocket.Chat/pull/7477))

- Option to select unread count style ([#7589](https://github.com/RocketChat/Rocket.Chat/pull/7589))

- Room type and recipient data for global event ([#7523](https://github.com/RocketChat/Rocket.Chat/pull/7523) by [@danischreiber](https://github.com/danischreiber))

- Search users also by email in toolbar ([#7334](https://github.com/RocketChat/Rocket.Chat/pull/7334) by [@shahar3012](https://github.com/shahar3012))

- Show different shape for alert numbers when have mentions ([#7580](https://github.com/RocketChat/Rocket.Chat/pull/7580))

- Show emojis and file uploads on notifications ([#7559](https://github.com/RocketChat/Rocket.Chat/pull/7559))

- Show room leader at top of chat when user scrolls down. Set and unset leader as admin. ([#7526](https://github.com/RocketChat/Rocket.Chat/pull/7526) by [@danischreiber](https://github.com/danischreiber))

- Update meteor to 1.5.1 ([#7496](https://github.com/RocketChat/Rocket.Chat/pull/7496))

### 🐛 Bug fixes


- "requirePasswordChange" property not being saved when set to false ([#7209](https://github.com/RocketChat/Rocket.Chat/pull/7209))

- Add needed dependency for snaps ([#8389](https://github.com/RocketChat/Rocket.Chat/pull/8389))

- Always set LDAP properties on login ([#7472](https://github.com/RocketChat/Rocket.Chat/pull/7472))

- Csv importer: work with more problematic data ([#7456](https://github.com/RocketChat/Rocket.Chat/pull/7456) by [@reist](https://github.com/reist))

- Duplicate code in rest api letting in a few bugs with the rest api ([#8408](https://github.com/RocketChat/Rocket.Chat/pull/8408))

- Error when acessing settings before ready ([#7622](https://github.com/RocketChat/Rocket.Chat/pull/7622))

- Error when updating message with an empty attachment array ([#7624](https://github.com/RocketChat/Rocket.Chat/pull/7624))

- Fix admin room list show the correct i18n type ([#7582](https://github.com/RocketChat/Rocket.Chat/pull/7582) by [@ccfang](https://github.com/ccfang))

- Fix Block Delete Message After (n) Minutes ([#7207](https://github.com/RocketChat/Rocket.Chat/pull/7207))

- Fix Custom Fields Crashing on Register ([#7617](https://github.com/RocketChat/Rocket.Chat/pull/7617))

- Fix editing others messages ([#7200](https://github.com/RocketChat/Rocket.Chat/pull/7200))

- Fix Emails in User Admin View ([#7431](https://github.com/RocketChat/Rocket.Chat/pull/7431))

- Fix emoji picker translations ([#7195](https://github.com/RocketChat/Rocket.Chat/pull/7195))

- Fix error on image preview due to undefined description|title  ([#7187](https://github.com/RocketChat/Rocket.Chat/pull/7187))

- Fix file upload on Slack import ([#7469](https://github.com/RocketChat/Rocket.Chat/pull/7469))

- Fix geolocation button ([#7322](https://github.com/RocketChat/Rocket.Chat/pull/7322))

- Fix hiding flex-tab on embedded view ([#7486](https://github.com/RocketChat/Rocket.Chat/pull/7486))

- Fix jump to unread button ([#7320](https://github.com/RocketChat/Rocket.Chat/pull/7320))

- Fix messagebox growth ([#7629](https://github.com/RocketChat/Rocket.Chat/pull/7629))

- Fix migration of avatars from version 0.57.0 ([#7428](https://github.com/RocketChat/Rocket.Chat/pull/7428))

- Fix oembed previews not being shown ([#7208](https://github.com/RocketChat/Rocket.Chat/pull/7208))

- Fix Private Channel List Submit ([#7432](https://github.com/RocketChat/Rocket.Chat/pull/7432))

- Fix room load on first hit ([#7687](https://github.com/RocketChat/Rocket.Chat/pull/7687))

- Fix Secret Url ([#7321](https://github.com/RocketChat/Rocket.Chat/pull/7321))

- Fix Unread Bar Disappearing ([#7403](https://github.com/RocketChat/Rocket.Chat/pull/7403))

- Fix Word Placement Anywhere on WebHooks ([#7392](https://github.com/RocketChat/Rocket.Chat/pull/7392))

- Issue #7365: added check for the existence of a parameter in the CAS URL ([#7471](https://github.com/RocketChat/Rocket.Chat/pull/7471) by [@wsw70](https://github.com/wsw70))

- Look for livechat visitor IP address on X-Forwarded-For header ([#7554](https://github.com/RocketChat/Rocket.Chat/pull/7554))

- make flex-tab visible again when reduced width ([#7738](https://github.com/RocketChat/Rocket.Chat/pull/7738))

- Markdown noopener/noreferrer: use correct HTML attribute ([#7644](https://github.com/RocketChat/Rocket.Chat/pull/7644) by [@jangmarker](https://github.com/jangmarker))

- Message box on safari ([#7621](https://github.com/RocketChat/Rocket.Chat/pull/7621))

- Prevent new room status from playing when user status changes ([#7487](https://github.com/RocketChat/Rocket.Chat/pull/7487))

- Remove warning about 2FA support being unavailable in mobile apps ([#7354](https://github.com/RocketChat/Rocket.Chat/pull/7354) by [@al3x](https://github.com/al3x))

- Revert emojione package version upgrade ([#7557](https://github.com/RocketChat/Rocket.Chat/pull/7557))

- S3 uploads not working for custom URLs ([#7443](https://github.com/RocketChat/Rocket.Chat/pull/7443))

- Slack import failing and not being able to be restarted ([#8390](https://github.com/RocketChat/Rocket.Chat/pull/8390))

- Stop logging mentions object to console ([#7562](https://github.com/RocketChat/Rocket.Chat/pull/7562) by [@gdelavald](https://github.com/gdelavald))

- Sweet-Alert modal popup position on mobile devices ([#7376](https://github.com/RocketChat/Rocket.Chat/pull/7376) by [@Oliver84](https://github.com/Oliver84))

- sweetalert alignment on mobile ([#7404](https://github.com/RocketChat/Rocket.Chat/pull/7404) by [@karlprieb](https://github.com/karlprieb))

- The username not being allowed to be passed into the user.setAvatar ([#7620](https://github.com/RocketChat/Rocket.Chat/pull/7620))

- Update node-engine in Snap to latest v4 LTS relase: 4.8.3 ([#7355](https://github.com/RocketChat/Rocket.Chat/pull/7355) by [@al3x](https://github.com/al3x))

- Uploading an unknown file type erroring out ([#7623](https://github.com/RocketChat/Rocket.Chat/pull/7623))

- url click events in the cordova app open in external browser or not at all ([#7205](https://github.com/RocketChat/Rocket.Chat/pull/7205) by [@flaviogrossi](https://github.com/flaviogrossi))

- URL parse error fix for issue #7169 ([#7538](https://github.com/RocketChat/Rocket.Chat/pull/7538) by [@satyapramodh](https://github.com/satyapramodh))

- Use I18n on "File Uploaded" ([#7199](https://github.com/RocketChat/Rocket.Chat/pull/7199))

- User avatar image background ([#7572](https://github.com/RocketChat/Rocket.Chat/pull/7572) by [@filipedelimabrito](https://github.com/filipedelimabrito))

- Wrong email subject when "All Messages" setting enabled ([#7639](https://github.com/RocketChat/Rocket.Chat/pull/7639))

- Wrong render of snippet’s name ([#7630](https://github.com/RocketChat/Rocket.Chat/pull/7630))

<details>
<summary>🔍 Minor changes</summary>


- [Fix] Don't save user to DB when a custom field is invalid ([#7513](https://github.com/RocketChat/Rocket.Chat/pull/7513) by [@Darkneon](https://github.com/Darkneon))

- [New] Add instance id to response headers ([#7211](https://github.com/RocketChat/Rocket.Chat/pull/7211))

- Add helm chart kubernetes deployment ([#6340](https://github.com/RocketChat/Rocket.Chat/pull/6340) by [@pierreozoux](https://github.com/pierreozoux))

- Add missing parts of `one click to direct message` ([#7608](https://github.com/RocketChat/Rocket.Chat/pull/7608))

- Better Issue Template ([#7492](https://github.com/RocketChat/Rocket.Chat/pull/7492))

- Develop sync ([#7590](https://github.com/RocketChat/Rocket.Chat/pull/7590))

- Develop sync ([#7500](https://github.com/RocketChat/Rocket.Chat/pull/7500) by [@thinkeridea](https://github.com/thinkeridea))

- Develop sync ([#7363](https://github.com/RocketChat/Rocket.Chat/pull/7363) by [@JSzaszvari](https://github.com/JSzaszvari))

- Escape error messages ([#7308](https://github.com/RocketChat/Rocket.Chat/pull/7308))

- Fix the Zapier oAuth return url to the new one ([#7215](https://github.com/RocketChat/Rocket.Chat/pull/7215))

- Improve link parser using tokens ([#7615](https://github.com/RocketChat/Rocket.Chat/pull/7615))

- Improve login error messages ([#7616](https://github.com/RocketChat/Rocket.Chat/pull/7616))

- Improve room leader ([#7578](https://github.com/RocketChat/Rocket.Chat/pull/7578))

- LingoHub based on develop ([#7613](https://github.com/RocketChat/Rocket.Chat/pull/7613))

- LingoHub based on develop ([#7594](https://github.com/RocketChat/Rocket.Chat/pull/7594))

- Only use "File Uploaded" prefix on files ([#7652](https://github.com/RocketChat/Rocket.Chat/pull/7652))

- Release 0.58.0 ([#7752](https://github.com/RocketChat/Rocket.Chat/pull/7752) by [@flaviogrossi](https://github.com/flaviogrossi) & [@jangmarker](https://github.com/jangmarker) & [@karlprieb](https://github.com/karlprieb) & [@pierreozoux](https://github.com/pierreozoux) & [@ryoshimizu](https://github.com/ryoshimizu))

- Sync Master with 0.57.3 ([#7690](https://github.com/RocketChat/Rocket.Chat/pull/7690))

- update meteor to 1.5.0 ([#7287](https://github.com/RocketChat/Rocket.Chat/pull/7287))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@AhmetS](https://github.com/AhmetS)
- [@Darkneon](https://github.com/Darkneon)
- [@JSzaszvari](https://github.com/JSzaszvari)
- [@Oliver84](https://github.com/Oliver84)
- [@al3x](https://github.com/al3x)
- [@borsden](https://github.com/borsden)
- [@ccfang](https://github.com/ccfang)
- [@danilomiranda](https://github.com/danilomiranda)
- [@danischreiber](https://github.com/danischreiber)
- [@filipedelimabrito](https://github.com/filipedelimabrito)
- [@flaviogrossi](https://github.com/flaviogrossi)
- [@gdelavald](https://github.com/gdelavald)
- [@jangmarker](https://github.com/jangmarker)
- [@jfchevrette](https://github.com/jfchevrette)
- [@karlprieb](https://github.com/karlprieb)
- [@lindoelio](https://github.com/lindoelio)
- [@pierreozoux](https://github.com/pierreozoux)
- [@reist](https://github.com/reist)
- [@ruKurz](https://github.com/ruKurz)
- [@ryoshimizu](https://github.com/ryoshimizu)
- [@satyapramodh](https://github.com/satyapramodh)
- [@shahar3012](https://github.com/shahar3012)
- [@stalley](https://github.com/stalley)
- [@thinkeridea](https://github.com/thinkeridea)
- [@wsw70](https://github.com/wsw70)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.57.4
`2017-10-05  ·  3 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.2`
- NPM: `4.5.0`

### 🐛 Bug fixes


- Add needed dependency for snaps ([#8389](https://github.com/RocketChat/Rocket.Chat/pull/8389))

- Duplicate code in rest api letting in a few bugs with the rest api ([#8408](https://github.com/RocketChat/Rocket.Chat/pull/8408))

- Slack import failing and not being able to be restarted ([#8390](https://github.com/RocketChat/Rocket.Chat/pull/8390))

### 👩‍💻👨‍💻 Core Team 🤓

- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@graywolf336](https://github.com/graywolf336)

# 0.57.3
`2017-08-08  ·  8 🐛  ·  1 🔍  ·  7 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.2`
- NPM: `4.5.0`

### 🐛 Bug fixes


- custom soundEdit.html ([#7390](https://github.com/RocketChat/Rocket.Chat/pull/7390) by [@rasos](https://github.com/rasos))

- file upload broken when running in subdirectory https://github.com… ([#7395](https://github.com/RocketChat/Rocket.Chat/pull/7395) by [@ryoshimizu](https://github.com/ryoshimizu))

- Fix Anonymous User ([#7444](https://github.com/RocketChat/Rocket.Chat/pull/7444))

- Fix Join Channel Without Preview Room Permission ([#7535](https://github.com/RocketChat/Rocket.Chat/pull/7535))

- Improve build script example ([#7555](https://github.com/RocketChat/Rocket.Chat/pull/7555))

- Missing eventName in unUser ([#7533](https://github.com/RocketChat/Rocket.Chat/pull/7533) by [@Darkneon](https://github.com/Darkneon))

- Modernize rate limiting of sendMessage ([#7325](https://github.com/RocketChat/Rocket.Chat/pull/7325) by [@jangmarker](https://github.com/jangmarker))

- Use UTF8 setting for /create command ([#7394](https://github.com/RocketChat/Rocket.Chat/pull/7394))

<details>
<summary>🔍 Minor changes</summary>


- [Fix] Users and Channels list not respecting permissions ([#7212](https://github.com/RocketChat/Rocket.Chat/pull/7212))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Darkneon](https://github.com/Darkneon)
- [@jangmarker](https://github.com/jangmarker)
- [@rasos](https://github.com/rasos)
- [@ryoshimizu](https://github.com/ryoshimizu)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@graywolf336](https://github.com/graywolf336)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.57.2
`2017-07-14  ·  6 🐛  ·  3 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.2`
- NPM: `4.5.0`

### 🐛 Bug fixes


- Always set LDAP properties on login ([#7472](https://github.com/RocketChat/Rocket.Chat/pull/7472))

- Fix Emails in User Admin View ([#7431](https://github.com/RocketChat/Rocket.Chat/pull/7431))

- Fix file upload on Slack import ([#7469](https://github.com/RocketChat/Rocket.Chat/pull/7469))

- Fix Private Channel List Submit ([#7432](https://github.com/RocketChat/Rocket.Chat/pull/7432))

- Fix Unread Bar Disappearing ([#7403](https://github.com/RocketChat/Rocket.Chat/pull/7403))

- S3 uploads not working for custom URLs ([#7443](https://github.com/RocketChat/Rocket.Chat/pull/7443))

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.57.1
`2017-07-05  ·  1 🐛  ·  2 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.2`
- NPM: `4.5.0`

### 🐛 Bug fixes


- Fix migration of avatars from version 0.57.0 ([#7428](https://github.com/RocketChat/Rocket.Chat/pull/7428))

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.57.0
`2017-07-03  ·  1 ️️️⚠️  ·  12 🎉  ·  45 🐛  ·  29 🔍  ·  25 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.2`
- NPM: `4.5.0`

### ⚠️ BREAKING CHANGES


- Internal hubot does not load hubot-scripts anymore, it loads scripts from custom folders ([#7095](https://github.com/RocketChat/Rocket.Chat/pull/7095))

### 🎉 New features


- API method and REST Endpoint for getting a single message by id ([#7085](https://github.com/RocketChat/Rocket.Chat/pull/7085))

- Feature/delete any message permission ([#6919](https://github.com/RocketChat/Rocket.Chat/pull/6919) by [@phutchins](https://github.com/phutchins))

- Force use of MongoDB for spotlight queries ([#7311](https://github.com/RocketChat/Rocket.Chat/pull/7311))

- Improve CI/Docker build/release ([#6938](https://github.com/RocketChat/Rocket.Chat/pull/6938))

- Increase unread message count on @here mention ([#7059](https://github.com/RocketChat/Rocket.Chat/pull/7059))

- Make channel/group delete call answer to roomName ([#6857](https://github.com/RocketChat/Rocket.Chat/pull/6857) by [@reist](https://github.com/reist))

- Migration to add <html> tags to email header and footer ([#7080](https://github.com/RocketChat/Rocket.Chat/pull/7080))

- New avatar storage types ([#6788](https://github.com/RocketChat/Rocket.Chat/pull/6788))

- postcss parser and cssnext implementation ([#6982](https://github.com/RocketChat/Rocket.Chat/pull/6982))

- Show full name in mentions if use full name setting enabled ([#6690](https://github.com/RocketChat/Rocket.Chat/pull/6690) by [@alexbrazier](https://github.com/alexbrazier))

- Show info about multiple instances at admin page ([#6953](https://github.com/RocketChat/Rocket.Chat/pull/6953))

- Start running unit tests ([#6605](https://github.com/RocketChat/Rocket.Chat/pull/6605))

### 🐛 Bug fixes


- "requirePasswordChange" property not being saved when set to false ([#7209](https://github.com/RocketChat/Rocket.Chat/pull/7209))

- Add <html> and </html> to header and footer ([#7025](https://github.com/RocketChat/Rocket.Chat/pull/7025) by [@ExTechOp](https://github.com/ExTechOp))

- Add option to ignore TLS in SMTP server settings ([#7084](https://github.com/RocketChat/Rocket.Chat/pull/7084) by [@colin-campbell](https://github.com/colin-campbell))

- Add support for carriage return in markdown code blocks ([#7072](https://github.com/RocketChat/Rocket.Chat/pull/7072) by [@jm-factorin](https://github.com/jm-factorin))

- Allow image insert from slack through slackbridge ([#6910](https://github.com/RocketChat/Rocket.Chat/pull/6910))

- Bugs in `isUserFromParams` helper ([#6904](https://github.com/RocketChat/Rocket.Chat/pull/6904) by [@abrom](https://github.com/abrom))

- Check that username is not in the room when being muted / unmuted ([#6840](https://github.com/RocketChat/Rocket.Chat/pull/6840) by [@matthewshirley](https://github.com/matthewshirley))

- click on image in a message ([#7345](https://github.com/RocketChat/Rocket.Chat/pull/7345))

- clipboard (permalink, copy, pin, star buttons) ([#7103](https://github.com/RocketChat/Rocket.Chat/pull/7103))

- do only store password if LDAP_Login_Fallback is on ([#7030](https://github.com/RocketChat/Rocket.Chat/pull/7030) by [@pmb0](https://github.com/pmb0))

- edit button on firefox ([#7105](https://github.com/RocketChat/Rocket.Chat/pull/7105))

- Fix all reactions having the same username ([#7157](https://github.com/RocketChat/Rocket.Chat/pull/7157))

- Fix avatar upload via users.setAvatar REST endpoint ([#7045](https://github.com/RocketChat/Rocket.Chat/pull/7045))

- Fix badge counter on iOS push notifications ([#6950](https://github.com/RocketChat/Rocket.Chat/pull/6950))

- fix bug in preview image ([#7121](https://github.com/RocketChat/Rocket.Chat/pull/7121))

- Fix editing others messages ([#7200](https://github.com/RocketChat/Rocket.Chat/pull/7200))

- Fix error handling for non-valid avatar URL ([#6972](https://github.com/RocketChat/Rocket.Chat/pull/6972))

- Fix highlightjs bug ([#6991](https://github.com/RocketChat/Rocket.Chat/pull/6991))

- Fix jump to unread button ([#7320](https://github.com/RocketChat/Rocket.Chat/pull/7320))

- Fix login with Meteor saving an object as email address ([#6974](https://github.com/RocketChat/Rocket.Chat/pull/6974))

- Fix missing CSS files on production builds ([#7104](https://github.com/RocketChat/Rocket.Chat/pull/7104))

- Fix oembed previews not being shown ([#7208](https://github.com/RocketChat/Rocket.Chat/pull/7208))

- Fix Secret Url ([#7321](https://github.com/RocketChat/Rocket.Chat/pull/7321))

- Fix the failing tests  ([#7094](https://github.com/RocketChat/Rocket.Chat/pull/7094))

- Fix the other tests failing due chimp update ([#6986](https://github.com/RocketChat/Rocket.Chat/pull/6986))

- Fix user's customFields not being saved correctly ([#7358](https://github.com/RocketChat/Rocket.Chat/pull/7358))

- Fixed typo hmtl -> html ([#7092](https://github.com/RocketChat/Rocket.Chat/pull/7092) by [@jautero](https://github.com/jautero))

- Improve avatar migration ([#7352](https://github.com/RocketChat/Rocket.Chat/pull/7352))

- Improve Tests ([#7049](https://github.com/RocketChat/Rocket.Chat/pull/7049))

- make channels.create API check for create-c ([#6968](https://github.com/RocketChat/Rocket.Chat/pull/6968) by [@reist](https://github.com/reist))

- Message being displayed unescaped ([#7379](https://github.com/RocketChat/Rocket.Chat/pull/7379) by [@gdelavald](https://github.com/gdelavald))

- New screen sharing Chrome extension checking method ([#7044](https://github.com/RocketChat/Rocket.Chat/pull/7044))

- overlapping text for users-typing-message ([#6999](https://github.com/RocketChat/Rocket.Chat/pull/6999) by [@darkv](https://github.com/darkv))

- Parse HTML on admin setting's descriptions ([#7014](https://github.com/RocketChat/Rocket.Chat/pull/7014))

- Parse markdown links last ([#6997](https://github.com/RocketChat/Rocket.Chat/pull/6997))

- Prevent Ctrl key on message field from reloading messages list ([#7033](https://github.com/RocketChat/Rocket.Chat/pull/7033))

- Proxy upload to correct instance ([#7304](https://github.com/RocketChat/Rocket.Chat/pull/7304))

- Remove room from roomPick setting ([#6912](https://github.com/RocketChat/Rocket.Chat/pull/6912))

- Removing the kadira package install from example build script. ([#7160](https://github.com/RocketChat/Rocket.Chat/pull/7160) by [@JSzaszvari](https://github.com/JSzaszvari))

- SAML: Only set KeyDescriptor when non empty ([#6961](https://github.com/RocketChat/Rocket.Chat/pull/6961) by [@sathieu](https://github.com/sathieu))

- Sidenav roomlist ([#7023](https://github.com/RocketChat/Rocket.Chat/pull/7023))

- Slackbridge text replacements ([#6913](https://github.com/RocketChat/Rocket.Chat/pull/6913))

- Updating Incoming Integration Post As Field Not Allowed ([#6903](https://github.com/RocketChat/Rocket.Chat/pull/6903))

- Use AWS Signature Version 4 signed URLs for uploads ([#6947](https://github.com/RocketChat/Rocket.Chat/pull/6947))

- video message recording dialog is shown in an incorrect position ([#7012](https://github.com/RocketChat/Rocket.Chat/pull/7012) by [@flaviogrossi](https://github.com/flaviogrossi))

<details>
<summary>🔍 Minor changes</summary>


- [Fix] Error when trying to show preview of undefined filetype ([#6935](https://github.com/RocketChat/Rocket.Chat/pull/6935))

- [New] LDAP: Use variables in User_Data_FieldMap for name mapping ([#6921](https://github.com/RocketChat/Rocket.Chat/pull/6921) by [@bbrauns](https://github.com/bbrauns))

- add server methods getRoomNameById ([#7102](https://github.com/RocketChat/Rocket.Chat/pull/7102) by [@thinkeridea](https://github.com/thinkeridea))

- Convert file unsubscribe.coffee to js ([#7145](https://github.com/RocketChat/Rocket.Chat/pull/7145))

- Convert hipchat importer to js ([#7146](https://github.com/RocketChat/Rocket.Chat/pull/7146))

- Convert irc package to js ([#7022](https://github.com/RocketChat/Rocket.Chat/pull/7022))

- Convert Livechat from Coffeescript to JavaScript ([#7096](https://github.com/RocketChat/Rocket.Chat/pull/7096))

- Convert meteor-autocomplete package to js ([#6936](https://github.com/RocketChat/Rocket.Chat/pull/6936))

- Convert oauth2-server-config package  to js ([#7017](https://github.com/RocketChat/Rocket.Chat/pull/7017))

- Convert Ui Account Package to Js ([#6795](https://github.com/RocketChat/Rocket.Chat/pull/6795))

- Convert ui-admin package to js ([#6911](https://github.com/RocketChat/Rocket.Chat/pull/6911))

- Convert WebRTC Package to Js ([#6775](https://github.com/RocketChat/Rocket.Chat/pull/6775))

- converted rocketchat-importer ([#7018](https://github.com/RocketChat/Rocket.Chat/pull/7018))

- converted rocketchat-ui coffee to js part 2 ([#6836](https://github.com/RocketChat/Rocket.Chat/pull/6836))

- Fix forbidden error on setAvatar REST endpoint ([#7159](https://github.com/RocketChat/Rocket.Chat/pull/7159))

- Fix mobile avatars ([#7177](https://github.com/RocketChat/Rocket.Chat/pull/7177))

- fix the crashing tests ([#6976](https://github.com/RocketChat/Rocket.Chat/pull/6976))

- Fix the Zapier oAuth return url to the new one ([#7215](https://github.com/RocketChat/Rocket.Chat/pull/7215))

- Ldap: User_Data_FieldMap description ([#7055](https://github.com/RocketChat/Rocket.Chat/pull/7055) by [@bbrauns](https://github.com/bbrauns))

- LingoHub based on develop ([#7114](https://github.com/RocketChat/Rocket.Chat/pull/7114))

- LingoHub based on develop ([#7005](https://github.com/RocketChat/Rocket.Chat/pull/7005))

- LingoHub based on develop ([#6978](https://github.com/RocketChat/Rocket.Chat/pull/6978))

- Remove missing CoffeeScript dependencies ([#7154](https://github.com/RocketChat/Rocket.Chat/pull/7154))

- Remove Useless Jasmine Tests  ([#7062](https://github.com/RocketChat/Rocket.Chat/pull/7062))

- Rocketchat ui message ([#6914](https://github.com/RocketChat/Rocket.Chat/pull/6914))

- Rocketchat ui3 ([#7006](https://github.com/RocketChat/Rocket.Chat/pull/7006))

- rocketchat-importer-slack coffee to js ([#6987](https://github.com/RocketChat/Rocket.Chat/pull/6987))

- rocketchat-lib[4] coffee to js ([#6735](https://github.com/RocketChat/Rocket.Chat/pull/6735))

- Switch logic of artifact name ([#7158](https://github.com/RocketChat/Rocket.Chat/pull/7158))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@ExTechOp](https://github.com/ExTechOp)
- [@JSzaszvari](https://github.com/JSzaszvari)
- [@abrom](https://github.com/abrom)
- [@alexbrazier](https://github.com/alexbrazier)
- [@bbrauns](https://github.com/bbrauns)
- [@colin-campbell](https://github.com/colin-campbell)
- [@darkv](https://github.com/darkv)
- [@flaviogrossi](https://github.com/flaviogrossi)
- [@gdelavald](https://github.com/gdelavald)
- [@jautero](https://github.com/jautero)
- [@jm-factorin](https://github.com/jm-factorin)
- [@matthewshirley](https://github.com/matthewshirley)
- [@phutchins](https://github.com/phutchins)
- [@pmb0](https://github.com/pmb0)
- [@reist](https://github.com/reist)
- [@sathieu](https://github.com/sathieu)
- [@thinkeridea](https://github.com/thinkeridea)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.56.0
`2017-05-15  ·  11 🎉  ·  21 🐛  ·  19 🔍  ·  19 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.2`
- NPM: `4.5.0`

### 🎉 New features


- Add a pointer cursor to message images ([#6881](https://github.com/RocketChat/Rocket.Chat/pull/6881))

- Add a setting to not run outgoing integrations on message edits ([#6615](https://github.com/RocketChat/Rocket.Chat/pull/6615))

- Add option on Channel Settings: Hide Notifications and Hide Unread Room Status (#2707, #2143) ([#5373](https://github.com/RocketChat/Rocket.Chat/pull/5373))

- Add SMTP settings for Protocol and Pool ([#6940](https://github.com/RocketChat/Rocket.Chat/pull/6940))

- create a method 'create token' ([#6807](https://github.com/RocketChat/Rocket.Chat/pull/6807))

- Improve CI/Docker build/release ([#6938](https://github.com/RocketChat/Rocket.Chat/pull/6938))

- Make channels.info accept roomName, just like groups.info ([#6827](https://github.com/RocketChat/Rocket.Chat/pull/6827) by [@reist](https://github.com/reist))

- Option to allow to signup as anonymous ([#6797](https://github.com/RocketChat/Rocket.Chat/pull/6797))

- Remove lesshat ([#6722](https://github.com/RocketChat/Rocket.Chat/pull/6722) by [@karlprieb](https://github.com/karlprieb))

- Show info about multiple instances at admin page ([#6953](https://github.com/RocketChat/Rocket.Chat/pull/6953))

- Use tokenSentVia parameter for clientid/secret to token endpoint ([#6692](https://github.com/RocketChat/Rocket.Chat/pull/6692) by [@intelradoux](https://github.com/intelradoux))

### 🐛 Bug fixes


- Added helper for testing if the current user matches the params ([#6845](https://github.com/RocketChat/Rocket.Chat/pull/6845) by [@abrom](https://github.com/abrom))

- Archiving Direct Messages ([#6737](https://github.com/RocketChat/Rocket.Chat/pull/6737))

- Compile CSS color variables ([#6939](https://github.com/RocketChat/Rocket.Chat/pull/6939))

- CSV importer: require that there is some data in the zip, not ALL data ([#6768](https://github.com/RocketChat/Rocket.Chat/pull/6768) by [@reist](https://github.com/reist))

- emoji picker exception ([#6709](https://github.com/RocketChat/Rocket.Chat/pull/6709) by [@gdelavald](https://github.com/gdelavald))

- Fix Caddy by forcing go 1.7 as needed by one of caddy's dependencies ([#6721](https://github.com/RocketChat/Rocket.Chat/pull/6721))

- fix german translation ([#6790](https://github.com/RocketChat/Rocket.Chat/pull/6790) by [@sscholl](https://github.com/sscholl))

- Fix iframe wise issues ([#6798](https://github.com/RocketChat/Rocket.Chat/pull/6798))

- Fix message types ([#6704](https://github.com/RocketChat/Rocket.Chat/pull/6704))

- Hides nav buttons when selecting own profile ([#6760](https://github.com/RocketChat/Rocket.Chat/pull/6760) by [@gdelavald](https://github.com/gdelavald))

- Improve and correct Iframe Integration help text ([#6793](https://github.com/RocketChat/Rocket.Chat/pull/6793))

- Incorrect error message when creating channel ([#6747](https://github.com/RocketChat/Rocket.Chat/pull/6747) by [@gdelavald](https://github.com/gdelavald))

- make channels.create API check for create-c ([#6968](https://github.com/RocketChat/Rocket.Chat/pull/6968) by [@reist](https://github.com/reist))

- Not showing unread count on electron app’s icon ([#6923](https://github.com/RocketChat/Rocket.Chat/pull/6923))

- Quoted and replied messages not retaining the original message's alias ([#6800](https://github.com/RocketChat/Rocket.Chat/pull/6800))

- Remove spaces from env PORT and INSTANCE_IP ([#6955](https://github.com/RocketChat/Rocket.Chat/pull/6955))

- REST API user.update throwing error due to rate limiting ([#6796](https://github.com/RocketChat/Rocket.Chat/pull/6796))

- Search full name on client side ([#6767](https://github.com/RocketChat/Rocket.Chat/pull/6767) by [@alexbrazier](https://github.com/alexbrazier))

- Sort by real name if use real name setting is enabled ([#6758](https://github.com/RocketChat/Rocket.Chat/pull/6758) by [@alexbrazier](https://github.com/alexbrazier))

- start/unstar message ([#6861](https://github.com/RocketChat/Rocket.Chat/pull/6861))

- Users status on main menu always offline ([#6896](https://github.com/RocketChat/Rocket.Chat/pull/6896))

<details>
<summary>🔍 Minor changes</summary>


- [Fix] Error when trying to show preview of undefined filetype ([#6935](https://github.com/RocketChat/Rocket.Chat/pull/6935))

- [New] Snap arm support ([#6842](https://github.com/RocketChat/Rocket.Chat/pull/6842))

- Anonymous use ([#5986](https://github.com/RocketChat/Rocket.Chat/pull/5986))

- Breaking long URLS to prevent overflow ([#6368](https://github.com/RocketChat/Rocket.Chat/pull/6368) by [@robertdown](https://github.com/robertdown))

- Convert Katex Package to Js ([#6671](https://github.com/RocketChat/Rocket.Chat/pull/6671))

- Convert Mailer Package to Js ([#6780](https://github.com/RocketChat/Rocket.Chat/pull/6780))

- Convert markdown to js ([#6694](https://github.com/RocketChat/Rocket.Chat/pull/6694) by [@ehkasper](https://github.com/ehkasper))

- Convert Mentions-Flextab Package to Js ([#6689](https://github.com/RocketChat/Rocket.Chat/pull/6689))

- Convert Message-Star Package to js  ([#6781](https://github.com/RocketChat/Rocket.Chat/pull/6781))

- Convert Oembed Package to Js ([#6688](https://github.com/RocketChat/Rocket.Chat/pull/6688))

- Converted rocketchat-lib 3 ([#6672](https://github.com/RocketChat/Rocket.Chat/pull/6672))

- disable proxy configuration ([#6654](https://github.com/RocketChat/Rocket.Chat/pull/6654) by [@glehmann](https://github.com/glehmann))

- LingoHub based on develop ([#6816](https://github.com/RocketChat/Rocket.Chat/pull/6816))

- LingoHub based on develop ([#6715](https://github.com/RocketChat/Rocket.Chat/pull/6715))

- LingoHub based on develop ([#6703](https://github.com/RocketChat/Rocket.Chat/pull/6703))

- Meteor update ([#6858](https://github.com/RocketChat/Rocket.Chat/pull/6858))

- meteor update to 1.4.4 ([#6706](https://github.com/RocketChat/Rocket.Chat/pull/6706))

- Missing useful fields in admin user list #5110 ([#6804](https://github.com/RocketChat/Rocket.Chat/pull/6804) by [@vlogic](https://github.com/vlogic))

- Rocketchat lib2 ([#6593](https://github.com/RocketChat/Rocket.Chat/pull/6593))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@abrom](https://github.com/abrom)
- [@alexbrazier](https://github.com/alexbrazier)
- [@ehkasper](https://github.com/ehkasper)
- [@gdelavald](https://github.com/gdelavald)
- [@glehmann](https://github.com/glehmann)
- [@intelradoux](https://github.com/intelradoux)
- [@karlprieb](https://github.com/karlprieb)
- [@reist](https://github.com/reist)
- [@robertdown](https://github.com/robertdown)
- [@sscholl](https://github.com/sscholl)
- [@vlogic](https://github.com/vlogic)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)

# 0.55.1
`2017-04-19  ·  1 🔍  ·  1 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.0`
- NPM: `4.3.0`

<details>
<summary>🔍 Minor changes</summary>


- [Fix] Bug with incoming integration (0.55.1) ([#6734](https://github.com/RocketChat/Rocket.Chat/pull/6734))

</details>

### 👩‍💻👨‍💻 Core Team 🤓

- [@rodrigok](https://github.com/rodrigok)

# 0.55.0
`2017-04-18  ·  1 ️️️⚠️  ·  9 🎉  ·  25 🐛  ·  87 🔍  ·  23 👩‍💻👨‍💻`

### Engine versions
- Node: `4.8.0`
- NPM: `4.3.0`

### ⚠️ BREAKING CHANGES


- `getUsersOfRoom` API to return array of objects with user and username, instead of array of strings

### 🎉 New features


- 'users.resetAvatar' rest api endpoint ([#6616](https://github.com/RocketChat/Rocket.Chat/pull/6616))

- Add monitoring package ([#6634](https://github.com/RocketChat/Rocket.Chat/pull/6634))

- Add shield.svg api route to generate custom shields/badges ([#6565](https://github.com/RocketChat/Rocket.Chat/pull/6565) by [@alexbrazier](https://github.com/alexbrazier))

- Drupal oAuth Integration for Rocketchat ([#6632](https://github.com/RocketChat/Rocket.Chat/pull/6632) by [@Lawri-van-Buel](https://github.com/Lawri-van-Buel))

- Expose Livechat to Incoming Integrations and allow response ([#6681](https://github.com/RocketChat/Rocket.Chat/pull/6681))

- Integrations, both incoming and outgoing, now have access to the models. Example: `Users.findOneById(id)` ([#6420](https://github.com/RocketChat/Rocket.Chat/pull/6420))

- Permission `join-without-join-code` assigned to admins and bots by default ([#6430](https://github.com/RocketChat/Rocket.Chat/pull/6430))

- resolve merge share function ([#6577](https://github.com/RocketChat/Rocket.Chat/pull/6577) by [@karlprieb](https://github.com/karlprieb) & [@tgxn](https://github.com/tgxn))

- Two Factor Auth ([#6476](https://github.com/RocketChat/Rocket.Chat/pull/6476))

### 🐛 Bug fixes


- Accounts from LinkedIn OAuth without name ([#6590](https://github.com/RocketChat/Rocket.Chat/pull/6590))

- Administrators being rate limited when editing users data ([#6659](https://github.com/RocketChat/Rocket.Chat/pull/6659))

- Allow question on OAuth token path ([#6684](https://github.com/RocketChat/Rocket.Chat/pull/6684))

- arguments logger ([#6617](https://github.com/RocketChat/Rocket.Chat/pull/6617))

- can not get access_token when using custom oauth ([#6531](https://github.com/RocketChat/Rocket.Chat/pull/6531) by [@fengt](https://github.com/fengt))

- Do not add default roles for users without services field ([#6594](https://github.com/RocketChat/Rocket.Chat/pull/6594))

- Do not escaping markdown on message attachments ([#6648](https://github.com/RocketChat/Rocket.Chat/pull/6648))

- Downgrade email package to from 1.2.0 to 1.1.18 ([#6680](https://github.com/RocketChat/Rocket.Chat/pull/6680))

- emoji picker exception ([#6709](https://github.com/RocketChat/Rocket.Chat/pull/6709) by [@gdelavald](https://github.com/gdelavald))

- Encode avatar url to prevent CSS injection ([#6651](https://github.com/RocketChat/Rocket.Chat/pull/6651))

- Error when returning undefined from incoming intergation’s script ([#6683](https://github.com/RocketChat/Rocket.Chat/pull/6683))

- Fix Logger stdout publication ([#6682](https://github.com/RocketChat/Rocket.Chat/pull/6682))

- Fix message types ([#6704](https://github.com/RocketChat/Rocket.Chat/pull/6704))

- Improve markdown code ([#6650](https://github.com/RocketChat/Rocket.Chat/pull/6650))

- Incoming integrations would break when trying to use the `Store` feature.`

- Incorrect curl command being generated on incoming integrations ([#6620](https://github.com/RocketChat/Rocket.Chat/pull/6620))

- Large files crashed browser when trying to show preview ([#6598](https://github.com/RocketChat/Rocket.Chat/pull/6598))

- Make sure username exists in findByActiveUsersExcept ([#6674](https://github.com/RocketChat/Rocket.Chat/pull/6674))

- messageBox: put "joinCodeRequired" back ([#6600](https://github.com/RocketChat/Rocket.Chat/pull/6600) by [@karlprieb](https://github.com/karlprieb))

- Outgoing webhooks which have an error and they're retrying would still retry even if the integration was disabled` ([#6478](https://github.com/RocketChat/Rocket.Chat/pull/6478))

- Removed Deprecated Package rocketchat:sharedsecret`

- Revert unwanted UI changes ([#6658](https://github.com/RocketChat/Rocket.Chat/pull/6658))

- Update server cache indexes on record updates ([#6686](https://github.com/RocketChat/Rocket.Chat/pull/6686))

- Usage of subtagged languages ([#6575](https://github.com/RocketChat/Rocket.Chat/pull/6575))

- UTC offset missing UTC text when positive ([#6562](https://github.com/RocketChat/Rocket.Chat/pull/6562) by [@alexbrazier](https://github.com/alexbrazier))

<details>
<summary>🔍 Minor changes</summary>


- 'allow reacting' should be a toggle option.otherwise, the style will display an error ([#6522](https://github.com/RocketChat/Rocket.Chat/pull/6522) by [@szluohua](https://github.com/szluohua))

- [New] Added oauth2 userinfo endpoint ([#6554](https://github.com/RocketChat/Rocket.Chat/pull/6554))

- [New] Switch Snaps to use oplog ([#6608](https://github.com/RocketChat/Rocket.Chat/pull/6608))

- Add `fname` to subscriptions in memory ([#6597](https://github.com/RocketChat/Rocket.Chat/pull/6597))

- Add candidate snap channel ([#6614](https://github.com/RocketChat/Rocket.Chat/pull/6614))

- Add ESLint rule `object-shorthand` ([#6457](https://github.com/RocketChat/Rocket.Chat/pull/6457))

- Add ESLint rule `one-var` ([#6458](https://github.com/RocketChat/Rocket.Chat/pull/6458))

- Add ESLint rules `one-var` and `no-var` ([#6459](https://github.com/RocketChat/Rocket.Chat/pull/6459))

- Add ESLint rules `prefer-template` and `template-curly-spacing` ([#6456](https://github.com/RocketChat/Rocket.Chat/pull/6456))

- Add permission check to the import methods and not just the UI ([#6400](https://github.com/RocketChat/Rocket.Chat/pull/6400))

- Added Deploy method and platform to stats ([#6649](https://github.com/RocketChat/Rocket.Chat/pull/6649))

- Allow livechat managers to transfer chats ([#6180](https://github.com/RocketChat/Rocket.Chat/pull/6180) by [@drallgood](https://github.com/drallgood))

- Allow Livechat visitors to switch the department ([#6035](https://github.com/RocketChat/Rocket.Chat/pull/6035) by [@drallgood](https://github.com/drallgood))

- Change all instances of Meteor.Collection for Mongo.Collection ([#6410](https://github.com/RocketChat/Rocket.Chat/pull/6410))

- Clipboard [Firefox version < 50] ([#6280](https://github.com/RocketChat/Rocket.Chat/pull/6280))

- Convert ChatOps Package to JavaScript ([#6425](https://github.com/RocketChat/Rocket.Chat/pull/6425))

- Convert Dolphin Package to JavaScript ([#6427](https://github.com/RocketChat/Rocket.Chat/pull/6427))

- Convert File Package to js ([#6503](https://github.com/RocketChat/Rocket.Chat/pull/6503))

- convert mapview package to js ([#6471](https://github.com/RocketChat/Rocket.Chat/pull/6471))

- Convert Message Pin Package to JS ([#6576](https://github.com/RocketChat/Rocket.Chat/pull/6576))

- convert rocketchat-ui part 2 ([#6539](https://github.com/RocketChat/Rocket.Chat/pull/6539))

- Convert Spotify Package to JS ([#6449](https://github.com/RocketChat/Rocket.Chat/pull/6449))

- Convert Statistics Package to JS ([#6447](https://github.com/RocketChat/Rocket.Chat/pull/6447))

- Convert Theme Package to JS ([#6491](https://github.com/RocketChat/Rocket.Chat/pull/6491))

- Convert Tutum Package to JS ([#6446](https://github.com/RocketChat/Rocket.Chat/pull/6446))

- Convert Ui-Login Package to Js ([#6561](https://github.com/RocketChat/Rocket.Chat/pull/6561))

- Convert Ui-Master Package to Js ([#6498](https://github.com/RocketChat/Rocket.Chat/pull/6498))

- Convert ui-vrecord Package to JS ([#6473](https://github.com/RocketChat/Rocket.Chat/pull/6473))

- Convert Version Package to JS ([#6494](https://github.com/RocketChat/Rocket.Chat/pull/6494))

- Convert Wordpress Package to js ([#6499](https://github.com/RocketChat/Rocket.Chat/pull/6499))

- converted getAvatarUrlFromUsername ([#6496](https://github.com/RocketChat/Rocket.Chat/pull/6496))

- converted messageAttachment coffee to js ([#6500](https://github.com/RocketChat/Rocket.Chat/pull/6500))

- converted meteor-accounts-saml coffee to js ([#6450](https://github.com/RocketChat/Rocket.Chat/pull/6450))

- converted Rocketchat logger coffee to js ([#6495](https://github.com/RocketChat/Rocket.Chat/pull/6495))

- converted rocketchat-integrations coffee to js ([#6502](https://github.com/RocketChat/Rocket.Chat/pull/6502))

- converted rocketchat-mentions coffee to js ([#6467](https://github.com/RocketChat/Rocket.Chat/pull/6467))

- converted rocketchat-message-mark-as-unread coffee/js ([#6445](https://github.com/RocketChat/Rocket.Chat/pull/6445))

- converted rocketchat-slashcommands-kick coffee to js ([#6453](https://github.com/RocketChat/Rocket.Chat/pull/6453))

- converted slashcommand-invite coffee to js ([#6497](https://github.com/RocketChat/Rocket.Chat/pull/6497))

- converted slashcommand-join coffee to js ([#6469](https://github.com/RocketChat/Rocket.Chat/pull/6469))

- converted slashcommand-leave coffee to js ([#6470](https://github.com/RocketChat/Rocket.Chat/pull/6470))

- converted slashcommand-me coffee to js ([#6468](https://github.com/RocketChat/Rocket.Chat/pull/6468))

- converted slashcommand-msg coffee to js ([#6501](https://github.com/RocketChat/Rocket.Chat/pull/6501))

- converted slashcommands-mute coffee to js ([#6474](https://github.com/RocketChat/Rocket.Chat/pull/6474))

- Create groups.addAll endpoint and add activeUsersOnly param. ([#6505](https://github.com/RocketChat/Rocket.Chat/pull/6505) by [@nathanmarcos](https://github.com/nathanmarcos))

- dependencies upgrade ([#6584](https://github.com/RocketChat/Rocket.Chat/pull/6584))

- Do not show reset button for hidden settings ([#6432](https://github.com/RocketChat/Rocket.Chat/pull/6432))

- Env override initial setting ([#6163](https://github.com/RocketChat/Rocket.Chat/pull/6163) by [@mrsimpson](https://github.com/mrsimpson))

- ESLint add rule `no-void` ([#6479](https://github.com/RocketChat/Rocket.Chat/pull/6479))

- fix channel merge option of user preferences ([#6493](https://github.com/RocketChat/Rocket.Chat/pull/6493) by [@billtt](https://github.com/billtt))

- Fix livechat permissions ([#6466](https://github.com/RocketChat/Rocket.Chat/pull/6466))

- fix livechat widget on small screens ([#6122](https://github.com/RocketChat/Rocket.Chat/pull/6122) by [@karlprieb](https://github.com/karlprieb))

- Fix recently introduced bug: OnePassword not defined ([#6591](https://github.com/RocketChat/Rocket.Chat/pull/6591))

- Fix typo of the safari pinned tab label ([#6487](https://github.com/RocketChat/Rocket.Chat/pull/6487) by [@qge](https://github.com/qge))

- Fix visitor ending livechat if multiples still open ([#6419](https://github.com/RocketChat/Rocket.Chat/pull/6419))

- fixed typo in readme.md ([#6580](https://github.com/RocketChat/Rocket.Chat/pull/6580) by [@sezinkarli](https://github.com/sezinkarli))

- Flex-Tab CoffeeScript to JavaScript I  ([#6276](https://github.com/RocketChat/Rocket.Chat/pull/6276))

- Flex-Tab CoffeeScript to JavaScript II ([#6277](https://github.com/RocketChat/Rocket.Chat/pull/6277))

- Flex-Tab CoffeeScript to JavaScript III ([#6278](https://github.com/RocketChat/Rocket.Chat/pull/6278))

- focus first textbox element ([#6257](https://github.com/RocketChat/Rocket.Chat/pull/6257) by [@a5his](https://github.com/a5his))

- Hide email settings on Sandstorm ([#6429](https://github.com/RocketChat/Rocket.Chat/pull/6429))

- Join command ([#6268](https://github.com/RocketChat/Rocket.Chat/pull/6268))

- Just admins can change a Default Channel to Private (the channel will be a non default channel) ([#6426](https://github.com/RocketChat/Rocket.Chat/pull/6426))

- LingoHub based on develop ([#6574](https://github.com/RocketChat/Rocket.Chat/pull/6574))

- LingoHub based on develop ([#6567](https://github.com/RocketChat/Rocket.Chat/pull/6567))

- LingoHub based on develop ([#6647](https://github.com/RocketChat/Rocket.Chat/pull/6647))

- Livechat fix office hours order ([#6413](https://github.com/RocketChat/Rocket.Chat/pull/6413))

- Make favicon package easier to read. ([#6422](https://github.com/RocketChat/Rocket.Chat/pull/6422) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- Max textarea height ([#6409](https://github.com/RocketChat/Rocket.Chat/pull/6409))

- meteor update ([#6631](https://github.com/RocketChat/Rocket.Chat/pull/6631))

- Move room display name logic to roomType definition ([#6585](https://github.com/RocketChat/Rocket.Chat/pull/6585))

- Move wordpress packages client files to client folder ([#6571](https://github.com/RocketChat/Rocket.Chat/pull/6571))

- New feature: Room announcement ([#6351](https://github.com/RocketChat/Rocket.Chat/pull/6351) by [@billtt](https://github.com/billtt))

- Only configure LoggerManager on server ([#6596](https://github.com/RocketChat/Rocket.Chat/pull/6596))

- Password reset Cleaner text ([#6319](https://github.com/RocketChat/Rocket.Chat/pull/6319))

- POC Google Natural Language integration ([#6298](https://github.com/RocketChat/Rocket.Chat/pull/6298))

- Remove coffeescript package from ui-flextab ([#6543](https://github.com/RocketChat/Rocket.Chat/pull/6543) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- Remove coffeescript package from ui-sidenav ([#6542](https://github.com/RocketChat/Rocket.Chat/pull/6542) by [@Kiran-Rao](https://github.com/Kiran-Rao))

- Remove Deprecated Shared Secret Package ([#6540](https://github.com/RocketChat/Rocket.Chat/pull/6540))

- rocketchat-channel-settings coffee to js ([#6551](https://github.com/RocketChat/Rocket.Chat/pull/6551))

- rocketchat-channel-settings-mail-messages coffee to js ([#6541](https://github.com/RocketChat/Rocket.Chat/pull/6541))

- rocketchat-lib part1 ([#6553](https://github.com/RocketChat/Rocket.Chat/pull/6553))

- rocketchat-ui coffee to js part1 ([#6504](https://github.com/RocketChat/Rocket.Chat/pull/6504))

- Side-nav CoffeeScript to JavaScript ([#6264](https://github.com/RocketChat/Rocket.Chat/pull/6264))

- Side-nav CoffeeScript to JavaScript II ([#6266](https://github.com/RocketChat/Rocket.Chat/pull/6266))

- Side-nav CoffeeScript to JavaScript III  ([#6274](https://github.com/RocketChat/Rocket.Chat/pull/6274))

- Use real name instead of username for messages and direct messages list ([#3851](https://github.com/RocketChat/Rocket.Chat/pull/3851) by [@alexbrazier](https://github.com/alexbrazier))

</details>

### 👩‍💻👨‍💻 Contributors 😍

- [@Kiran-Rao](https://github.com/Kiran-Rao)
- [@Lawri-van-Buel](https://github.com/Lawri-van-Buel)
- [@a5his](https://github.com/a5his)
- [@alexbrazier](https://github.com/alexbrazier)
- [@billtt](https://github.com/billtt)
- [@drallgood](https://github.com/drallgood)
- [@fengt](https://github.com/fengt)
- [@gdelavald](https://github.com/gdelavald)
- [@karlprieb](https://github.com/karlprieb)
- [@mrsimpson](https://github.com/mrsimpson)
- [@nathanmarcos](https://github.com/nathanmarcos)
- [@qge](https://github.com/qge)
- [@sezinkarli](https://github.com/sezinkarli)
- [@szluohua](https://github.com/szluohua)
- [@tgxn](https://github.com/tgxn)

### 👩‍💻👨‍💻 Core Team 🤓

- [@MartinSchoeler](https://github.com/MartinSchoeler)
- [@engelgabriel](https://github.com/engelgabriel)
- [@geekgonecrazy](https://github.com/geekgonecrazy)
- [@ggazzo](https://github.com/ggazzo)
- [@graywolf336](https://github.com/graywolf336)
- [@marceloschmidt](https://github.com/marceloschmidt)
- [@rodrigok](https://github.com/rodrigok)
- [@sampaiodiego](https://github.com/sampaiodiego)
