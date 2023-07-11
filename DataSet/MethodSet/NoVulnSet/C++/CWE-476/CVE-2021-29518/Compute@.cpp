  void Compute(OpKernelContext* ctx) override {
    const Tensor& handle = ctx->input(0);
    const string& name = handle.scalar<tstring>()();
    auto session_state = ctx->session_state();
    OP_REQUIRES(ctx, session_state != nullptr,
                errors::FailedPrecondition(
                    "DeleteSessionTensor called on null session state"));
    OP_REQUIRES_OK(ctx, session_state->DeleteTensor(name));
  }