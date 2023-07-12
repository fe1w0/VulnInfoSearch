TEST(BasicFlatBufferModel, TestHandleMalformedModel) {
  const auto model_paths = {
      // These models use the same tensor as both input and ouput of a node
      "tensorflow/lite/testdata/add_shared_tensors.bin",
  };

  for (const auto& model_path : model_paths) {
    std::unique_ptr<tflite::FlatBufferModel> model =
        FlatBufferModel::BuildFromFile(model_path);
    ASSERT_NE(model, nullptr);

    tflite::ops::builtin::BuiltinOpResolver resolver;
    InterpreterBuilder builder(*model, resolver);
    std::unique_ptr<Interpreter> interpreter;
    ASSERT_EQ(builder(&interpreter), kTfLiteOk);
    ASSERT_NE(interpreter, nullptr);
    ASSERT_NE(interpreter->AllocateTensors(), kTfLiteOk);
  }
}