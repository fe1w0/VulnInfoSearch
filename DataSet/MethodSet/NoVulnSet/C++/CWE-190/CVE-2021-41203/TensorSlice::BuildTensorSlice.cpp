Status TensorSlice::BuildTensorSlice(const TensorSliceProto& proto,
                                     TensorSlice* output) {
  output->Clear();
  output->starts_.reserve(proto.extent_size());
  output->lengths_.reserve(proto.extent_size());
  for (const auto& e : proto.extent()) {
    int64_t l = GetExtentLength(e);
    if (e.start() != 0 || l != kFullExtent) {
      if (e.start() < 0 || l <= 0) {
        return errors::InvalidArgument(
            "Expected non-negative start and positive length but got start = ",
            e.start(), ", length = ", l, ": extent = ", e.ShortDebugString());
      }
      // Calculating the extent end must not cause signed integer overflow.
      if (static_cast<uint64_t>(e.start()) + static_cast<uint64_t>(e.length()) >
          std::numeric_limits<int64_t>::max()) {
        return errors::InvalidArgument(
            "Extent end exceeds the maximum possible size: extent = ",
            e.ShortDebugString());
      }
    }
    output->starts_.push_back(e.start());
    output->lengths_.push_back(l);
  }

  return Status::OK();
}