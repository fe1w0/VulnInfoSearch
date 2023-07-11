  void Compute(OpKernelContext* context) override {
    const Tensor& input = context->input(0);
    const float input_min = context->input(1).flat<float>()(0);
    const float input_max = context->input(2).flat<float>()(0);
    const Tensor& mean = context->input(3);
    const float mean_min = context->input(4).flat<float>()(0);
    const float mean_max = context->input(5).flat<float>()(0);
    const Tensor& var = context->input(6);
    const float var_min = context->input(7).flat<float>()(0);
    const float var_max = context->input(8).flat<float>()(0);
    const Tensor& beta = context->input(9);
    const float beta_min = context->input(10).flat<float>()(0);
    const float beta_max = context->input(11).flat<float>()(0);
    const Tensor& gamma = context->input(12);
    const float gamma_min = context->input(13).flat<float>()(0);
    const float gamma_max = context->input(14).flat<float>()(0);

    OP_REQUIRES(context, input.dims() == 4,
                errors::InvalidArgument("input must be 4-dimensional",
                                        input.shape().DebugString()));
    OP_REQUIRES(context, mean.dims() == 1,
                errors::InvalidArgument("mean must be 1-dimensional",
                                        mean.shape().DebugString()));
    OP_REQUIRES(context, var.dims() == 1,
                errors::InvalidArgument("var must be 1-dimensional",
                                        var.shape().DebugString()));
    OP_REQUIRES(context, beta.dims() == 1,
                errors::InvalidArgument("beta must be 1-dimensional",
                                        beta.shape().DebugString()));
    OP_REQUIRES(context, gamma.dims() == 1,
                errors::InvalidArgument("gamma must be 1-dimensional",
                                        gamma.shape().DebugString()));

    Tensor* output = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, input.shape(), &output));
    float output_min;
    float output_max;
    FixedPointBatchNorm<T1, T2>(input, input_min, input_max, mean, mean_min,
                                mean_max, var, var_min, var_max, beta, beta_min,
                                beta_max, gamma, gamma_min, gamma_max,
                                variance_epsilon_, scale_after_normalization_,
                                output, &output_min, &output_max);

    Tensor* output_min_tensor = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(1, {}, &output_min_tensor));
    output_min_tensor->flat<float>()(0) = output_min;

    Tensor* output_max_tensor = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(2, {}, &output_max_tensor));
    output_max_tensor->flat<float>()(0) = output_max;
  }