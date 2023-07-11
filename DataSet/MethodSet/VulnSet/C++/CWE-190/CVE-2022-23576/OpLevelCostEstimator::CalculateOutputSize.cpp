int64_t OpLevelCostEstimator::CalculateOutputSize(const OpInfo& op_info,
                                                  bool* found_unknown_shapes) {
  int64_t total_output_size = 0;
  // Use float as default for calculations.
  for (const auto& output : op_info.outputs()) {
    DataType dt = output.dtype();
    const auto& original_output_shape = output.shape();
    int64_t output_size = DataTypeSize(BaseType(dt));
    int num_dims = std::max(1, original_output_shape.dim_size());
    auto output_shape = MaybeGetMinimumShape(original_output_shape, num_dims,
                                             found_unknown_shapes);
    for (const auto& dim : output_shape.dim()) {
      output_size *= dim.size();
    }
    total_output_size += output_size;
    VLOG(1) << "Output Size: " << output_size
            << " Total Output Size:" << total_output_size;
  }
  return total_output_size;
}