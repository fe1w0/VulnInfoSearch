  void Compute(OpKernelContext* context) override {
    const Tensor& input = context->input(0);

    // MatrixDiagPart and MatrixDiagPartV2 both use this OpKernel.
    // MatrixDiagPart only has one input, so we have to check the number of
    // inputs before reading additional parameters in MatrixDiagV2.
    int32_t lower_diag_index = 0;
    int32_t upper_diag_index = 0;
    T padding_value(0);

    // MatrixDiagPartV2-specific.
    if (context->num_inputs() > kNumV1Inputs) {
      auto& diag_index = context->input(1);
      OP_REQUIRES(context,
                  TensorShapeUtils::IsScalar(diag_index.shape()) ||
                      TensorShapeUtils::IsVector(diag_index.shape()),
                  errors::InvalidArgument(
                      "diag_index must be a scalar or vector, received shape: ",
                      diag_index.shape().DebugString()));
      OP_REQUIRES(context, diag_index.NumElements() > 0,
                  errors::InvalidArgument(
                      "Expected diag_index to have at least 1 element"));
      lower_diag_index = diag_index.flat<int32>()(0);
      upper_diag_index = lower_diag_index;
      if (TensorShapeUtils::IsVector(diag_index.shape())) {
        auto diag_index_size = diag_index.dim_size(0);
        OP_REQUIRES(
            context, 0 < diag_index_size && diag_index_size <= 2,
            errors::InvalidArgument(
                "diag_index must have only one or two elements, received ",
                diag_index_size, " elements."));
        if (diag_index_size > 1) {
          upper_diag_index = diag_index.flat<int32>()(1);
        }
      }
      padding_value = context->input(2).flat<T>()(0);
    }
    const TensorShape& input_shape = input.shape();

    // Preliminary validation of sizes.
    OP_REQUIRES(context, TensorShapeUtils::IsMatrixOrHigher(input_shape),
                errors::InvalidArgument(
                    "input must be at least 2-dim, received shape: ",
                    input.shape().DebugString()));

    // Make sure lower_diag_index and upper_diag_index is valid.
    const int rank = input_shape.dims();
    const Eigen::Index num_rows = input_shape.dim_size(rank - 2);
    const Eigen::Index num_cols = input_shape.dim_size(rank - 1);
    OP_REQUIRES(  // Checks lower_diag_index == 0 for when matrix shape = 0.
        context,
        (-num_rows < lower_diag_index && lower_diag_index < num_cols) ||
            lower_diag_index == 0,
        errors::InvalidArgument(
            "lower_diag_index is out of bound: ", lower_diag_index,
            ". It must be between ", -num_rows, " and ", num_cols));
    OP_REQUIRES(context,
                (-num_rows < upper_diag_index && upper_diag_index < num_cols) ||
                    upper_diag_index == 0,
                errors::InvalidArgument(
                    "upper_diag_index is out of bound: ", upper_diag_index,
                    " It must be between ", -num_rows, " and ", num_cols));
    OP_REQUIRES(
        context, lower_diag_index <= upper_diag_index,
        errors::InvalidArgument(
            "lower_diag_index must not be larger than upper_diag_index: ",
            lower_diag_index, " > ", upper_diag_index));

    TensorShape output_shape;
    for (int i = 0; i < rank - 2; ++i) {
      output_shape.AddDim(input_shape.dim_size(i));
    }
    const Eigen::Index num_diags = upper_diag_index - lower_diag_index + 1;
    if (num_diags > 1) output_shape.AddDim(num_diags);
    const int32_t max_diag_len =
        std::min(num_rows + std::min(upper_diag_index, 0),
                 num_cols - std::max(lower_diag_index, 0));
    output_shape.AddDim(max_diag_len);

    Tensor* output = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(0, output_shape, &output));
    auto output_reshaped = output->flat<T>();
    auto input_reshaped = input.flat_inner_dims<T, 3>();
    functor::MatrixDiagPart<Device, T>::Compute(
        context, context->eigen_device<Device>(), input_reshaped,
        output_reshaped, lower_diag_index, upper_diag_index, max_diag_len,
        padding_value, left_align_superdiagonal_, left_align_subdiagonal_);
  }