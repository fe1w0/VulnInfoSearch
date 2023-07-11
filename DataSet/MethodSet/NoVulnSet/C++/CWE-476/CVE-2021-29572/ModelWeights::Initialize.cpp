Status ModelWeights::Initialize(OpKernelContext* const context) {
  OpInputList sparse_indices_inputs;
  TF_RETURN_IF_ERROR(
      context->input_list("sparse_indices", &sparse_indices_inputs));
  OpInputList sparse_weights_inputs;
  TF_RETURN_IF_ERROR(
      context->input_list("sparse_weights", &sparse_weights_inputs));
  if (sparse_indices_inputs.size() != sparse_weights_inputs.size())
    return errors::InvalidArgument(
        "sparse_indices and sparse_weights must have the same length, got ",
        sparse_indices_inputs.size(), " and ", sparse_weights_inputs.size());
  OpInputList dense_weights_inputs;
  TF_RETURN_IF_ERROR(
      context->input_list("dense_weights", &dense_weights_inputs));

  OpOutputList sparse_weights_outputs;
  TF_RETURN_IF_ERROR(context->output_list("out_delta_sparse_weights",
                                          &sparse_weights_outputs));
  if (sparse_weights_outputs.size() != sparse_weights_inputs.size())
    return errors::InvalidArgument(
        "out_delta_sparse_weights and sparse_weights must have the same "
        "length, got ",
        sparse_weights_outputs.size(), " and ", sparse_weights_inputs.size());

  OpOutputList dense_weights_outputs;
  TF_RETURN_IF_ERROR(
      context->output_list("out_delta_dense_weights", &dense_weights_outputs));
  if (dense_weights_outputs.size() != dense_weights_inputs.size())
    return errors::InvalidArgument(
        "out_delta_dense_weights and dense_weights must have the same length, "
        "got ",
        dense_weights_outputs.size(), " and ", dense_weights_inputs.size());

  for (int i = 0; i < sparse_weights_inputs.size(); ++i) {
    Tensor* delta_t;
    TF_RETURN_IF_ERROR(sparse_weights_outputs.allocate(
        i, sparse_weights_inputs[i].shape(), &delta_t));
    // Convert the input vector to a row matrix in internal representation.
    auto deltas = delta_t->shaped<float, 2>({1, delta_t->NumElements()});
    deltas.setZero();
    sparse_weights_.emplace_back(FeatureWeightsSparseStorage{
        sparse_indices_inputs[i].flat<int64>(),
        sparse_weights_inputs[i].shaped<float, 2>(
            {1, sparse_weights_inputs[i].NumElements()}),
        deltas});
  }

  // Reads in the weights, and allocates and initializes the delta weights.
  const auto initialize_weights =
      [&](const OpInputList& weight_inputs, OpOutputList* const weight_outputs,
          std::vector<FeatureWeightsDenseStorage>* const feature_weights) {
        for (int i = 0; i < weight_inputs.size(); ++i) {
          Tensor* delta_t;
          TF_RETURN_IF_ERROR(
              weight_outputs->allocate(i, weight_inputs[i].shape(), &delta_t));
          // Convert the input vector to a row matrix in internal
          // representation.
          auto deltas = delta_t->shaped<float, 2>({1, delta_t->NumElements()});
          deltas.setZero();
          feature_weights->emplace_back(FeatureWeightsDenseStorage{
              weight_inputs[i].shaped<float, 2>(
                  {1, weight_inputs[i].NumElements()}),
              deltas});
        }
        return Status::OK();
      };

  return initialize_weights(dense_weights_inputs, &dense_weights_outputs,
                            &dense_weights_);
}