Status ValidateShapes(OpKernelContext* ctx, const Tensor& hypothesis_indices,
                      const Tensor& hypothesis_values,
                      const Tensor& hypothesis_shape,
                      const Tensor& truth_indices, const Tensor& truth_values,
                      const Tensor& truth_shape) {
  if (!TensorShapeUtils::IsMatrix(hypothesis_indices.shape()))
    return errors::InvalidArgument(
        "hypothesis_indices should be a matrix, but got shape: ",
        hypothesis_indices.shape().DebugString());
  if (!TensorShapeUtils::IsMatrix(truth_indices.shape()))
    return errors::InvalidArgument(
        "truth_indices should be a matrix, but got shape: ",
        truth_indices.shape().DebugString());
  if (!TensorShapeUtils::IsVector(hypothesis_values.shape()))
    return errors::InvalidArgument(
        "hypothesis_values should be a vector, but got shape: ",
        hypothesis_values.shape().DebugString());
  if (!TensorShapeUtils::IsVector(truth_values.shape()))
    return errors::InvalidArgument(
        "truth_values should be a vector, but got shape: ",
        truth_values.shape().DebugString());
  if (!TensorShapeUtils::IsVector(hypothesis_shape.shape()))
    return errors::InvalidArgument(
        "hypothesis_shape should be a vector, but got shape: ",
        hypothesis_shape.shape().DebugString());
  if (!TensorShapeUtils::IsVector(truth_shape.shape()))
    return errors::InvalidArgument(
        "truth_shape should be a vector, but got shape: ",
        truth_shape.shape().DebugString());
  if (hypothesis_shape.NumElements() != hypothesis_indices.dim_size(1))
    return errors::InvalidArgument(
        "Expected hypothesis_shape.NumElements == "
        "#cols(hypothesis_indices), their shapes are: ",
        hypothesis_shape.shape().DebugString(), " and ",
        hypothesis_indices.shape().DebugString());
  if (truth_shape.NumElements() < 2)
    return errors::InvalidArgument(
        "Input SparseTensors must have rank at least 2, but truth_shape "
        "rank is: ",
        truth_shape.NumElements());
  if (truth_shape.NumElements() != truth_indices.dim_size(1))
    return errors::InvalidArgument(
        "Expected truth_shape.NumElements == "
        "#cols(truth_indices), their shapes are: ",
        truth_shape.shape().DebugString(), " and ",
        truth_indices.shape().DebugString());
  if (truth_shape.NumElements() != hypothesis_shape.NumElements())
    return errors::InvalidArgument(
        "Expected truth and hypothesis to have matching ranks, but "
        "their shapes are: ",
        truth_shape.shape().DebugString(), " and ",
        hypothesis_shape.shape().DebugString());

  return Status::OK();
}