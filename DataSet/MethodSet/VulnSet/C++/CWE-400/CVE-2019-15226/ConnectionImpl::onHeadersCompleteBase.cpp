int ConnectionImpl::onHeadersCompleteBase() {
  ENVOY_CONN_LOG(trace, "headers complete", connection_);
  completeLastHeader();
  if (!(parser_.http_major == 1 && parser_.http_minor == 1)) {
    // This is not necessarily true, but it's good enough since higher layers only care if this is
    // HTTP/1.1 or not.
    protocol_ = Protocol::Http10;
  }
  if (Utility::isUpgrade(*current_header_map_)) {
    // Ignore h2c upgrade requests until we support them.
    // See https://github.com/envoyproxy/envoy/issues/7161 for details.
    if (current_header_map_->Upgrade() &&
        absl::EqualsIgnoreCase(current_header_map_->Upgrade()->value().getStringView(),
                               Http::Headers::get().UpgradeValues.H2c)) {
      ENVOY_CONN_LOG(trace, "removing unsupported h2c upgrade headers.", connection_);
      current_header_map_->removeUpgrade();
      if (current_header_map_->Connection()) {
        const auto& tokens_to_remove = caseUnorderdSetContainingUpgradeAndHttp2Settings();
        std::string new_value = StringUtil::removeTokens(
            current_header_map_->Connection()->value().getStringView(), ",", tokens_to_remove, ",");
        if (new_value.empty()) {
          current_header_map_->removeConnection();
        } else {
          current_header_map_->Connection()->value(new_value);
        }
      }
      current_header_map_->remove(Headers::get().Http2Settings);
    } else {
      ENVOY_CONN_LOG(trace, "codec entering upgrade mode.", connection_);
      handling_upgrade_ = true;
    }
  }

  int rc = onHeadersComplete(std::move(current_header_map_));
  current_header_map_.reset();
  header_parsing_state_ = HeaderParsingState::Done;

  // Returning 2 informs http_parser to not expect a body or further data on this connection.
  return handling_upgrade_ ? 2 : rc;
}