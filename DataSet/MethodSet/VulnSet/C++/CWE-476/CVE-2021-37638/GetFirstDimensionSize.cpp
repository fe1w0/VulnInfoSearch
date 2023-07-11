  Status GetFirstDimensionSize(OpKernelContext* context, INDEX_TYPE* result) {
    const Tensor first_partition_tensor =
        context->input(kFirstPartitionInputIndex);
    const RowPartitionType first_partition_type = row_partition_types_[0];
    switch (first_partition_type) {
      case RowPartitionType::FIRST_DIM_SIZE:
        *result = first_partition_tensor.scalar<INDEX_TYPE>()();
        return Status::OK();
      case RowPartitionType::VALUE_ROWIDS:
        return errors::InvalidArgument(
            "Cannot handle VALUE_ROWIDS in first dimension.");
      case RowPartitionType::ROW_SPLITS:
        *result = first_partition_tensor.shape().dim_size(0) - 1;
        return Status::OK();
      default:
        return errors::InvalidArgument(
            "Cannot handle type ",
            RowPartitionTypeToString(first_partition_type));
    }
  }