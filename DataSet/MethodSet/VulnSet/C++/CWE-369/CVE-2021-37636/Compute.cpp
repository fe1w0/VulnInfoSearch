  void Compute(OpKernelContext *ctx) override {
    const Tensor *indices_t, *values_t, *shape_t, *dense_t;
    OP_REQUIRES_OK(ctx, ctx->input("sp_indices", &indices_t));
    OP_REQUIRES_OK(ctx, ctx->input("sp_values", &values_t));
    OP_REQUIRES_OK(ctx, ctx->input("sp_shape", &shape_t));
    OP_REQUIRES_OK(ctx, ctx->input("dense", &dense_t));

    // Validations.
    OP_REQUIRES(ctx, TensorShapeUtils::IsMatrix(indices_t->shape()),
                errors::InvalidArgument(
                    "Input sp_indices should be a matrix but received shape: ",
                    indices_t->shape().DebugString()));
    OP_REQUIRES(ctx,
                TensorShapeUtils::IsVector(values_t->shape()) &&
                    TensorShapeUtils::IsVector(shape_t->shape()),
                errors::InvalidArgument(
                    "Inputs sp_values and sp_shape should be vectors "
                    "but received shapes: ",
                    values_t->shape().DebugString(), " and ",
                    shape_t->shape().DebugString()));
    OP_REQUIRES(
        ctx, values_t->dim_size(0) == indices_t->dim_size(0),
        errors::InvalidArgument(
            "The first dimension of values and indices should match. (",
            values_t->dim_size(0), " vs. ", indices_t->dim_size(0), ")"));

    const auto indices_mat = indices_t->matrix<int64>();
    const auto shape_vec = shape_t->vec<int64>();
    const auto lhs_dims = BCast::FromShape(TensorShape(shape_vec));
    const auto rhs_dims = BCast::FromShape(dense_t->shape());
    BCast b(lhs_dims, rhs_dims, false);  // false for keeping the same num dims.

    // True iff (size(lhs) >= size(rhs)) and all dims in lhs is greater or equal
    // to dims in rhs (from right to left).
    auto VecGreaterEq = [](ArraySlice<int64> lhs, ArraySlice<int64> rhs) {
      if (lhs.size() < rhs.size()) return false;
      for (size_t i = 0; i < rhs.size(); ++i) {
        if (lhs[lhs.size() - 1 - i] < rhs[rhs.size() - 1 - i]) return false;
      }
      return true;
    };
    OP_REQUIRES(ctx, VecGreaterEq(lhs_dims, rhs_dims) && b.IsValid(),
                errors::InvalidArgument(
                    "SparseDenseBinaryOpShared broadcasts dense to sparse "
                    "only; got incompatible shapes: [",
                    absl::StrJoin(lhs_dims, ","), "] vs. [",
                    absl::StrJoin(rhs_dims, ","), "]"));

    Tensor *output_values = nullptr;
    Tensor dense_gathered;
    const int64 nnz = indices_t->dim_size(0);
    OP_REQUIRES_OK(ctx,
                   ctx->allocate_output(0, TensorShape({nnz}), &output_values));
    OP_REQUIRES_OK(
        ctx, ctx->allocate_temp(DataTypeToEnum<T>::value, TensorShape({nnz}),
                                &dense_gathered));

    // Pulls relevant entries from the dense side, with reshape and broadcasting
    // *of the dense side* taken into account.  Use a TensorRef to avoid blowing
    // up memory.
    //
    // We can directly use the sparse indices to look up dense side, because
    // "b.y_reshape()" and "b.y_bcast()" are guaranteed to have rank "ndims".
    auto dense_gathered_flat = dense_gathered.flat<T>();
    const int ndims = lhs_dims.size();
    switch (ndims) {
#define CASE(NDIM)                                                             \
  case NDIM: {                                                                 \
    TensorRef<Eigen::Tensor<const T, NDIM, Eigen::RowMajor>> rhs_ref =         \
        dense_t->shaped<T, NDIM>(b.y_reshape())                                \
            .broadcast(BCast::ToIndexArray<NDIM>(b.y_bcast()));                \
    Eigen::array<Eigen::DenseIndex, NDIM> idx;                                 \
    bool indices_valid = true;                                                 \
    for (int i = 0; i < nnz; ++i) {                                            \
      for (int d = 0; d < NDIM; ++d) {                                         \
        idx[d] = internal::SubtleMustCopy(indices_mat(i, d));                  \
        if (!FastBoundsCheck(idx[d], rhs_ref.dimension(d))) {                  \
          indices_valid = false;                                               \
        }                                                                      \
      }                                                                        \
      OP_REQUIRES(                                                             \
          ctx, indices_valid,                                                  \
          errors::InvalidArgument("Provided indices are out-of-bounds w.r.t. " \
                                  "dense side with broadcasted shape"));       \
      dense_gathered_flat(i) = rhs_ref.coeff(idx);                             \
    }                                                                          \
    break;                                                                     \
  }

      CASE(1);
      CASE(2);
      CASE(3);
      CASE(4);
      CASE(5);
      default:
        OP_REQUIRES(
            ctx, false,
            errors::InvalidArgument("Only tensors with ranks between 1 and 5 "
                                    "are currently supported.  Tensor rank: ",
                                    ndims));
#undef CASE
    }

    output_values->flat<T>().device(ctx->eigen_device<Device>()) =
        values_t->flat<T>().binaryExpr(dense_gathered_flat,
                                       typename Functor::func());
  }