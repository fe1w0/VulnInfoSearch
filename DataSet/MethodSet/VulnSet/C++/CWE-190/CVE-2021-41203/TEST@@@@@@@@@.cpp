TEST(TensorSliceTest, Equality) {
  {  // Dims are different.
    TensorSlice slice1(3);
    TensorSlice slice2(2);
    EXPECT_TRUE(slice1 != slice2);
    EXPECT_TRUE(slice2 != slice1);
  }
  {  // Both are 3-dim full slices.
    TensorSlice slice1(3);
    TensorSlice slice2({{0, -1}, {0, -1}, {0, -1}});
    EXPECT_TRUE(slice1 == slice2);
    EXPECT_TRUE(slice2 == slice1);
  }
  {  // Differs in one dimension.
    TensorSlice slice1(3);
    TensorSlice slice2({{0, -1}, {0, 1}, {0, -1}});
    EXPECT_TRUE(slice1 != slice2);
    EXPECT_TRUE(slice2 != slice1);
  }
}