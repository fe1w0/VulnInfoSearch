  void Compute(OpKernelContext* context) override {
    const Tensor& input = context->input(0);
    const Tensor& dims = context->input(1);

    if (TensorShapeUtils::IsScalar(input.shape())) {
      context->set_output(0, input);
    } else {
      const int input_dims = input.dims();
      OP_REQUIRES(context, TensorShapeUtils::IsVector(dims.shape()),
                  errors::InvalidArgument("'dims' must be 1-dimension, not ",
                                          dims.dims()));

      OP_REQUIRES(
          context, input_dims == dims.dim_size(0),
          errors::InvalidArgument(
              "'dims' must have the same number of values as 'input' has "
              "dimensions. 'input' has ",
              input_dims, "'dims' has ", dims.dim_size(0), " values"));
      OP_REQUIRES(context, input_dims <= 8,
                  errors::Unimplemented(
                      "reverse is not implemented for tensors of rank > 8."));

      Tensor* output = nullptr;
      OP_REQUIRES_OK(context,
                     context->allocate_output(0, input.shape(), &output));

#define HANDLE_REVERSE(NDIMS)                                               \
  case NDIMS:                                                               \
    HandleReverseCase<Device, T, NDIMS>(context, dims.vec<bool>(), output); \
    return;

      switch (input_dims) {
        HANDLE_REVERSE(0);
        HANDLE_REVERSE(1);
        HANDLE_REVERSE(2);
        HANDLE_REVERSE(3);
        HANDLE_REVERSE(4);
        HANDLE_REVERSE(5);
        HANDLE_REVERSE(6);
        HANDLE_REVERSE(7);
        HANDLE_REVERSE(8);
      }
#undef HANDLE_REVERSE
    }
  }