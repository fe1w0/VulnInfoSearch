TfLiteStatus ResizeOutputTensor(TfLiteContext* context,
                                SpaceToBatchNDContext* op_context) {
  TfLiteIntArray* input_size = op_context->input->dims;
  const int32* block_shape = GetTensorData<int32>(op_context->block_shape);
  const int32* paddings_data = GetTensorData<int32>(op_context->paddings);

  int spatial_dims_num = input_size->size - 2;
  // Block_shape should be a 1D tensor with dimension [spatial_dims_num].
  TF_LITE_ENSURE_EQ(context, NumDimensions(op_context->block_shape), 1);
  TF_LITE_ENSURE_EQ(context, op_context->block_shape->dims->data[0],
                    spatial_dims_num);
  // Paddings should be a 2D tensor with dimension [spatial_dims_num, 2].
  TF_LITE_ENSURE_EQ(context, NumDimensions(op_context->paddings), 2);
  TF_LITE_ENSURE_EQ(context, op_context->paddings->dims->data[0],
                    spatial_dims_num);
  TF_LITE_ENSURE_EQ(context, op_context->paddings->dims->data[1], 2);

  TfLiteIntArray* output_size = TfLiteIntArrayCopy(input_size);

  // Ensures the input height and width (with padding) is a multiple of block
  // shape height and width.
  int output_batch_size = input_size->data[0];
  for (int dim = 0; dim < spatial_dims_num; ++dim) {
    int final_dim_size = (input_size->data[dim + 1] + paddings_data[dim * 2] +
                          paddings_data[dim * 2 + 1]);
    TF_LITE_ENSURE_EQ(context, final_dim_size % block_shape[dim], 0);
    output_size->data[dim + 1] = final_dim_size / block_shape[dim];
    output_batch_size *= block_shape[dim];
  }

  output_size->data[0] = output_batch_size;
  output_size->data[input_size->size - 1] =
      input_size->data[input_size->size - 1];

  return context->ResizeTensor(context, op_context->output, output_size);
}