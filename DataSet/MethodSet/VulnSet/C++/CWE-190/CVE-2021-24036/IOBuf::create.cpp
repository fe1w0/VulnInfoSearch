unique_ptr<IOBuf> IOBuf::create(std::size_t capacity) {
  // For smaller-sized buffers, allocate the IOBuf, SharedInfo, and the buffer
  // all with a single allocation.
  //
  // We don't do this for larger buffers since it can be wasteful if the user
  // needs to reallocate the buffer but keeps using the same IOBuf object.
  // In this case we can't free the data space until the IOBuf is also
  // destroyed.  Callers can explicitly call createCombined() or
  // createSeparate() if they know their use case better, and know if they are
  // likely to reallocate the buffer later.
  if (capacity <= kDefaultCombinedBufSize) {
    return createCombined(capacity);
  }

  // if we have nallocx, we want to allocate the capacity and the overhead in
  // a single allocation only if we do not cross into the next allocation class
  // for some buffer sizes, this can use about 25% extra memory
  if (canNallocx()) {
    auto mallocSize = goodMallocSize(capacity);
    // round capacity to a multiple of 8
    size_t minSize = ((capacity + 7) & ~7) + sizeof(SharedInfo);
    // if we do not have space for the overhead, allocate the mem separateley
    if (mallocSize < minSize) {
      auto* buf = checkedMalloc(mallocSize);
      return takeOwnership(SIZED_FREE, buf, mallocSize, 0, 0);
    }
  }

  return createSeparate(capacity);
}