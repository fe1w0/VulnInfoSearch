TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  const auto* params = reinterpret_cast<TfLiteSVDFParams*>(node->builtin_data);
  OpData* op_data = reinterpret_cast<OpData*>(node->user_data);
  int scratch_tensor_index = op_data->scratch_tensor_index;

  // Check we have all the inputs and outputs we need.
  TF_LITE_ENSURE_EQ(context, node->outputs->size, 1);
  TF_LITE_ENSURE_EQ(context, node->inputs->size, 5);

  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kInputTensor, &input));
  const TfLiteTensor* weights_feature;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kWeightsFeatureTensor,
                                          &weights_feature));
  const TfLiteTensor* weights_time;
  TF_LITE_ENSURE_OK(
      context, GetInputSafe(context, node, kWeightsTimeTensor, &weights_time));

  TF_LITE_ENSURE(context,
                 input->type == kTfLiteFloat32 || input->type == kTfLiteInt8);

  // Check all the parameters of tensor match within themselves and match the
  // input configuration.
  const int rank = params->rank;
  const int batch_size = input->dims->data[0];
  const int num_filters = weights_feature->dims->data[0];
  TF_LITE_ENSURE(context, rank != 0);
  TF_LITE_ENSURE_EQ(context, num_filters % rank, 0);
  const int num_units = num_filters / rank;
  const int memory_size = weights_time->dims->data[1];
  TF_LITE_ENSURE_EQ(context, input->dims->data[1],
                    weights_feature->dims->data[1]);
  TF_LITE_ENSURE_EQ(context, weights_time->dims->data[0], num_filters);

  const TfLiteTensor* bias = GetOptionalInputTensor(context, node, kBiasTensor);
  if (bias) {
    TF_LITE_ENSURE_EQ(context, bias->dims->data[0], num_units);
  }

  const TfLiteTensor* state;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kStateTensor, &state));
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));

  // Check the shape of input state tensors.
  TF_LITE_ENSURE_EQ(context, NumDimensions(state), 2);
  TF_LITE_ENSURE_EQ(context, SizeOfDimension(state, 0), batch_size);
  TF_LITE_ENSURE_EQ(context, SizeOfDimension(state, 1),
                    memory_size * num_filters);

  // Resize output.
  TfLiteIntArray* output_size_array = TfLiteIntArrayCreate(2);
  output_size_array->data[0] = batch_size;
  output_size_array->data[1] = num_units;
  TF_LITE_ENSURE_OK(context,
                    context->ResizeTensor(context, output, output_size_array));

  // The weights are of consistent type, so it suffices to check one.
  const bool is_hybrid_op = IsHybridOp(input, weights_feature);
  const bool is_full_integer = input->type == kTfLiteInt8;

  // Resize scratch.
  TfLiteIntArrayFree(node->temporaries);
  if (is_hybrid_op) {
    node->temporaries = TfLiteIntArrayCreate(6);
  } else if (is_full_integer) {
    node->temporaries = TfLiteIntArrayCreate(2);
  } else {
    node->temporaries = TfLiteIntArrayCreate(1);
  }
  node->temporaries->data[0] = scratch_tensor_index;

  TfLiteIntArray* scratch_size_array = TfLiteIntArrayCreate(2);
  scratch_size_array->data[0] = batch_size;
  scratch_size_array->data[1] = num_filters;

  TfLiteTensor* scratch_tensor;
  TF_LITE_ENSURE_OK(
      context, GetTemporarySafe(context, node, /*index=*/0, &scratch_tensor));

  // The scratch buffer is of type int32 for full integer svdf and it's of type
  // float32 for hybrid and float case.
  if (is_full_integer) {
    scratch_tensor->type = kTfLiteInt32;
  } else {
    scratch_tensor->type = kTfLiteFloat32;
  }
  scratch_tensor->allocation_type = kTfLiteArenaRw;
  TF_LITE_ENSURE_OK(context, context->ResizeTensor(context, scratch_tensor,
                                                   scratch_size_array));

  if (is_hybrid_op) {
    op_data->compute_row_sums = true;
    // Tell interpreter to allocate temporary tensors to store quantized values
    // of input tensors.
    node->temporaries->data[1] = scratch_tensor_index + 1;
    TfLiteTensor* input_quantized;
    TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/1,
                                                &input_quantized));
    input_quantized->type = weights_feature->type;
    input_quantized->allocation_type = kTfLiteArenaRw;
    if (!TfLiteIntArrayEqual(input_quantized->dims, input->dims)) {
      TfLiteIntArray* input_quantized_size = TfLiteIntArrayCopy(input->dims);
      TF_LITE_ENSURE_OK(context, context->ResizeTensor(context, input_quantized,
                                                       input_quantized_size));
    }

    // Tell interpreter to allocate temporary tensors to store scaling factors.
    node->temporaries->data[2] = scratch_tensor_index + 2;
    TfLiteTensor* scaling_factors;
    TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/2,
                                                &scaling_factors));
    scaling_factors->type = kTfLiteFloat32;
    scaling_factors->allocation_type = kTfLiteArenaRw;
    int scaling_dims[1] = {batch_size};
    if (!TfLiteIntArrayEqualsArray(scaling_factors->dims, 1, scaling_dims)) {
      TfLiteIntArray* scaling_factors_size = TfLiteIntArrayCreate(1);
      scaling_factors_size->data[0] = batch_size;
      TF_LITE_ENSURE_OK(context, context->ResizeTensor(context, scaling_factors,
                                                       scaling_factors_size));
    }

    // Used to store dequantized weights_time matrix for hybrid computation of
    // matmul(state, weights_time), which occurs in floating point.
    node->temporaries->data[3] = scratch_tensor_index + 3;
    TfLiteTensor* float_weights_time;
    TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/3,
                                                &float_weights_time));
    float_weights_time->type = kTfLiteFloat32;
    // Persistent so that we can compute the dequantized weights only once.
    float_weights_time->allocation_type = kTfLiteArenaRwPersistent;
    if (!TfLiteIntArrayEqual(float_weights_time->dims, weights_time->dims)) {
      TfLiteIntArray* float_weights_time_size =
          TfLiteIntArrayCopy(weights_time->dims);
      TF_LITE_ENSURE_OK(context,
                        context->ResizeTensor(context, float_weights_time,
                                              float_weights_time_size));
    }

    node->temporaries->data[4] = scratch_tensor_index + 4;
    TfLiteTensor* zero_points;
    TF_LITE_ENSURE_OK(
        context, GetTemporarySafe(context, node, /*index=*/4, &zero_points));
    zero_points->type = kTfLiteFloat32;
    zero_points->allocation_type = kTfLiteArenaRw;
    int zero_points_dims[1] = {batch_size};
    if (!TfLiteIntArrayEqualsArray(zero_points->dims, 1, zero_points_dims)) {
      TfLiteIntArray* zero_points_size = TfLiteIntArrayCreate(1);
      zero_points_size->data[0] = zero_points_dims[0];
      TF_LITE_ENSURE_OK(context, context->ResizeTensor(context, zero_points,
                                                       zero_points_size));
    }

    node->temporaries->data[5] = scratch_tensor_index + 5;
    TfLiteTensor* row_sums;
    TF_LITE_ENSURE_OK(context,
                      GetTemporarySafe(context, node, /*index=*/5, &row_sums));
    row_sums->type = kTfLiteFloat32;
    row_sums->allocation_type = kTfLiteArenaRwPersistent;
    int row_sums_dims[1] = {num_filters};
    if (!TfLiteIntArrayEqualsArray(row_sums->dims, 1, row_sums_dims)) {
      TfLiteIntArray* row_sums_size = TfLiteIntArrayCreate(1);
      row_sums_size->data[0] = row_sums_dims[0];
      TF_LITE_ENSURE_OK(
          context, context->ResizeTensor(context, row_sums, row_sums_size));
    }
  }
  if (is_full_integer) {
    // Allocated one extra tensor.
    TfLiteIntArray* output_temp_size_array = TfLiteIntArrayCreate(2);
    output_temp_size_array->data[0] = num_units;
    output_temp_size_array->data[1] = batch_size;
    node->temporaries->data[1] = scratch_tensor_index + 1;
    TfLiteTensor* output_temp;
    TF_LITE_ENSURE_OK(
        context, GetTemporarySafe(context, node, /*index=*/1, &output_temp));
    output_temp->type = kTfLiteInt32;
    output_temp->allocation_type = kTfLiteArenaRw;
    TF_LITE_ENSURE_OK(context, context->ResizeTensor(context, output_temp,
                                                     output_temp_size_array));

    // Calculate effective scales.
    auto* input_params =
        reinterpret_cast<TfLiteAffineQuantization*>(input->quantization.params);
    auto* weights_feature_params = reinterpret_cast<TfLiteAffineQuantization*>(
        weights_feature->quantization.params);
    auto* state_params =
        reinterpret_cast<TfLiteAffineQuantization*>(state->quantization.params);
    auto* weight_time_params = reinterpret_cast<TfLiteAffineQuantization*>(
        weights_time->quantization.params);
    auto* output_params = reinterpret_cast<TfLiteAffineQuantization*>(
        output->quantization.params);
    const double effective_scale_1 = input_params->scale->data[0] *
                                     weights_feature_params->scale->data[0] /
                                     state_params->scale->data[0];
    const double effective_scale_2 = state_params->scale->data[0] *
                                     weight_time_params->scale->data[0] /
                                     output_params->scale->data[0];
    QuantizeMultiplier(effective_scale_1, &op_data->effective_scale_1_a,
                       &op_data->effective_scale_1_b);
    QuantizeMultiplier(effective_scale_2, &op_data->effective_scale_2_a,
                       &op_data->effective_scale_2_b);
  }
  return kTfLiteOk;
}