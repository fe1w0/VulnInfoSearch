TEST_F(QuantizedConv2DTest, Small32Bit) {
  const int stride = 1;
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
  const int image_height = 3;
  const int image_batch_count = 1;
  AddInputFromArray<quint8>(
      TensorShape({image_batch_count, image_height, image_width, depth}),
      {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120});
  const int filter_size = 3;
  const int filter_count = 1;
  AddInputFromArray<quint8>(
      TensorShape({filter_size, filter_size, depth, filter_count}),
      {10, 40, 70, 20, 50, 80, 30, 60, 90});
  AddInputFromArray<float>(TensorShape({1}), {0});
  AddInputFromArray<float>(TensorShape({1}), {255.0f});
  AddInputFromArray<float>(TensorShape({1}), {0});
  AddInputFromArray<float>(TensorShape({1}), {255.0f});

  TF_ASSERT_OK(RunOpKernel());
  const int expected_width = image_width;
  const int expected_height = image_height * filter_count;
  Tensor expected(DT_QINT32, TensorShape({image_batch_count, expected_height,
                                          expected_width, filter_count}));
  test::FillValues<qint32>(
      &expected, {10500, 15000, 18300, 9500, 23500, 31200, 35700, 17800, 18700,
                  23400, 26100, 12100});
  test::ExpectTensorEqual<qint32>(expected, *GetOutput(0));
}