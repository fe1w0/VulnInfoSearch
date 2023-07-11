  void Compute(OpKernelContext* context) override {
    auto min_input_tensor = context->input(1);
    auto max_input_tensor = context->input(2);
    OP_REQUIRES(
        context, min_input_tensor.NumElements() == 1,
        errors::InvalidArgument(
            "min_input must be a scalar float value, got tensor with shape ",
            min_input_tensor.shape()));
    OP_REQUIRES(
        context, max_input_tensor.NumElements() == 1,
        errors::InvalidArgument(
            "max_input must be a scalar float value, got tensor with shape ",
            max_input_tensor.shape()));
    const float min_input = context->input(1).flat<float>()(0);
    const float max_input = context->input(2).flat<float>()(0);
    MaxPoolingOp<Device, T>::Compute(context);
    Tensor* output_min = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(1, {}, &output_min));
    output_min->flat<float>()(0) = min_input;
    Tensor* output_max = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(2, {}, &output_max));
    output_max->flat<float>()(0) = max_input;
  }