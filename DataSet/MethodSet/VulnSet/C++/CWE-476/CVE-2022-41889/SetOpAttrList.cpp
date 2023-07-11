bool SetOpAttrList(TFE_Context* ctx, TFE_Op* op, const char* key,
                   PyObject* py_list, TF_AttrType type,
                   tensorflow::gtl::FlatMap<string, int64_t>* attr_list_sizes,
                   TF_Status* status) {
  if (!PySequence_Check(py_list)) {
    TF_SetStatus(
        status, TF_INVALID_ARGUMENT,
        tensorflow::strings::StrCat("Expecting sequence value for attr ", key,
                                    ", got ", py_list->ob_type->tp_name)
            .c_str());
    return false;
  }
  const int num_values = PySequence_Size(py_list);
  if (attr_list_sizes != nullptr) (*attr_list_sizes)[key] = num_values;

#define PARSE_LIST(c_type, parse_fn)                                      \
  std::unique_ptr<c_type[]> values(new c_type[num_values]);               \
  for (int i = 0; i < num_values; ++i) {                                  \
    tensorflow::Safe_PyObjectPtr py_value(PySequence_ITEM(py_list, i));   \
    if (!parse_fn(key, py_value.get(), status, &values[i])) return false; \
  }

  if (type == TF_ATTR_STRING) {
    std::unique_ptr<const void*[]> values(new const void*[num_values]);
    std::unique_ptr<size_t[]> lengths(new size_t[num_values]);
    for (int i = 0; i < num_values; ++i) {
      tensorflow::StringPiece value;
      tensorflow::Safe_PyObjectPtr py_value(PySequence_ITEM(py_list, i));
      if (!ParseStringValue(key, py_value.get(), status, &value)) return false;
      values[i] = value.data();
      lengths[i] = value.size();
    }
    TFE_OpSetAttrStringList(op, key, values.get(), lengths.get(), num_values);
  } else if (type == TF_ATTR_INT) {
    PARSE_LIST(int64_t, ParseInt64Value);
    TFE_OpSetAttrIntList(op, key, values.get(), num_values);
  } else if (type == TF_ATTR_FLOAT) {
    PARSE_LIST(float, ParseFloatValue);
    TFE_OpSetAttrFloatList(op, key, values.get(), num_values);
  } else if (type == TF_ATTR_BOOL) {
    PARSE_LIST(unsigned char, ParseBoolValue);
    TFE_OpSetAttrBoolList(op, key, values.get(), num_values);
  } else if (type == TF_ATTR_TYPE) {
    PARSE_LIST(int, ParseTypeValue);
    TFE_OpSetAttrTypeList(op, key,
                          reinterpret_cast<const TF_DataType*>(values.get()),
                          num_values);
  } else if (type == TF_ATTR_SHAPE) {
    // Make one pass through the input counting the total number of
    // dims across all the input lists.
    int total_dims = 0;
    for (int i = 0; i < num_values; ++i) {
      tensorflow::Safe_PyObjectPtr py_value(PySequence_ITEM(py_list, i));
      if (py_value.get() != Py_None) {
        if (!PySequence_Check(py_value.get())) {
          TF_SetStatus(
              status, TF_INVALID_ARGUMENT,
              tensorflow::strings::StrCat(
                  "Expecting None or sequence value for element", i,
                  " of attr ", key, ", got ", py_value->ob_type->tp_name)
                  .c_str());
          return false;
        }
        const auto size = TensorShapeNumDims(py_value.get());
        if (size >= 0) {
          total_dims += size;
        }
      }
    }
    // Allocate a buffer that can fit all of the dims together.
    std::unique_ptr<int64_t[]> buffer(new int64_t[total_dims]);
    // Copy the input dims into the buffer and set dims to point to
    // the start of each list's dims.
    std::unique_ptr<const int64_t*[]> dims(new const int64_t*[num_values]);
    std::unique_ptr<int[]> num_dims(new int[num_values]);
    int64_t* offset = buffer.get();
    for (int i = 0; i < num_values; ++i) {
      tensorflow::Safe_PyObjectPtr py_value(PySequence_ITEM(py_list, i));
      if (py_value.get() == Py_None) {
        dims[i] = nullptr;
        num_dims[i] = -1;
      } else {
        const auto size = TensorShapeNumDims(py_value.get());
        if (size == -1) {
          dims[i] = nullptr;
          num_dims[i] = -1;
          continue;
        }
        dims[i] = offset;
        num_dims[i] = size;
        for (int j = 0; j < size; ++j) {
          tensorflow::Safe_PyObjectPtr inner_py_value(
              PySequence_ITEM(py_value.get(), j));
          if (inner_py_value.get() == Py_None) {
            *offset = -1;
          } else if (!ParseDimensionValue(key, inner_py_value.get(), status,
                                          offset)) {
            return false;
          }
          ++offset;
        }
      }
    }
    TFE_OpSetAttrShapeList(op, key, dims.get(), num_dims.get(), num_values,
                           status);
    if (!status->status.ok()) return false;
  } else if (type == TF_ATTR_FUNC) {
    std::unique_ptr<const TFE_Op*[]> funcs(new const TFE_Op*[num_values]);
    for (int i = 0; i < num_values; ++i) {
      tensorflow::Safe_PyObjectPtr py_value(PySequence_ITEM(py_list, i));
      // Allow:
      // (1) String function name, OR
      // (2) A Python object with a .name attribute
      //     (A crude test for being a
      //     tensorflow.python.framework.function._DefinedFunction)
      //     (which is what the various "defun" or "Defun" decorators do).
      // And in the future also allow an object that can encapsulate
      // the function name and its attribute values.
      tensorflow::StringPiece func_name;
      if (!ParseStringValue(key, py_value.get(), status, &func_name)) {
        PyObject* name_attr = PyObject_GetAttrString(py_value.get(), "name");
        if (name_attr == nullptr ||
            !ParseStringValue(key, name_attr, status, &func_name)) {
          TF_SetStatus(
              status, TF_INVALID_ARGUMENT,
              tensorflow::strings::StrCat(
                  "unable to set function value attribute from a ",
                  py_value.get()->ob_type->tp_name,
                  " object. If you think this is an error, please file an "
                  "issue at "
                  "https://github.com/tensorflow/tensorflow/issues/new")
                  .c_str());
          return false;
        }
      }
      funcs[i] = TFE_NewOp(ctx, func_name.data(), status);
      if (!status->status.ok()) return false;
    }
    TFE_OpSetAttrFunctionList(op, key, funcs.get(), num_values);
    if (!status->status.ok()) return false;
  } else {
    TF_SetStatus(status, TF_UNIMPLEMENTED,
                 tensorflow::strings::StrCat("Attr ", key,
                                             " has unhandled list type ", type)
                     .c_str());
    return false;
  }
#undef PARSE_LIST
  return true;
}