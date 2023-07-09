TfLiteIntArray* TfLiteIntArrayCreate(int size) {
  TfLiteIntArray* ret =
      (TfLiteIntArray*)malloc(TfLiteIntArrayGetSizeInBytes(size));
  ret->size = size;
  return ret;
}