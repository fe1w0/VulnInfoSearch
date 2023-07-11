inline int TensorProtoDataSize<qint32>(const TensorProto& t) {
  return t.int_val_size();
}