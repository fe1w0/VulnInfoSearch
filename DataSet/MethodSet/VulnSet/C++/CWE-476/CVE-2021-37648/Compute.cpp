  void Compute(OpKernelContext* context) override {
    const Tensor& prefix = context->input(0);
    const Tensor& tensor_names = context->input(1);
    const Tensor& shape_and_slices = context->input(2);
    ValidateInputs(true /* is save op */, context, prefix, tensor_names,
                   shape_and_slices);

    const int kFixedInputs = 3;  // Prefix, tensor names, shape_and_slices.
    const int num_tensors = static_cast<int>(tensor_names.NumElements());
    const string& prefix_string = prefix.scalar<tstring>()();
    const auto& tensor_names_flat = tensor_names.flat<tstring>();
    const auto& shape_and_slices_flat = shape_and_slices.flat<tstring>();

    BundleWriter writer(Env::Default(), prefix_string);
    OP_REQUIRES_OK(context, writer.status());
    VLOG(1) << "BundleWriter, prefix_string: " << prefix_string;

    for (int i = 0; i < num_tensors; ++i) {
      const string& tensor_name = tensor_names_flat(i);
      const Tensor& tensor = context->input(i + kFixedInputs);
      VLOG(2) << "Starting save of " << tensor_name;

      if (!shape_and_slices_flat(i).empty()) {
        const string& shape_spec = shape_and_slices_flat(i);
        TensorShape shape;
        TensorSlice slice(tensor.dims());
        TensorShape slice_shape;

        OP_REQUIRES_OK(context, checkpoint::ParseShapeAndSlice(
                                    shape_spec, &shape, &slice, &slice_shape));
        OP_REQUIRES(context, slice_shape.IsSameSize(tensor.shape()),
                    errors::InvalidArgument("Slice in shape_and_slice "
                                            "specification does not match the "
                                            "shape of the tensor to  save: ",
                                            shape_spec, ", tensor: ",
                                            tensor.shape().DebugString()));

        OP_REQUIRES_OK(context,
                       writer.AddSlice(tensor_name, shape, slice, tensor));
      } else {
        OP_REQUIRES_OK(context, writer.Add(tensor_name, tensor));
      }

      if (VLOG_IS_ON(5)) {
        if (tensor.dtype() == DT_FLOAT) {
          const float* t_data = tensor.flat<float>().data();
          float min = std::numeric_limits<float>::infinity();
          float max = -std::numeric_limits<float>::infinity();
          double avg = 0.0;
          for (int i = 0; i < tensor.NumElements(); ++i) {
            if (t_data[i] < min) min = t_data[i];
            if (t_data[i] > max) max = t_data[i];
            avg += t_data[i];
          }
          VLOG(5) << " min " << min << " max " << max << " avg "
                  << avg / tensor.NumElements() << " total elts "
                  << tensor.NumElements();
        }
      }

      VLOG(2) << "Done save of " << tensor_name;
    }
    OP_REQUIRES_OK(context, writer.Finish());
    VLOG(1) << "Done BundleWriter, prefix_string: " << prefix_string;
  }