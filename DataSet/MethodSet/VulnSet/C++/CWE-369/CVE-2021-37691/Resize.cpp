TfLiteStatus Resize(TfLiteContext* context, TfLiteNode* node) {
  auto* params =
      reinterpret_cast<TfLiteLSHProjectionParams*>(node->builtin_data);
  TF_LITE_ENSURE(context, NumInputs(node) == 2 || NumInputs(node) == 3);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  const TfLiteTensor* hash;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 0, &hash));
  TF_LITE_ENSURE_EQ(context, NumDimensions(hash), 2);
  // Support up to 32 bits.
  TF_LITE_ENSURE(context, SizeOfDimension(hash, 1) <= 32);

  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 1, &input));
  TF_LITE_ENSURE(context, NumDimensions(input) >= 1);

  if (NumInputs(node) == 3) {
    const TfLiteTensor* weight;
    TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 2, &weight));
    TF_LITE_ENSURE_EQ(context, NumDimensions(weight), 1);
    TF_LITE_ENSURE_EQ(context, SizeOfDimension(weight, 0),
                      SizeOfDimension(input, 0));
  }

  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, 0, &output));
  TfLiteIntArray* outputSize = TfLiteIntArrayCreate(1);
  switch (params->type) {
    case kTfLiteLshProjectionSparse:
      outputSize->data[0] = SizeOfDimension(hash, 0);
      break;
    case kTfLiteLshProjectionDense:
      outputSize->data[0] = SizeOfDimension(hash, 0) * SizeOfDimension(hash, 1);
      break;
    default:
      return kTfLiteError;
  }
  return context->ResizeTensor(context, output, outputSize);
}