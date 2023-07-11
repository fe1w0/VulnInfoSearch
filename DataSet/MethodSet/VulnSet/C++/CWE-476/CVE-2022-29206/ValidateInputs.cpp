Status ValidateInputs(const Tensor *a_indices, const Tensor *a_values,
                      const Tensor *a_shape, const Tensor *b) {
  if (!TensorShapeUtils::IsMatrix(a_indices->shape())) {
    return errors::InvalidArgument(
        "Input a_indices should be a matrix but received shape: ",
        a_indices->shape().DebugString());
  }
  if (!TensorShapeUtils::IsVector(a_values->shape()) ||
      !TensorShapeUtils::IsVector(a_shape->shape())) {
    return errors::InvalidArgument(
        "Inputs a_values and a_shape should be vectors "
        "but received shapes: ",
        a_values->shape().DebugString(), " and ",
        a_shape->shape().DebugString());
  }
  if (a_shape->NumElements() != b->dims()) {
    return errors::InvalidArgument(
        "Two operands have different ranks; received: ", a_shape->NumElements(),
        " and ", b->dims());
  }
  const auto a_shape_flat = a_shape->flat<Index>();
  for (int i = 0; i < b->dims(); ++i) {
    if (a_shape_flat(i) != b->dim_size(i)) {
      return errors::InvalidArgument(
          "Dimension ", i,
          " does not equal (no broadcasting is supported): sparse side ",
          a_shape_flat(i), " vs dense side ", b->dim_size(i));
    }
  }
  return Status::OK();
}