Network::FilterStatus Context::onDownstreamData(int data_length, bool end_of_stream) {
  if (!wasm_->onDownstreamData_) {
    return Network::FilterStatus::Continue;
  }
  auto result = wasm_->onDownstreamData_(this, id_, static_cast<uint32_t>(data_length),
                                         static_cast<uint32_t>(end_of_stream));
  // TODO(PiotrSikora): pull Proxy-WASM's FilterStatus values.
  return result.u64_ == 0 ? Network::FilterStatus::Continue : Network::FilterStatus::StopIteration;
}