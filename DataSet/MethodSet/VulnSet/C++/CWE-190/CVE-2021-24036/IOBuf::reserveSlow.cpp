void IOBuf::reserveSlow(std::size_t minHeadroom, std::size_t minTailroom) {
  size_t newCapacity = (size_t)length_ + minHeadroom + minTailroom;
  DCHECK_LT(newCapacity, UINT32_MAX);

  // reserveSlow() is dangerous if anyone else is sharing the buffer, as we may
  // reallocate and free the original buffer.  It should only ever be called if
  // we are the only user of the buffer.
  DCHECK(!isSharedOne());

  // We'll need to reallocate the buffer.
  // There are a few options.
  // - If we have enough total room, move the data around in the buffer
  //   and adjust the data_ pointer.
  // - If we're using an internal buffer, we'll switch to an external
  //   buffer with enough headroom and tailroom.
  // - If we have enough headroom (headroom() >= minHeadroom) but not too much
  //   (so we don't waste memory), we can try one of two things, depending on
  //   whether we use jemalloc or not:
  //   - If using jemalloc, we can try to expand in place, avoiding a memcpy()
  //   - If not using jemalloc and we don't have too much to copy,
  //     we'll use realloc() (note that realloc might have to copy
  //     headroom + data + tailroom, see smartRealloc in folly/memory/Malloc.h)
  // - Otherwise, bite the bullet and reallocate.
  if (headroom() + tailroom() >= minHeadroom + minTailroom) {
    uint8_t* newData = writableBuffer() + minHeadroom;
    memmove(newData, data_, length_);
    data_ = newData;
    return;
  }

  size_t newAllocatedCapacity = 0;
  uint8_t* newBuffer = nullptr;
  std::size_t newHeadroom = 0;
  std::size_t oldHeadroom = headroom();

  // If we have a buffer allocated with malloc and we just need more tailroom,
  // try to use realloc()/xallocx() to grow the buffer in place.
  SharedInfo* info = sharedInfo();
  bool useHeapFullStorage = info && info->useHeapFullStorage;
  if (info && (info->freeFn == nullptr) && length_ != 0 &&
      oldHeadroom >= minHeadroom) {
    size_t headSlack = oldHeadroom - minHeadroom;
    newAllocatedCapacity = goodExtBufferSize(newCapacity + headSlack);
    if (usingJEMalloc()) {
      // We assume that tailroom is more useful and more important than
      // headroom (not least because realloc / xallocx allow us to grow the
      // buffer at the tail, but not at the head)  So, if we have more headroom
      // than we need, we consider that "wasted".  We arbitrarily define "too
      // much" headroom to be 25% of the capacity.
      if (headSlack * 4 <= newCapacity) {
        size_t allocatedCapacity = capacity() + sizeof(SharedInfo);
        void* p = buf_;
        if (allocatedCapacity >= jemallocMinInPlaceExpandable) {
          if (xallocx(p, newAllocatedCapacity, 0, 0) == newAllocatedCapacity) {
            if (io_buf_free_cb) {
              io_buf_free_cb(p, reinterpret_cast<size_t>(info->userData));
            }
            newBuffer = static_cast<uint8_t*>(p);
            newHeadroom = oldHeadroom;
            // update the userData
            info->userData = reinterpret_cast<void*>(newAllocatedCapacity);
            if (io_buf_alloc_cb) {
              io_buf_alloc_cb(newBuffer, newAllocatedCapacity);
            }
          }
          // if xallocx failed, do nothing, fall back to malloc/memcpy/free
        }
      }
    } else { // Not using jemalloc
      size_t copySlack = capacity() - length_;
      if (copySlack * 2 <= length_) {
        void* p = realloc(buf_, newAllocatedCapacity);
        if (UNLIKELY(p == nullptr)) {
          throw_exception<std::bad_alloc>();
        }
        newBuffer = static_cast<uint8_t*>(p);
        newHeadroom = oldHeadroom;
      }
    }
  }

  // None of the previous reallocation strategies worked (or we're using
  // an internal buffer).  malloc/copy/free.
  if (newBuffer == nullptr) {
    newAllocatedCapacity = goodExtBufferSize(newCapacity);
    newBuffer = static_cast<uint8_t*>(checkedMalloc(newAllocatedCapacity));
    if (length_ > 0) {
      assert(data_ != nullptr);
      memcpy(newBuffer + minHeadroom, data_, length_);
    }
    if (sharedInfo()) {
      freeExtBuffer();
    }
    newHeadroom = minHeadroom;
  }

  std::size_t cap;
  initExtBuffer(newBuffer, newAllocatedCapacity, &info, &cap);

  if (flags() & kFlagFreeSharedInfo) {
    delete sharedInfo();
  } else {
    if (useHeapFullStorage) {
      SharedInfo::releaseStorage(sharedInfo());
    }
  }

  setFlagsAndSharedInfo(0, info);
  capacity_ = cap;
  buf_ = newBuffer;
  data_ = newBuffer + newHeadroom;
  // length_ is unchanged
}