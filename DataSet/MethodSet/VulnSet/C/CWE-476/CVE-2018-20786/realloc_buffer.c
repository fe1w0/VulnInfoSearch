static ScreenCell *realloc_buffer(VTermScreen *screen, ScreenCell *buffer, int new_rows, int new_cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, sizeof(ScreenCell) * new_rows * new_cols);
  int row, col;

  for(row = 0; row < new_rows; row++) {
    for(col = 0; col < new_cols; col++) {
      ScreenCell *new_cell = new_buffer + row*new_cols + col;

      if(buffer && row < screen->rows && col < screen->cols)
        *new_cell = buffer[row * screen->cols + col];
      else {
        new_cell->chars[0] = 0;
        new_cell->pen = screen->pen;
      }
    }
  }

  if(buffer)
    vterm_allocator_free(screen->vt, buffer);

  return new_buffer;
}