TEST(TensorSliceReaderTest, UnsupportedTensorType) {
  const string fname = io::JoinPath(testing::TmpDir(), "int32_ref_checkpoint");
  TensorSliceWriter writer(fname, CreateTableTensorSliceBuilder);
  const int32 data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  TensorShape shape({4, 5});
  TensorSlice slice = TensorSlice::ParseOrDie("0,2:-");
  TF_CHECK_OK(writer.Add("test", shape, slice, data));
  TF_CHECK_OK(writer.Finish());

  MutateSavedTensorSlices(fname, [](SavedTensorSlices sts) {
    if (sts.has_meta()) {
      for (auto& tensor : *sts.mutable_meta()->mutable_tensor()) {
        tensor.set_type(DT_INT32_REF);
      }
    }
    return sts.SerializeAsString();
  });

  TensorSliceReader reader(fname, OpenTableTensorSliceReader);
  TF_CHECK_OK(reader.status());

  // The tensor should be present, but loading it should fail due to the
  // unsupported type.
  EXPECT_TRUE(reader.HasTensor("test", nullptr, nullptr));
  std::unique_ptr<Tensor> tensor;
  EXPECT_FALSE(reader.GetTensor("test", &tensor).ok());
}