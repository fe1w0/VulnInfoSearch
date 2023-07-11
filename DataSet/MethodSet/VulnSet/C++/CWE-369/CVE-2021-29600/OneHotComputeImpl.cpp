void OneHotComputeImpl(const OneHotContext& op_context) {
  // prefix_dim_size == # of elements before the axis
  // depth == # of elements per axis
  // suffix_dim_size == # of elements after the axis
  int prefix_dim_size = 1;
  for (int i = 0; i < op_context.axis; ++i) {
    prefix_dim_size *= op_context.indices->dims->data[i];
  }
  const int suffix_dim_size = NumElements(op_context.indices) / prefix_dim_size;
  const int depth = *op_context.depth->data.i32;

  const T on_value = *GetTensorData<T>(op_context.on_value);
  const T off_value = *GetTensorData<T>(op_context.off_value);

  // View the indices as a matrix of size:
  //     prefix_dim_size x suffix_dim_size
  // View the output as a matrix of size:
  //     prefix_dim_size x depth x suffix_dim_size
  // Then the output is:
  //     output(i, j, k) == (indices(i, k) == j) ? on : off
  T* output = GetTensorData<T>(op_context.output);
  const TI* indices = GetTensorData<TI>(op_context.indices);
  for (int i = 0; i < prefix_dim_size; ++i) {
    for (int j = 0; j < depth; ++j) {
      for (int k = 0; k < suffix_dim_size; ++k, ++output) {
        *output = static_cast<int>(indices[i * suffix_dim_size + k]) == j
                      ? on_value
                      : off_value;
      }
    }
  }
}