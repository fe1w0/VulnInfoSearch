void Lua::setParamsTable(lua_State* vm, const char* table_name,
			 const char* query) const {
  char outbuf[FILENAME_MAX];
  char *where;
  char *tok;

  char *query_string = query ? strdup(query) : NULL;

  lua_newtable(L);

  if (query_string) {
    // ntop->getTrace()->traceEvent(TRACE_WARNING, "[HTTP] %s", query_string);

    tok = strtok_r(query_string, "&", &where);

    while(tok != NULL) {
      char *_equal;

      if(strncmp(tok, "csrf", strlen("csrf")) /* Do not put csrf into the params table */
	 && (_equal = strchr(tok, '='))) {
	char *decoded_buf;
        int len;

        _equal[0] = '\0';
        _equal = &_equal[1];
        len = strlen(_equal);

        purifyHTTPParameter(tok), purifyHTTPParameter(_equal);

        // ntop->getTrace()->traceEvent(TRACE_WARNING, "%s = %s", tok, _equal);

        if((decoded_buf = (char*)malloc(len+1)) != NULL) {

          Utils::urlDecode(_equal, decoded_buf, len+1);

	  Utils::purifyHTTPparam(tok, true, false);
	  Utils::purifyHTTPparam(decoded_buf, false, false);

	  /* Now make sure that decoded_buf is not a file path */
	  FILE *fd;
	  if((decoded_buf[0] == '.')
	     && ((fd = fopen(decoded_buf, "r"))
		 || (fd = fopen(realpath(decoded_buf, outbuf), "r")))) {

	    ntop->getTrace()->traceEvent(TRACE_WARNING, "Discarded '%s'='%s' as argument is a valid file path",
					 tok, decoded_buf);
	    decoded_buf[0] = '\0';
	    fclose(fd);
	  }

	  /* ntop->getTrace()->traceEvent(TRACE_WARNING, "'%s'='%s'", tok, decoded_buf); */

	  /* put tok and the decoded buffer in to the table */
	  lua_push_str_table_entry(vm, tok, decoded_buf);

          free(decoded_buf);
        } else
          ntop->getTrace()->traceEvent(TRACE_WARNING, "Not enough memory");
      }

      tok = strtok_r(NULL, "&", &where);
    } /* while */
  }

  if(query_string) free(query_string);

  if(table_name)
    lua_setglobal(L, table_name);
  else
    lua_setglobal(L, (char*)"_GET"); /* Default */
}