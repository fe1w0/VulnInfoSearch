TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  auto* params = reinterpret_cast<TfLiteSVDFParams*>(node->builtin_data);
  OpData* op_data = reinterpret_cast<OpData*>(node->user_data);

  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kInputTensor, &input));
  const TfLiteTensor* weights_feature;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kWeightsFeatureTensor,
                                          &weights_feature));
  const TfLiteTensor* weights_time;
  TF_LITE_ENSURE_OK(
      context, GetInputSafe(context, node, kWeightsTimeTensor, &weights_time));
  const TfLiteTensor* bias = GetOptionalInputTensor(context, node, kBiasTensor);

  TfLiteTensor* scratch;
  TF_LITE_ENSURE_OK(context,
                    GetTemporarySafe(context, node, /*index=*/0, &scratch));

  TfLiteTensor* state = GetVariableInput(context, node, kStateTensor);
  TF_LITE_ENSURE(context, state != nullptr);
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));

  switch (weights_feature->type) {
    case kTfLiteFloat32: {
      reference_ops::EvalFloatSVDF(
          params, GetTensorShape(input), GetTensorData<float>(input),
          GetTensorShape(weights_feature),
          GetTensorData<float>(weights_feature), GetTensorShape(weights_time),
          GetTensorData<float>(weights_time), GetTensorShape(bias),
          GetTensorData<float>(bias), GetTensorData<float>(scratch),
          GetTensorData<float>(state), GetTensorShape(output),
          GetTensorData<float>(output));
      return kTfLiteOk;
    }
    case kTfLiteUInt8:
    case kTfLiteInt8: {
      if (input->type == kTfLiteFloat32) {
        TfLiteTensor* input_quantized;
        TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/1,
                                                    &input_quantized));
        TfLiteTensor* scaling_factors;
        TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/2,
                                                    &scaling_factors));
        TfLiteTensor* float_weights_time;
        TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/3,
                                                    &float_weights_time));
        TfLiteTensor* zero_points;
        TF_LITE_ENSURE_OK(context, GetTemporarySafe(context, node, /*index=*/4,
                                                    &zero_points));
        TfLiteTensor* row_sums;
        TF_LITE_ENSURE_OK(
            context, GetTemporarySafe(context, node, /*index=*/5, &row_sums));
        // Dequantize weights time.
        // TODO(alanchiao): this dequantization initialization only needs to
        // happen once per model and should theoretically be placed in either
        // Init or Prepare. However, TFLite doesn't allocate float_weights_time
        // until the Eval function.
        // TODO(alanchiao): refactor logic out into dequantize function.
        if (!op_data->float_weights_time_initialized) {
          const float dequantization_scale = weights_time->params.scale;
          const int8_t* weights_time_ptr = GetTensorData<int8_t>(weights_time);
          float* float_weights_time_ptr =
              GetTensorData<float>(float_weights_time);
          for (int i = 0; i < NumElements(float_weights_time); ++i) {
            float_weights_time_ptr[i] =
                weights_time_ptr[i] * dequantization_scale;
          }
          op_data->float_weights_time_initialized = true;
        }

        int32_t* zero_points_ptr = nullptr;
        int32_t* row_sums_ptr = nullptr;
        if (params->asymmetric_quantize_inputs && row_sums != nullptr) {
          zero_points_ptr = GetTensorData<int32_t>(zero_points);
          row_sums_ptr = GetTensorData<int32_t>(row_sums);
        }

        reference_ops::EvalHybridSVDF(
            params, GetTensorShape(input), GetTensorData<float>(input),
            GetTensorShape(weights_feature),
            GetTensorData<int8_t>(weights_feature),
            weights_feature->params.scale, GetTensorShape(float_weights_time),
            GetTensorData<float>(float_weights_time), GetTensorShape(bias),
            GetTensorData<float>(bias), GetTensorData<float>(scratch),
            GetTensorData<float>(scaling_factors),
            GetTensorData<int8_t>(input_quantized), GetTensorData<float>(state),
            GetTensorShape(output), GetTensorData<float>(output),
            zero_points_ptr, row_sums_ptr, &op_data->compute_row_sums);
        return kTfLiteOk;
      }
      auto* input_params = reinterpret_cast<TfLiteAffineQuantization*>(
          input->quantization.params);
      auto* output_params = reinterpret_cast<TfLiteAffineQuantization*>(
          output->quantization.params);
      TfLiteTensor* output_temp;
      TF_LITE_ENSURE_OK(
          context, GetTemporarySafe(context, node, /*index=*/1, &output_temp));

      // Currently supports only ReLU.
      // TODO(jianlijianli): support other activations.
      TF_LITE_ENSURE_EQ(context, params->activation, kTfLiteActRelu);

      reference_ops::EvalIntegerSVDF(
          params, GetTensorShape(input), GetTensorData<int8_t>(input),
          GetTensorShape(weights_feature),
          GetTensorData<int8_t>(weights_feature), GetTensorShape(weights_time),
          GetTensorData<int16_t>(weights_time), GetTensorShape(bias),
          GetTensorData<int32_t>(bias), GetTensorData<int16_t>(state),
          GetTensorShape(output), GetTensorData<int8_t>(output),
          GetTensorData<int32_t>(scratch), GetTensorData<int32_t>(output_temp),
          op_data->effective_scale_1_a, op_data->effective_scale_1_b,
          op_data->effective_scale_2_a, op_data->effective_scale_2_b,
          input_params->zero_point->data[0],
          output_params->zero_point->data[0]);
      return kTfLiteOk;
    }
    default:
      context->ReportError(context, "Type %s not currently supported.",
                           TfLiteTypeGetName(weights_feature->type));
      return kTfLiteError;
  }
}