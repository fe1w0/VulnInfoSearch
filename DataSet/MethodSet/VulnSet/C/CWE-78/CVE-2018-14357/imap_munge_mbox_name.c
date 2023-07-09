void imap_munge_mbox_name(struct ImapData *idata, char *dest, size_t dlen, const char *src)
{
  char *buf = mutt_str_strdup(src);
  imap_utf_encode(idata, &buf);

  imap_quote_string(dest, dlen, buf);

  FREE(&buf);
}