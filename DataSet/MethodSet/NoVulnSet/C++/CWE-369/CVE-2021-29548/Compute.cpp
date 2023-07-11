  void Compute(OpKernelContext* context) override {
    const Tensor& input = context->input(0);
    const auto& input_min_tensor = context->input(1);
    OP_REQUIRES(context, input_min_tensor.NumElements() == 1,
                errors::InvalidArgument("input_min must have 1 element"));
    const float input_min = input_min_tensor.flat<float>()(0);
    const auto& input_max_tensor = context->input(2);
    OP_REQUIRES(context, input_max_tensor.NumElements() == 1,
                errors::InvalidArgument("input_max must have 1 element"));
    const float input_max = input_max_tensor.flat<float>()(0);
    const Tensor& mean = context->input(3);
    const auto& mean_min_tensor = context->input(4);
    OP_REQUIRES(context, mean_min_tensor.NumElements() == 1,
                errors::InvalidArgument("mean_min must have 1 element"));
    const float mean_min = mean_min_tensor.flat<float>()(0);
    const auto& mean_max_tensor = context->input(5);
    OP_REQUIRES(context, mean_max_tensor.NumElements() == 1,
                errors::InvalidArgument("mean_max must have 1 element"));
    const float mean_max = mean_max_tensor.flat<float>()(0);
    const Tensor& var = context->input(6);
    const auto& var_min_tensor = context->input(7);
    OP_REQUIRES(context, var_min_tensor.NumElements() == 1,
                errors::InvalidArgument("var_min must have 1 element"));
    const float var_min = var_min_tensor.flat<float>()(0);
    const auto& var_max_tensor = context->input(8);
    OP_REQUIRES(context, var_max_tensor.NumElements() == 1,
                errors::InvalidArgument("var_max must have 1 element"));
    const float var_max = var_max_tensor.flat<float>()(0);
    const Tensor& beta = context->input(9);
    const auto& beta_min_tensor = context->input(10);
    OP_REQUIRES(context, beta_min_tensor.NumElements() == 1,
                errors::InvalidArgument("beta_min must have 1 element"));
    const float beta_min = beta_min_tensor.flat<float>()(0);
    const auto& beta_max_tensor = context->input(11);
    OP_REQUIRES(context, beta_max_tensor.NumElements() == 1,
                errors::InvalidArgument("beta_max must have 1 element"));
    const float beta_max = beta_max_tensor.flat<float>()(0);
    const Tensor& gamma = context->input(12);
    const auto& gamma_min_tensor = context->input(13);
    OP_REQUIRES(context, gamma_min_tensor.NumElements() == 1,
                errors::InvalidArgument("gamma_min must have 1 element"));
    const float gamma_min = gamma_min_tensor.flat<float>()(0);
    const auto& gamma_max_tensor = context->input(14);
    OP_REQUIRES(context, gamma_max_tensor.NumElements() == 1,
                errors::InvalidArgument("gamma_max must have 1 element"));
    const float gamma_max = gamma_max_tensor.flat<float>()(0);

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
    OP_REQUIRES(context, mean.NumElements() > 1,
                errors::InvalidArgument("Must have at least a mean value",
                                        gamma.shape().DebugString()));
    OP_REQUIRES(context, mean.NumElements() > 1,
                errors::InvalidArgument("Must have at least a mean value"));
    const auto last_dim = input.shape().dims() - 1;
    OP_REQUIRES(context,
                mean.shape().dim_size(0) == input.shape().dim_size(last_dim),
                errors::InvalidArgument("Must provide as many means as the "
                                        "last dimension of the input tensor: ",
                                        mean.shape().DebugString(), " vs. ",
                                        input.shape().DebugString()));
    OP_REQUIRES(
        context, mean.shape().dim_size(0) == var.shape().dim_size(0),
        errors::InvalidArgument(
            "Mean and variance tensors must have the same shape: ",
            mean.shape().DebugString(), " vs. ", var.shape().DebugString()));
    OP_REQUIRES(
        context, mean.shape().dim_size(0) == beta.shape().dim_size(0),
        errors::InvalidArgument(
            "Mean and beta tensors must have the same shape: ",
            mean.shape().DebugString(), " vs. ", beta.shape().DebugString()));
    OP_REQUIRES(
        context, mean.shape().dim_size(0) == gamma.shape().dim_size(0),
        errors::InvalidArgument(
            "Mean and gamma tensors must have the same shape: ",
            mean.shape().DebugString(), " vs. ", gamma.shape().DebugString()));

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