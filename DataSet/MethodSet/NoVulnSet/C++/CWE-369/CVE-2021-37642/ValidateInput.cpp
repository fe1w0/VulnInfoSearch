bool ValidateInput(const Tensor& updates) {
  const auto updates_flat = updates.flat<T>();
  const T zero(0);
  for (int i = 0; i < updates.NumElements(); i++) {
    if (updates_flat(i) == zero) return false;
  }
  return true;
}