  void Compute(OpKernelContext* context) override {
    const Tensor& tensor_in = context->input(0);
    const Tensor& tensor_out = context->input(1);
    const Tensor& out_grad_backprop = context->input(2);

    // For maxpooling3d, tensor_in should have 5 dimensions.
    OP_REQUIRES(context, tensor_in.dims() == 5,
                errors::InvalidArgument("tensor_in must be 5-dimensional"));
    OP_REQUIRES(context, tensor_out.dims() == 5,
                errors::InvalidArgument("tensor_out must be 5-dimensional"));
    // For maxpooling3d, out_grad_backprop should have 5 dimensions.
    OP_REQUIRES(
        context, out_grad_backprop.dims() == 5,
        errors::InvalidArgument("out_grad_backprop must be 5-dimensional"));

    Pool3dParameters params{context,  ksize_,       stride_,
                            padding_, data_format_, tensor_in.shape()};

    Tensor* output = nullptr;
    OP_REQUIRES_OK(context, context->forward_input_or_allocate_output(
                                {2}, 0, tensor_out.shape(), &output));

    // Given access patterns in LaunchMaxPooling3dGradGradOp, these tensors must
    // have elements.
    OP_REQUIRES(context, tensor_in.NumElements() > 0,
                errors::InvalidArgument("received empty tensor tensor_in: ",
                                        tensor_in.DebugString()));
    OP_REQUIRES(context, tensor_out.NumElements() > 0,
                errors::InvalidArgument("received empty tensor tensor_out: ",
                                        tensor_out.DebugString()));
    OP_REQUIRES(
        context, out_grad_backprop.NumElements() > 0,
        errors::InvalidArgument("received empty tensor out_grad_backprop: ",
                                out_grad_backprop.DebugString()));

    LaunchMaxPooling3dGradGradOp<Device, T>::launch(
        context, params, tensor_in, tensor_out, out_grad_backprop, output);
  }