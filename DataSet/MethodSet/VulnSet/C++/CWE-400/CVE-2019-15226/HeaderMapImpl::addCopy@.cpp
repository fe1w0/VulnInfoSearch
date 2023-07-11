void HeaderMapImpl::addCopy(const LowerCaseString& key, const std::string& value) {
  auto* entry = getExistingInline(key.get());
  if (entry != nullptr) {
    appendToHeader(entry->value(), value);
    return;
  }
  HeaderString new_key;
  new_key.setCopy(key.get().c_str(), key.get().size());
  HeaderString new_value;
  new_value.setCopy(value.c_str(), value.size());
  insertByKey(std::move(new_key), std::move(new_value));
  ASSERT(new_key.empty());   // NOLINT(bugprone-use-after-move)
  ASSERT(new_value.empty()); // NOLINT(bugprone-use-after-move)
}