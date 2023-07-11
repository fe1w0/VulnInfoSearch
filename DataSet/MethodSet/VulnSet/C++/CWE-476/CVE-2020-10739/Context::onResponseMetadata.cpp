Http::FilterMetadataStatus Context::onResponseMetadata() {
  if (!wasm_->onResponseMetadata_) {
    return Http::FilterMetadataStatus::Continue;
  }
  if (wasm_->onResponseMetadata_(this, id_).u64_ == 0) {
    return Http::FilterMetadataStatus::Continue;
  }
  return Http::FilterMetadataStatus::Continue; // This is currently the only return code.
}