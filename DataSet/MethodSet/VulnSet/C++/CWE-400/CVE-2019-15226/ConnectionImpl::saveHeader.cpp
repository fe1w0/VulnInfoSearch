int ConnectionImpl::saveHeader(const nghttp2_frame* frame, HeaderString&& name,
                               HeaderString&& value) {
  StreamImpl* stream = getStream(frame->hd.stream_id);
  if (!stream) {
    // We have seen 1 or 2 crashes where we get a headers callback but there is no associated
    // stream data. I honestly am not sure how this can happen. However, from reading the nghttp2
    // code it looks possible that inflate_header_block() can safely inflate headers for an already
    // closed stream, but will still call the headers callback. Since that seems possible, we should
    // ignore this case here.
    // TODO(mattklein123): Figure out a test case that can hit this.
    stats_.headers_cb_no_stream_.inc();
    return 0;
  }

  stream->saveHeader(std::move(name), std::move(value));
  if (stream->headers_->byteSize() > max_request_headers_kb_ * 1024) {
    // This will cause the library to reset/close the stream.
    stats_.header_overflow_.inc();
    return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
  } else {
    return 0;
  }
}