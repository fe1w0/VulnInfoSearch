  void Compute(OpKernelContext* context) override {
    const Tensor& x = context->input(0);
    const Tensor& y = context->input(1);
    const float min_x = context->input(2).flat<float>()(0);
    const float max_x = context->input(3).flat<float>()(0);
    const float min_y = context->input(4).flat<float>()(0);
    const float max_y = context->input(5).flat<float>()(0);

    BCast bcast(BCast::FromShape(x.shape()), BCast::FromShape(y.shape()));
    if (!bcast.IsValid()) {
      context->SetStatus(errors::InvalidArgument(
          "Incompatible shapes: ", x.shape().DebugString(), " vs. ",
          y.shape().DebugString()));
      return;
    }
    Tensor* z;
    OP_REQUIRES_OK(context, context->allocate_output(
                                0, BCast::ToShape(bcast.output_shape()), &z));

    // Make sure that we have valid quantization ranges for the input buffers.
    // If the difference between the min and max is negative or zero, it makes
    // it hard to do meaningful intermediate operations on the values.
    OP_REQUIRES(context, (max_x > min_x),
                errors::InvalidArgument("max_x must be larger than min_a."));
    OP_REQUIRES(context, (max_y > min_y),
                errors::InvalidArgument("max_x must be larger than min_b."));
    const int32 offset_x = FloatToQuantizedUnclamped<T>(0.0f, min_x, max_x);
    const int32 offset_y = FloatToQuantizedUnclamped<T>(0.0f, min_y, max_y);
    const T* x_data = x.flat<T>().data();
    const T* y_data = y.flat<T>().data();
    Toutput* z_data = z->flat<Toutput>().data();

    const int ndims = bcast.x_reshape().size();
    if (ndims <= 1) {
      if (x.NumElements() == 1) {
        ScalarMultiply<T, Toutput>(context, y_data, offset_y, y.NumElements(),
                                   x_data[0], offset_x, z_data);
      } else if (y.NumElements() == 1) {
        ScalarMultiply<T, Toutput>(context, x_data, offset_x, x.NumElements(),
                                   y_data[0], offset_y, z_data);
      } else {
        VectorMultiply<T, Toutput>(context, x_data, offset_x, y_data, offset_y,
                                   x.NumElements(), z_data);
      }
    } else if (ndims == 2) {
      const T* vector_data;
      int64 vector_num_elements;
      int32 vector_offset;
      const T* tensor_data;
      int64 tensor_num_elements;
      int32 tensor_offset;
      if (x.NumElements() < y.NumElements()) {
        vector_data = x_data;
        vector_num_elements = x.NumElements();
        vector_offset = offset_x;
        tensor_data = y_data;
        tensor_num_elements = y.NumElements();
        tensor_offset = offset_y;
      } else {
        vector_data = y_data;
        vector_num_elements = y.NumElements();
        vector_offset = offset_y;
        tensor_data = x_data;
        tensor_num_elements = x.NumElements();
        tensor_offset = offset_x;
      }
      VectorTensorMultiply<T, Toutput>(
          vector_data, vector_offset, vector_num_elements, tensor_data,
          tensor_offset, tensor_num_elements, z_data);
    } else {
      LOG(INFO) << "ndims=" << ndims;
      LOG(INFO) << "bcast.x_reshape()="
                << TensorShape(bcast.x_reshape()).DebugString();
      LOG(INFO) << "bcast.y_reshape()="
                << TensorShape(bcast.y_reshape()).DebugString();
      LOG(INFO) << "bcast.x_bcast()="
                << TensorShape(bcast.x_bcast()).DebugString();
      LOG(INFO) << "bcast.y_bcast()="
                << TensorShape(bcast.y_bcast()).DebugString();

      context->SetStatus(errors::Unimplemented(
          "Broadcast between ", context->input(0).shape().DebugString(),
          " and ", context->input(1).shape().DebugString(),
          " is not supported yet."));
      return;
    }

    float min_z_value;
    float max_z_value;
    QuantizationRangeForMultiplication<T, T, Toutput>(
        min_x, max_x, min_y, max_y, &min_z_value, &max_z_value);
    Tensor* z_min = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(1, {}, &z_min));
    z_min->flat<float>()(0) = min_z_value;

    Tensor* z_max = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(2, {}, &z_max));
    z_max->flat<float>()(0) = max_z_value;
  }