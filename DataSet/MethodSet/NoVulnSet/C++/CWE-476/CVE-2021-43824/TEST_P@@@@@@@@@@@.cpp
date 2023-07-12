TEST_P(RemoteJwksIntegrationTest, WithGoodTokenAsyncFetch) {
  on_server_init_function_ = [this]() { waitForJwksResponse("200", PublicKey); };
  initializeAsyncFetchFilter(false);

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"Authorization", "Bearer " + std::string(GoodToken)},
  });

  waitForNextUpstreamRequest();

  const auto payload_entry =
      upstream_request_->headers().get(Http::LowerCaseString("sec-istio-auth-userinfo"));
  EXPECT_FALSE(payload_entry.empty());
  EXPECT_EQ(payload_entry[0]->value().getStringView(), ExpectedPayloadValue);
  // Verify the token is removed.
  EXPECT_TRUE(upstream_request_->headers().get(Http::CustomHeaders::get().Authorization).empty());

  upstream_request_->encodeHeaders(Http::TestResponseHeaderMapImpl{{":status", "200"}}, true);

  ASSERT_TRUE(response->waitForEndStream());
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("200", response->headers().getStatusValue());

  cleanup();
}