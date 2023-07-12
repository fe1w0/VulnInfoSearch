TEST_F(QuantizedConv2DTest, SmallWithNoZero) {
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
  // Here we're testing a slow implementation path, where zero is not
  // representable in the image data and so simple border padding is not
  // possible, so we have a min value greater than 0.
  const float image_min = 1.0f;
  const float image_max = 12.0f;
  Tensor image_float(DT_FLOAT,
                     {image_batch_count, image_height, image_width, depth});
  test::FillValues<float>(&image_float,
                          {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
  Tensor image_quantized =
      FloatTensorToQuantized<quint8>(image_float, image_min, image_max);
  const int filter_size = 3;
  const int filter_count = 1;
  const float filter_min = 1.0f;
  const float filter_max = 9.0f;
  Tensor filter_float(DT_FLOAT,
                      {filter_size, filter_size, depth, filter_count});
  test::FillValues<float>(&filter_float, {1, 4, 7, 2, 5, 8, 3, 6, 9});
  Tensor filter_quantized =
      FloatTensorToQuantized<quint8>(filter_float, filter_min, filter_max);
  AddInputFromArray<quint8>(image_quantized.shape(),
                            image_quantized.flat<quint8>());
  AddInputFromArray<quint8>(filter_quantized.shape(),
                            filter_quantized.flat<quint8>());
  AddInputFromArray<float>(TensorShape({1}), {image_min});
  AddInputFromArray<float>(TensorShape({1}), {image_max});
  AddInputFromArray<float>(TensorShape({1}), {filter_min});
  AddInputFromArray<float>(TensorShape({1}), {filter_max});
  TF_ASSERT_OK(RunOpKernel());
  const int expected_width = image_width;
  const int expected_height = image_height * filter_count;
  Tensor expected_float(
      DT_FLOAT, TensorShape({image_batch_count, expected_height, expected_width,
                             filter_count}));
  test::FillValues<float>(&expected_float, {105, 150, 183, 95, 235, 312, 357,
                                            178, 187, 234, 261, 121});
  const Tensor& output_quantized = *GetOutput(0);
  const float output_min = GetOutput(1)->flat<float>()(0);
  const float output_max = GetOutput(2)->flat<float>()(0);
  Tensor output_float =
      QuantizedTensorToFloat<qint32>(output_quantized, output_min, output_max);
  test::ExpectTensorNear<float>(expected_float, output_float, 1.0);
}