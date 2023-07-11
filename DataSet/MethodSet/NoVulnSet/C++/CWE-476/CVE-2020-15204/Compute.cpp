  void Compute(OpKernelContext* ctx) override {
    const Tensor& val = ctx->input(0);
    auto session_state = ctx->session_state();
    OP_REQUIRES(ctx, session_state != nullptr,
                errors::FailedPrecondition(
                    "GetSessionHandle called on null session state"));
    int64 id = session_state->GetNewId();
    TensorStore::TensorAndKey tk{val, id, requested_device()};
    OP_REQUIRES_OK(ctx, ctx->tensor_store()->AddTensor(name(), tk));

    Tensor* handle = nullptr;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape({}), &handle));
    if (ctx->expected_output_dtype(0) == DT_RESOURCE) {
      ResourceHandle resource_handle = MakeResourceHandle<Tensor>(
          ctx, SessionState::kTensorHandleResourceTypeName,
          tk.GetHandle(name()));
      resource_handle.set_maybe_type_name(
          SessionState::kTensorHandleResourceTypeName);
      handle->scalar<ResourceHandle>()() = resource_handle;
    } else {
      // Legacy behavior in V1.
      handle->flat<tstring>().setConstant(tk.GetHandle(name()));
    }
  }