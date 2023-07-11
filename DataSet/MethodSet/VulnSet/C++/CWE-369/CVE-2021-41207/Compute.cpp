  void Compute(OpKernelContext* ctx) override {
    auto value = ctx->input(0);
    auto update = ctx->input(1);

    OP_REQUIRES(
        ctx, value.dims() == update.dims(),
        errors::InvalidArgument("value and update shape doesn't match: ",
                                value.shape().DebugString(), " vs. ",
                                update.shape().DebugString()));
    for (int i = 1; i < value.dims(); ++i) {
      OP_REQUIRES(
          ctx, value.dim_size(i) == update.dim_size(i),
          errors::InvalidArgument("value and update shape doesn't match ",
                                  value.shape().DebugString(), " vs. ",
                                  update.shape().DebugString()));
    }
    OP_REQUIRES(ctx, 1 == update.dim_size(0),
                errors::InvalidArgument("update shape doesn't match: ",
                                        update.shape().DebugString()));

    Tensor output = value;  // This creates an alias intentionally.
    const auto& d = ctx->eigen_device<Device>();
    OP_REQUIRES_OK(
        ctx, ::tensorflow::functor::DoParallelConcat(d, update, loc_, &output));
    ctx->set_output(0, output);
  }