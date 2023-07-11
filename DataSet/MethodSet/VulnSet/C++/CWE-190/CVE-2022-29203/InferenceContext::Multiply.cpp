Status InferenceContext::Multiply(DimensionHandle first,
                                  DimensionOrConstant second,
                                  DimensionHandle* out) {
  const int64_t first_value = Value(first);
  const int64_t second_value = Value(second);
  // Special cases.
  if (first_value == 0) {
    *out = first;
  } else if (second_value == 0) {
    *out = MakeDim(second);
  } else if (first_value == 1) {
    *out = MakeDim(second);
  } else if (second_value == 1) {
    *out = first;
  } else if (first_value == kUnknownDim || second_value == kUnknownDim) {
    *out = UnknownDim();
  } else {
    // Invariant: Both values are known and greater than 1.
    const int64_t product = first_value * second_value;
    if (product < 0) {
      return errors::InvalidArgument(
          "Negative dimension size caused by overflow when multiplying ",
          first_value, " and ", second_value);
    }
    *out = MakeDim(product);
  }
  return Status::OK();
}