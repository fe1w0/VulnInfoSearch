void Context::onDownstreamConnectionClose(PeerType peer_type) {
  if (wasm_->onDownstreamConnectionClose_) {
    wasm_->onDownstreamConnectionClose_(this, id_, static_cast<uint32_t>(peer_type));
  }
}