void ConnectionImpl::onHeaderValue(const char* data, size_t length) {
  if (header_parsing_state_ == HeaderParsingState::Done) {
    // Ignore trailers.
    return;
  }

  const absl::string_view header_value = absl::string_view(data, length);

  if (strict_header_validation_) {
    if (!Http::HeaderUtility::headerIsValid(header_value)) {
      ENVOY_CONN_LOG(debug, "invalid header value: {}", connection_, header_value);
      error_code_ = Http::Code::BadRequest;
      sendProtocolError();
      throw CodecProtocolException("http/1.1 protocol error: header value contains invalid chars");
    }
  } else if (header_value.find('\0') != absl::string_view::npos) {
    // http-parser should filter for this
    // (https://tools.ietf.org/html/rfc7230#section-3.2.6), but it doesn't today. HeaderStrings
    // have an invariant that they must not contain embedded zero characters
    // (NUL, ASCII 0x0).
    throw CodecProtocolException("http/1.1 protocol error: header value contains NUL");
  }

  header_parsing_state_ = HeaderParsingState::Value;
  current_header_value_.append(data, length);

  const uint32_t total =
      current_header_field_.size() + current_header_value_.size() + current_header_map_->byteSize();
  if (total > (max_request_headers_kb_ * 1024)) {
    error_code_ = Http::Code::RequestHeaderFieldsTooLarge;
    sendProtocolError();
    throw CodecProtocolException("headers size exceeds limit");
  }
}