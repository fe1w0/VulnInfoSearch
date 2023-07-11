  void Compute(OpKernelContext* context) override {
    const Tensor& input_sizes = context->input(0);
    const Tensor& filter = context->input(1);
    const Tensor& out_backprop = context->input(2);

    TensorShape input_shape;
    OP_REQUIRES_OK(context,
                   Conv2DBackpropComputeInputShape(input_sizes, filter.shape(),
                                                   out_backprop.shape(),
                                                   data_format_, &input_shape));

    ConvBackpropDimensions dims;
    OP_REQUIRES_OK(context,
                   ConvBackpropComputeDimensionsV2(
                       "Conv2DCustomBackpropInput", /*num_spatial_dims=*/2,
                       input_shape, filter.shape(), out_backprop.shape(),
                       /*dilations=*/{1, 1, 1, 1}, strides_, padding_,
                       explicit_paddings_, data_format_, &dims));

    OP_REQUIRES(context, dims.in_depth == filter.shape().dim_size(2),
                errors::InvalidArgument("Computed input depth ", dims.in_depth,
                                        " doesn't match filter input depth ",
                                        filter.shape().dim_size(2)));
    OP_REQUIRES(
        context, dims.out_depth == filter.shape().dim_size(3),
        errors::InvalidArgument("Computed output depth ", dims.out_depth,
                                " doesn't match filter output depth ",
                                filter.shape().dim_size(3)));

    Tensor* in_backprop = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, input_shape, &in_backprop));

    // If there is nothing to compute, return.
    if (input_shape.num_elements() == 0) {
      return;
    }

// TODO(ezhulenev): Remove custom kernel and move XSMM support to
// LaunchConv2DBackpropInputOp functor.
#if defined TENSORFLOW_USE_LIBXSMM_CONVOLUTIONS && \
    defined TENSORFLOW_USE_LIBXSMM_BACKWARD_CONVOLUTIONS
    int64 pad_top, pad_bottom;
    int64 pad_left, pad_right;
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

    if (pad_left == pad_right && pad_top == pad_bottom) {
      if (LaunchXsmmBackwardInputConvolution<Device, T>()(
              context, context->eigen_device<Device>(),
              in_backprop->tensor<T, 4>(), filter.tensor<T, 4>(),
              out_backprop.tensor<T, 4>(), dims.spatial_dims[0].input_size,
              dims.spatial_dims[1].input_size,
              static_cast<int>(dims.spatial_dims[0].stride),
              static_cast<int>(dims.spatial_dims[1].stride),
              static_cast<int>(pad_top), static_cast<int>(pad_left),
              data_format_)) {
        return;
      }
    }
#else
    int64 pad_top, pad_bottom;
    int64 pad_left, pad_right;
