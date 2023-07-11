void GrpcHealthCheckerImpl::GrpcActiveHealthCheckSession::onGoAway(
    Http::GoAwayErrorCode error_code) {
  ENVOY_CONN_LOG(debug, "connection going away health_flags={}", *client_,
                 HostUtility::healthFlagsToString(*host_));
  // If we have an active health check probe and receive a GOAWAY indicating
  // graceful shutdown, allow the probe to complete before closing the connection.
  // The connection will be closed when the active check completes or another
  // terminal condition occurs, such as a timeout or stream reset.
  if (request_encoder_ && error_code == Http::GoAwayErrorCode::NoError) {
    received_no_error_goaway_ = true;
    return;
  }

  // Even if we have active health check probe, fail it on GOAWAY and schedule new one.
  if (request_encoder_) {
    handleFailure(envoy::data::core::v3::NETWORK);
    expect_reset_ = true;
    request_encoder_->getStream().resetStream(Http::StreamResetReason::LocalReset);
  }
  client_->close();
}