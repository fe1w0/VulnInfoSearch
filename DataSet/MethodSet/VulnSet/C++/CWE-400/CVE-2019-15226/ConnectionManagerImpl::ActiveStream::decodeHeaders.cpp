void ConnectionManagerImpl::ActiveStream::decodeHeaders(HeaderMapPtr&& headers, bool end_stream) {
  ScopeTrackerScopeState scope(this,
                               connection_manager_.read_callbacks_->connection().dispatcher());
  request_headers_ = std::move(headers);
  // For Admin thread, we don't use routeConfigProvider or SRDS route provider.
  if (dynamic_cast<Server::Admin*>(&connection_manager_.config_) == nullptr &&
      connection_manager_.config_.scopedRouteConfigProvider() != nullptr) {
    ASSERT(snapped_route_config_ == nullptr,
           "Route config already latched to the active stream when scoped RDS is enabled.");
    // We need to snap snapped_route_config_ here as it's used in mutateRequestHeaders later.
    snapScopedRouteConfig();
  }

  if (Http::Headers::get().MethodValues.Head ==
      request_headers_->Method()->value().getStringView()) {
    is_head_request_ = true;
  }
  ENVOY_STREAM_LOG(debug, "request headers complete (end_stream={}):\n{}", *this, end_stream,
                   *request_headers_);

  // We end the decode here only if the request is header only. If we convert the request to a
  // header only, the stream will be marked as done once a subsequent decodeData/decodeTrailers is
  // called with end_stream=true.
  maybeEndDecode(end_stream);

  // Drop new requests when overloaded as soon as we have decoded the headers.
  if (connection_manager_.overload_stop_accepting_requests_ref_ ==
      Server::OverloadActionState::Active) {
    // In this one special case, do not create the filter chain. If there is a risk of memory
    // overload it is more important to avoid unnecessary allocation than to create the filters.
    state_.created_filter_chain_ = true;
    connection_manager_.stats_.named_.downstream_rq_overload_close_.inc();
    sendLocalReply(Grpc::Common::hasGrpcContentType(*request_headers_),
                   Http::Code::ServiceUnavailable, "envoy overloaded", nullptr, is_head_request_,
                   absl::nullopt, StreamInfo::ResponseCodeDetails::get().Overload);
    return;
  }

  if (!connection_manager_.config_.proxy100Continue() && request_headers_->Expect() &&
      request_headers_->Expect()->value() == Headers::get().ExpectValues._100Continue.c_str()) {
    // Note in the case Envoy is handling 100-Continue complexity, it skips the filter chain
    // and sends the 100-Continue directly to the encoder.
    chargeStats(continueHeader());
    response_encoder_->encode100ContinueHeaders(continueHeader());
    // Remove the Expect header so it won't be handled again upstream.
    request_headers_->removeExpect();
  }

  connection_manager_.user_agent_.initializeFromHeaders(
      *request_headers_, connection_manager_.stats_.prefix_, connection_manager_.stats_.scope_);

  // Make sure we are getting a codec version we support.
  Protocol protocol = connection_manager_.codec_->protocol();
  if (protocol == Protocol::Http10) {
    // Assume this is HTTP/1.0. This is fine for HTTP/0.9 but this code will also affect any
    // requests with non-standard version numbers (0.9, 1.3), basically anything which is not
    // HTTP/1.1.
    //
    // The protocol may have shifted in the HTTP/1.0 case so reset it.
    stream_info_.protocol(protocol);
    if (!connection_manager_.config_.http1Settings().accept_http_10_) {
      // Send "Upgrade Required" if HTTP/1.0 support is not explicitly configured on.
      sendLocalReply(false, Code::UpgradeRequired, "", nullptr, is_head_request_, absl::nullopt,
                     StreamInfo::ResponseCodeDetails::get().LowVersion);
      return;
    } else {
      // HTTP/1.0 defaults to single-use connections. Make sure the connection
      // will be closed unless Keep-Alive is present.
      state_.saw_connection_close_ = true;
      if (request_headers_->Connection() &&
          absl::EqualsIgnoreCase(request_headers_->Connection()->value().getStringView(),
                                 Http::Headers::get().ConnectionValues.KeepAlive)) {
        state_.saw_connection_close_ = false;
      }
    }
  }

  if (!request_headers_->Host()) {
    if ((protocol == Protocol::Http10) &&
        !connection_manager_.config_.http1Settings().default_host_for_http_10_.empty()) {
      // Add a default host if configured to do so.
      request_headers_->insertHost().value(
          connection_manager_.config_.http1Settings().default_host_for_http_10_);
    } else {
      // Require host header. For HTTP/1.1 Host has already been translated to :authority.
      sendLocalReply(Grpc::Common::hasGrpcContentType(*request_headers_), Code::BadRequest, "",
                     nullptr, is_head_request_, absl::nullopt,
                     StreamInfo::ResponseCodeDetails::get().MissingHost);
      return;
    }
  }

  ASSERT(connection_manager_.config_.maxRequestHeadersKb() > 0);
  if (request_headers_->byteSize() > (connection_manager_.config_.maxRequestHeadersKb() * 1024)) {
    sendLocalReply(Grpc::Common::hasGrpcContentType(*request_headers_),
                   Code::RequestHeaderFieldsTooLarge, "", nullptr, is_head_request_, absl::nullopt,
                   StreamInfo::ResponseCodeDetails::get().RequestHeadersTooLarge);
    return;
  }

  // Currently we only support relative paths at the application layer. We expect the codec to have
  // broken the path into pieces if applicable. NOTE: Currently the HTTP/1.1 codec only does this
  // when the allow_absolute_url flag is enabled on the HCM.
  // https://tools.ietf.org/html/rfc7230#section-5.3 We also need to check for the existence of
  // :path because CONNECT does not have a path, and we don't support that currently.
  if (!request_headers_->Path() || request_headers_->Path()->value().getStringView().empty() ||
      request_headers_->Path()->value().getStringView()[0] != '/') {
    const bool has_path =
        request_headers_->Path() && !request_headers_->Path()->value().getStringView().empty();
    connection_manager_.stats_.named_.downstream_rq_non_relative_path_.inc();
    sendLocalReply(Grpc::Common::hasGrpcContentType(*request_headers_), Code::NotFound, "", nullptr,
                   is_head_request_, absl::nullopt,
                   has_path ? StreamInfo::ResponseCodeDetails::get().AbsolutePath
                            : StreamInfo::ResponseCodeDetails::get().MissingPath);
    return;
  }

  // Path sanitization should happen before any path access other than the above sanity check.
  if (!ConnectionManagerUtility::maybeNormalizePath(*request_headers_,
                                                    connection_manager_.config_)) {
    sendLocalReply(Grpc::Common::hasGrpcContentType(*request_headers_), Code::BadRequest, "",
                   nullptr, is_head_request_, absl::nullopt,
                   StreamInfo::ResponseCodeDetails::get().PathNormalizationFailed);
    return;
  }

  if (protocol == Protocol::Http11 && request_headers_->Connection() &&
      absl::EqualsIgnoreCase(request_headers_->Connection()->value().getStringView(),
                             Http::Headers::get().ConnectionValues.Close)) {
    state_.saw_connection_close_ = true;
  }
  // Note: Proxy-Connection is not a standard header, but is supported here
  // since it is supported by http-parser the underlying parser for http
  // requests.
  if (protocol != Protocol::Http2 && !state_.saw_connection_close_ &&
      request_headers_->ProxyConnection() &&
      absl::EqualsIgnoreCase(request_headers_->ProxyConnection()->value().getStringView(),
                             Http::Headers::get().ConnectionValues.Close)) {
    state_.saw_connection_close_ = true;
  }

  if (!state_.is_internally_created_) { // Only sanitize headers on first pass.
    // Modify the downstream remote address depending on configuration and headers.
    stream_info_.setDownstreamRemoteAddress(ConnectionManagerUtility::mutateRequestHeaders(
        *request_headers_, connection_manager_.read_callbacks_->connection(),
        connection_manager_.config_, *snapped_route_config_, connection_manager_.random_generator_,
        connection_manager_.local_info_));
  }
  ASSERT(stream_info_.downstreamRemoteAddress() != nullptr);

  ASSERT(!cached_route_);
  refreshCachedRoute();

  if (!state_.is_internally_created_) { // Only mutate tracing headers on first pass.
    ConnectionManagerUtility::mutateTracingRequestHeader(
        *request_headers_, connection_manager_.runtime_, connection_manager_.config_,
        cached_route_.value().get());
  }

  const bool upgrade_rejected = createFilterChain() == false;

  // TODO if there are no filters when starting a filter iteration, the connection manager
  // should return 404. The current returns no response if there is no router filter.
  if (protocol == Protocol::Http11 && hasCachedRoute()) {
    if (upgrade_rejected) {
      // Do not allow upgrades if the route does not support it.
      connection_manager_.stats_.named_.downstream_rq_ws_on_non_ws_route_.inc();
      sendLocalReply(Grpc::Common::hasGrpcContentType(*request_headers_), Code::Forbidden, "",
                     nullptr, is_head_request_, absl::nullopt,
                     StreamInfo::ResponseCodeDetails::get().UpgradeFailed);
      return;
    }
    // Allow non websocket requests to go through websocket enabled routes.
  }

  if (hasCachedRoute()) {
    const Router::RouteEntry* route_entry = cached_route_.value()->routeEntry();
    if (route_entry != nullptr && route_entry->idleTimeout()) {
      idle_timeout_ms_ = route_entry->idleTimeout().value();
      if (idle_timeout_ms_.count()) {
        // If we have a route-level idle timeout but no global stream idle timeout, create a timer.
        if (stream_idle_timer_ == nullptr) {
          stream_idle_timer_ =
              connection_manager_.read_callbacks_->connection().dispatcher().createTimer(
                  [this]() -> void { onIdleTimeout(); });
        }
      } else if (stream_idle_timer_ != nullptr) {
        // If we had a global stream idle timeout but the route-level idle timeout is set to zero
        // (to override), we disable the idle timer.
        stream_idle_timer_->disableTimer();
        stream_idle_timer_ = nullptr;
      }
    }
  }

  // Check if tracing is enabled at all.
  if (connection_manager_.config_.tracingConfig()) {
    traceRequest();
  }

  decodeHeaders(nullptr, *request_headers_, end_stream);

  // Reset it here for both global and overridden cases.
  resetIdleTimer();
}