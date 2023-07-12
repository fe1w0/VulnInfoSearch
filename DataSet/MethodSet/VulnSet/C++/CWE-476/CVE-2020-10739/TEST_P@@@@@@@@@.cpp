TEST_P(WasmTest, StatsHighLevel) {
  Stats::IsolatedStoreImpl stats_store;
  Api::ApiPtr api = Api::createApiForTest(stats_store);
  Upstream::MockClusterManager cluster_manager;
  Event::DispatcherPtr dispatcher(api->allocateDispatcher());
  auto scope = Stats::ScopeSharedPtr(stats_store.createScope("wasm."));
  NiceMock<LocalInfo::MockLocalInfo> local_info;
  auto name = "";
  auto root_id = "";
  auto vm_id = "";
  auto vm_configuration = "";
  auto plugin = std::make_shared<Extensions::Common::Wasm::Plugin>(
      name, root_id, vm_id, envoy::api::v2::core::TrafficDirection::UNSPECIFIED, local_info,
      nullptr);
  auto wasm = std::make_unique<Extensions::Common::Wasm::Wasm>(
      absl::StrCat("envoy.wasm.runtime.", GetParam()), vm_id, vm_configuration, plugin, scope,
      cluster_manager, *dispatcher);
  EXPECT_NE(wasm, nullptr);
  const auto code = TestEnvironment::readFileToStringForTest(TestEnvironment::substitute(
      "{{ test_rundir }}/test/extensions/wasm/test_data/stats_cpp.wasm"));
  EXPECT_FALSE(code.empty());
  auto context = std::make_unique<TestContext>(wasm.get());

  EXPECT_CALL(*context, scriptLog_(spdlog::level::trace, Eq("get counter = 1")));
  EXPECT_CALL(*context, scriptLog_(spdlog::level::debug, Eq("get counter = 2")));
  // recordMetric on a Counter is the same as increment.
  EXPECT_CALL(*context, scriptLog_(spdlog::level::info, Eq("get counter = 5")));
  EXPECT_CALL(*context, scriptLog_(spdlog::level::warn, Eq("get gauge = 2")));
  // Get is not supported on histograms.
  // EXPECT_CALL(*context, scriptLog(spdlog::level::err, Eq(std::string("resolved histogram name
  // = int_tag.7_string_tag.test_tag.bool_tag.true.test_histogram"))));
  EXPECT_CALL(*context,
              scriptLog_(spdlog::level::err,
                         Eq("h_id = int_tag.7.string_tag.test_tag.bool_tag.true.test_histogram")));
  EXPECT_CALL(*context, scriptLog_(spdlog::level::err, Eq("stack_c = 1")));
  EXPECT_CALL(*context, scriptLog_(spdlog::level::err, Eq("stack_g = 2")));
  // Get is not supported on histograms.
  // EXPECT_CALL(*context, scriptLog_(spdlog::level::err, Eq("stack_h = 3")));
  EXPECT_TRUE(wasm->initialize(code, false));
  wasm->setContext(context.get());
  context->onLog();
}