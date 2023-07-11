TfLiteStatus ResizeOutputTensor(TfLiteContext* context,
                                BatchToSpaceNDContext* op_context) {
  TfLiteIntArray* input_size = op_context->input->dims;
  const int* block_shape = GetTensorData<int32>(op_context->block_shape);
  const int* crops = GetTensorData<int32>(op_context->crops);

  int spatial_dims_num = input_size->size - 2;
  // Block_shape should be a 1D tensor with dimension [spatial_dims_num].
  TF_LITE_ENSURE_EQ(context, NumDimensions(op_context->block_shape), 1);
  TF_LITE_ENSURE_EQ(context, op_context->block_shape->dims->data[0],
                    spatial_dims_num);
  // Crops should be a 2D tensor with dimension [spatial_dims_num, 2].
  TF_LITE_ENSURE_EQ(context, NumDimensions(op_context->crops), 2);
  TF_LITE_ENSURE_EQ(context, op_context->crops->dims->data[0],
                    spatial_dims_num);
  TF_LITE_ENSURE_EQ(context, op_context->crops->dims->data[1], 2);

  for (int i = 0; i < spatial_dims_num * 2; ++i) {
    TF_LITE_ENSURE(context, crops[i] >= 0);
  }

  TfLiteIntArray* output_size = TfLiteIntArrayCopy(input_size);
  int output_batch_size = input_size->data[0];
  for (int dim = 0; dim < spatial_dims_num; ++dim) {
    // Number of batch must be multiple of (block_shape[dim]).
    TF_LITE_ENSURE_EQ(context, output_batch_size % block_shape[dim], 0);
    output_batch_size = output_batch_size / block_shape[dim];
    output_size->data[dim + 1] = input_size->data[dim + 1] * block_shape[dim] -
                                 crops[dim * 2] - crops[dim * 2 + 1];
  }

  output_size->data[0] = output_batch_size;
  output_size->data[input_size->size - 1] =
      input_size->data[input_size->size - 1];

  return context->ResizeTensor(context, op_context->output, output_size);
}