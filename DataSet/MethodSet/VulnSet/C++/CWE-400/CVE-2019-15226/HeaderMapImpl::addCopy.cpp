void HeaderMapImpl::addCopy(const LowerCaseString& key, uint64_t value) {
  auto* entry = getExistingInline(key.get());
  if (entry != nullptr) {
    char buf[32];
    StringUtil::itoa(buf, sizeof(buf), value);
    appendToHeader(entry->value(), buf);
    return;
  }
  HeaderString new_key;
  new_key.setCopy(key.get().c_str(), key.get().size());
  HeaderString new_value;
  new_value.setInteger(value);
  insertByKey(std::move(new_key), std::move(new_value));
  ASSERT(new_key.empty());   // NOLINT(bugprone-use-after-move)
  ASSERT(new_value.empty()); // NOLINT(bugprone-use-after-move)
}