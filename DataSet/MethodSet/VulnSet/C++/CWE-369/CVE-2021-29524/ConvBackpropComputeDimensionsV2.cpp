Status ConvBackpropComputeDimensionsV2(
    StringPiece label, int num_spatial_dims, const TensorShape& input_shape,
    const TensorShape& filter_shape, const TensorShape& out_backprop_shape,
    const gtl::ArraySlice<int32>& dilations, const std::vector<int32>& strides,
    Padding padding, absl::Span<const int64> explicit_paddings,
    TensorFormat data_format, ConvBackpropDimensions* dims) {
  // The + 2 in the following line is for the batch and feature dimensions.
  const int num_dims = num_spatial_dims + 2;
  if (input_shape.dims() != num_dims) {
    return errors::InvalidArgument(label, ": input must be ", num_dims,
                                   "-dimensional");
  }
  if (filter_shape.dims() != num_dims) {
    return errors::InvalidArgument(label, ": filter must be ", num_dims,
                                   "-dimensional");
  }
  if (out_backprop_shape.dims() != num_dims) {
    return errors::InvalidArgument(label, ": out_backprop must be ", num_dims,
                                   "-dimensional");
  }
  int batch_dim = GetTensorBatchDimIndex(num_dims, data_format);
  dims->batch_size = input_shape.dim_size(batch_dim);
  if (dims->batch_size != out_backprop_shape.dim_size(batch_dim)) {
    return errors::InvalidArgument(
        label, ": input and out_backprop must have the same batch size.",
        " Input batch: ", dims->batch_size,
        ", outbackprop batch: ", out_backprop_shape.dim_size(batch_dim),
        ", batch_dim: ", batch_dim);
  }

  int feature_dim = GetTensorFeatureDimIndex(num_dims, data_format);
  dims->in_depth = input_shape.dim_size(feature_dim);
  // The input and output feature dimensions are the second last and last
  // dimensions of the filter Tensor.
  VLOG(2) << "input vs filter_in depth " << dims->in_depth << " "
          << filter_shape.dim_size(num_dims - 2);
  if (dims->in_depth % filter_shape.dim_size(num_dims - 2)) {
    return errors::InvalidArgument(
        label, ": input depth must be evenly divisible by filter depth");
  }
  dims->out_depth = filter_shape.dim_size(num_dims - 1);
  if (dims->out_depth != out_backprop_shape.dim_size(feature_dim)) {
    return errors::InvalidArgument(
        label, ": filter and out_backprop must have the same out_depth");
  }
  dims->spatial_dims.resize(num_spatial_dims);
  for (int i = 0; i < num_spatial_dims; ++i) {
    int image_dim = GetTensorSpatialDimIndex(num_dims, data_format, i);
    int64 padding_before = -1, padding_after = -1;
    if (padding == EXPLICIT) {
      padding_before = explicit_paddings[2 * image_dim];
      padding_after = explicit_paddings[2 * image_dim + 1];
    }
    TF_RETURN_IF_ERROR(ConvBackpropExtractAndVerifyDimension(
        label, input_shape, filter_shape, out_backprop_shape, dilations,
        strides, padding, padding_before, padding_after, image_dim, i,
        &dims->spatial_dims[i]));
  }
  return Status::OK();
}