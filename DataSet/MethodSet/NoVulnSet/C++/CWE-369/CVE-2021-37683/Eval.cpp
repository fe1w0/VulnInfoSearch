TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  auto* params = reinterpret_cast<TfLiteDivParams*>(node->builtin_data);
  OpData* data = reinterpret_cast<OpData*>(node->user_data);

  const TfLiteTensor* input1;
  TF_LITE_ENSURE_OK(context,
                    GetInputSafe(context, node, kInputTensor1, &input1));
  const TfLiteTensor* input2;
  TF_LITE_ENSURE_OK(context,
                    GetInputSafe(context, node, kInputTensor2, &input2));
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));

  // TODO(b/193904910): This can written with C++ templates
#define TF_LITE_CHECK_DIV_NON_ZERO(data_type)                       \
  const auto* input2_data = GetTensorData<data_type>(input2);       \
  const size_t input2_elements = input2->bytes / sizeof(data_type); \
  for (size_t i = 0; i < input2_elements; i++) {                    \
    TF_LITE_ENSURE(context, input2_data[i] != 0);                   \
  }

  if (output->type == kTfLiteFloat32) {
    // Div by zero seems ok in this case, just like in TF case infinities are
    // returned. So we don't do a check at this point.
    EvalDiv<kernel_type>(context, node, params, data, input1, input2, output);
  } else if (output->type == kTfLiteInt32) {
    TF_LITE_CHECK_DIV_NON_ZERO(int32_t);
    EvalDiv<kernel_type>(context, node, params, data, input1, input2, output);
  } else if (output->type == kTfLiteUInt8) {
    TF_LITE_CHECK_DIV_NON_ZERO(uint8_t);
    TF_LITE_ENSURE_OK(
        context, EvalQuantized<kernel_type>(context, node, params, data, input1,
                                            input2, output));
  } else {
    context->ReportError(
        context,
        "Div only supports FLOAT32, INT32 and quantized UINT8 now, got %d.",
        output->type);
    return kTfLiteError;
  }
#undef TF_LITE_CHECK_DIV_NON_ZERO

  return kTfLiteOk;
}