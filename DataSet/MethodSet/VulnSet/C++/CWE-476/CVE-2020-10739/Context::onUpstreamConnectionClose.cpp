void Context::onUpstreamConnectionClose(PeerType peer_type) {
  if (wasm_->onUpstreamConnectionClose_) {
    wasm_->onUpstreamConnectionClose_(this, id_, static_cast<uint32_t>(peer_type));
  }
}