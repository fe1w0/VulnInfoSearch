void RestoreTensor(OpKernelContext* context,
                   checkpoint::TensorSliceReader::OpenTableFunction open_func,
                   int preferred_shard, bool restore_slice, int restore_index) {
  const Tensor& file_pattern_t = context->input(0);
  {
    const int64_t size = file_pattern_t.NumElements();
    OP_REQUIRES(
        context, size == 1,
        errors::InvalidArgument(
            "Input 0 (file_pattern) must be a string scalar; got a tensor of ",
            size, "elements"));
  }
  const string& file_pattern = file_pattern_t.flat<tstring>()(0);

  const Tensor& tensor_name_t = context->input(1);
  const string& tensor_name = tensor_name_t.flat<tstring>()(restore_index);

  // If we cannot find a cached reader we will allocate our own.
  std::unique_ptr<checkpoint::TensorSliceReader> allocated_reader;

  const checkpoint::TensorSliceReader* reader = nullptr;

  if (context->slice_reader_cache()) {
    reader = context->slice_reader_cache()->GetReader(file_pattern, open_func,
                                                      preferred_shard);
  }
  if (!reader) {
    allocated_reader.reset(new checkpoint::TensorSliceReader(
        file_pattern, open_func, preferred_shard));
    reader = allocated_reader.get();
  }
  OP_REQUIRES_OK(context, CHECK_NOTNULL(reader)->status());

  // Get the shape and type from the save file.
  DataType type;
  TensorShape saved_shape;
  OP_REQUIRES(
      context, reader->HasTensor(tensor_name, &saved_shape, &type),
      errors::NotFound("Tensor name \"", tensor_name,
                       "\" not found in checkpoint files ", file_pattern));
  OP_REQUIRES(
      context, type == context->expected_output_dtype(restore_index),
      errors::InvalidArgument("Expected to restore a tensor of type ",
                              DataTypeString(context->expected_output_dtype(0)),
                              ", got a tensor of type ", DataTypeString(type),
                              " instead: tensor_name = ", tensor_name));

  // Shape of the output and slice to load.
  TensorShape output_shape(saved_shape);
  TensorSlice slice_to_load(saved_shape.dims());
  if (restore_slice) {
    const tstring& shape_spec =
        context->input(2).flat<tstring>()(restore_index);
    if (!shape_spec.empty()) {
      TensorShape parsed_shape;
      OP_REQUIRES_OK(context, checkpoint::ParseShapeAndSlice(
                                  shape_spec, &parsed_shape, &slice_to_load,
                                  &output_shape));
      OP_REQUIRES(
          context, parsed_shape.IsSameSize(saved_shape),
          errors::InvalidArgument(
              "Shape in shape_and_slice spec does not match the shape in the "
              "save file: ",
              parsed_shape.DebugString(),
              ", save file shape: ", saved_shape.DebugString()));
    }
  }

  Tensor* t = nullptr;
  OP_REQUIRES_OK(context,
                 context->allocate_output(restore_index, output_shape, &t));

  if (output_shape.num_elements() == 0) return;

#define READER_COPY(T)                                                \
  case DataTypeToEnum<T>::value:                                      \
    OP_REQUIRES(context,                                              \
                reader->CopySliceData(tensor_name, slice_to_load,     \
                                      t->flat<T>().data()),           \
                errors::InvalidArgument("Error copying slice data")); \
    break;

  switch (type) {
    TF_CALL_SAVE_RESTORE_TYPES(READER_COPY)
    default:
      context->SetStatus(errors::Unimplemented(
          "Restoring data type ", DataTypeString(type), " not yet supported"));
  }
#undef READER_COPY
}