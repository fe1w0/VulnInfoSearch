TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  // Retrieve tensors (All should be allocated by now)
  const TfLiteTensor* output_shape;
  TF_LITE_ENSURE_OK(
      context, GetInputSafe(context, node, kOutputShapeTensor, &output_shape));
  const TfLiteTensor* weights;
  TF_LITE_ENSURE_OK(context,
                    GetInputSafe(context, node, kWeightsTensor, &weights));
  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context,
                    GetInputSafe(context, node, kDataInputTensor, &input));
  const TfLiteTensor* bias =
      (NumInputs(node) == 4)
          ? GetOptionalInputTensor(context, node, kBiasTensor)
          : nullptr;
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));
  OpData* data = reinterpret_cast<OpData*>(node->user_data);
  TfLiteTensor* col2im = data->has_col2im
                             ? GetTemporary(context, node, data->col2im_index)
                             : nullptr;
  TfLiteTensor* transposed_weights =
      data->weights_are_transposed
          ? GetTemporary(context, node, data->transposed_weights_index)
          : nullptr;
  const auto* params =
      reinterpret_cast<TfLiteTransposeConvParams*>(node->builtin_data);

  // Resize any deferred dynamic tensors
  if (IsDynamicTensor(output)) {
    TF_LITE_ENSURE_OK(context, ResizeTensor(context, output_shape, output));
  }
  if (data->has_col2im && IsDynamicTensor(col2im)) {
    TF_LITE_ENSURE_OK(context, ResizeCol2ImTensor(context, output_shape,
                                                  weights, input, col2im));
  }

  // Get height and width of the output image.
  const int width = SizeOfDimension(output, 2);
  const int height = SizeOfDimension(output, 1);
  const int filter_width = SizeOfDimension(weights, 2);
  const int filter_height = SizeOfDimension(weights, 1);

  int unused_output_height, unused_output_width;
  data->padding = ComputePaddingHeightWidth(
      params->stride_height, params->stride_width, 1, 1, height, width,
      filter_height, filter_width, params->padding, &unused_output_height,
      &unused_output_width);

  // Currently support float32, uint8, int8, int16.
  switch (input->type) {
    case kTfLiteFloat32: {
      // Only for GenericOptimized path, we use transposed weights.
      if (data->weights_are_transposed) {
        if (!IsConstantTensor(weights)) {
          ResizeAndTransposeWeights(context, weights, transposed_weights);
        }
      }
      EvalFloat<kernel_type>(context, params, data, input, weights, bias,
                             transposed_weights, col2im, output);
      break;
    }
    case kTfLiteUInt8: {
      TfLiteTensor* scratch_buffer;
      TF_LITE_ENSURE_OK(
          context, GetTemporarySafe(context, node, data->scratch_tensor_index,
                                    &scratch_buffer));
      if (IsDynamicTensor(scratch_buffer)) {
        TF_LITE_ENSURE_OK(context,
                          ResizeTensor(context, output_shape, scratch_buffer));
      }
      if (data->weights_are_transposed) {
        if (!IsConstantTensor(weights)) {
          ResizeAndTransposeWeights(context, weights, transposed_weights);
        }
      }
      EvalQuantized<kernel_type>(context, params, data, input, weights,
                                 transposed_weights, bias, col2im, output,
                                 scratch_buffer);
      break;
    }
    case kTfLiteInt8: {
      TfLiteTensor* scratch_buffer;
      TF_LITE_ENSURE_OK(
          context, GetTemporarySafe(context, node, data->scratch_tensor_index,
                                    &scratch_buffer));
      if (IsDynamicTensor(scratch_buffer)) {
        TF_LITE_ENSURE_OK(context,
                          ResizeTensor(context, output_shape, scratch_buffer));
      }
      if (data->weights_are_transposed && !IsConstantTensor(weights)) {
        ResizeAndTransposeWeights(context, weights, transposed_weights);
      }
      EvalQuantizedPerChannel<kernel_type>(context, params, data, input,
                                           weights, transposed_weights, bias,
                                           col2im, output, scratch_buffer);
      break;
    }
    case kTfLiteInt16: {
      TfLiteTensor* scratch_buffer;
      TF_LITE_ENSURE_OK(
          context, GetTemporarySafe(context, node, data->scratch_tensor_index,
                                    &scratch_buffer));
      if (IsDynamicTensor(scratch_buffer)) {
        TF_LITE_ENSURE_OK(context,
                          ResizeTensor(context, output_shape, scratch_buffer));
      }
      if (data->weights_are_transposed && !IsConstantTensor(weights)) {
        ResizeAndTransposeWeights(context, weights, transposed_weights);
      }
      EvalQuantizedPerChannel16x8(context, params, data, input, weights,
                                  transposed_weights, bias, col2im, output,
                                  scratch_buffer);
      break;
    }
    default:
      context->ReportError(context, "Type '%s' is not currently supported.",
                           TfLiteTypeGetName(input->type));
      return kTfLiteError;
  }
  return kTfLiteOk;
}