#endif
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

    // The total dimension size of each kernel.
    const int filter_total_size = dims.spatial_dims[0].filter_size *
                                  dims.spatial_dims[1].filter_size *
                                  dims.in_depth;
    // The output image size is the spatial size of the output.
    const int output_image_size =
        dims.spatial_dims[0].output_size * dims.spatial_dims[1].output_size;

    // TODO(andydavis) Get L2/L3 cache sizes from device.
    const size_t l2_cache_size = 256LL << 10;
    const size_t l3_cache_size = 30LL << 20;

    // Use L3 cache size as target working set size.
    const size_t target_working_set_size = l3_cache_size / sizeof(T);

    // Calculate size of matrices involved in MatMul: C = A x B.
    const size_t size_A = output_image_size * dims.out_depth;

    const size_t size_B = filter_total_size * dims.out_depth;

    const size_t size_C = output_image_size * filter_total_size;

    const size_t work_unit_size = size_A + size_B + size_C;

    auto worker_threads = *(context->device()->tensorflow_cpu_worker_threads());

    // Calculate per-thread work unit size.
    const size_t thread_work_unit_size =
        work_unit_size / worker_threads.num_threads;

    // Set minimum per-thread work unit size to size of L2 cache.
    const size_t min_thread_work_unit_size = l2_cache_size / sizeof(T);

    // Use parallel tensor contractions if there is no batching, or if the
    // minimum per-thread work unit size threshold has been exceeded.
    // Otherwise, revert to multiple single-threaded matmul ops running in
    // parallel to keep all threads busy.
    // TODO(andydavis) Explore alternatives to branching the code in this way
    // (i.e. run multiple, parallel tensor contractions in another thread pool).
    const bool use_parallel_contraction =
        dims.batch_size == 1 ||
        thread_work_unit_size >= min_thread_work_unit_size;

    OP_REQUIRES(
        context, work_unit_size > 0,
        errors::InvalidArgument("input, filter_sizes and out_backprop tensors "
                                "must all have at least 1 element"));

    const size_t shard_size =
        use_parallel_contraction
            ? 1
            : (target_working_set_size + work_unit_size - 1) / work_unit_size;

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

    const T* filter_data = filter.template flat<T>().data();
    T* col_buffer_data = col_buffer.template flat<T>().data();
    const T* out_backprop_data = out_backprop.template flat<T>().data();

    auto in_backprop_flat = in_backprop->template flat<T>();
    T* input_backprop_data = in_backprop_flat.data();
    in_backprop_flat.device(context->eigen_device<Device>()) =
        in_backprop_flat.constant(T(0));

    if (use_parallel_contraction) {
      typedef Eigen::TensorMap<Eigen::Tensor<T, 2, Eigen::RowMajor>,
                               Eigen::Unaligned>
          TensorMap;
      typedef Eigen::TensorMap<Eigen::Tensor<const T, 2, Eigen::RowMajor>,
                               Eigen::Unaligned>
          ConstTensorMap;

      // Initialize contraction dims (we need to transpose 'B' below).
      Eigen::array<Eigen::IndexPair<Eigen::DenseIndex>, 1> contract_dims;
      contract_dims[0].first = 1;
      contract_dims[0].second = 1;

      for (int image_id = 0; image_id < dims.batch_size; ++image_id) {
        // Compute gradient into col_buffer.
        TensorMap C(col_buffer_data, output_image_size, filter_total_size);

        ConstTensorMap A(out_backprop_data + output_offset * image_id,
                         output_image_size, dims.out_depth);
        ConstTensorMap B(filter_data, filter_total_size, dims.out_depth);

        C.device(context->eigen_cpu_device()) = A.contract(B, contract_dims);

        Col2im<T>(
            col_buffer_data, dims.in_depth, dims.spatial_dims[0].input_size,
            dims.spatial_dims[1].input_size, dims.spatial_dims[0].filter_size,
            dims.spatial_dims[1].filter_size, pad_top, pad_left, pad_bottom,
            pad_right, dims.spatial_dims[0].stride, dims.spatial_dims[1].stride,
            input_backprop_data);

        input_backprop_data += input_offset;
      }
    } else {
      for (int image_id = 0; image_id < dims.batch_size;
           image_id += shard_size) {
        const int shard_limit =
            std::min(static_cast<int>(shard_size),
                     static_cast<int>(dims.batch_size) - image_id);

        auto shard = [&context, &dims, &pad_top, &pad_left, &pad_bottom,
                      &pad_right, &output_image_size, &filter_total_size,
                      &input_backprop_data, &col_buffer_data,
                      &out_backprop_data, &filter_data, &input_offset,
                      &output_offset, &size_C](int64 start, int64 limit) {
          for (int shard_id = start; shard_id < limit; ++shard_id) {
            T* im2col_buf = col_buffer_data + shard_id * size_C;
            T* input_data = input_backprop_data + shard_id * input_offset;
            const T* out_data = out_backprop_data + shard_id * output_offset;

            Conv2DCustomBackpropInputMatMulFunctor<T>()(
                context, out_data, filter_data, filter_total_size,
                output_image_size, dims.out_depth, im2col_buf);

            Col2im<T>(im2col_buf, dims.in_depth,
                      dims.spatial_dims[0].input_size,
                      dims.spatial_dims[1].input_size,
                      dims.spatial_dims[0].filter_size,
                      dims.spatial_dims[1].filter_size, pad_top, pad_left,
                      pad_bottom, pad_right, dims.spatial_dims[0].stride,
                      dims.spatial_dims[1].stride, input_data);
          }
        };
        Shard(worker_threads.num_threads, worker_threads.workers, shard_limit,
              work_unit_size, shard);

        input_backprop_data += input_offset * shard_limit;
        out_backprop_data += output_offset * shard_limit;
      }
    }
  }