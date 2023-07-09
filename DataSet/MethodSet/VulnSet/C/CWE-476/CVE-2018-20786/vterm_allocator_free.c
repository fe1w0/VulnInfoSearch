INTERNAL void vterm_allocator_free(VTerm *vt, void *ptr)
{
  (*vt->allocator->free)(ptr, vt->allocdata);
}