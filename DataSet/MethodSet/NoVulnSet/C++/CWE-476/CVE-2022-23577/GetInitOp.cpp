Status GetInitOp(const string& export_dir, const MetaGraphDef& meta_graph_def,
                 string* init_op_name) {
  const auto& sig_def_map = meta_graph_def.signature_def();
  const auto& init_op_sig_it =
      meta_graph_def.signature_def().find(kSavedModelInitOpSignatureKey);
  if (init_op_sig_it != sig_def_map.end()) {
    const auto& sig_def_outputs = init_op_sig_it->second.outputs();
    const auto& sig_def_outputs_it =
        sig_def_outputs.find(kSavedModelInitOpSignatureKey);
    if (sig_def_outputs_it == sig_def_outputs.end()) {
      return errors::FailedPrecondition("Could not find output ",
                                        kSavedModelInitOpSignatureKey);
    }
    *init_op_name = sig_def_outputs_it->second.name();
    return Status::OK();
  }

  const auto& collection_def_map = meta_graph_def.collection_def();
  string init_op_collection_key;
  if (collection_def_map.find(kSavedModelMainOpKey) !=
      collection_def_map.end()) {
    init_op_collection_key = kSavedModelMainOpKey;
  } else {
    init_op_collection_key = kSavedModelLegacyInitOpKey;
  }

  const auto init_op_it = collection_def_map.find(init_op_collection_key);
  if (init_op_it != collection_def_map.end()) {
    if (init_op_it->second.node_list().value_size() != 1) {
      return errors::FailedPrecondition(
          strings::StrCat("Expected exactly one main op in : ", export_dir));
    }
    *init_op_name = init_op_it->second.node_list().value(0);
  }
  return Status::OK();
}