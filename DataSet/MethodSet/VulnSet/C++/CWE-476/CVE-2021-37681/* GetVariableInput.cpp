TfLiteTensor* GetVariableInput(TfLiteContext* context, const TfLiteNode* node,
                               int index) {
  TfLiteTensor* tensor = GetMutableInput(context, node, index);
  return tensor->is_variable ? tensor : nullptr;
}