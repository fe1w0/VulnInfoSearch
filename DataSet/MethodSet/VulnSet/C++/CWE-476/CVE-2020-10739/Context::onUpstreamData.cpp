Network::FilterStatus Context::onUpstreamData(int data_length, bool end_of_stream) {
  if (!wasm_->onUpstreamData_) {
    return Network::FilterStatus::Continue;
  }
  auto result = wasm_->onUpstreamData_(this, id_, static_cast<uint32_t>(data_length),
                                       static_cast<uint32_t>(end_of_stream));
  // TODO(PiotrSikora): pull Proxy-WASM's FilterStatus values.
  return result.u64_ == 0 ? Network::FilterStatus::Continue : Network::FilterStatus::StopIteration;
}