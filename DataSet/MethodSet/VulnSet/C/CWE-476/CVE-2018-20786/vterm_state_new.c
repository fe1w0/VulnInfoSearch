static VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, sizeof(VTermState));

  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = NULL;
  state->cbdata    = NULL;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  return state;
}