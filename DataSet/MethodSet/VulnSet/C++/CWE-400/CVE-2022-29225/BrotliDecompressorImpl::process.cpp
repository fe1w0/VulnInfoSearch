bool BrotliDecompressorImpl::process(Common::BrotliContext& ctx, Buffer::Instance& output_buffer) {
  BrotliDecoderResult result;
  result = BrotliDecoderDecompressStream(state_.get(), &ctx.avail_in_, &ctx.next_in_,
                                         &ctx.avail_out_, &ctx.next_out_, nullptr);
  if (result == BROTLI_DECODER_RESULT_ERROR) {
    // TODO(rojkov): currently the Brotli library doesn't specify possible errors in its API. Add
    // more detailed stats when they are documented.
    stats_.brotli_error_.inc();
    return false;
  }

  ctx.updateOutput(output_buffer);

  return true;
}