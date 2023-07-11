TfLiteStatus Subgraph::Invoke() {
  if (!consistent_) {
    ReportError("Invoke called on model that is not consistent.");
    return kTfLiteError;
  }

  TfLiteStatus status = kTfLiteOk;
  if (state_ == kStateUninvokable) {
    ReportError("Invoke called on model that is not ready.");
    return kTfLiteError;
  } else if (memory_planner_ && !memory_planner_->HasNonPersistentMemory()) {
    ReportError("Non-persistent memory is not available.");
    return kTfLiteError;
  }

  // This is only needed for UseNNAPI(true);
  if (should_apply_nnapi_delegate_ && !applied_nnapi_delegate_) {
    TF_LITE_ENSURE_OK(&context_, ModifyGraphWithDelegate(NnApiDelegate()));
    // only need to modify the graph once upon the first invocation.
    applied_nnapi_delegate_ = true;
  }

  // Invocations are always done in node order.
  // Note that calling Invoke repeatedly will cause the original memory plan to
  // be reused, unless either ResizeInputTensor() or AllocateTensors() has been
  // called.
  for (int execution_plan_index = 0;
       execution_plan_index < execution_plan_.size(); execution_plan_index++) {
    if (execution_plan_index == next_execution_plan_index_to_prepare_) {
      TF_LITE_ENSURE_STATUS(PrepareOpsAndTensors());
      TF_LITE_ENSURE(&context_, next_execution_plan_index_to_prepare_ >=
                                    execution_plan_index);
    }
    int node_index = execution_plan_[execution_plan_index];
    TfLiteNode& node = nodes_and_registration_[node_index].first;
    const TfLiteRegistration& registration =
        nodes_and_registration_[node_index].second;

    const char* op_name = nullptr;
    if (profiler_) op_name = GetTFLiteOpName(registration);
    TFLITE_SCOPED_TAGGED_OPERATOR_PROFILE(profiler_.get(), op_name, node_index);

    // TODO(ycling): This is an extra loop through inputs to check if the data
    // need to be copied from Delegate buffer to raw memory, which is often not
    // needed. We may want to cache this in prepare to know if this needs to be
    // done for a node or not.
    for (int i = 0; i < node.inputs->size; ++i) {
      int tensor_index = node.inputs->data[i];
      if (tensor_index == kTfLiteOptionalTensor) {
        continue;
      }
      TfLiteTensor* tensor = &tensors_[tensor_index];
      if (tensor->delegate && tensor->delegate != node.delegate &&
          tensor->data_is_stale) {
        TF_LITE_ENSURE_STATUS(EnsureTensorDataIsReadable(tensor_index));
      }
    }

    if (check_cancelled_func_ != nullptr &&
        check_cancelled_func_(cancellation_data_)) {
      ReportError("Client requested cancel during Invoke()");
      return kTfLiteError;
    }

    EnsureTensorsVectorCapacity();
    tensor_resized_since_op_invoke_ = false;
    if (OpInvoke(registration, &node) != kTfLiteOk) {
      return ReportOpError(&context_, node, registration, node_index,
                           "failed to invoke");
    }

    // Force execution prep for downstream ops if the latest op triggered the
    // resize of a dynamic tensor.
    if (tensor_resized_since_op_invoke_ &&
        HasDynamicTensor(context_, node.outputs)) {
      next_execution_plan_index_to_prepare_ = execution_plan_index + 1;

      // This happens when an intermediate dynamic tensor is resized.
      // We don't have to prepare all the ops, but we need to recompute
      // the allocation plan.
      if (next_execution_plan_index_to_plan_allocation_ >
          next_execution_plan_index_to_prepare_) {
        next_execution_plan_index_to_plan_allocation_ =
            next_execution_plan_index_to_prepare_;
        if (memory_planner_) {
          TF_LITE_ENSURE_STATUS(memory_planner_->ResetAllocationsAfter(
              next_execution_plan_index_to_plan_allocation_ - 1));
        }
      }
    }
  }

  return status;
}