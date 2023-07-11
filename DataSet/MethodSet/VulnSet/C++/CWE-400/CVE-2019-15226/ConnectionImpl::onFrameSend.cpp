int ConnectionImpl::onFrameSend(const nghttp2_frame* frame) {
  // The nghttp2 library does not cleanly give us a way to determine whether we received invalid
  // data from our peer. Sometimes it raises the invalid frame callback, and sometimes it does not.
  // In all cases however it will attempt to send a GOAWAY frame with an error status. If we see
  // an outgoing frame of this type, we will return an error code so that we can abort execution.
  ENVOY_CONN_LOG(trace, "sent frame type={}", connection_, static_cast<uint64_t>(frame->hd.type));
  switch (frame->hd.type) {
  case NGHTTP2_GOAWAY: {
    ENVOY_CONN_LOG(debug, "sent goaway code={}", connection_, frame->goaway.error_code);
    if (frame->goaway.error_code != NGHTTP2_NO_ERROR) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    break;
  }

  case NGHTTP2_RST_STREAM: {
    ENVOY_CONN_LOG(debug, "sent reset code={}", connection_, frame->rst_stream.error_code);
    stats_.tx_reset_.inc();
    break;
  }

  case NGHTTP2_HEADERS:
  case NGHTTP2_DATA: {
    StreamImpl* stream = getStream(frame->hd.stream_id);
    stream->local_end_stream_sent_ = frame->hd.flags & NGHTTP2_FLAG_END_STREAM;
    break;
  }
  }

  return 0;
}