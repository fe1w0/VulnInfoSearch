  void Compute(OpKernelContext* context) override {
    typedef Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
        ConstEigenMatrixMap;
    typedef Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
        EigenMatrixMap;

    constexpr int tensor_in_and_out_dims = 4;

    const Tensor& tensor_in = context->input(0);
    OP_REQUIRES(context, tensor_in.dims() == tensor_in_and_out_dims,
                errors::InvalidArgument("tensor_in must be 4-dimensional"));

    std::vector<int> input_size(tensor_in_and_out_dims);
    std::vector<int> output_size(tensor_in_and_out_dims);
    for (int i = 0; i < tensor_in_and_out_dims; ++i) {
      input_size[i] = tensor_in.dim_size(i);
    }
    // Output size.
    for (int i = 0; i < tensor_in_and_out_dims; ++i) {
      output_size[i] =
          static_cast<int>(std::floor(input_size[i] / pooling_ratio_[i]));
      DCHECK_GT(output_size[i], 0);
    }

    // Generate pooling sequence.
    std::vector<int64> row_cum_seq;
    std::vector<int64> col_cum_seq;
    GuardedPhiloxRandom generator;
    generator.Init(seed_, seed2_);
    row_cum_seq = GeneratePoolingSequence(input_size[1], output_size[1],
                                          &generator, pseudo_random_);
    col_cum_seq = GeneratePoolingSequence(input_size[2], output_size[2],
                                          &generator, pseudo_random_);

    // Prepare output.
    Tensor* output_tensor = nullptr;
    OP_REQUIRES_OK(context, context->allocate_output(
                                0,
                                TensorShape({output_size[0], output_size[1],
                                             output_size[2], output_size[3]}),
                                &output_tensor));
    Tensor* output_row_seq_tensor = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(
                       1, TensorShape({static_cast<int64>(row_cum_seq.size())}),
                       &output_row_seq_tensor));
    Tensor* output_col_seq_tensor = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(
                       2, TensorShape({static_cast<int64>(col_cum_seq.size())}),
                       &output_col_seq_tensor));

    ConstEigenMatrixMap in_mat(tensor_in.flat<T>().data(), input_size[3],
                               input_size[2] * input_size[1] * input_size[0]);

    EigenMatrixMap out_mat(output_tensor->flat<T>().data(), output_size[3],
                           output_size[2] * output_size[1] * output_size[0]);
    // out_count corresponds to number of elements in each pooling cell.
    Eigen::Matrix<T, Eigen::Dynamic, 1> out_count(out_mat.cols());

    // Initializes the output tensor and out_count with 0.
    out_mat.setZero();
    out_count.setZero();

    auto output_row_seq_flat = output_row_seq_tensor->flat<int64>();
    auto output_col_seq_flat = output_col_seq_tensor->flat<int64>();

    // Set output tensors.
    for (int i = 0; i < row_cum_seq.size(); ++i) {
      output_row_seq_flat(i) = row_cum_seq[i];
    }

    for (int i = 0; i < col_cum_seq.size(); ++i) {
      output_col_seq_flat(i) = col_cum_seq[i];
    }

    // For both input and output,
    // 0: batch
    // 1: row / row
    // 2: col / col
    // 3: depth / channel
    const int64 row_max = input_size[1] - 1;
    const int64 col_max = input_size[2] - 1;
    for (int64 b = 0; b < input_size[0]; ++b) {
      // row sequence.
      for (int64 hs = 0; hs < row_cum_seq.size() - 1; ++hs) {
        // row start and end.
        const int64 row_start = row_cum_seq[hs];
        int64 row_end =
            overlapping_ ? row_cum_seq[hs + 1] : row_cum_seq[hs + 1] - 1;
        row_end = std::min(row_end, row_max);

        // col sequence.
        for (int64 ws = 0; ws < col_cum_seq.size() - 1; ++ws) {
          const int64 out_offset =
              (b * output_size[1] + hs) * output_size[2] + ws;
          // col start and end.
          const int64 col_start = col_cum_seq[ws];
          int64 col_end =
              overlapping_ ? col_cum_seq[ws + 1] : col_cum_seq[ws + 1] - 1;
          col_end = std::min(col_end, col_max);
          for (int64 h = row_start; h <= row_end; ++h) {
            for (int64 w = col_start; w <= col_end; ++w) {
              const int64 in_offset =
                  (b * input_size[1] + h) * input_size[2] + w;
              out_mat.col(out_offset) += in_mat.col(in_offset);
              out_count(out_offset)++;
            }
          }
        }
      }
    }
    DCHECK_GT(out_count.minCoeff(), 0);
    out_mat.array().rowwise() /= out_count.transpose().array();
  }