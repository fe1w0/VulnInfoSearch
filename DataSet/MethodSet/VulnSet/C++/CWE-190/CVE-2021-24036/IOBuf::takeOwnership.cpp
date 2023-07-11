unique_ptr<IOBuf> IOBuf::takeOwnership(
    void* buf,
    std::size_t capacity,
    std::size_t offset,
    std::size_t length,
    FreeFunction freeFn,
    void* userData,
    bool freeOnError,
    TakeOwnershipOption option) {
  // do not allow only user data without a freeFn
  // since we use that for folly::sizedFree

  DCHECK(
      !userData || (userData && freeFn) ||
      (userData && !freeFn && (option == TakeOwnershipOption::STORE_SIZE)));

  HeapFullStorage* storage = nullptr;
  auto rollback = makeGuard([&] {
    if (storage) {
      free(storage);
    }
    takeOwnershipError(freeOnError, buf, freeFn, userData);
  });

  size_t requiredStorage = sizeof(HeapFullStorage);
  size_t mallocSize = goodMallocSize(requiredStorage);
  storage = static_cast<HeapFullStorage*>(checkedMalloc(mallocSize));

  new (&storage->hs.prefix)
      HeapPrefix(kIOBufInUse | kSharedInfoInUse, mallocSize);
  new (&storage->shared)
      SharedInfo(freeFn, userData, true /*useHeapFullStorage*/);

  auto result = unique_ptr<IOBuf>(new (&storage->hs.buf) IOBuf(
      InternalConstructor(),
      packFlagsAndSharedInfo(0, &storage->shared),
      static_cast<uint8_t*>(buf),
      capacity,
      static_cast<uint8_t*>(buf) + offset,
      length));

  rollback.dismiss();

  if (io_buf_alloc_cb) {
    io_buf_alloc_cb(storage, mallocSize);
    if (userData && !freeFn && (option == TakeOwnershipOption::STORE_SIZE)) {
      // Even though we did not allocate the buffer, call io_buf_alloc_cb()
      // since we will call io_buf_free_cb() on destruction, and we want these
      // calls to be 1:1.
      io_buf_alloc_cb(buf, capacity);
    }
  }

  return result;
}