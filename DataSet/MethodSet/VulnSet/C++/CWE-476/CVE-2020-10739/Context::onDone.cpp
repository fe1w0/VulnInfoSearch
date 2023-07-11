void Context::onDone() {
  if (wasm_->onDone_) {
    wasm_->onDone_(this, id_);
  }
}