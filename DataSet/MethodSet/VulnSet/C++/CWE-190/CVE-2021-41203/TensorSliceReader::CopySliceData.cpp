bool TensorSliceReader::CopySliceData(const string& name,
                                      const TensorSlice& slice, T* data) const {
  std::vector<std::pair<TensorSlice, string>> details;
  const TensorSliceSet* tss;
  {
    mutex_lock l(mu_);
    tss = FindTensorSlice(name, slice, &details);
    if (!tss && !all_shards_loaded_) {
      VLOG(1) << "Did not find slice in preferred shard, loading all shards."
              << name << ": " << slice.DebugString();
      LoadAllShards();
      tss = FindTensorSlice(name, slice, &details);
    }
    if (!tss) {
      // No such tensor
      return false;
    }
  }
  // We have the data -- copy it over.
  string value;
  for (const auto& x : details) {
    const TensorSlice& slice_s = x.first;
    const string& fname = x.second;
    int idx = gtl::FindWithDefault(fname_to_index_, fname, -1);
    CHECK_GE(idx, 0) << "Failed to find the index for filename " << fname;
    // We read a record in the corresponding sstable
    const string key = EncodeTensorNameSlice(name, slice_s);
    if (!sss_[idx]->Get(key, &value)) {
      VLOG(1) << "Failed to seek to the record for tensor " << name
              << ", slice " << slice_s.DebugString()
              << ": computed key = " << key;
      return false;
    }
    SavedTensorSlices sts;
    if (!ParseProtoUnlimited(&sts, value)) {
      VLOG(1) << "Failed to parse the record for tensor " << name << ", slice "
              << slice_s.DebugString() << ": computed key = " << key;
      return false;
    }
    CopyDataFromTensorSliceToTensorSlice(
        tss->shape(), slice_s, slice,
        checkpoint::TensorProtoData<T>(sts.data().data()), data);
  }
  return true;
}