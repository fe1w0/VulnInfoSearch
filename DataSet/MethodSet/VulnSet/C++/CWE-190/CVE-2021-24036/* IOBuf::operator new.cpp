void* IOBuf::operator new(size_t size) {
  size_t fullSize = offsetof(HeapStorage, buf) + size;
  auto storage = static_cast<HeapStorage*>(checkedMalloc(fullSize));

  new (&storage->prefix) HeapPrefix(kIOBufInUse, fullSize);

  if (io_buf_alloc_cb) {
    io_buf_alloc_cb(storage, fullSize);
  }

  return &(storage->buf);
}