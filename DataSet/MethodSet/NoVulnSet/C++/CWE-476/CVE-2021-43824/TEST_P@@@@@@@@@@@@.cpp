TEST_P(RemoteJwksIntegrationTest, WithGoodTokenAsyncFetchFast) {
  on_server_init_function_ = [this]() { waitForJwksResponse("200", PublicKey); };
  initializeAsyncFetchFilter(true);

  // This test is only expecting one jwks fetch, but there is a race condition in the test:
  // In fast fetch mode, the listener is activated without waiting for jwks fetch to be
  // completed. When the first request comes at the worker thread, jwks fetch could be at
  // any state at the main thread. If its result is not saved into jwks thread local slot,
  // the first request will trigger a second jwks fetch, this is not expected, test will fail.
  // To avoid such race condition, before making the first request, wait for the first
  // fetch stats to be updated.
  test_server_->waitForCounterGe("http.config_test.jwt_authn.jwks_fetch_success", 1);

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