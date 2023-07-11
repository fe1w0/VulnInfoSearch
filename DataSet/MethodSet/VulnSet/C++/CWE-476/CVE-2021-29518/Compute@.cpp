  void Compute(OpKernelContext* ctx) override {
    const Tensor& handle = ctx->input(0);
    const string& name = handle.scalar<tstring>()();
    OP_REQUIRES_OK(ctx, ctx->session_state()->DeleteTensor(name));
  }