void Context::onLog() {
  if (wasm_->onLog_) {
    wasm_->onLog_(this, id_);
  }
}