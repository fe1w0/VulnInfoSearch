Http::FilterMetadataStatus Context::onRequestMetadata() {
  if (!wasm_->onRequestMetadata_) {
    return Http::FilterMetadataStatus::Continue;
  }
  if (wasm_->onRequestMetadata_(this, id_).u64_ == 0) {
    return Http::FilterMetadataStatus::Continue;
  }
  return Http::FilterMetadataStatus::Continue; // This is currently the only return code.
}