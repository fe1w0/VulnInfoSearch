  void Compute(OpKernelContext* context) override {
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