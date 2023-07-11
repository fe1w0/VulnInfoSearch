Http::FilterTrailersStatus Context::onResponseTrailers() {
  if (!wasm_->onResponseTrailers_) {
    return Http::FilterTrailersStatus::Continue;
  }
  if (wasm_->onResponseTrailers_(this, id_).u64_ == 0) {
    return Http::FilterTrailersStatus::Continue;
  }
  return Http::FilterTrailersStatus::StopIteration;
}