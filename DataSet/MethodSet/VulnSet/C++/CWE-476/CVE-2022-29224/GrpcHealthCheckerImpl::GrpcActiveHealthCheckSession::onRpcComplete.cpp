void GrpcHealthCheckerImpl::GrpcActiveHealthCheckSession::onRpcComplete(
    Grpc::Status::GrpcStatus grpc_status, const std::string& grpc_message, bool end_stream) {
  logHealthCheckStatus(grpc_status, grpc_message);
  if (isHealthCheckSucceeded(grpc_status)) {
    handleSuccess(false);
  } else {
    handleFailure(envoy::data::core::v3::ACTIVE);
  }

  // Read the value as we may call resetState() and clear it.
  const bool goaway = received_no_error_goaway_;

  // |end_stream| will be false if we decided to stop healthcheck before HTTP stream has ended -
  // invalid gRPC payload, unexpected message stream or wrong content-type.
  if (end_stream) {
    resetState();
  } else {
    // resetState() will be called by onResetStream().
    expect_reset_ = true;
    request_encoder_->getStream().resetStream(Http::StreamResetReason::LocalReset);
  }

  if (!parent_.reuse_connection_ || goaway) {
    client_->close();
  }
}