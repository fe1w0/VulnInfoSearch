TEST(FloatPoolingOpTest, MaxPoolWithZeroStride) {
  EXPECT_DEATH(
      FloatPoolingOpModel m(BuiltinOperator_MAX_POOL_2D,
                            /*input=*/{TensorType_FLOAT32, {1, 2, 4, 1}},
                            /*filter_width=*/2, /*filter_height=*/2,
                            /*output=*/{TensorType_FLOAT32, {}},
                            /*padding=*/Padding_VALID,
                            /*stride_w=*/0, /*stride_h=*/0),
      "Cannot allocate tensors");
}