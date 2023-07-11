void SparseFillEmptyRowsOpImpl(OpKernelContext* context,
                               AsyncOpKernel::DoneCallback done = nullptr) {
  // Note that setting this empty lambda as the default parameter value directly
  // can cause strange compiler/linker errors, so we do it like this instead.
  if (!done) {
    done = [] {};
  }

  const int kIndicesInput = 0;
  const int kValuesInput = 1;
  const int kDenseShapeInput = 2;
  const int kDefaultValueInput = 3;

  const Tensor& indices_t = context->input(kIndicesInput);
  const Tensor& values_t = context->input(kValuesInput);
  const Tensor& dense_shape_t = context->input(kDenseShapeInput);
  const Tensor& default_value_t = context->input(kDefaultValueInput);

  OP_REQUIRES_ASYNC(
      context, TensorShapeUtils::IsVector(dense_shape_t.shape()),
      errors::InvalidArgument("dense_shape must be a vector, saw: ",
                              dense_shape_t.shape().DebugString()),
      done);
  OP_REQUIRES_ASYNC(context, TensorShapeUtils::IsMatrix(indices_t.shape()),
                    errors::InvalidArgument("indices must be a matrix, saw: ",
                                            indices_t.shape().DebugString()),
                    done);
  OP_REQUIRES_ASYNC(context, TensorShapeUtils::IsVector(values_t.shape()),
                    errors::InvalidArgument("values must be a vector, saw: ",
                                            values_t.shape().DebugString()),
                    done);
  OP_REQUIRES_ASYNC(
      context, TensorShapeUtils::IsScalar(default_value_t.shape()),
      errors::InvalidArgument("default_value must be a scalar, saw: ",
                              default_value_t.shape().DebugString()),
      done);
  // TODO(ebrevdo): add shape checks between values, indices,
  // Also add check that dense rank > 0.
  OP_REQUIRES_ASYNC(context, dense_shape_t.NumElements() != 0,
                    errors::InvalidArgument("Dense shape cannot be empty."),
                    done);

  using FunctorType = functor::SparseFillEmptyRows<Device, T, Tindex>;
  OP_REQUIRES_OK_ASYNC(context,
                       FunctorType()(context, default_value_t, indices_t,
                                     values_t, dense_shape_t, done),
                       done);
}