TEST_F(QuantizedConv2DTest, OddPadding) {
  const int stride = 2;
  TF_ASSERT_OK(NodeDefBuilder("quantized_conv_op", "QuantizedConv2D")
                   .Input(FakeInput(DT_QUINT8))
                   .Input(FakeInput(DT_QUINT8))
                   .Input(FakeInput(DT_FLOAT))
                   .Input(FakeInput(DT_FLOAT))
                   .Input(FakeInput(DT_FLOAT))
                   .Input(FakeInput(DT_FLOAT))
                   .Attr("out_type", DataTypeToEnum<qint32>::v())
                   .Attr("strides", {1, stride, stride, 1})
                   .Attr("padding", "SAME")
                   .Finalize(node_def()));
  TF_ASSERT_OK(InitOp());

  const int depth = 1;
  const int image_width = 4;
  const int image_height = 4;
  const int image_batch_count = 1;
  AddInputFromArray<quint8>(
      TensorShape({image_batch_count, image_height, image_width, depth}),
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
  const int filter_size = 3;
  const int filter_count = 1;
  AddInputFromArray<quint8>(
      TensorShape({filter_size, filter_size, depth, filter_count}),
      {1, 2, 3, 4, 5, 6, 7, 8, 9});
  AddInputFromArray<float>(TensorShape({1}), {0});
  AddInputFromArray<float>(TensorShape({1}), {255.0f});
  AddInputFromArray<float>(TensorShape({1}), {0});
  AddInputFromArray<float>(TensorShape({1}), {255.0f});

  TF_ASSERT_OK(RunOpKernel());
  const int expected_width = image_width / stride;
  const int expected_height = (image_height * filter_count) / stride;
  Tensor expected(DT_QINT32, TensorShape({image_batch_count, expected_height,
                                          expected_width, filter_count}));
  test::FillValues<qint32>(&expected, {348, 252, 274, 175});
  test::ExpectTensorEqual<qint32>(expected, *GetOutput(0));
}