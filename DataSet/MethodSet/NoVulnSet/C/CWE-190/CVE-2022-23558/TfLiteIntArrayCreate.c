TfLiteIntArray* TfLiteIntArrayCreate(int size) {
  size_t alloc_size = TfLiteIntArrayGetSizeInBytes(size);
  if (alloc_size <= 0) return NULL;
  TfLiteIntArray* ret = (TfLiteIntArray*)malloc(alloc_size);
  if (!ret) return ret;
  ret->size = size;
  return ret;
}