TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteTensor* params;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kParams, &params));
  const TfLiteTensor* indices;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kIndices, &indices));
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));

  switch (indices->type) {
    case kTfLiteInt32:
      return EvalGatherNd<int32_t>(context, params, indices, output);
    case kTfLiteInt64:
      return EvalGatherNd<int64_t>(context, params, indices, output);
    default:
      context->ReportError(
          context, "Indices of type '%s' are not supported by gather_nd.",
          TfLiteTypeGetName(indices->type));
      return kTfLiteError;
  }
}