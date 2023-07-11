BrotliContext::BrotliContext(const uint32_t chunk_size)
    : chunk_size_{chunk_size}, chunk_ptr_{std::make_unique<uint8_t[]>(chunk_size)}, next_in_{},
      next_out_{chunk_ptr_.get()}, avail_in_{0}, avail_out_{chunk_size} {}