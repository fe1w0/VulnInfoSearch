TEST(TensorSliceReaderTest, MissingTensorData) {
  const string fname =
      io::JoinPath(testing::TmpDir(), "missing_data_checkpoint");
  TensorSliceWriter writer(fname, CreateTableTensorSliceBuilder);
  const int32 data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  TF_ASSERT_OK(writer.Add("test", TensorShape({4, 5}),
                          TensorSlice::ParseOrDie("0,2:-"), data));
  TF_ASSERT_OK(writer.Finish());

  MutateSavedTensorSlices(fname, [&](SavedTensorSlices sts) {
    if (sts.has_data()) {
      // Replace the data with only 4 elements.
      Fill(data, 4, sts.mutable_data()->mutable_data());
    }
    return sts.SerializeAsString();
  });

  TensorSliceReader reader(fname, OpenTableTensorSliceReader);
  TF_ASSERT_OK(reader.status());

  // The tensor should be present, but loading it should fail due to the missing
  // data.
  EXPECT_TRUE(reader.HasTensor("test", nullptr, nullptr));
  std::unique_ptr<Tensor> tensor;
  EXPECT_FALSE(reader.GetTensor("test", &tensor).ok());
}