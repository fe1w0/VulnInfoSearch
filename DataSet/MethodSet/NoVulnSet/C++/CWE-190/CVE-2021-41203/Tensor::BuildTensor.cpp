Status Tensor::BuildTensor(DataType type, const TensorShape& shape,
                           Tensor* out_tensor) {
  // Avoid crashes due to invalid or unsupported types.
  CASES_WITH_DEFAULT(
      type, {}, return errors::InvalidArgument("Type not set"),
      return errors::InvalidArgument("Unexpected type: ", DataType_Name(type)));
  *out_tensor = Tensor(type, shape);
  return Status::OK();
}