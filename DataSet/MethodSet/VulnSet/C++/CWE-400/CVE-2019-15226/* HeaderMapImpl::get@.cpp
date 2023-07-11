HeaderEntry* HeaderMapImpl::get(const LowerCaseString& key) {
  for (HeaderEntryImpl& header : headers_) {
    if (header.key() == key.get().c_str()) {
      return &header;
    }
  }

  return nullptr;
}