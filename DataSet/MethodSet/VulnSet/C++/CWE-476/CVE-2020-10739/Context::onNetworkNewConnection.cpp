Network::FilterStatus Context::onNetworkNewConnection() {
  onCreate(root_context_id_);
  if (!wasm_->onNewConnection_) {
    return Network::FilterStatus::Continue;
  }
  if (wasm_->onNewConnection_(this, id_).u64_ == 0) {
    return Network::FilterStatus::Continue;
  }
  return Network::FilterStatus::StopIteration;
}