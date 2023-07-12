TEST(CachedTensorSliceReaderTest, SimpleFloat) {
  CachedTensorSliceReaderTesterHelper(CreateTableTensorSliceBuilder,
                                      OpenTableTensorSliceReader);
}