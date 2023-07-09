Status Conv2DShapeImpl(shape_inference::InferenceContext* c,
                       bool supports_explicit_padding) {
  string data_format_str, filter_format_str;
  if (!c->GetAttr("data_format", &data_format_str).ok()) {
    data_format_str = "NHWC";
  }
  if (!c->GetAttr("filter_format", &filter_format_str).ok()) {
    filter_format_str = "HWIO";
  }

  TensorFormat data_format;
  if (!FormatFromString(data_format_str, &data_format)) {
    return errors::InvalidArgument("Invalid data format string: ",
                                   data_format_str);
  }
  FilterTensorFormat filter_format;
  if (!FilterFormatFromString(filter_format_str, &filter_format)) {
    return errors::InvalidArgument("Invalid filter format string: ",
                                   filter_format_str);
  }

  constexpr int num_spatial_dims = 2;
  const int rank = GetTensorDimsFromSpatialDims(num_spatial_dims, data_format);
  ShapeHandle conv_input_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), rank, &conv_input_shape));
  TF_RETURN_IF_ERROR(CheckFormatConstraintsOnShape(
      data_format, conv_input_shape, "conv_input", c));

  // The filter rank should match the input (4 for NCHW, 5 for NCHW_VECT_C).
  ShapeHandle filter_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(1), rank, &filter_shape));
  TF_RETURN_IF_ERROR(
      CheckFormatConstraintsOnShape(data_format, filter_shape, "filter", c));

  std::vector<int32> dilations;
  TF_RETURN_IF_ERROR(c->GetAttr("dilations", &dilations));

  if (dilations.size() != 4) {
    return errors::InvalidArgument(
        "Conv2D requires the dilation attribute to contain 4 values, but got: ",
        dilations.size());
  }

  std::vector<int32> strides;
  TF_RETURN_IF_ERROR(c->GetAttr("strides", &strides));

  // strides.size() should be 4 (NCHW) even if the input is 5 (NCHW_VECT_C).
  if (strides.size() != 4) {
    return errors::InvalidArgument("Conv2D on data format ", data_format_str,
                                   " requires the stride attribute to contain"
                                   " 4 values, but got: ",
                                   strides.size());
  }

  const int32_t stride_rows = GetTensorDim(strides, data_format, 'H');
  const int32_t stride_cols = GetTensorDim(strides, data_format, 'W');
  const int32_t dilation_rows = GetTensorDim(dilations, data_format, 'H');
  const int32_t dilation_cols = GetTensorDim(dilations, data_format, 'W');

  DimensionHandle batch_size_dim;
  DimensionHandle input_depth_dim;
  gtl::InlinedVector<DimensionHandle, 2> input_spatial_dims(2);
  TF_RETURN_IF_ERROR(DimensionsFromShape(
      conv_input_shape, data_format, &batch_size_dim,
      absl::MakeSpan(input_spatial_dims), &input_depth_dim, c));

  DimensionHandle output_depth_dim = c->Dim(
      filter_shape, GetFilterDimIndex<num_spatial_dims>(filter_format, 'O'));
  DimensionHandle filter_rows_dim = c->Dim(
      filter_shape, GetFilterDimIndex<num_spatial_dims>(filter_format, 'H'));
  DimensionHandle filter_cols_dim = c->Dim(
      filter_shape, GetFilterDimIndex<num_spatial_dims>(filter_format, 'W'));
  DimensionHandle filter_input_depth_dim;
  if (filter_format == FORMAT_OIHW_VECT_I) {
    TF_RETURN_IF_ERROR(c->Multiply(
        c->Dim(filter_shape,
               GetFilterDimIndex<num_spatial_dims>(filter_format, 'I')),
        c->Dim(filter_shape,
               GetFilterTensorInnerInputChannelsDimIndex(rank, filter_format)),
        &filter_input_depth_dim));
  } else {
    filter_input_depth_dim = c->Dim(
        filter_shape, GetFilterDimIndex<num_spatial_dims>(filter_format, 'I'));
  }

  // Check that the input tensor and the filter tensor agree on the channel
  // count.
  if (c->ValueKnown(input_depth_dim) && c->ValueKnown(filter_input_depth_dim)) {
    int64_t input_depth_value = c->Value(input_depth_dim),
            filter_input_depth_value = c->Value(filter_input_depth_dim);
    if (input_depth_value % filter_input_depth_value != 0)
      return errors::InvalidArgument(
          "Depth of input (", input_depth_value,
          ") is not a multiple of input depth of filter (",
          filter_input_depth_value, ")");
    if (input_depth_value != filter_input_depth_value) {
      int64_t num_groups = input_depth_value / filter_input_depth_value;
      if (c->ValueKnown(output_depth_dim)) {
        int64_t output_depth_value = c->Value(output_depth_dim);
        if (output_depth_value % num_groups != 0)
          return errors::InvalidArgument(
              "Depth of output (", output_depth_value,
              ") is not a multiple of the number of groups (", num_groups, ")");
      }
    }
  }

  Padding padding;
  TF_RETURN_IF_ERROR(c->GetAttr("padding", &padding));

  std::vector<int64> explicit_paddings;
  if (supports_explicit_padding) {
    Status s = c->GetAttr("explicit_paddings", &explicit_paddings);
    // Use the default value, which is an empty list, if the attribute is not
    // found. Otherwise return the error to the caller.
    if (!s.ok() && !errors::IsNotFound(s)) {
      return s;
    }
    TF_RETURN_IF_ERROR(CheckValidPadding(padding, explicit_paddings,
                                         /*num_dims=*/4, data_format));
  } else {
    CHECK(padding != Padding::EXPLICIT);  // Crash ok.
  }

  DimensionHandle output_rows, output_cols;
  int64_t pad_rows_before = -1, pad_rows_after = -1;
  int64_t pad_cols_before = -1, pad_cols_after = -1;
  if (padding == Padding::EXPLICIT) {
    GetExplicitPaddingForDim(explicit_paddings, data_format, 'H',
                             &pad_rows_before, &pad_rows_after);
    GetExplicitPaddingForDim(explicit_paddings, data_format, 'W',
                             &pad_cols_before, &pad_cols_after);
  }
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, input_spatial_dims[0], filter_rows_dim, dilation_rows, stride_rows,
      padding, pad_rows_before, pad_rows_after, &output_rows));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, input_spatial_dims[1], filter_cols_dim, dilation_cols, stride_cols,
      padding, pad_cols_before, pad_cols_after, &output_cols));

  absl::optional<DimensionHandle> vect_size;
  if (data_format == FORMAT_NCHW_VECT_C) {
    vect_size.emplace(c->Dim(conv_input_shape,
                             GetTensorInnerFeatureDimIndex(rank, data_format)));
  }
  ShapeHandle output_shape;
  TF_RETURN_IF_ERROR(ShapeFromDimensions(
      batch_size_dim, {output_rows, output_cols}, output_depth_dim, data_format,
      vect_size, c, &output_shape));
  c->set_output(0, output_shape);
  return Status::OK();
}