size_t TfLiteIntArrayGetSizeInBytes(int size) {
  static TfLiteIntArray dummy;

  size_t computed_size = sizeof(dummy) + sizeof(dummy.data[0]) * size;
#if defined(_MSC_VER)
  // Context for why this is needed is in http://b/189926408#comment21
  computed_size -= sizeof(dummy.data[0]);
#endif
  return computed_size;
}