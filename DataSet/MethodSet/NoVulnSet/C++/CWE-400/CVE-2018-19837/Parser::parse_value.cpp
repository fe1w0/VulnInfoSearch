  Expression_Obj Parser::parse_value()
  {
    lex< css_comments >(false);
    if (lex< ampersand >())
    {
      if (match< ampersand >()) {
        warning("In Sass, \"&&\" means two copies of the parent selector. You probably want to use \"and\" instead.", pstate);
      }
      return SASS_MEMORY_NEW(Parent_Selector, pstate); }

    if (lex< kwd_important >())
    { return SASS_MEMORY_NEW(String_Constant, pstate, "!important"); }

    // parse `10%4px` into separated items and not a schema
    if (lex< sequence < percentage, lookahead < number > > >())
    { return lexed_percentage(lexed); }

    if (lex< sequence < number, lookahead< sequence < op, number > > > >())
    { return lexed_number(lexed); }

    // string may be interpolated
    if (lex< sequence < quoted_string, lookahead < exactly <'-'> > > >())
    { return parse_string(); }

    if (const char* stop = peek< value_schema >())
    { return parse_value_schema(stop); }

    // string may be interpolated
    if (lex< quoted_string >())
    { return parse_string(); }

    if (lex< kwd_true >())
    { return SASS_MEMORY_NEW(Boolean, pstate, true); }

    if (lex< kwd_false >())
    { return SASS_MEMORY_NEW(Boolean, pstate, false); }

    if (lex< kwd_null >())
    { return SASS_MEMORY_NEW(Null, pstate); }

    if (lex< identifier >()) {
      return color_or_string(lexed);
    }

    if (lex< percentage >())
    { return lexed_percentage(lexed); }

    // match hex number first because 0x000 looks like a number followed by an identifier
    if (lex< sequence < alternatives< hex, hex0 >, negate < exactly<'-'> > > >())
    { return lexed_hex_color(lexed); }

    if (lex< hexa >())
    {
      std::string s = lexed.to_string();

      deprecated(
        "The value \""+s+"\" is currently parsed as a string, but it will be parsed as a color in",
        "future versions of Sass. Use \"unquote('"+s+"')\" to continue parsing it as a string.",
        true, pstate
      );

      return SASS_MEMORY_NEW(String_Quoted, pstate, lexed);
    }

    if (lex< sequence < exactly <'#'>, identifier > >())
    { return SASS_MEMORY_NEW(String_Quoted, pstate, lexed); }

    // also handle the 10em- foo special case
    // alternatives < exactly < '.' >, .. > -- `1.5em-.75em` is split into a list, not a binary expression
    if (lex< sequence< dimension, optional< sequence< exactly<'-'>, lookahead< alternatives < space > > > > > >())
    { return lexed_dimension(lexed); }

    if (lex< sequence< static_component, one_plus< strict_identifier > > >())
    { return SASS_MEMORY_NEW(String_Constant, pstate, lexed); }

    if (lex< number >())
    { return lexed_number(lexed); }

    if (lex< variable >())
    { return SASS_MEMORY_NEW(Variable, pstate, Util::normalize_underscores(lexed)); }

    css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");

    // unreachable statement
    return 0;
  }