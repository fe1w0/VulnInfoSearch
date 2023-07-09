TfLiteStatus ResizeOutputTensors(TfLiteContext* context, TfLiteNode* node,
                                 const TfLiteTensor* axis,
                                 const TfLiteTensor* input, int num_splits) {
  int axis_value = GetTensorData<int>(axis)[0];
  if (axis_value < 0) {
    axis_value += NumDimensions(input);
  }

  TF_LITE_ENSURE(context, axis_value >= 0);
  TF_LITE_ENSURE(context, axis_value < NumDimensions(input));

  const int input_size = SizeOfDimension(input, axis_value);
  TF_LITE_ENSURE_MSG(context, input_size % num_splits == 0,
                     "Not an even split");
  const int slice_size = input_size / num_splits;

  for (int i = 0; i < NumOutputs(node); ++i) {
    TfLiteIntArray* output_dims = TfLiteIntArrayCopy(input->dims);
    output_dims->data[axis_value] = slice_size;
    TfLiteTensor* output;
    TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, i, &output));
    TF_LITE_ENSURE_STATUS(context->ResizeTensor(context, output, output_dims));
  }

  return kTfLiteOk;
}