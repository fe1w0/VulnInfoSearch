TEST_P(PerRouteIntegrationTest, PerRouteConfigDisabled) {
  // per-route config has disabled flag.
  PerRouteConfig per_route;
  per_route.set_disabled(true);
  // Use a normal filter config that requires jwt_auth.
  setup(ExampleConfig, per_route);

  codec_client_ = makeHttpConnection(lookupPort("http"));

  // So the request without a JWT token is OK.
  auto response = codec_client_->makeHeaderOnlyRequest(Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
  });

  waitForNextUpstreamRequest();
  upstream_request_->encodeHeaders(Http::TestResponseHeaderMapImpl{{":status", "200"}}, true);

  ASSERT_TRUE(response->waitForEndStream());
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("200", response->headers().getStatusValue());
}