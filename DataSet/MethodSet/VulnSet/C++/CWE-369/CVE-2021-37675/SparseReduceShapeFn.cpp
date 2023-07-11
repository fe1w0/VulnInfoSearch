Status SparseReduceShapeFn(InferenceContext* c) {
  // Input 0: input_indices
  // Input 1: input_values
  // Input 2: input_shape
  // Input 3: reduction_axes
  // Attr: keep_dims
  bool keep_dims = false;
  TF_RETURN_IF_ERROR(c->GetAttr("keep_dims", &keep_dims));

  const Tensor* shape_tensor = c->input_tensor(2);
  const Tensor* axes_tensor = c->input_tensor(3);
  if (shape_tensor != nullptr && axes_tensor != nullptr) {
    auto shape_vec = shape_tensor->flat<int64>();
    auto axes_vec = axes_tensor->flat<int32>();

    int64_t ndims = shape_vec.size();
    absl::flat_hash_set<int64> axes;
    for (int i = 0; i < axes_vec.size(); i++) {
      axes.insert((axes_vec(i) + ndims) % ndims);
    }

    std::vector<DimensionHandle> dims;
    if (keep_dims) {
      dims.reserve(ndims);
      for (int d = 0; d < ndims; ++d) {
        if (axes.find(d) == axes.end()) {
          dims.push_back(c->MakeDim(shape_vec(d)));
        } else {
          dims.push_back(c->MakeDim(1));
        }
      }
    } else {
      for (int d = 0; d < ndims; ++d) {
        if (axes.find(d) == axes.end()) {
          dims.push_back(c->MakeDim(shape_vec(d)));
        }
      }
    }

    c->set_output(0, c->MakeShape(dims));
    return Status::OK();
  }
  return UnknownShape(c);
}