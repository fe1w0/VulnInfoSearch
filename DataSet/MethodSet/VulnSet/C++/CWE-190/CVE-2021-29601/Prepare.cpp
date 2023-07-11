TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  auto* params =
      reinterpret_cast<TfLiteConcatenationParams*>(node->builtin_data);
  int axis = params->axis;
  int num_inputs = node->inputs->size;

  // The number of dimensions of the input tensors must match, and all
  // dimensions except 'axis' must be equal.
  const TfLiteTensor* t0;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 0, &t0));
  TfLiteType input_type = t0->type;
  if (axis < 0) axis += t0->dims->size;
  TF_LITE_ENSURE(context, axis >= 0);
  TF_LITE_ENSURE(context, axis < t0->dims->size);

  TF_LITE_ENSURE_EQ(context, params->activation, kTfLiteActNone);
  TF_LITE_ENSURE(context,
                 input_type == kTfLiteFloat32 || input_type == kTfLiteUInt8 ||
                     input_type == kTfLiteInt8 || input_type == kTfLiteInt16 ||
                     input_type == kTfLiteInt32 || input_type == kTfLiteInt64 ||
                     input_type == kTfLiteBool);

  // Output dimensions will match input dimensions, except 'axis', which
  // will be the sum of inputs
  int sum_axis = t0->dims->data[axis];
  for (int i = 1; i < num_inputs; ++i) {
    const TfLiteTensor* t;
    TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, i, &t));
    TF_LITE_ENSURE_EQ(context, t->dims->size, t0->dims->size);
    TF_LITE_ENSURE_EQ(context, t->type, input_type);
    for (int d = 0; d < t0->dims->size; ++d) {
      if (d == axis) {
        sum_axis += t->dims->data[axis];
      } else {
        TF_LITE_ENSURE_EQ(context, t->dims->data[d], t0->dims->data[d]);
      }
    }
  }

  TfLiteIntArray* output_size = TfLiteIntArrayCreate(t0->dims->size);
  for (int d = 0; d < t0->dims->size; ++d) {
    output_size->data[d] = (d == axis) ? sum_axis : t0->dims->data[d];
  }

  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, 0, &output));
  TF_LITE_ENSURE_TYPES_EQ(context, output->type, input_type);

  if (input_type == kTfLiteInt8) {
    // Make sure there is no re-scaling needed for Int8 quantized kernel. This
    // is a restriction we introduced to Int8 kernels.
    VectorOfTensors<int8_t> all_inputs(*context, *node->inputs);
    for (int i = 0; i < node->inputs->size; ++i) {
      const TfLiteTensor* t;
      TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, i, &t));
      TF_LITE_ENSURE_EQ(context, t->params.scale, output->params.scale);
      TF_LITE_ENSURE_EQ(context, t->params.zero_point,
                        output->params.zero_point);
    }
  }

  if (input_type == kTfLiteInt16) {
    // Make sure that all Int16 inputs have a null zero-point.
    for (int i = 0; i < node->inputs->size; ++i) {
      const TfLiteTensor* t = GetInput(context, node, i);
      TF_LITE_ENSURE_EQ(context, t->params.zero_point, 0);
    }
    TF_LITE_ENSURE_EQ(context, output->params.zero_point, 0);
  }

  return context->ResizeTensor(context, output, output_size);
}