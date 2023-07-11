void TensorSliceReader::LoadShard(int shard) const {
  CHECK_LT(shard, sss_.size());
  if (sss_[shard] || !status_.ok()) {
    return;  // Already loaded, or invalid.
  }
  string value;
  SavedTensorSlices sts;
  const string fname = fnames_[shard];
  VLOG(1) << "Reading meta data from file " << fname << "...";
  Table* table;
  Status s = open_function_(fname, &table);
  if (!s.ok()) {
    status_ = errors::DataLoss("Unable to open table file ", fname, ": ",
                               s.ToString());
    return;
  }
  sss_[shard].reset(table);
  if (!(table->Get(kSavedTensorSlicesKey, &value) &&
        ParseProtoUnlimited(&sts, value))) {
    status_ = errors::Internal(
        "Failed to find the saved tensor slices at the beginning of the "
        "checkpoint file: ",
        fname);
    return;
  }
  status_ = CheckVersions(sts.meta().versions(), TF_CHECKPOINT_VERSION,
                          TF_CHECKPOINT_VERSION_MIN_PRODUCER, "Checkpoint",
                          "checkpoint");
  if (!status_.ok()) return;
  for (const SavedSliceMeta& ssm : sts.meta().tensor()) {
    TensorShape ssm_shape;
    status_ = TensorShape::BuildTensorShapeBase(ssm.shape(), &ssm_shape);
    if (!status_.ok()) return;
    for (const TensorSliceProto& tsp : ssm.slice()) {
      TensorSlice ss_slice(tsp);
      status_ = RegisterTensorSlice(ssm.name(), ssm_shape, ssm.type(), fname,
                                    ss_slice, &tensors_);
      if (!status_.ok()) return;
    }
  }
}