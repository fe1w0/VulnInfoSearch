void imap_quote_string(char *dest, size_t dlen, const char *src)
{
  static const char quote[] = "\"\\";
  char *pt = dest;
  const char *s = src;

  *pt++ = '"';
  /* save room for trailing quote-char */
  dlen -= 2;

  for (; *s && dlen; s++)
  {
    if (strchr(quote, *s))
    {
      dlen -= 2;
      if (dlen == 0)
        break;
      *pt++ = '\\';
      *pt++ = *s;
    }
    else
    {
      *pt++ = *s;
      dlen--;
    }
  }
  *pt++ = '"';
  *pt = '\0';
}