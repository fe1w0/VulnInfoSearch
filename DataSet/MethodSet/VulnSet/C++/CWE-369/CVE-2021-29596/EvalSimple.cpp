TfLiteStatus EvalSimple(TfLiteContext* context, TfLiteNode* node,
                        const TfLiteTensor* lookup, const TfLiteTensor* value,
                        TfLiteTensor* output) {
  const int row_size = SizeOfDimension(value, 0);
  const int row_bytes = value->bytes / row_size;

  char* output_raw = GetTensorData<char>(output);
  const char* value_raw = GetTensorData<char>(value);
  const int32_t* lookup_data = GetTensorData<int32_t>(lookup);
  for (int i = 0; i < SizeOfDimension(lookup, 0); i++) {
    int idx = lookup_data[i];
    if (idx >= row_size || idx < 0) {
      context->ReportError(context,
                           "Embedding Lookup: index out of bounds. "
                           "Got %d, and bounds are [0, %d]",
                           idx, row_size - 1);
      return kTfLiteError;
    } else {
      std::memcpy(output_raw + i * row_bytes, value_raw + idx * row_bytes,
                  row_bytes);
    }
  }

  return kTfLiteOk;
}