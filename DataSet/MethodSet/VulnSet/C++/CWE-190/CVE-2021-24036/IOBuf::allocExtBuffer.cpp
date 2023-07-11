void IOBuf::allocExtBuffer(
    std::size_t minCapacity,
    uint8_t** bufReturn,
    SharedInfo** infoReturn,
    std::size_t* capacityReturn) {
  size_t mallocSize = goodExtBufferSize(minCapacity);
  auto buf = static_cast<uint8_t*>(checkedMalloc(mallocSize));
  initExtBuffer(buf, mallocSize, infoReturn, capacityReturn);

  // the userData and the freeFn are nullptr here
  // just store the mallocSize in userData
  (*infoReturn)->userData = reinterpret_cast<void*>(mallocSize);
  if (io_buf_alloc_cb) {
    io_buf_alloc_cb(buf, mallocSize);
  }

  *bufReturn = buf;
}