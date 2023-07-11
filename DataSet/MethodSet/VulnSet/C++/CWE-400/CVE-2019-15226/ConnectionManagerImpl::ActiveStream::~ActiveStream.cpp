ConnectionManagerImpl::ActiveStream::~ActiveStream() {
  stream_info_.onRequestComplete();

  // A downstream disconnect can be identified for HTTP requests when the upstream returns with a 0
  // response code and when no other response flags are set.
  if (!stream_info_.hasAnyResponseFlag() && !stream_info_.responseCode()) {
    stream_info_.setResponseFlag(StreamInfo::ResponseFlag::DownstreamConnectionTermination);
  }

  connection_manager_.stats_.named_.downstream_rq_active_.dec();
  for (const AccessLog::InstanceSharedPtr& access_log : connection_manager_.config_.accessLogs()) {
    access_log->log(request_headers_.get(), response_headers_.get(), response_trailers_.get(),
                    stream_info_);
  }
  for (const auto& log_handler : access_log_handlers_) {
    log_handler->log(request_headers_.get(), response_headers_.get(), response_trailers_.get(),
                     stream_info_);
  }

  if (stream_info_.healthCheck()) {
    connection_manager_.config_.tracingStats().health_check_.inc();
  }

  if (active_span_) {
    Tracing::HttpTracerUtility::finalizeDownstreamSpan(
        *active_span_, request_headers_.get(), response_headers_.get(), response_trailers_.get(),
        stream_info_, *this);
  }
  if (state_.successful_upgrade_) {
    connection_manager_.stats_.named_.downstream_cx_upgrades_active_.dec();
  }

  ASSERT(state_.filter_call_state_ == 0);
}