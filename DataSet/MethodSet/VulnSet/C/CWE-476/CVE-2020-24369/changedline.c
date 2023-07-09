static int changedline (const Proto *p, int oldpc, int newpc) {
  while (oldpc++ < newpc) {
    if (p->lineinfo[oldpc] != 0)
      return (luaG_getfuncline(p, oldpc - 1) != luaG_getfuncline(p, newpc));
  }
  return 0;  /* no line changes in the way */
}