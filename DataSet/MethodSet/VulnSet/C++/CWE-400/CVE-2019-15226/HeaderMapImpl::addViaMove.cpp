void HeaderMapImpl::addViaMove(HeaderString&& key, HeaderString&& value) {
  // If this is an inline header, we can't addViaMove, because we'll overwrite
  // the existing value.
  auto* entry = getExistingInline(key.getStringView());
  if (entry != nullptr) {
    appendToHeader(entry->value(), value.getStringView());
    key.clear();
    value.clear();
  } else {
    insertByKey(std::move(key), std::move(value));
  }
}