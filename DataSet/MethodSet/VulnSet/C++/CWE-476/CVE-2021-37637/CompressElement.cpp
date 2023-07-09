Status CompressElement(const std::vector<Tensor>& element,
                       CompressedElement* out) {
  // Step 1: Determine the total uncompressed size. This requires serializing
  // non-memcopyable tensors, which we save to use again later.
  std::vector<TensorProto> non_memcpy_components;
  int64 total_size = 0;
  for (auto& component : element) {
    if (DataTypeCanUseMemcpy(component.dtype())) {
      // Some datatypes can be memcopied, allowing us to save two copies
      // (AsProtoTensorContent and SerializeToArray).
      total_size += DMAHelper::buffer(&component)->size();
    } else {
      non_memcpy_components.emplace_back();
      component.AsProtoTensorContent(&non_memcpy_components.back());
      total_size += non_memcpy_components.back().ByteSizeLong();
    }
  }

  // Step 2: Write the tensor data to a buffer, and compress that buffer.
  // We use tstring for access to resize_uninitialized.
  tstring uncompressed;
  uncompressed.resize_uninitialized(total_size);
  // Position in `uncompressed` to write the next component.
  char* position = uncompressed.mdata();
  int non_memcpy_component_index = 0;
  for (auto& component : element) {
    CompressedComponentMetadata* metadata =
        out->mutable_component_metadata()->Add();
    metadata->set_dtype(component.dtype());
    component.shape().AsProto(metadata->mutable_tensor_shape());
    if (DataTypeCanUseMemcpy(component.dtype())) {
      const TensorBuffer* buffer = DMAHelper::buffer(&component);
      memcpy(position, buffer->data(), buffer->size());
      metadata->set_tensor_size_bytes(buffer->size());
    } else {
      TensorProto& proto = non_memcpy_components[non_memcpy_component_index++];
      proto.SerializeToArray(position, proto.ByteSizeLong());
      metadata->set_tensor_size_bytes(proto.ByteSizeLong());
    }
    position += metadata->tensor_size_bytes();
  }
  DCHECK_EQ(position, uncompressed.mdata() + total_size);

  if (!port::Snappy_Compress(uncompressed.mdata(), total_size,
                             out->mutable_data())) {
    return errors::Internal("Failed to compress using snappy.");
  }
  VLOG(3) << "Compressed element from " << total_size << " bytes to "
          << out->data().size() << " bytes";
  return Status::OK();
}