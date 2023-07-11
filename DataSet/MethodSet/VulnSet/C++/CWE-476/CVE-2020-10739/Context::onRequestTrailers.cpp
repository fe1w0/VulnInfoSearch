Http::FilterTrailersStatus Context::onRequestTrailers() {
  if (!wasm_->onRequestTrailers_) {
    return Http::FilterTrailersStatus::Continue;
  }
  if (wasm_->onRequestTrailers_(this, id_).u64_ == 0) {
    return Http::FilterTrailersStatus::Continue;
  }
  return Http::FilterTrailersStatus::StopIteration;
}