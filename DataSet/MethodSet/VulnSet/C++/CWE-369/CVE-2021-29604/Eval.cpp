TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, 0, &output));
  TfLiteTensor* hits;
  TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, 1, &hits));
  const TfLiteTensor* lookup;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 0, &lookup));
  const TfLiteTensor* key;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 1, &key));
  const TfLiteTensor* value;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 2, &value));

  const int num_rows = SizeOfDimension(value, 0);
  const int row_bytes = value->bytes / num_rows;
  void* pointer = nullptr;
  DynamicBuffer buf;

  for (int i = 0; i < SizeOfDimension(lookup, 0); i++) {
    int idx = -1;
    pointer = bsearch(&(lookup->data.i32[i]), key->data.i32, num_rows,
                      sizeof(int32_t), greater);
    if (pointer != nullptr) {
      idx = (reinterpret_cast<char*>(pointer) - (key->data.raw)) /
            sizeof(int32_t);
    }

    if (idx >= num_rows || idx < 0) {
      if (output->type == kTfLiteString) {
        buf.AddString(nullptr, 0);
      } else {
        memset(output->data.raw + i * row_bytes, 0, row_bytes);
      }
      hits->data.uint8[i] = 0;
    } else {
      if (output->type == kTfLiteString) {
        buf.AddString(GetString(value, idx));
      } else {
        memcpy(output->data.raw + i * row_bytes,
               value->data.raw + idx * row_bytes, row_bytes);
      }
      hits->data.uint8[i] = 1;
    }
  }
  if (output->type == kTfLiteString) {
    buf.WriteToTensorAsVector(output);
  }

  return kTfLiteOk;
}