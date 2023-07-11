  explicit Pooling3DOp(OpKernelConstruction* context) : UnaryOp<T>(context) {
    string data_format;
    OP_REQUIRES_OK(context, context->GetAttr("data_format", &data_format));
    OP_REQUIRES(context, FormatFromString(data_format, &data_format_),
                errors::InvalidArgument("Invalid data format"));
    if (context->device_type() == DEVICE_CPU) {
      OP_REQUIRES(
          context, data_format_ == FORMAT_NHWC,
          errors::InvalidArgument("Default Pooling3DOp only supports NDHWC ",
                                  "on device type ",
                                  DeviceTypeString(context->device_type())));
    }
    OP_REQUIRES_OK(context, context->GetAttr("ksize", &ksize_));
    OP_REQUIRES(context, ksize_.size() == 5,
                errors::InvalidArgument("Sliding window ksize field must "
                                        "specify 5 dimensions"));
    OP_REQUIRES_OK(context, context->GetAttr("strides", &stride_));
    OP_REQUIRES(context, stride_.size() == 5,
                errors::InvalidArgument("Sliding window stride field must "
                                        "specify 5 dimensions"));
    OP_REQUIRES_OK(context, context->GetAttr("padding", &padding_));
    OP_REQUIRES(context,
                (GetTensorDim(ksize_, data_format_, 'N') == 1 &&
                 GetTensorDim(stride_, data_format_, 'N') == 1),
                errors::Unimplemented(
                    "Pooling is not yet supported on the batch dimension."));
    OP_REQUIRES(context,
                (GetTensorDim(ksize_, data_format_, 'C') == 1 &&
                 GetTensorDim(stride_, data_format_, 'C') == 1),
                errors::Unimplemented(
                    "Pooling is not yet supported on the depth dimension."));
  }