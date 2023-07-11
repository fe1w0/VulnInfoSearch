int ConnectionImpl::onFrameReceived(const nghttp2_frame* frame) {
  ENVOY_CONN_LOG(trace, "recv frame type={}", connection_, static_cast<uint64_t>(frame->hd.type));

  // onFrameReceived() is called with a complete HEADERS frame assembled from all the HEADERS
  // and CONTINUATION frames, but we track them separately: HEADERS frames in onBeginHeaders()
  // and CONTINUATION frames in onBeforeFrameReceived().
  ASSERT(frame->hd.type != NGHTTP2_CONTINUATION);

  if (frame->hd.type == NGHTTP2_DATA) {
    if (!trackInboundFrames(&frame->hd, frame->data.padlen)) {
      return NGHTTP2_ERR_FLOODED;
    }
  }

  // Only raise GOAWAY once, since we don't currently expose stream information. Shutdown
  // notifications are the same as a normal GOAWAY.
  if (frame->hd.type == NGHTTP2_GOAWAY && !raised_goaway_) {
    ASSERT(frame->hd.stream_id == 0);
    raised_goaway_ = true;
    callbacks().onGoAway();
    return 0;
  }

  StreamImpl* stream = getStream(frame->hd.stream_id);
  if (!stream) {
    return 0;
  }

  switch (frame->hd.type) {
  case NGHTTP2_HEADERS: {
    stream->remote_end_stream_ = frame->hd.flags & NGHTTP2_FLAG_END_STREAM;
    if (!stream->cookies_.empty()) {
      HeaderString key(Headers::get().Cookie);
      stream->headers_->addViaMove(std::move(key), std::move(stream->cookies_));
    }

    switch (frame->headers.cat) {
    case NGHTTP2_HCAT_RESPONSE: {
      if (CodeUtility::is1xx(Http::Utility::getResponseStatus(*stream->headers_))) {
        stream->waiting_for_non_informational_headers_ = true;
      }

      if (stream->headers_->Status()->value() == "100") {
        ASSERT(!stream->remote_end_stream_);
        stream->decoder_->decode100ContinueHeaders(std::move(stream->headers_));
      } else {
        stream->decodeHeaders();
      }
      break;
    }

    case NGHTTP2_HCAT_REQUEST: {
      stream->decodeHeaders();
      break;
    }

    case NGHTTP2_HCAT_HEADERS: {
      // It's possible that we are waiting to send a deferred reset, so only raise headers/trailers
      // if local is not complete.
      if (!stream->deferred_reset_) {
        if (!stream->waiting_for_non_informational_headers_) {
          if (!stream->remote_end_stream_) {
            // This indicates we have received more headers frames than Envoy
            // supports. Even if this is valid HTTP (something like 103 early hints) fail here
            // rather than trying to push unexpected headers through the Envoy pipeline as that
            // will likely result in Envoy crashing.
            // It would be cleaner to reset the stream rather than reset the/ entire connection but
            // it's also slightly more dangerous so currently we err on the side of safety.
            stats_.too_many_header_frames_.inc();
            throw CodecProtocolException("Unexpected 'trailers' with no end stream.");
          } else {
            stream->decoder_->decodeTrailers(std::move(stream->headers_));
          }
        } else {
          ASSERT(!nghttp2_session_check_server_session(session_));
          stream->waiting_for_non_informational_headers_ = false;

          // Even if we have :status 100 in the client case in a response, when
          // we received a 1xx to start out with, nghttp2 message checking
          // guarantees proper flow here.
          stream->decodeHeaders();
        }
      }

      break;
    }

    default:
      // We do not currently support push.
      NOT_IMPLEMENTED_GCOVR_EXCL_LINE;
    }

    stream->headers_.reset();
    break;
  }
  case NGHTTP2_DATA: {
    stream->remote_end_stream_ = frame->hd.flags & NGHTTP2_FLAG_END_STREAM;

    // It's possible that we are waiting to send a deferred reset, so only raise data if local
    // is not complete.
    if (!stream->deferred_reset_) {
      stream->decoder_->decodeData(stream->pending_recv_data_, stream->remote_end_stream_);
    }

    stream->pending_recv_data_.drain(stream->pending_recv_data_.length());
    break;
  }
  case NGHTTP2_RST_STREAM: {
    ENVOY_CONN_LOG(trace, "remote reset: {}", connection_, frame->rst_stream.error_code);
    stats_.rx_reset_.inc();
    break;
  }
  }

  return 0;
}