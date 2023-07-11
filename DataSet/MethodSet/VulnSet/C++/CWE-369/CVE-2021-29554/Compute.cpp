  void Compute(OpKernelContext* context) override {
    const Tensor& data = context->input(0);
    const Tensor& weights = context->input(1);
    bool use_weights = weights.NumElements() > 0;

    OP_REQUIRES(context,
                TensorShapeUtils::IsVector(data.shape()) ||
                    TensorShapeUtils::IsMatrix(data.shape()),
                errors::InvalidArgument(
                    "Input must be a 1 or 2-dimensional tensor. Got: ",
                    data.shape().DebugString()));

    if (use_weights) {
      OP_REQUIRES(
          context, weights.shape() == data.shape(),
          errors::InvalidArgument(
              "Weights and data must have the same shape. Weight shape: ",
              weights.shape().DebugString(),
              "; data shape: ", data.shape().DebugString()));
    }

    bool is_1d = TensorShapeUtils::IsVector(data.shape());
    int negative_valued_axis = -1;
    int num_batch_dimensions = (data.shape().dims() + negative_valued_axis);

    int num_batch_elements = 1;
    for (int i = 0; i < num_batch_dimensions; ++i) {
      num_batch_elements *= data.shape().dim_size(i);
    }
    int num_value_elements = data.shape().num_elements() / num_batch_elements;
    auto per_batch_counts = BatchedMap<W>(num_batch_elements);

    T max_value = 0;

    const auto data_values = data.flat<T>();
    const auto weight_values = weights.flat<W>();
    int i = 0;
    for (int b = 0; b < num_batch_elements; ++b) {
      for (int v = 0; v < num_value_elements; ++v) {
        const auto& value = data_values(i);
        if (value >= 0 && (maxlength_ <= 0 || value < maxlength_)) {
          if (binary_output_) {
            per_batch_counts[b][value] = 1;
          } else if (use_weights) {
            per_batch_counts[b][value] += weight_values(i);
          } else {
            per_batch_counts[b][value]++;
          }
          if (value > max_value) {
            max_value = value;
          }
        }
        ++i;
      }
    }

    int num_output_values = GetOutputSize(max_value, maxlength_, minlength_);
    OP_REQUIRES_OK(context, OutputSparse<W>(per_batch_counts, num_output_values,
                                            is_1d, context));
  }