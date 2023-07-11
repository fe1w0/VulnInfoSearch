static VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  VTermScreen *screen;
  int rows, cols;

  if(!state)
    return NULL;

  screen = vterm_allocator_malloc(vt, sizeof(VTermScreen));

  vterm_get_size(vt, &rows, &cols);

  screen->vt = vt;
  screen->state = state;

  screen->damage_merge = VTERM_DAMAGE_CELL;
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;

  screen->rows = rows;
  screen->cols = cols;

  screen->callbacks = NULL;
  screen->cbdata    = NULL;

  screen->buffers[0] = realloc_buffer(screen, NULL, rows, cols);

  screen->buffer = screen->buffers[0];

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, sizeof(VTermScreenCell) * cols);

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);

  return screen;
}