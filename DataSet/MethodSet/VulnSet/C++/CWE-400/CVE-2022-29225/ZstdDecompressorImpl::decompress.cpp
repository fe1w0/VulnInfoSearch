void ZstdDecompressorImpl::decompress(const Buffer::Instance& input_buffer,
                                      Buffer::Instance& output_buffer) {
  for (const Buffer::RawSlice& input_slice : input_buffer.getRawSlices()) {
    if (input_slice.len_ > 0) {
      if (ddict_manager_ && !is_dictionary_set_) {
        is_dictionary_set_ = true;
        // If id == 0, it means that dictionary id could not be decoded.
        dictionary_id_ =
            ZSTD_getDictID_fromFrame(static_cast<uint8_t*>(input_slice.mem_), input_slice.len_);
        if (dictionary_id_ != 0) {
          auto dictionary = ddict_manager_->getDictionaryById(dictionary_id_);
          if (!dictionary) {
            stats_.zstd_dictionary_error_.inc();
            return;
          }
          const size_t result = ZSTD_DCtx_refDDict(dctx_.get(), dictionary);
          if (isError(result)) {
            return;
          }
        }
      }

      setInput(input_slice);
      if (!process(output_buffer)) {
        return;
      }
    }
  }
}