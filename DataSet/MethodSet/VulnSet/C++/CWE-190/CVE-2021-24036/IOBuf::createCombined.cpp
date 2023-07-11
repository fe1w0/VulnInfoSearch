unique_ptr<IOBuf> IOBuf::createCombined(std::size_t capacity) {
  // To save a memory allocation, allocate space for the IOBuf object, the
  // SharedInfo struct, and the data itself all with a single call to malloc().
  size_t requiredStorage = offsetof(HeapFullStorage, align) + capacity;
  size_t mallocSize = goodMallocSize(requiredStorage);
  auto storage = static_cast<HeapFullStorage*>(checkedMalloc(mallocSize));

  new (&storage->hs.prefix) HeapPrefix(kIOBufInUse | kDataInUse, mallocSize);
  new (&storage->shared) SharedInfo(freeInternalBuf, storage);

  if (io_buf_alloc_cb) {
    io_buf_alloc_cb(storage, mallocSize);
  }

  auto bufAddr = reinterpret_cast<uint8_t*>(&storage->align);
  uint8_t* storageEnd = reinterpret_cast<uint8_t*>(storage) + mallocSize;
  auto actualCapacity = size_t(storageEnd - bufAddr);
  unique_ptr<IOBuf> ret(new (&storage->hs.buf) IOBuf(
      InternalConstructor(),
      packFlagsAndSharedInfo(0, &storage->shared),
      bufAddr,
      actualCapacity,
      bufAddr,
      0));
  return ret;
}