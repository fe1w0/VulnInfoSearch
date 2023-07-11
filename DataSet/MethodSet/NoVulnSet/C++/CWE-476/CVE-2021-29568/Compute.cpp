  void Compute(OpKernelContext* ctx) override {
    const Tensor& shape_tensor = ctx->input(0);
    const Tensor& means_tensor = ctx->input(1);
    const Tensor& stddevs_tensor = ctx->input(2);
    const Tensor& minvals_tensor = ctx->input(3);
    const Tensor& maxvals_tensor = ctx->input(4);

    OP_REQUIRES(
        ctx, TensorShapeUtils::IsVector(shape_tensor.shape()),
        errors::InvalidArgument("Input shape should be a vector, got shape: ",
                                shape_tensor.shape().DebugString()));
    OP_REQUIRES(ctx, shape_tensor.NumElements() > 0,
                errors::InvalidArgument("Shape tensor must not be empty, got ",
                                        shape_tensor.DebugString()));
    int32 num_batches = shape_tensor.flat<int32>()(0);

    int32 samples_per_batch = 1;
    const int32 num_dims = shape_tensor.dim_size(0);
    for (int32 i = 1; i < num_dims; i++) {
      samples_per_batch *= shape_tensor.flat<int32>()(i);
    }
    const int32 num_elements = num_batches * samples_per_batch;

    // Allocate the output before fudging num_batches and samples_per_batch.
    auto shape_vec = shape_tensor.flat<int32>();
    TensorShape tensor_shape;
    OP_REQUIRES_OK(ctx, TensorShapeUtils::MakeShape(
                            shape_vec.data(), shape_vec.size(), &tensor_shape));
    Tensor* samples_tensor;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, tensor_shape, &samples_tensor));

    // Parameters must be 0-d or 1-d.
    OP_REQUIRES(ctx, means_tensor.dims() <= 1,
                errors::InvalidArgument(
                    "Input means should be a scalar or vector, got shape: ",
                    means_tensor.shape().DebugString()));
    OP_REQUIRES(ctx, stddevs_tensor.dims() <= 1,
                errors::InvalidArgument(
                    "Input stddevs should be a scalar or vector, got shape: ",
                    stddevs_tensor.shape().DebugString()));
    OP_REQUIRES(ctx, minvals_tensor.dims() <= 1,
                errors::InvalidArgument(
                    "Input minvals should be a scalar or vector, got shape: ",
                    minvals_tensor.shape().DebugString()));
    OP_REQUIRES(ctx, maxvals_tensor.dims() <= 1,
                errors::InvalidArgument(
                    "Input maxvals should be a scalar or vector, got shape: ",
                    maxvals_tensor.shape().DebugString()));

    if ((means_tensor.dims() == 0 || means_tensor.dim_size(0) == 1) &&
        (stddevs_tensor.dims() == 0 || stddevs_tensor.dim_size(0) == 1) &&
        minvals_tensor.dims() == 0 && maxvals_tensor.dims() == 0) {
      // All batches have the same parameters, so we can update the batch size
      // to a reasonable value to improve parallelism (ensure enough batches,
      // and no very small batches which have high overhead).
      int32 size = num_batches * samples_per_batch;
      int32 adjusted_samples = kDesiredBatchSize;
      // Ensure adjusted_batches * adjusted_samples >= size.
      int32 adjusted_batches = Eigen::divup(size, adjusted_samples);
      num_batches = adjusted_batches;
      samples_per_batch = adjusted_samples;
    } else {
      // Parameters must be broadcastable to the shape [num_batches].
      OP_REQUIRES(
          ctx,
          TensorShapeUtils::IsScalar(means_tensor.shape()) ||
              means_tensor.dim_size(0) == 1 ||
              means_tensor.dim_size(0) == num_batches,
          errors::InvalidArgument(
              "Input means should have length 1 or shape[0], got shape: ",
              means_tensor.shape().DebugString()));
      OP_REQUIRES(
          ctx,
          TensorShapeUtils::IsScalar(stddevs_tensor.shape()) ||
              stddevs_tensor.dim_size(0) == 1 ||
              stddevs_tensor.dim_size(0) == num_batches,
          errors::InvalidArgument(
              "Input stddevs should have length 1 or shape[0], got shape: ",
              stddevs_tensor.shape().DebugString()));
      OP_REQUIRES(
          ctx,
          TensorShapeUtils::IsScalar(minvals_tensor.shape()) ||
              minvals_tensor.dim_size(0) == 1 ||
              minvals_tensor.dim_size(0) == num_batches,
          errors::InvalidArgument(
              "Input minvals should have length 1 or shape[0], got shape: ",
              minvals_tensor.shape().DebugString()));
      OP_REQUIRES(
          ctx,
          TensorShapeUtils::IsScalar(maxvals_tensor.shape()) ||
              maxvals_tensor.dim_size(0) == 1 ||
              maxvals_tensor.dim_size(0) == num_batches,
          errors::InvalidArgument(
              "Input maxvals should have length 1 or shape[0], got shape: ",
              maxvals_tensor.shape().DebugString()));
    }

    auto truncFunctor = functor::TruncatedNormalFunctor<Device, T>();
    // Each worker has the fudge factor for samples_per_batch, so use it here.
    random::PhiloxRandom rng =
        generator_.ReserveSamples128(num_batches * 2 * functor::kMaxIterations *
                                     (samples_per_batch + 3) / 4);
    truncFunctor(ctx, ctx->eigen_device<Device>(), num_batches,
                 samples_per_batch, num_elements, means_tensor.flat<T>(),
                 stddevs_tensor.flat<T>(), minvals_tensor.flat<T>(),
                 maxvals_tensor.flat<T>(), rng, samples_tensor->flat<T>());
  }