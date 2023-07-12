TEST_P(LocalJwksIntegrationTest, ConnectRequestWithRegExMatch) {
  config_helper_.prependFilter(getAuthFilterConfig(ExampleConfigWithRegEx, true));
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto encoder_decoder = codec_client_->startRequest(Http::TestRequestHeaderMapImpl{
      {":method", "CONNECT"},
      {":authority", "host.com:80"},
      {"authorization", "Bearer " + std::string(GoodToken)},
  });
  request_encoder_ = &encoder_decoder.first;
  auto response = std::move(encoder_decoder.second);

  if (downstreamProtocol() == Http::CodecType::HTTP1) {
    // Because CONNECT requests for HTTP/1 do not include a path, they will fail
    // to find a route match and return a 404.
    ASSERT_TRUE(response->waitForEndStream());
    ASSERT_TRUE(response->complete());
    EXPECT_EQ("404", response->headers().getStatusValue());
  } else {
    ASSERT_TRUE(response->waitForReset());
    ASSERT_TRUE(codec_client_->waitForDisconnect());
  }
}