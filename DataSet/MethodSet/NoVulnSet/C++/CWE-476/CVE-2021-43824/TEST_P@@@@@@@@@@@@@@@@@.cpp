TEST_P(PerRouteIntegrationTest, PerRouteConfigOK) {
  // A config with a requirement_map
  const std::string filter_conf = R"(
  providers:
    example_provider:
      issuer: https://example.com
      audiences:
      - example_service
  requirement_map:
    abc:
      provider_name: "example_provider"
)";

  // Per-route config with correct requirement_name
  PerRouteConfig per_route;
  per_route.set_requirement_name("abc");
  setup(filter_conf, per_route);

  codec_client_ = makeHttpConnection(lookupPort("http"));

  // So the request with a JWT token is OK.
  auto response = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"Authorization", "Bearer " + std::string(GoodToken)},
  });

  waitForNextUpstreamRequest();
  upstream_request_->encodeHeaders(Http::TestResponseHeaderMapImpl{{":status", "200"}}, true);

  ASSERT_TRUE(response->waitForEndStream());
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("200", response->headers().getStatusValue());

  // A request with missing token is rejected.
  auto response1 = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
  });

  ASSERT_TRUE(response1->waitForEndStream());
  ASSERT_TRUE(response1->complete());
  EXPECT_EQ("401", response1->headers().getStatusValue());
}