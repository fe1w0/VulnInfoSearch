  void Compute(OpKernelContext* ctx) override {
    const Tensor& a = ctx->input(0);
    const Tensor& b = ctx->input(1);
    OP_REQUIRES(ctx, TensorShapeUtils::IsMatrix(a.shape()),
                errors::InvalidArgument("a is not a matrix"));
    OP_REQUIRES(ctx, TensorShapeUtils::IsMatrix(b.shape()),
                errors::InvalidArgument("b is not a matrix"));

    const int m = transpose_a_ ? a.dim_size(1) : a.dim_size(0);
    const int k = transpose_a_ ? a.dim_size(0) : a.dim_size(1);
    const int n = transpose_b_ ? b.dim_size(0) : b.dim_size(1);
    const int k2 = transpose_b_ ? b.dim_size(1) : b.dim_size(0);

    OP_REQUIRES(ctx, k == k2,
                errors::InvalidArgument(
                    "Matrix size incompatible: a: ", a.shape().DebugString(),
                    ", b: ", b.shape().DebugString()));
    Tensor* output = nullptr;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape({m, n}), &output));

    if (k == 0) {
      // If the inner dimension k in the matrix multiplication is zero, we fill
      // the output with zeros.
      functor::SetZeroFunctor<CPUDevice, float> f;
      f(ctx->eigen_device<CPUDevice>(), output->flat<float>());
      return;
    }

    auto out = output->matrix<float>();

    std::unique_ptr<Tensor> a_float;
    std::unique_ptr<Tensor> b_float;
    if (!a_is_sparse_ && !b_is_sparse_) {
      auto left = &a;
      auto right = &b;
      // TODO(agarwal): multi-thread the conversions from bfloat16 to float.
      if (std::is_same<TL, bfloat16>::value) {
        a_float.reset(new Tensor(DT_FLOAT, a.shape()));
        BFloat16ToFloat(a.flat<bfloat16>().data(),
                        a_float->flat<float>().data(), a.NumElements());
        left = a_float.get();
      }
      if (std::is_same<TR, bfloat16>::value) {
        b_float.reset(new Tensor(DT_FLOAT, b.shape()));
        BFloat16ToFloat(b.flat<bfloat16>().data(),
                        b_float->flat<float>().data(), b.NumElements());
        right = b_float.get();
      }
      Eigen::array<Eigen::IndexPair<Eigen::DenseIndex>, 1> dim_pair;
      dim_pair[0].first = transpose_a_ ? 0 : 1;
      dim_pair[0].second = transpose_b_ ? 1 : 0;

      out.device(ctx->template eigen_device<CPUDevice>()) =
          left->matrix<float>().contract(right->matrix<float>(), dim_pair);
      return;
    }

    auto left = &a;
    auto right = &b;
    bool transpose_output = false;
    bool transpose_a = transpose_a_;
    bool transpose_b = transpose_b_;
    if (!a_is_sparse_) {
      // Swap the order of multiplications using the identity:
      // A * B = (B' *  A')'.
      std::swap(left, right);
      std::swap(transpose_a, transpose_b);
      transpose_a = !transpose_a;
      transpose_b = !transpose_b;
      transpose_output = !transpose_output;
    }

    std::unique_ptr<Tensor> right_tr;
    if (transpose_b) {
      // TODO(agarwal): avoid transposing the matrix here and directly handle
      // transpose in CreateDenseSlices.
      OP_REQUIRES(ctx, right->dim_size(0) != 0,
                  errors::InvalidArgument("b has an entry 0 in it's shape."));
      OP_REQUIRES(ctx, right->dim_size(1) != 0,
                  errors::InvalidArgument("b has an entry 0 in it's shape."));
      right_tr.reset(
          new Tensor(right->dtype(),
                     TensorShape({right->dim_size(1), right->dim_size(0)})));

      const auto perm = dsizes_10();
      if (transpose_output) {
        right_tr->matrix<TL>().device(ctx->template eigen_device<CPUDevice>()) =
            right->matrix<TL>().shuffle(perm);
      } else {
        right_tr->matrix<TR>().device(ctx->template eigen_device<CPUDevice>()) =
            right->matrix<TR>().shuffle(perm);
      }
      right = right_tr.get();
    }

    if (transpose_output) {
      DoMatMul<TR, TL>::Compute(&this->cache_tr_, left->matrix<TR>(),
                                right->matrix<TL>(), transpose_a,
                                ctx->device()->tensorflow_cpu_worker_threads(),
                                transpose_output, &out);
    } else {
      DoMatMul<TL, TR>::Compute(&this->cache_nt_, left->matrix<TL>(),
                                right->matrix<TR>(), transpose_a,
                                ctx->device()->tensorflow_cpu_worker_threads(),
                                transpose_output, &out);
    }
  }