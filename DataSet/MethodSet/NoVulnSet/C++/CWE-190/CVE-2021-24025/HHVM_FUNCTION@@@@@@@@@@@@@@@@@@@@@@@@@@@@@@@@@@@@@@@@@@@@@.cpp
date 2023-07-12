TypedValue HHVM_FUNCTION(substr_compare,
                         const String& main_str,
                         const String& str,
                         int offset,
                         int length /* = INT_MAX */,
                         bool case_insensitivity /* = false */) {
  int s1_len = main_str.size();
  int s2_len = str.size();

  if (length <= 0) {
    raise_warning("The length must be greater than zero");
    return make_tv<KindOfBoolean>(false);
  }

  if (offset < 0) {
    offset = s1_len + offset;
    if (offset < 0) offset = 0;
  }

  if (offset >= s1_len) {
    raise_warning("The start position cannot exceed initial string length");
    return make_tv<KindOfBoolean>(false);
  }

  auto const cmp_len = std::min(s1_len - offset, std::min(s2_len, length));

  auto const ret = [&] {
    const char *s1 = main_str.data();
    if (case_insensitivity) {
      return bstrcasecmp(s1 + offset, cmp_len, str.data(), cmp_len);
    }
    return string_ncmp(s1 + offset, str.data(), cmp_len);
  }();
  if (ret == 0) {
    auto const m1 = std::min(s1_len - offset, length);
    auto const m2 = std::min(s2_len, length);
    if (m1 > m2) return tvReturn(1);
    if (m1 < m2) return tvReturn(-1);
    return tvReturn(0);
  }
  return tvReturn(ret);
}