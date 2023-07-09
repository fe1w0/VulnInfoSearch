INTERNAL void vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[0]);
  if(screen->buffers[1])
    vterm_allocator_free(screen->vt, screen->buffers[1]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}