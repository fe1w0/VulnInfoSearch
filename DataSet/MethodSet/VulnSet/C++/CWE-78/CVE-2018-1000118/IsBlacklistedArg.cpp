bool IsBlacklistedArg(const base::CommandLine::CharType* arg) {
#if defined(OS_WIN)
  const auto converted = base::WideToUTF8(arg);
  const char* a = converted.c_str();
#else
  const char* a = arg;
#endif

  static const char* prefixes[] = {"--", "-", "/"};

  int prefix_length = 0;
  for (auto& prefix : prefixes) {
    if (base::StartsWith(a, prefix, base::CompareCase::SENSITIVE)) {
      prefix_length = strlen(prefix);
      break;
    }
  }

  if (prefix_length > 0) {
    a += prefix_length;
    std::string switch_name(a, strcspn(a, "="));
    auto* iter = std::lower_bound(std::begin(kBlacklist), std::end(kBlacklist),
                                  switch_name);
    if (iter != std::end(kBlacklist) && switch_name == *iter) {
      return true;
    }
  }

  return false;
}