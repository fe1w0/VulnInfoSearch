TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  auto* params =
      reinterpret_cast<TfLiteSpaceToDepthParams*>(node->builtin_data);

  TF_LITE_ENSURE_EQ(context, NumInputs(node), 1);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kInputTensor, &input));
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));

  TF_LITE_ENSURE_EQ(context, NumDimensions(input), 4);

  auto data_type = output->type;
  TF_LITE_ENSURE(context,
                 data_type == kTfLiteFloat32 || data_type == kTfLiteUInt8 ||
                     data_type == kTfLiteInt8 || data_type == kTfLiteInt32 ||
                     data_type == kTfLiteInt64);
  TF_LITE_ENSURE_TYPES_EQ(context, input->type, output->type);

  const int block_size = params->block_size;
  const int input_height = input->dims->data[1];
  const int input_width = input->dims->data[2];
  int output_height = input_height / block_size;
  int output_width = input_width / block_size;

  TF_LITE_ENSURE_EQ(context, input_height, output_height * block_size);
  TF_LITE_ENSURE_EQ(context, input_width, output_width * block_size);

  TfLiteIntArray* output_size = TfLiteIntArrayCreate(4);
  output_size->data[0] = input->dims->data[0];
  output_size->data[1] = output_height;
  output_size->data[2] = output_width;
  output_size->data[3] = input->dims->data[3] * block_size * block_size;

  return context->ResizeTensor(context, output, output_size);
}