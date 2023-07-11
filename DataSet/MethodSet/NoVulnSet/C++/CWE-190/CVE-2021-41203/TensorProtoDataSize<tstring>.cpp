inline int TensorProtoDataSize<tstring>(const TensorProto& t) {
  return t.string_val_size();
}