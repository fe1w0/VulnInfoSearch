VTermState *vterm_obtain_state(VTerm *vt)
{
  VTermState *state;
  if(vt->state)
    return vt->state;

  state = vterm_state_new(vt);
  vt->state = state;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * sizeof(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfo = vterm_allocator_malloc(state->vt, state->rows * sizeof(VTermLineInfo));

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  if(*state->encoding_utf8.enc->init != NULL)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  return state;
}