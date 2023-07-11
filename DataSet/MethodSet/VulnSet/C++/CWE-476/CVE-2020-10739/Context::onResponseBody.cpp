Http::FilterDataStatus Context::onResponseBody(int body_buffer_length, bool end_of_stream) {
  if (!wasm_->onResponseBody_) {
    return Http::FilterDataStatus::Continue;
  }
  switch (wasm_
              ->onResponseBody_(this, id_, static_cast<uint32_t>(body_buffer_length),
                                static_cast<uint32_t>(end_of_stream))
              .u64_) {
  case 0:
    return Http::FilterDataStatus::Continue;
  case 1:
    return Http::FilterDataStatus::StopIterationAndBuffer;
  case 2:
    return Http::FilterDataStatus::StopIterationAndWatermark;
  default:
    return Http::FilterDataStatus::StopIterationNoBuffer;
  }
}