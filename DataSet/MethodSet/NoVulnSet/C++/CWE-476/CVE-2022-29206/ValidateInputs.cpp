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
  int64_t nnz = a_indices->dim_size(0);
  int64_t ndims = a_indices->dim_size(1);
  if (a_values->dim_size(0) != nnz) {
    return errors::InvalidArgument("Dimensions ", nnz, " and ",
                                   a_values->dim_size(0),
                                   " are not compatible");
  }
  if (a_shape->dim_size(0) != ndims) {
    return errors::InvalidArgument("Dimensions ", ndims, " and ",
                                   a_shape->dim_size(0), " are not compatible");
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

  // Check for invalid indices.
  const auto a_indices_mat = a_indices->flat_inner_dims<Index>();

  for (int64_t zidx = 0; zidx < nnz; ++zidx) {
    for (int64_t didx = 0; didx < ndims; ++didx) {
      const Index idx = a_indices_mat(zidx, didx);
      if (idx < 0 || idx >= a_shape_flat(didx)) {
        return errors::InvalidArgument(
            "Sparse tensor has an invalid index on dimension ", didx,
            ": "
            "a_indices(",
            zidx, ",", didx, ") = ", idx,
            ", dense tensor shape: ", a_shape_flat);
      }
    }
  }

  return Status::OK();
}