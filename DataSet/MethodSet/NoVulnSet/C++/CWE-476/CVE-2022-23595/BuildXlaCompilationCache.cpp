Status BuildXlaCompilationCache(DeviceBase* device, FunctionLibraryRuntime* flr,
                                const XlaPlatformInfo& platform_info,
                                XlaCompilationCache** cache) {
  if (platform_info.xla_device_metadata()) {
    *cache = new XlaCompilationCache(
        platform_info.xla_device_metadata()->client(),
        platform_info.xla_device_metadata()->jit_device_type());
    return Status::OK();
  }

  auto platform =
      se::MultiPlatformManager::PlatformWithId(platform_info.platform_id());
  if (!platform.ok()) {
    return platform.status();
  }

  StatusOr<xla::Compiler*> compiler_for_platform =
      xla::Compiler::GetForPlatform(platform.ValueOrDie());
  if (!compiler_for_platform.ok()) {
    // In some rare cases (usually in unit tests with very small clusters) we
    // may end up transforming an XLA cluster with at least one GPU operation
    // (which would normally force the cluster to be compiled using XLA:GPU)
    // into an XLA cluster with no GPU operations (i.e. containing only CPU
    // operations).  Such a cluster can fail compilation (in way that
    // MarkForCompilation could not have detected) if the CPU JIT is not linked
    // in.
    //
    // So bail out of _XlaCompile in this case, and let the executor handle the
    // situation for us.
    const Status& status = compiler_for_platform.status();
    if (status.code() == error::NOT_FOUND) {
      return errors::Unimplemented("Could not find compiler for platform ",
                                   platform.ValueOrDie()->Name(), ": ",
                                   status.ToString());
    }
  }

  xla::LocalClientOptions client_options;
  client_options.set_platform(platform.ValueOrDie());
  client_options.set_intra_op_parallelism_threads(
      device->tensorflow_cpu_worker_threads()->num_threads);

  if (flr->config_proto()) {
    string allowed_gpus =
        flr->config_proto()->gpu_options().visible_device_list();
    TF_ASSIGN_OR_RETURN(absl::optional<std::set<int>> gpu_ids,
                        ParseVisibleDeviceList(allowed_gpus));
    client_options.set_allowed_devices(gpu_ids);
  }

  auto client = xla::ClientLibrary::GetOrCreateLocalClient(client_options);
  if (!client.ok()) {
    return client.status();
  }
  const XlaOpRegistry::DeviceRegistration* registration;
  if (!XlaOpRegistry::GetCompilationDevice(platform_info.device_type().type(),
                                           &registration)) {
    return errors::InvalidArgument("No JIT device registered for ",
                                   platform_info.device_type().type());
  }
  *cache = new XlaCompilationCache(
      client.ValueOrDie(), DeviceType(registration->compilation_device_name));
  return Status::OK();
}