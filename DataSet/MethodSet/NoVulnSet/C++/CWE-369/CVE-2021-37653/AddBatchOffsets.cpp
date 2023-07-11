  void AddBatchOffsets(OpKernelContext* ctx, Tensor* indices,
                       const Tensor& params) {
    int64_t batch_size = 1;  // The size of all batch dimensions.
    for (int idx = 0; idx < batch_dims_; ++idx) {
      batch_size *= params.dim_size(idx);
    }
    OP_REQUIRES(
        ctx, batch_size != 0,
        errors::InvalidArgument(
            "Inner size of indices would result in batch_size of 0 and a ",
            "division by 0 in the implementation. This is illegal"));

    auto indices_flat = indices->flat<Index>();
    int64_t const index_inner_size = indices->NumElements() / batch_size;
    int64_t const batch_offset = params.dim_size(batch_dims_);
    for (int64_t batch_idx = 0, dest_idx = 0; batch_idx < batch_size;
         ++batch_idx) {
      for (int64_t idx = 0; idx < index_inner_size; ++idx) {
        indices_flat(dest_idx++) += batch_offset * batch_idx;
      }
    }
  }