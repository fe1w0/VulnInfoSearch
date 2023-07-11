int64_t OpLevelCostEstimator::CalculateTensorSize(
    const OpInfo::TensorProperties& tensor, bool* found_unknown_shapes) {
  int64_t count = CalculateTensorElementCount(tensor, found_unknown_shapes);
  int size = DataTypeSize(BaseType(tensor.dtype()));
  VLOG(2) << "Count: " << count << " DataTypeSize: " << size;
  return count * size;
}