TEST_P(RemoteJwksIntegrationTest, WithFailedJwksAsyncFetch) {
  on_server_init_function_ = [this]() { waitForJwksResponse("500", ""); };
  initializeAsyncFetchFilter(false);

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"Authorization", "Bearer " + std::string(GoodToken)},
  });

  ASSERT_TRUE(response->waitForEndStream());
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("401", response->headers().getStatusValue());

  cleanup();
}