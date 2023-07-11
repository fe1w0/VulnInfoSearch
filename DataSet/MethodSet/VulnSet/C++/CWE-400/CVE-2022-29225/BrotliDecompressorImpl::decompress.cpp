void BrotliDecompressorImpl::decompress(const Buffer::Instance& input_buffer,
                                        Buffer::Instance& output_buffer) {
  Common::BrotliContext ctx(chunk_size_);

  for (const Buffer::RawSlice& input_slice : input_buffer.getRawSlices()) {
    ctx.avail_in_ = input_slice.len_;
    ctx.next_in_ = static_cast<uint8_t*>(input_slice.mem_);

    while (ctx.avail_in_ > 0) {
      if (!process(ctx, output_buffer)) {
        ctx.finalizeOutput(output_buffer);
        return;
      }
    }
  }

  // Even though the input has been fully consumed by the decoder it still can
  // be unfolded into output not fitting the output chunk. Thus keep processing
  // until the decoder's output is fully depleted.
  bool success;
  do {
    success = process(ctx, output_buffer);
  } while (success && BrotliDecoderHasMoreOutput(state_.get()));

  ctx.finalizeOutput(output_buffer);
}