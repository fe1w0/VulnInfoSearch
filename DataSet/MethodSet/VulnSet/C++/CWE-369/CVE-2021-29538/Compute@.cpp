  void Compute(OpKernelContext* context) override {
    const Tensor& input = context->input(0);
    const Tensor& filter_sizes = context->input(1);
    const Tensor& out_backprop = context->input(2);
    OP_REQUIRES(
        context, TensorShapeUtils::IsVector(filter_sizes.shape()),
        errors::InvalidArgument(
            "Conv2DCustomBackpropFilter: filter_sizes input must be 1-dim, "
            "not ",
            filter_sizes.dims()));
    TensorShape filter_shape;
    OP_REQUIRES_OK(context, TensorShapeUtils::MakeShape(
                                filter_sizes.vec<int32>(), &filter_shape));

    ConvBackpropDimensions dims;
    OP_REQUIRES_OK(
        context,
        ConvBackpropComputeDimensionsV2(
            "Conv2DCustomBackpropFilter", /*num_spatial_dims=*/2, input.shape(),
            filter_shape, out_backprop.shape(), dilations_, strides_, padding_,
            explicit_paddings_, data_format_, &dims));

    Tensor* filter_backprop;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, filter_shape, &filter_backprop));

    // If there is nothing to compute, return.
    if (filter_shape.num_elements() == 0) {
      return;
    }

    int64 pad_top, pad_bottom;
    int64 pad_left, pad_right;
    if (padding_ == Padding::EXPLICIT) {
      pad_top = explicit_paddings_[2];
      pad_bottom = explicit_paddings_[3];
      pad_left = explicit_paddings_[4];
      pad_right = explicit_paddings_[5];
    }
    OP_REQUIRES_OK(
        context,
        GetWindowedOutputSizeVerbose(
            dims.spatial_dims[0].input_size, dims.spatial_dims[0].filter_size,
            dims.spatial_dims[0].stride, padding_,
            &dims.spatial_dims[0].output_size, &pad_top, &pad_bottom));
    OP_REQUIRES_OK(
        context,
        GetWindowedOutputSizeVerbose(
            dims.spatial_dims[1].input_size, dims.spatial_dims[1].filter_size,
            dims.spatial_dims[1].stride, padding_,
            &dims.spatial_dims[1].output_size, &pad_left, &pad_right));
#if defined TENSORFLOW_USE_LIBXSMM_CONVOLUTIONS && \
    defined TENSORFLOW_USE_LIBXSMM_BACKWARD_CONVOLUTIONS
    if (pad_left == pad_right && pad_top == pad_bottom) {
      if (LaunchXsmmBackwardFilter<Device, T>()(
              context, context->eigen_device<Device>(), input.tensor<T, 4>(),
              filter_backprop->tensor<T, 4>(), out_backprop.tensor<T, 4>(),
              dims.spatial_dims[0].input_size, dims.spatial_dims[1].input_size,
              static_cast<int>(dims.spatial_dims[0].stride),
              static_cast<int>(dims.spatial_dims[1].stride),
              static_cast<int>(pad_top), static_cast<int>(pad_left),
              data_format_)) {
        return;
      }
    }
