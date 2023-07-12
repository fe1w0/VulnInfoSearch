TEST_P(WasmTest, DivByZero) {
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
      "{{ test_rundir }}/test/extensions/wasm/test_data/segv_cpp.wasm"));
  EXPECT_FALSE(code.empty());
  auto context = std::make_unique<TestContext>(wasm.get());
  EXPECT_CALL(*context, scriptLog_(spdlog::level::err, Eq("before div by zero")));
  EXPECT_TRUE(wasm->initialize(code, false));
  wasm->setContext(context.get());

  if (GetParam() == "v8") {
    EXPECT_THROW_WITH_MESSAGE(
        context->onLog(), Extensions::Common::Wasm::WasmException,
        "Function: proxy_onLog failed: Uncaught RuntimeError: divide by zero");
  } else if (GetParam() == "wavm") {
    EXPECT_THROW_WITH_REGEX(context->onLog(), Extensions::Common::Wasm::WasmException,
                            "Function: proxy_onLog failed: wavm.integerDivideByZeroOrOverflow.*");
  } else {
    ASSERT_FALSE(true); // Neither of the above was matched.
  }
}