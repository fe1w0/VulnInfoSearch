Status PyArrayDescr_to_TF_DataType(PyArray_Descr* descr,
                                   TF_DataType* out_tf_datatype) {
  PyObject* key;
  PyObject* value;
  Py_ssize_t pos = 0;
  if (PyDict_Next(descr->fields, &pos, &key, &value)) {
    // In Python 3, the keys of numpy custom struct types are unicode, unlike
    // Python 2, where the keys are bytes.
    const char* key_string =
        PyBytes_Check(key) ? PyBytes_AsString(key)
                           : PyBytes_AsString(PyUnicode_AsASCIIString(key));
    if (!key_string) {
      return errors::Internal("Corrupt numpy type descriptor");
    }
    tensorflow::string key = key_string;
    // The typenames here should match the field names in the custom struct
    // types constructed in test_util.py.
    // TODO(mrry,keveman): Investigate Numpy type registration to replace this
    // hard-coding of names.
    if (key == "quint8") {
      *out_tf_datatype = TF_QUINT8;
    } else if (key == "qint8") {
      *out_tf_datatype = TF_QINT8;
    } else if (key == "qint16") {
      *out_tf_datatype = TF_QINT16;
    } else if (key == "quint16") {
      *out_tf_datatype = TF_QUINT16;
    } else if (key == "qint32") {
      *out_tf_datatype = TF_QINT32;
    } else if (key == "resource") {
      *out_tf_datatype = TF_RESOURCE;
    } else {
      return errors::Internal("Unsupported numpy data type");
    }
    return Status::OK();
  }
  return errors::Internal("Unsupported numpy data type");
}