#endif

    // The total dimension size of each kernel.
    const int filter_total_size = dims.spatial_dims[0].filter_size *
                                  dims.spatial_dims[1].filter_size *
                                  dims.in_depth;
    // The output image size is the spatial size of the output.
    const int output_image_size =
        dims.spatial_dims[0].output_size * dims.spatial_dims[1].output_size;

    // Shard 'batch' images into 'shard_size' groups of images to be fed
    // into the parallel matmul. Calculate 'shard_size' by dividing the L3 cache
    // size ('target_working_set_size') by the matmul size of an individual
    // image ('work_unit_size').

    // TODO(andydavis)
    // *) Get L3 cache size from device at runtime (30MB is from ivybridge).
    // *) Consider reducing 'target_working_set_size' if L3 is shared by
    //    other concurrently running tensorflow ops.
    const size_t target_working_set_size = (30LL << 20) / sizeof(T);

    const size_t size_A = output_image_size * filter_total_size;

    const size_t size_B = output_image_size * dims.out_depth;

    const size_t size_C = filter_total_size * dims.out_depth;

    const size_t work_unit_size = size_A + size_B + size_C;

    const size_t shard_size =
        (target_working_set_size + work_unit_size - 1) / work_unit_size;

    Tensor col_buffer;
    OP_REQUIRES_OK(context,
                   context->allocate_temp(
                       DataTypeToEnum<T>::value,
                       TensorShape({static_cast<int64>(shard_size),
                                    static_cast<int64>(output_image_size),
                                    static_cast<int64>(filter_total_size)}),
                       &col_buffer));

    // The input offset corresponding to a single input image.
    const int input_offset = dims.spatial_dims[0].input_size *
                             dims.spatial_dims[1].input_size * dims.in_depth;
    // The output offset corresponding to a single output image.
    const int output_offset = dims.spatial_dims[0].output_size *
                              dims.spatial_dims[1].output_size * dims.out_depth;

    const T* input_data = input.template flat<T>().data();
    T* col_buffer_data = col_buffer.template flat<T>().data();
    const T* out_backprop_data = out_backprop.template flat<T>().data();
    T* filter_backprop_data = filter_backprop->template flat<T>().data();

    typedef Eigen::TensorMap<Eigen::Tensor<T, 2, Eigen::RowMajor>,
                             Eigen::Unaligned>
        TensorMap;
    typedef Eigen::TensorMap<Eigen::Tensor<const T, 2, Eigen::RowMajor>,
                             Eigen::Unaligned>
        ConstTensorMap;

    TensorMap C(filter_backprop_data, filter_total_size, dims.out_depth);
    C.setZero();

    // Initialize contraction dims (we need to transpose 'A' below).
    Eigen::array<Eigen::IndexPair<Eigen::DenseIndex>, 1> contract_dims;
    contract_dims[0].first = 0;
    contract_dims[0].second = 0;

    auto worker_threads = *(context->device()->tensorflow_cpu_worker_threads());

    for (int image_id = 0; image_id < dims.batch_size; image_id += shard_size) {
      const int shard_limit =
          std::min(static_cast<int>(shard_size),
                   static_cast<int>(dims.batch_size) - image_id);

      auto shard = [&input_data, &col_buffer_data, &dims, &pad_top, &pad_left,
                    &pad_bottom, &pad_right, &input_offset,
                    &size_A](int64 start, int64 limit) {
        for (int shard_id = start; shard_id < limit; ++shard_id) {
          const T* input_data_shard = input_data + shard_id * input_offset;
          T* col_data_shard = col_buffer_data + shard_id * size_A;

          // When we compute the gradient with respect to the filters, we need
          // to do im2col to allow gemm-type computation.
          Im2col<T>(
              input_data_shard, dims.in_depth, dims.spatial_dims[0].input_size,
              dims.spatial_dims[1].input_size, dims.spatial_dims[0].filter_size,
              dims.spatial_dims[1].filter_size, pad_top, pad_left, pad_bottom,
              pad_right, dims.spatial_dims[0].stride,
              dims.spatial_dims[1].stride, col_data_shard);
        }
      };
      Shard(worker_threads.num_threads, worker_threads.workers, shard_limit,
            size_A, shard);

      ConstTensorMap A(col_buffer_data, output_image_size * shard_limit,
                       filter_total_size);
      ConstTensorMap B(out_backprop_data, output_image_size * shard_limit,
                       dims.out_depth);

      // Gradient with respect to filter.
      C.device(context->eigen_cpu_device()) += A.contract(B, contract_dims);

      input_data += input_offset * shard_limit;
      out_backprop_data += output_offset * shard_limit;
    }
  }