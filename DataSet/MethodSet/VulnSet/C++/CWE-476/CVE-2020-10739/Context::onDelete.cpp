void Context::onDelete() {
  if (wasm_->onDelete_) {
    wasm_->onDelete_(this, id_);
  }
}