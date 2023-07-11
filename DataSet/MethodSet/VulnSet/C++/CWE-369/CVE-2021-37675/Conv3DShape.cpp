Status Conv3DShape(shape_inference::InferenceContext* c) {
  ShapeHandle input_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), 5, &input_shape));
  ShapeHandle filter_shape;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(1), 5, &filter_shape));

  string data_format;
  Status s = c->GetAttr("data_format", &data_format);

  std::vector<int32> dilations;
  TF_RETURN_IF_ERROR(c->GetAttr("dilations", &dilations));

  if (dilations.size() != 5) {
    return errors::InvalidArgument(
        "Conv3D requires the dilation attribute to contain 5 values, but got: ",
        dilations.size());
  }

  std::vector<int32> strides;
  TF_RETURN_IF_ERROR(c->GetAttr("strides", &strides));
  if (strides.size() != 5) {
    return errors::InvalidArgument(
        "Conv3D requires the stride attribute to contain 5 values, but got: ",
        strides.size());
  }

  int32_t stride_planes, stride_rows, stride_cols;
  int32_t dilation_planes, dilation_rows, dilation_cols;
  if (s.ok() && data_format == "NCDHW") {
    // Convert input_shape to NDHWC.
    auto dim = [&](char dimension) {
      return c->Dim(input_shape, GetTensorDimIndex<3>(FORMAT_NCHW, dimension));
    };
    input_shape =
        c->MakeShape({{dim('N'), dim('0'), dim('1'), dim('2'), dim('C')}});
    stride_planes = strides[2];
    stride_rows = strides[3];
    stride_cols = strides[4];
    dilation_planes = dilations[2];
    dilation_cols = dilations[3];
    dilation_rows = dilations[4];
  } else {
    stride_planes = strides[1];
    stride_rows = strides[2];
    stride_cols = strides[3];
    dilation_planes = dilations[1];
    dilation_cols = dilations[2];
    dilation_rows = dilations[3];
  }

  DimensionHandle batch_size_dim = c->Dim(input_shape, 0);
  DimensionHandle in_planes_dim = c->Dim(input_shape, 1);
  DimensionHandle in_rows_dim = c->Dim(input_shape, 2);
  DimensionHandle in_cols_dim = c->Dim(input_shape, 3);
  DimensionHandle input_depth_dim = c->Dim(input_shape, 4);

  DimensionHandle filter_planes_dim = c->Dim(filter_shape, 0);
  DimensionHandle filter_rows_dim = c->Dim(filter_shape, 1);
  DimensionHandle filter_cols_dim = c->Dim(filter_shape, 2);
  DimensionHandle filter_input_depth_dim = c->Dim(filter_shape, 3);
  DimensionHandle output_depth_dim = c->Dim(filter_shape, 4);

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
  DimensionHandle output_planes, output_rows, output_cols;

  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, in_planes_dim, filter_planes_dim, dilation_planes, stride_planes,
      padding, -1, -1, &output_planes));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, in_rows_dim, filter_rows_dim, dilation_rows, stride_rows, padding, -1,
      -1, &output_rows));
  TF_RETURN_IF_ERROR(GetWindowedOutputSizeFromDimsV2(
      c, in_cols_dim, filter_cols_dim, dilation_cols, stride_cols, padding, -1,
      -1, &output_cols));

  ShapeHandle output_shape;
  if (data_format == "NCDHW") {
    output_shape = c->MakeShape({batch_size_dim, output_depth_dim,
                                 output_planes, output_rows, output_cols});
  } else {
    output_shape = c->MakeShape({batch_size_dim, output_planes, output_rows,
                                 output_cols, output_depth_dim});
  }
  c->set_output(0, output_shape);
  return Status::OK();
}