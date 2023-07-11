inline int TensorProtoDataSize<Eigen::half>(const TensorProto& t) {
  return t.half_val_size();
}