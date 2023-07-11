void ZlibDecompressorImpl::decompress(const Buffer::Instance& input_buffer,
                                      Buffer::Instance& output_buffer) {
  for (const Buffer::RawSlice& input_slice : input_buffer.getRawSlices()) {
    zstream_ptr_->avail_in = input_slice.len_;
    zstream_ptr_->next_in = static_cast<Bytef*>(input_slice.mem_);
    while (inflateNext()) {
      if (zstream_ptr_->avail_out == 0) {
        updateOutput(output_buffer);
      }
    }
  }

  // Flush z_stream and reset its buffer. Otherwise the stale content of the buffer
  // will pollute output upon the next call to decompress().
  updateOutput(output_buffer);
}