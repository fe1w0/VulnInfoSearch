TEST_P(PerRouteIntegrationTest, PerRouteConfigWrongRequireName) {
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

  // Per-route config has a wrong requirement_name.
  PerRouteConfig per_route;
  per_route.set_requirement_name("wrong-requirement-name");
  setup(filter_conf, per_route);

  codec_client_ = makeHttpConnection(lookupPort("http"));

  // So the request with a good Jwt token is rejected.
  auto response = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"Authorization", "Bearer " + std::string(GoodToken)},
  });

  ASSERT_TRUE(response->waitForEndStream());
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("403", response->headers().getStatusValue());
}