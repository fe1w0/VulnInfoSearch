Status KernelAndDeviceOp::Run(
    ScopedStepContainer* step_container, const EagerKernelArgs& inputs,
    std::vector<EagerKernelRet>* outputs,
    CancellationManager* cancellation_manager,
    const absl::optional<EagerRemoteFunctionParams>& remote_func_params) {
  OpKernelContext::Params params;
  params.device = device_;
  params.frame_iter = FrameAndIter(0, 0);
  params.inputs = inputs.GetTensorValues();
  params.op_kernel = kernel_.get();
  params.resource_manager = device_->resource_manager();
  params.input_alloc_attrs = &input_alloc_attrs_;
  params.output_attr_array = output_alloc_attrs_.data();
  params.function_library = flr_;
  params.slice_reader_cache = &slice_reader_cache_;
  params.rendezvous = rendezvous_;
  OpExecutionState* op_execution_state = nullptr;

  CancellationManager default_cancellation_manager;
  if (cancellation_manager) {
    params.cancellation_manager = cancellation_manager;
  } else if (kernel_->is_deferred()) {
    op_execution_state = new OpExecutionState;
    params.cancellation_manager = &op_execution_state->cancellation_manager;
    params.inc_num_deferred_ops_function = [op_execution_state]() {
      op_execution_state->Ref();
    };
    params.dec_num_deferred_ops_function = [op_execution_state]() {
      op_execution_state->Unref();
    };
  } else {
    params.cancellation_manager = &default_cancellation_manager;
  }

  params.log_memory = log_memory_;

  params.runner = get_runner();

  params.step_container =
      step_container == nullptr ? &step_container_ : step_container;
  auto step_container_cleanup = gtl::MakeCleanup([step_container, this] {
    if (step_container == nullptr) {
      this->step_container_.CleanUp();
    }
  });

  params.collective_executor =
      collective_executor_ ? collective_executor_->get() : nullptr;

  OpKernelContext context(&params);

  {
    port::ScopedFlushDenormal flush;
    port::ScopedSetRound round(FE_TONEAREST);
    // 'AnnotatedTraceMe' will trace both scheduling time on host and execution
    // time on device of the OpKernel.
    profiler::AnnotatedTraceMe activity(
        [&] { return kernel_->TraceString(context, /*verbose=*/false); },
        profiler::TraceMeLevel::kInfo);
    device_->Compute(kernel_.get(), &context);
  }

  // Clean up execution op_execution_state if deferred ops aren't running.
  if (op_execution_state != nullptr) {
    op_execution_state->Unref();
  }

  if (!context.status().ok()) return context.status();

  if (outputs != nullptr) {
    outputs->clear();
    for (int i = 0; i < context.num_outputs(); ++i) {
      outputs->push_back(Tensor(*context.mutable_output(i)));
    }
  }
  return Status::OK();
}