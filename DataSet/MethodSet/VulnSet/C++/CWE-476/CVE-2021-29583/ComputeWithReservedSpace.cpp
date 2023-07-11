  virtual void ComputeWithReservedSpace(OpKernelContext* context,
                                        bool use_reserved_space) {
    Tensor x = context->input(0);
    const Tensor& scale = context->input(1);
    const Tensor& offset = context->input(2);
    const Tensor& estimated_mean = context->input(3);
    const Tensor& estimated_variance = context->input(4);
    const Tensor* side_input = has_side_input_ ? &context->input(5) : nullptr;

    OP_REQUIRES(context, x.dims() == 4 || x.dims() == 5,
                errors::InvalidArgument("input must be 4 or 5-dimensional",
                                        x.shape().DebugString()));
    OP_REQUIRES(context, scale.dims() == 1,
                errors::InvalidArgument("scale must be 1-dimensional",
                                        scale.shape().DebugString()));
    OP_REQUIRES(context, offset.dims() == 1,
                errors::InvalidArgument("offset must be 1-dimensional",
                                        offset.shape().DebugString()));
    OP_REQUIRES(context, estimated_mean.dims() == 1,
                errors::InvalidArgument("estimated_mean must be 1-dimensional",
                                        estimated_mean.shape().DebugString()));
    OP_REQUIRES(
        context, estimated_variance.dims() == 1,
        errors::InvalidArgument("estimated_variance must be 1-dimensional",
                                estimated_variance.shape().DebugString()));
    bool use_reshape = (x.dims() == 5);
    auto x_shape = x.shape();
    TensorShape dest_shape;
    if (use_reshape) {
      const int64 in_batch = GetTensorDim(x, tensor_format_, 'N');
      int64 in_planes = GetTensorDim(x, tensor_format_, '0');
      int64 in_rows = GetTensorDim(x, tensor_format_, '1');
      int64 in_cols = GetTensorDim(x, tensor_format_, '2');
      const int64 in_depth = GetTensorDim(x, tensor_format_, 'C');
      dest_shape = ShapeFromFormat(tensor_format_, in_batch,
                                   {{in_planes, in_rows * in_cols}}, in_depth);
      OP_REQUIRES(context, x.CopyFrom(x, dest_shape),
                  errors::InvalidArgument("Error during tensor copy."));
    }

    if (has_side_input_) {
      OP_REQUIRES(context, side_input->shape() == x.shape(),
                  errors::InvalidArgument(
                      "side_input shape must be equal to input shape: ",
                      side_input->shape().DebugString(),
                      " != ", x.shape().DebugString()));
    }

    if (activation_mode_ != FbnActivationMode::kIdentity) {
      // NOTE(ezhulenev): This requirement is coming from implementation
      // details of cudnnBatchNormalizationForwardTrainingEx.
      OP_REQUIRES(
          context, !is_training_ || x.dim_size(3) % 4 == 0,
          errors::InvalidArgument("FusedBatchNorm with activation requires "
                                  "channel dimension to be a multiple of 4."));
    }

    Tensor* y = nullptr;
    auto alloc_shape = use_reshape ? dest_shape : x_shape;
    OP_REQUIRES_OK(context, context->forward_input_or_allocate_output(
                                {0}, 0, alloc_shape, &y));

    Tensor* batch_mean = nullptr;
    OP_REQUIRES_OK(context, context->forward_input_or_allocate_output(
                                {3}, 1, scale.shape(), &batch_mean));
    Tensor* batch_var = nullptr;
    OP_REQUIRES_OK(context, context->forward_input_or_allocate_output(
                                {4}, 2, scale.shape(), &batch_var));
    Tensor* saved_mean = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(3, scale.shape(), &saved_mean));
    Tensor* saved_maybe_inv_var = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(4, scale.shape(),
                                                     &saved_maybe_inv_var));

    if (is_training_) {
      functor::FusedBatchNorm<Device, T, U, true>()(
          context, x, scale, offset, estimated_mean, estimated_variance,
          side_input, epsilon_, exponential_avg_factor_, activation_mode_, y,
          batch_mean, batch_var, saved_mean, saved_maybe_inv_var,
          tensor_format_, use_reserved_space);
    } else {
      functor::FusedBatchNorm<Device, T, U, false>()(
          context, x, scale, offset, estimated_mean, estimated_variance,
          side_input, epsilon_, exponential_avg_factor_, activation_mode_, y,
          batch_mean, batch_var, saved_mean, saved_maybe_inv_var,
          tensor_format_, use_reserved_space);
    }
    if (use_reshape) {
      OP_REQUIRES(context, y->CopyFrom(*y, x_shape),
                  errors::InvalidArgument("Error during tensor copy."));
    }
  }