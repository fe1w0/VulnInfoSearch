# Changes in HTMLDOC v1.9.12

- Fixed a crash bug with "data:" URIs and EPUB output (Issue #410)
- Fixed crash bugs with bogus table attributes (Issue #416, Issue #417)
- Fixed a crash bug with malformed URIs (Issue #418)
- Fixed a crash bug with malformed GIF files (Issue #423)
- Fixed some issues reported by Coverity.


# Changes in HTMLDOC v1.9.11

- Added high-resolution desktop icons for Linux.
- Updated the internal HTTP library to fix truncation of redirection URLs
  (Issue #396)
- Fixed a regression in the handling of character entities for UTF-8 input
  (Issue #401)
- The `--numbered` option did not work when the table-of-contents was disabled
  (Issue #405)


# Changes in HTMLDOC v1.9.10

- Updated local zlib to v1.2.11.
- Updated local libpng to v1.6.37.
- Fixed packaging issues on macOS and Windows (Issue #377, Issue #386)
- Now ignore sRGB profile errors in PNG files (Issue #390)
- The GUI would crash when saving (Issue #391)
- Page comments are now allowed in `pre` text (Issue #394)


# Changes in HTMLDOC v1.9.9

- Fixed a redirection issue - some sites (incorrectly) provide an incomplete
  Location: URL in the HTTP response.
- Fixed https: support on newer versions of Windows (Issue #378)
- Fixed a problem with remote URLs containing spaces (Issue #379)
- Fixed a UTF-8 processing bug for Markdown files (Issue #383)
- Added support for `<FONT FACE="monospace">` (Issue #385)


# Changes in HTMLDOC v1.9.8

- Added support for a `HTMLDOC.filename` META keyword that controls the filename
  reported in CGI mode; the default remains "htmldoc.pdf" (Issue #367)
- Fixed a paragraph formatting issue with large inline images (Issue #369)
- Fixed a buffer underflow issue (Issue #370)
- Fixed PDF page numbers (Issue #371)
- Added support for a new `L` header/footer format (`$LETTERHEAD`), which
  inserts a letterhead image at its full size (Issue #372, Issue #373,
  Issue #375)
- Updated the build documentation (Issue #374)


# Changes in HTMLDOC v1.9.7

- Refactored the PRE rendering code to work around compiler optimization bugs
  (Issue #349)
- Added support for links with targets (Issue #351)
- Fixed a table rowspan + valign bug (Issue #360)


# Changes in HTMLDOC v1.9.6

- Added support for data URIs (Issue #340)
- HTMLDOC no longer includes a PDF table of contents when converting a single
  web page (Issue #344)
- Updated the markdown support with external links, additional inline markup,
  and hard line breaks.
- Links in markdown text no longer render with a leading space as part of the
  link (Issue #346)
- Fixed a buffer underflow bug discovered by AddressSanitizer.
- Fixed a bug in UTF-8 support (Issue #348)
- PDF output now includes the base language of the input document(s)
  (Issue #350)
- Optimized the loading of font widths (Issue #354)
- Optimized PDF page resources (Issue #356)
- Optimized the base memory used for font widths (Issue #357)
- Added proper `&shy;` support (Issue #361)
- Title files can now be markdown.


# Changes in HTMLDOC v1.9.5

- The GUI did not support EPUB output.
- Empty markdown table cells were not rendered in PDF or PostScript output.
- The automatically-generated title page now supports both "docnumber" and
  "version" metadata.
- Added support for dc:subject and dc:language metadata in EPUB output from the
  HTML keywords and lang values.
- Added support for the subject and language metadata in markdown input.
- Fixed a buffer underflow bug (Issue #338)
- `htmldoc --help` now reports whether HTTPS URLs are supported (Issue #339)
- Fixed an issue with HTML title pages and EPUB output.


# Changes in HTMLDOC v1.9.4

- Inline fixed-width text is no longer reduced in size automatically
  (Issue #309)
- Optimized initialization of font width data (Issue #334)


# Changes in HTMLDOC v1.9.3

- Fixed formatting bugs with aligned images (Issue #322, Issue #324)
- Fixed support for three digit "#RGB" color values (Issue #323)
- Fixed character set support for markdown metadata.
- Updated libpng to v1.6.34 (Issue #326)
- The makefiles did not use the CPPFLAGS value (Issue #328)


# Changes in HTMLDOC v1.9.2

- Added Markdown table support.
- Fixed parsing of TBODY, TFOOT, and THEAD elements in HTML files.


# Changes in HTMLDOC v1.9.1

- Fixed monospace font size issue (Issue #309)
- Added support for reproducible builds (Issue #310)
- Added limited support for the HTML 4.0 SPAN element (Issue #311)
- Added (extremely limited) UTF-8 support for input files (Issue #314)
- Fixed buffer underflow for (invalid) short HTML comments (Issue #316)
- Now indent PRE text, by popular request.
- EPUB output now makes sure that `<element property>` is written as
  `<element property="property">`.
- Now support both NAME and ID for table-of-contents targets.


# Changes in HTMLDOC v1.9

- Added support for repeating a single header row for tables that span multiple
  pages (Issue #16)
- Added support for embedding the current filename/URL in the header or footer
  (Issue #50)
- Added EPUB support (Issue #301)
- Added Markdown support (Issue #302)
- Fixed a regression in header/footer image scaling (Issue #303)
- Documentation updates (Issue #305)
- Compiler fixes (Issue #304, Issue #306)
- Fixed a bug when running HTMLDOC as a macOS application.
- Updated the bundled libpng to v1.6.29.


# Changes in HTMLDOC v1.8.30

- Updated documentation to reflect new project page on Github.
- Dropped old CDE and IRIX desktop integration files.
- Cleaned up the GUI and adopted new default text editors for Linux and macOS.
- PAGE BREAK comments at the end of a file in web page mode would lose the
  first page (Issue #251)
- Fixed the scaling of header/footer images to limit them to the height of the
  header or footer (Issue #273)
- Fixed an issue with the top-level makefile not exiting with an error as
  needed (Issue #282)
- Fixed a URL referencing bug when the same hostname but a different port was
  used (Issue #290)
- Fixed build issue on macOS (Issue #291)
- Fixed handling of indexed+alpha PNG images (Issue #295)


# Changes in HTMLDOC v1.8.29

- Updated local PNG library to version 1.6.20.
- Updated local JPEG library to version 9b.
- Dropped support for OpenSSL.
- Added configure script support for libjpeg-turbo.
- Updated HTTP code to latest CUPS/ippsample sources.
- Duplex PDF output incorrectly forced an even number of pages
- The table of contents showed the wrong page numbers after headings containing
  the "_HD_OMIT_TOC" attribute.
- Fixed reported build issues
- The configure script's --enable-local* options did not work.


# Changes in HTMLDOC v1.8.28

- Updated local zlib to version 1.2.8.
- Updated local PNG library to version 1.6.8.
- Updated local JPEG library to version 9.
- Updated default PDF version to 1.4.
- SECURITY: Fixed three buffer overflow issues when reading AFM files and
  parsing page sizes.
- Fixed incompatibility with Fortify's version of strcpy, which does not work
  properly with variable-length arrays
- Fixed compilation against PNG library 1.5 or later
- Fixed documentation errors
- Marked Zapf-Dingbats as a standard font
- Fixed GPL license text in GUI
- Fixed a table formatting problem when a column has multiple colspan values
- Fixed parsing of HTML comments
- Fixed potential out-of-bounds read in table-of-contents rendering code
- Fixed handling of image URLs with ampersands in them
- Fixed top/bottom margins for logo and header/footer images
- Fixed image alignment bug
- Fixed X11 build problem


# Changes in HTMLDOC v1.8.27

- Fixed a crash bug that appeared when more than 10 blank pages were present in
  a document
- Color changes were not reflected in PRE text
- Remote URLs did not always work on older operating systems
- Image filenames using % escapes were not decoded properly.
- Rows using BGCOLOR that spanned across multiple pages did not render properly
- Rows no longer start on a new page due to a cell with both HEIGHT and ROWSPAN
  specified
- CMYK JPEG images caused HTMLDOC to crash
- Table cell width calculations didn't always account for the proper minimum
  width
- Images were not copied when generating indexed HTML output to a directory
- Changing the bottom margin resulted in text that was formatted below the
  bottom margin.
- The Monospace-Oblique font was not embedded properly in PDF files.


# Changes in HTMLDOC v1.8.26

- Outline and keyword strings in PDF files are now stored as Unicode
- The Flate compression code could get in an infinite loop if it ran out of
  memory
- Book files saved from the GUI did not handle filenames with spaces
- Fixed and re-enabled the ASCII85Device filter support in PostScript Level 2/3
  output
- Character entities in the first word of a file were not rendered properly
- Fixed-size table columns were incorrectly resized when a table width was also
  specified and there was extra space to distribute
- Text could "walk" up or down when in-line images were used
- Row backgrounds incorrectly replaced cell backgrounds when the first cell in a
  row used ROWSPAN
- HTMLDOC did not correctly parse FONT FACE attributes
- Images in Level 2/3 PostScript output did not work on some printers
- The GUI did not use the first page header


# Changes in HTMLDOC v1.8.25

- Added "--overflow" and "--no-overflow" command-line options to show or hide
  the content-too-large errors; the default is "--no-overflow".
- Added "--header1" command-line option and "HEADER1" page comments to set the
  page header for the first page of each chapter.
- Added "timing" and "remotebytes" debug data generation.
- Added DejaVu font collection to better support Cyrillic and Greek text; the
  new fonts are available under the generic names "monospace", "sans", and
  "serif".
- Added "--referer" command-line option and corresponding CGI-mode support to
  pass Referer: information in HTTP requests
- On Windows, HTMLDOC now logs CGI mode errors to a file called "htmldoc.log" in
  the Windows temporary directory.
- HTMLDOC no longer uses Base-85 encoding for image data when producing Level 2
  and 3 PostScript output. It appears that many printers and PostScript
  interpreters cannot properly decode this data when the original image data is
  not a multiple of 8 bits.
- HTMLDOC now renders STRONG elements in boldface instead of bold-italic to
  match the W3C recommendations.
- HTMLDOC now automatically inserts a TR element before a TD or TH element as
  needed to improve web site compatibility; this also triggers a HTML error in
  --strict mode.
- "$HFIMAGEn" didn't work in a header/footer string.
- HTMLDOC could crash when rendering a table.
- Book files were not used in CGI mode
- Cookies were not sent in HTTP requests
- Table cells were not aligned properly when the ROWSPAN attribute was set to 1
- HTMLDOC crashed when rendering unresolved hyperlinks in aligned images
- Documented the HTMLDOC_NOCGI environment variable
- HTMLDOC sometimes crashed when rendering tables with background colors
- HTMLDOC would crash when writing encrypted strings longer than 1024 bytes
- HTMLDOC didn't set the data directory when running in CGI mode on Windows.
- HTMLDOC could crash when loading the Symbol.afm file
- HTMLDOC did not always honor HEIGHT attributes in table rows.
- Tables with a mix of colspan and rowspan sometimes caused cells to be moved
  vertically outside the cell.
