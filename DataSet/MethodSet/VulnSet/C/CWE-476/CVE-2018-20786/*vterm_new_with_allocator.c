VTerm *vterm_new_with_allocator(int rows, int cols, VTermAllocatorFunctions *funcs, void *allocdata)
{
  /* Need to bootstrap using the allocator function directly */
  VTerm *vt = (*funcs->malloc)(sizeof(VTerm), allocdata);

  vt->allocator = funcs;
  vt->allocdata = allocdata;

  vt->rows = rows;
  vt->cols = cols;

  vt->parser.state = NORMAL;

  vt->parser.callbacks = NULL;
  vt->parser.cbdata    = NULL;

  vt->parser.strbuffer_len = 500; /* should be able to hold an OSC string */
  vt->parser.strbuffer_cur = 0;
  vt->parser.strbuffer = vterm_allocator_malloc(vt, vt->parser.strbuffer_len);

  vt->outbuffer_len = 200;
  vt->outbuffer_cur = 0;
  vt->outbuffer = vterm_allocator_malloc(vt, vt->outbuffer_len);

  return vt;
}