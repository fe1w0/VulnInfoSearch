Status InferenceContext::InternalMakeShapeFromTensor(
    bool treat_unknown_scalar_tensor_as_unknown_shape, const Tensor* t,
    ShapeHandle tensor_shape, ShapeHandle* out) {
  // Only callers who have set
  if (!treat_unknown_scalar_tensor_as_unknown_shape) {
    TF_RETURN_IF_ERROR(WithRank(tensor_shape, 1, &tensor_shape));
  }
  if (t == nullptr) {
    // This is guarded by the check above.
    if (Rank(tensor_shape) == 0) {
      return ReturnUnknownShape(out);
    }
    // Shape tensor is not known, but if the shape of the shape tensor is then
    // the right number of unknown dims can be created.
    DimensionHandle shape_dim = Dim(tensor_shape, 0);
    if (!ValueKnown(shape_dim)) {
      return ReturnUnknownShape(out);
    }
    const auto num_dims = Value(shape_dim);
    std::vector<DimensionHandle> dims;
    dims.reserve(num_dims);
    for (int i = 0; i < num_dims; i++) dims.push_back(UnknownDim());
    return ReturnCreatedShape(dims, out);
  }

  if (t->shape().dims() == 0) {
    if (t->dtype() == DataType::DT_INT32) {
      auto flat_t = t->scalar<int32>();
      if (flat_t() != -1) {
        *out = nullptr;
        return errors::InvalidArgument(
            "Input tensor must be rank 1, or if its rank 0 it must have value "
            "-1 "
            "(representing an unknown shape).  Saw value: ",
            flat_t());
      }
      return ReturnUnknownShape(out);
    } else if (t->dtype() == DataType::DT_INT64) {
      auto flat_t = t->scalar<int64_t>();
      if (flat_t() != -1) {
        *out = nullptr;
        return errors::InvalidArgument(
            "Input tensor must be rank 1, or if its rank 0 it must have value "
            "-1 "
            "(representing an unknown shape).  Saw value: ",
            flat_t());
      }
      return ReturnUnknownShape(out);
    } else {
      *out = nullptr;
      return errors::InvalidArgument(
          "Input tensor must be int32 or int64, but was ",
          DataTypeString(t->dtype()));
    }
  }

  if (t->shape().dims() != 1) {
    *out = nullptr;
    return errors::InvalidArgument(
        "Input tensor must be rank 1, but was rank ", t->shape().dims(), ".",
        ((t->shape().dims() == 0)
             ? "If it is rank 0 rank 0 it must have statically known value -1 "
               "(representing an unknown shape). "
             : " "),
        "Saw tensor shape ", t->shape().DebugString());
  }
  std::vector<DimensionHandle> dims;
  if (t->dtype() == DataType::DT_INT32) {
    auto flat_t = t->flat<int32>();
    for (int i = 0; i < flat_t.size(); ++i) {
      const int32_t val = flat_t(i);
      if (val < -1) {
        return errors::InvalidArgument(
            "Invalid value in tensor used for shape: ", val);
      }
      // -1 will become an unknown dim.
      dims.push_back(MakeDim(val));
    }
  } else if (t->dtype() == DataType::DT_INT64) {
    auto flat_t = t->flat<int64_t>();
    for (int i = 0; i < flat_t.size(); ++i) {
      const int64_t val = flat_t(i);
      if (val < -1) {
        return errors::InvalidArgument(
            "Invalid value in tensor used for shape: ", val);
      }
      // -1 will become an unknown dim.
      dims.push_back(MakeDim(val));
    }
  } else {
    *out = nullptr;
    return errors::InvalidArgument(
        "Input tensor must be int32 or int64, but was ",
        DataTypeString(t->dtype()));
  }

  return ReturnCreatedShape(dims, out);
}