String StringUtil::Implode(const Variant& items, const String& delim,
                           const bool checkIsContainer /* = true */) {
  if (checkIsContainer && !isContainer(items)) {
    throw_param_is_not_container();
  }
  int size = getContainerSize(items);
  if (size == 0) return empty_string();

  req::vector<String> sitems;
  sitems.reserve(size);
  int len = 0;
  int lenDelim = delim.size();
  for (ArrayIter iter(items); iter; ++iter) {
    sitems.emplace_back(iter.second().toString());
    len += sitems.back().size() + lenDelim;
  }
  len -= lenDelim; // always one delimiter less than count of items
  assert(sitems.size() == size);

  String s = String(len, ReserveString);
  char *buffer = s.mutableData();
  const char *sdelim = delim.data();
  char *p = buffer;
  String &init_str = sitems[0];
  int init_len = init_str.size();
  memcpy(p, init_str.data(), init_len);
  p += init_len;
  for (int i = 1; i < size; i++) {
    String &item = sitems[i];
    memcpy(p, sdelim, lenDelim);
    p += lenDelim;
    int lenItem = item.size();
    memcpy(p, item.data(), lenItem);
    p += lenItem;
  }
  assert(p - buffer == len);
  s.setSize(len);
  return s;
}