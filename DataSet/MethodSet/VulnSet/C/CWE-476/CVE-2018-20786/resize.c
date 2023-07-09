static int resize(int new_rows, int new_cols, VTermPos *delta, void *user)
{
  VTermScreen *screen = user;

  int is_altscreen = (screen->buffers[1] && screen->buffer == screen->buffers[1]);

  int old_rows = screen->rows;
  int old_cols = screen->cols;
  int first_blank_row;

  if(!is_altscreen && new_rows < old_rows) {
    // Fewer rows - determine if we're going to scroll at all, and if so, push
    // those lines to scrollback
    VTermPos pos = { 0, 0 };
    VTermPos cursor = screen->state->pos;
    // Find the first blank row after the cursor.
    for(pos.row = old_rows - 1; pos.row >= new_rows; pos.row--)
      if(!vterm_screen_is_eol(screen, pos) || cursor.row == pos.row)
        break;

    first_blank_row = pos.row + 1;
    if(first_blank_row > new_rows) {
      VTermRect rect = {0,0,0,0};
      rect.end_row   = old_rows;
      rect.end_col   = old_cols;
      scrollrect(rect, first_blank_row - new_rows, 0, user);
      vterm_screen_flush_damage(screen);

      delta->row -= first_blank_row - new_rows;
    }
  }

  screen->buffers[0] = realloc_buffer(screen, screen->buffers[0], new_rows, new_cols);
  if(screen->buffers[1])
    screen->buffers[1] = realloc_buffer(screen, screen->buffers[1], new_rows, new_cols);

  screen->buffer = is_altscreen ? screen->buffers[1] : screen->buffers[0];

  screen->rows = new_rows;
  screen->cols = new_cols;

  if(screen->sb_buffer)
    vterm_allocator_free(screen->vt, screen->sb_buffer);

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, sizeof(VTermScreenCell) * new_cols);

  if(new_cols > old_cols) {
    VTermRect rect;
    rect.start_row = 0;
    rect.end_row   = old_rows;
    rect.start_col = old_cols;
    rect.end_col   = new_cols;
    damagerect(screen, rect);
  }

  if(new_rows > old_rows) {
    if(!is_altscreen && screen->callbacks && screen->callbacks->sb_popline) {
      int rows = new_rows - old_rows;
      while(rows) {
        VTermRect rect = {0,0,0,0};
        VTermPos pos = { 0, 0 };
        if(!(screen->callbacks->sb_popline(screen->cols, screen->sb_buffer, screen->cbdata)))
          break;

	rect.end_row   = screen->rows;
	rect.end_col   = screen->cols;
        scrollrect(rect, -1, 0, user);

        for(pos.col = 0; pos.col < screen->cols; pos.col += screen->sb_buffer[pos.col].width)
          vterm_screen_set_cell(screen, pos, screen->sb_buffer + pos.col);

        rect.end_row = 1;
        damagerect(screen, rect);

        vterm_screen_flush_damage(screen);

        rows--;
        delta->row++;
      }
    }

    {
      VTermRect rect;
      rect.start_row = old_rows;
      rect.end_row   = new_rows;
      rect.start_col = 0;
      rect.end_col   = new_cols;
      damagerect(screen, rect);
    }
  }

  if(screen->callbacks && screen->callbacks->resize)
    return (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  return 1;
}