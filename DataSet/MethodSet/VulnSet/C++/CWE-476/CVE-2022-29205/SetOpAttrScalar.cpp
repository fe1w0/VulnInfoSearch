bool SetOpAttrScalar(TFE_Context* ctx, TFE_Op* op, const char* key,
                     PyObject* py_value, TF_AttrType type,
                     tensorflow::gtl::FlatMap<string, int64_t>* attr_list_sizes,
                     TF_Status* status) {
  if (type == TF_ATTR_STRING) {
    tensorflow::StringPiece value;
    if (!ParseStringValue(key, py_value, status, &value)) return false;
    TFE_OpSetAttrString(op, key, value.data(), value.size());
  } else if (type == TF_ATTR_INT) {
    int64_t value;
    if (!ParseInt64Value(key, py_value, status, &value)) return false;
    TFE_OpSetAttrInt(op, key, value);
    // attr_list_sizes is set for all int attributes (since at this point we are
    // not aware if that attribute might be used to calculate the size of an
    // output list or not).
    if (attr_list_sizes != nullptr) (*attr_list_sizes)[key] = value;
  } else if (type == TF_ATTR_FLOAT) {
    float value;
    if (!ParseFloatValue(key, py_value, status, &value)) return false;
    TFE_OpSetAttrFloat(op, key, value);
  } else if (type == TF_ATTR_BOOL) {
    unsigned char value;
    if (!ParseBoolValue(key, py_value, status, &value)) return false;
    TFE_OpSetAttrBool(op, key, value);
  } else if (type == TF_ATTR_TYPE) {
    int value;
    if (!ParseTypeValue(key, py_value, status, &value)) return false;
    TFE_OpSetAttrType(op, key, static_cast<TF_DataType>(value));
  } else if (type == TF_ATTR_SHAPE) {
    if (py_value == Py_None) {
      TFE_OpSetAttrShape(op, key, nullptr, -1, status);
    } else {
      if (!PySequence_Check(py_value)) {
        TF_SetStatus(status, TF_INVALID_ARGUMENT,
                     tensorflow::strings::StrCat(
                         "Expecting None or sequence value for attr", key,
                         ", got ", py_value->ob_type->tp_name)
                         .c_str());
        return false;
      }
      const auto num_dims = TensorShapeNumDims(py_value);
      if (num_dims == -1) {
        TFE_OpSetAttrShape(op, key, nullptr, -1, status);
        return true;
      }
      std::unique_ptr<int64_t[]> dims(new int64_t[num_dims]);
      for (int i = 0; i < num_dims; ++i) {
        tensorflow::Safe_PyObjectPtr inner_py_value(
            PySequence_ITEM(py_value, i));
        if (inner_py_value.get() == Py_None) {
          dims[i] = -1;
        } else if (!ParseDimensionValue(key, inner_py_value.get(), status,
                                        &dims[i])) {
          return false;
        }
      }
      TFE_OpSetAttrShape(op, key, dims.get(), num_dims, status);
    }
    if (!status->status.ok()) return false;
  } else if (type == TF_ATTR_FUNC) {
    // Allow:
    // (1) String function name, OR
    // (2) A Python object with a .name attribute
    //     (A crude test for being a
    //     tensorflow.python.framework.function._DefinedFunction)
    //     (which is what the various "defun" or "Defun" decorators do).
    // And in the future also allow an object that can encapsulate
    // the function name and its attribute values.
    tensorflow::StringPiece func_name;
    if (!ParseStringValue(key, py_value, status, &func_name)) {
      PyObject* name_attr = PyObject_GetAttrString(py_value, "name");
      if (name_attr == nullptr ||
          !ParseStringValue(key, name_attr, status, &func_name)) {
        TF_SetStatus(
            status, TF_INVALID_ARGUMENT,
            tensorflow::strings::StrCat(
                "unable to set function value attribute from a ",
                py_value->ob_type->tp_name,
                " object. If you think this is an error, please file an issue "
                "at https://github.com/tensorflow/tensorflow/issues/new")
                .c_str());
        return false;
      }
    }
    TF_SetStatus(status, TF_OK, "");
    TFE_OpSetAttrFunctionName(op, key, func_name.data(), func_name.size());
  } else {
    TF_SetStatus(
        status, TF_UNIMPLEMENTED,
        tensorflow::strings::StrCat("Attr ", key, " has unhandled type ", type)
            .c_str());
    return false;
  }
  return true;
}