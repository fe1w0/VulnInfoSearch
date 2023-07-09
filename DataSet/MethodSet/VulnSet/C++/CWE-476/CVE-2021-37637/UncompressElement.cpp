Status UncompressElement(const CompressedElement& compressed,
                         std::vector<Tensor>* out) {
  int num_components = compressed.component_metadata_size();
  out->clear();
  out->reserve(num_components);

  // Step 1: Prepare the memory that we will uncompress into.
  std::vector<struct iovec> iov(num_components);
  // We use tstring for access to resize_uninitialized.
  std::vector<tstring> tensor_proto_strs;
  // num_components is a conservative estimate. It is important to reserve
  // vector space so that the vector doesn't resize itself, which could
  // invalidate pointers to its strings' data.
  tensor_proto_strs.reserve(num_components);
  int64 total_size = 0;
  for (int i = 0; i < num_components; ++i) {
    const CompressedComponentMetadata& metadata =
        compressed.component_metadata(i);
    if (DataTypeCanUseMemcpy(metadata.dtype())) {
      out->emplace_back(metadata.dtype(), metadata.tensor_shape());
      TensorBuffer* buffer = DMAHelper::buffer(&out->back());
      iov[i].iov_base = buffer->data();
      iov[i].iov_len = buffer->size();
    } else {
      // Allocate an empty Tensor. We will fill it out later after
      // uncompressing into the tensor_proto_str.
      out->emplace_back();
      tensor_proto_strs.emplace_back();
      tstring& tensor_proto_str = tensor_proto_strs.back();
      tensor_proto_str.resize_uninitialized(metadata.tensor_size_bytes());
      iov[i].iov_base = tensor_proto_str.mdata();
      iov[i].iov_len = tensor_proto_str.size();
    }
    total_size += iov[i].iov_len;
  }

  // Step 2: Uncompress into the iovec.
  const std::string& compressed_data = compressed.data();
  size_t uncompressed_size;
  if (!port::Snappy_GetUncompressedLength(
          compressed_data.data(), compressed_data.size(), &uncompressed_size)) {
    return errors::Internal(
        "Could not get snappy uncompressed length. Compressed data size: ",
        compressed_data.size());
  }
  if (uncompressed_size != static_cast<size_t>(total_size)) {
    return errors::Internal(
        "Uncompressed size mismatch. Snappy expects ", uncompressed_size,
        " whereas the tensor metadata suggests ", total_size);
  }
  if (!port::Snappy_UncompressToIOVec(compressed_data.data(),
                                      compressed_data.size(), iov.data(),
                                      num_components)) {
    return errors::Internal("Failed to perform snappy decompression.");
  }

  // Step 3: Deserialize tensor proto strings to tensors.
  int tensor_proto_strs_index = 0;
  for (int i = 0; i < num_components; ++i) {
    if (DataTypeCanUseMemcpy(compressed.component_metadata(i).dtype())) {
      continue;
    }
    TensorProto tp;
    if (!tp.ParseFromString(tensor_proto_strs[tensor_proto_strs_index++])) {
      return errors::Internal("Could not parse TensorProto");
    }
    if (!out->at(i).FromProto(tp)) {
      return errors::Internal("Could not parse Tensor");
    }
  }
  return Status::OK();
}