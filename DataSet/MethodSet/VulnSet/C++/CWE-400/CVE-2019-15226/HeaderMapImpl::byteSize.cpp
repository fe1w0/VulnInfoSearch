uint64_t HeaderMapImpl::byteSize() const {
  uint64_t byte_size = 0;
  for (const HeaderEntryImpl& header : headers_) {
    byte_size += header.key().size();
    byte_size += header.value().size();
  }

  return byte_size;
}