void Context::onStart(absl::string_view root_id, absl::string_view vm_configuration) {
  if (wasm_->onStart_) {
    auto root_id_addr = wasm_->copyString(root_id);
    auto config_addr = wasm_->copyString(vm_configuration);
    wasm_->onStart_(this, id_, root_id_addr, root_id.size(), config_addr, vm_configuration.size());
  }
}