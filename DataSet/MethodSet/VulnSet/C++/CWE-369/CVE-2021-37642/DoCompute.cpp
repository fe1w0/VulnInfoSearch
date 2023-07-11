  void DoCompute(OpKernelContext* c) {
    core::RefCountPtr<Var> v;
    OP_REQUIRES_OK(c, LookupResource(c, HandleFromInput(c, 0), &v));
    Tensor* params = v->tensor();
    const Tensor& indices = c->input(1);
    const Tensor& updates = c->input(2);

    // Check that rank(updates.shape) = rank(indices.shape + params.shape[1:])
    OP_REQUIRES(c,
                updates.dims() == 0 ||
                    updates.dims() == indices.dims() + params->dims() - 1,
                errors::InvalidArgument(
                    "Must have updates.shape = indices.shape + "
                    "params.shape[1:] or updates.shape = [], got ",
                    "updates.shape ", updates.shape().DebugString(),
                    ", indices.shape ", indices.shape().DebugString(),
                    ", params.shape ", params->shape().DebugString()));

    // Check that we have enough index space
    const int64_t N_big = indices.NumElements();
    OP_REQUIRES(
        c, N_big <= std::numeric_limits<Index>::max(),
        errors::InvalidArgument("indices has too many elements for ",
                                DataTypeString(DataTypeToEnum<Index>::v()),
                                " indexing: ", N_big, " > ",
                                std::numeric_limits<Index>::max()));
    const Index N = static_cast<Index>(N_big);
    OP_REQUIRES(
        c, params->dim_size(0) <= std::numeric_limits<Index>::max(),
        errors::InvalidArgument("params.shape[0] too large for ",
                                DataTypeString(DataTypeToEnum<Index>::v()),
                                " indexing: ", params->dim_size(0), " > ",
                                std::numeric_limits<Index>::max()));

    if (N > 0) {
      auto indices_flat = indices.flat<Index>();
      auto params_flat = params->flat_outer_dims<T>();
      if (TensorShapeUtils::IsScalar(updates.shape())) {
        const auto update = updates.scalar<T>();

        functor::ScatterScalarFunctor<Device, T, Index, op> functor;
        const Index bad_i = functor(c, c->template eigen_device<Device>(),
                                    params_flat, update, indices_flat);
        OP_REQUIRES(c, bad_i < 0,
                    errors::InvalidArgument(
                        "indices", SliceDebugString(indices.shape(), bad_i),
                        " = ", indices_flat(bad_i), " is not in [0, ",
                        params->dim_size(0), ")"));
      } else {
        int64_t num_updates = updates.NumElements();
        OP_REQUIRES(
            c, TensorShapeUtils::StartsWith(updates.shape(), indices.shape()),
            errors::InvalidArgument(
                "The shape of indices (", indices.shape().DebugString(),
                ") must be a prefix of the shape of updates (",
                updates.shape().DebugString(), ")"));
        auto updates_flat = updates.shaped<T, 2>({N, num_updates / N});

        functor::ScatterFunctor<Device, T, Index, op> functor;
        const Index bad_i = functor(c, c->template eigen_device<Device>(),
                                    params_flat, updates_flat, indices_flat);
        OP_REQUIRES(c, bad_i < 0,
                    errors::InvalidArgument(
                        "indices", SliceDebugString(indices.shape(), bad_i),
                        " = ", indices_flat(bad_i), " is not in [0, ",
                        params->dim_size(0), ")"));
      }
    }
  }