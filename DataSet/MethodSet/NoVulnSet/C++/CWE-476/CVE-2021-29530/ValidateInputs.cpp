  Status ValidateInputs(const CSRSparseMatrix& sparse_matrix,
                        const Tensor& permutation_indices, int* batch_size,
                        int64* num_rows) {
    if (sparse_matrix.dtype() != DataTypeToEnum<T>::value)
      return errors::InvalidArgument(
          "Asked for a CSRSparseMatrix of type ",
          DataTypeString(DataTypeToEnum<T>::value),
          " but saw dtype: ", DataTypeString(sparse_matrix.dtype()));

    const Tensor& dense_shape = sparse_matrix.dense_shape();
    const int rank = dense_shape.dim_size(0);
    if (rank < 2 || rank > 3)
      return errors::InvalidArgument("sparse matrix must have rank 2 or 3; ",
                                     "but dense_shape has size ", rank);
    const int row_dim = (rank == 2) ? 0 : 1;
    auto dense_shape_vec = dense_shape.vec<int64>();
    *num_rows = dense_shape_vec(row_dim);
    const int64 num_cols = dense_shape_vec(row_dim + 1);
    if (*num_rows != num_cols)
      return errors::InvalidArgument(
          "sparse matrix must be square; got: ", *num_rows, " != ", num_cols);
    const TensorShape& perm_shape = permutation_indices.shape();
    if (perm_shape.dims() + 1 != rank)
      return errors::InvalidArgument(
          "sparse matrix must have the same rank as permutation; got: ", rank,
          " != ", perm_shape.dims(), " + 1.");
    if (perm_shape.dim_size(rank - 2) != *num_rows)
      return errors::InvalidArgument(
          "permutation must have the same number of elements in each batch "
          "as the number of rows in sparse matrix; got: ",
          perm_shape.dim_size(rank - 2), " != ", *num_rows);

    *batch_size = sparse_matrix.batch_size();
    if (*batch_size > 1) {
      if (perm_shape.dim_size(0) != *batch_size)
        return errors::InvalidArgument(
            "permutation must have the same batch size "
            "as sparse matrix; got: ",
            perm_shape.dim_size(0), " != ", *batch_size);
    }

    return Status::OK();
  }