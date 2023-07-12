TEST_P(RemoteJwksIntegrationTest, FetchFailedJwks) {
  initializeFilter(/*add_cluster=*/true);

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"Authorization", "Bearer " + std::string(GoodToken)},
  });

  // Fails the jwks fetching.
  waitForJwksResponse("500", "");

  ASSERT_TRUE(response->waitForEndStream());
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("401", response->headers().getStatusValue());
  EXPECT_EQ(
      "Bearer realm=\"http://host/\", error=\"invalid_token\"",
      response->headers().get(Http::Headers::get().WWWAuthenticate)[0]->value().getStringView());

  cleanup();
}