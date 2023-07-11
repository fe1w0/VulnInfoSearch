TEST_F(QuantizedConv2DTest, Small) {
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
  // The image data should always be able to represent zero, to allow a fast
  // implementation of border padding, so we set the min value to 0.
  const float image_min = 0.0f;
  const float image_max = 12.0f;
  // The image matrix is:
  // |  1 |  2 |  3 |  4 |
  // |  5 |  6 |  7 |  8 |
  // |  9 | 10 | 11 | 12 |
  Tensor image_float(DT_FLOAT,
                     {image_batch_count, image_height, image_width, depth});
  test::FillValues<float>(&image_float,
                          {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
  Tensor image_quantized =
      FloatTensorToQuantized<quint8>(image_float, image_min, image_max);

  // The filter matrix is:
  // | 1 | 4 | 7 |
  // | 2 | 5 | 8 |
  // | 3 | 6 | 9 |
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

  // We're sliding the 3x3 filter across the 3x4 image, with accesses outside
  // the input set to zero because we're using the 'SAME' padding mode.
  // The calculations behind the expected output are:
  // (1*0)+(4*0)+(7*0)+(2*0)+(5*1)+(8*2)+(3*0)+(6*5)+(9*6)=105
  // (1*0)+(4*0)+(7*0)+(2*1)+(5*2)+(8*3)+(3*5)+(6*6)+(9*7)=150
  // (1*0)+(4*0)+(7*0)+(2*2)+(5*3)+(8*4)+(3*6)+(6*7)+(9*8)=183
  // (1*0)+(4*0)+(7*0)+(2*3)+(5*4)+(8*0)+(3*7)+(6*8)+(9*0)=95
  // (1*0)+(4*1)+(7*2)+(2*0)+(5*5)+(8*6)+(3*0)+(6*9)+(9*10)=235
  // (1*1)+(4*2)+(7*3)+(2*5)+(5*6)+(8*7)+(3*9)+(6*10)+(9*11)=312
  // (1*2)+(4*3)+(7*4)+(2*6)+(5*7)+(8*8)+(3*10)+(6*11)+(9*12)=357
  // (1*3)+(4*4)+(7*0)+(2*7)+(5*8)+(8*0)+(3*11)+(6*12)+(9*0)=178
  // (1*0)+(4*5)+(7*6)+(2*0)+(5*9)+(8*10)+(3*0)+(6*0)+(9*0)=187
  // (1*5)+(4*6)+(7*7)+(2*9)+(5*10)+(8*11)+(3*0)+(6*0)+(9*0)=234
  // (1*6)+(4*7)+(7*8)+(2*10)+(5*11)+(8*12)+(3*0)+(6*0)+(9*0)=261
  // (1*7)+(4*11)+(7*0)+(2*8)+(5*12)+(8*0)+(3*0)+(6*0)+(9*0)=121
  // This means we should end up with this matrix:
  // |  105  |  150  |  183  |   95  |
  // |  235  |  312  |  357  |  178  |
  // |  187  |  234  |  261  |  121  |
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