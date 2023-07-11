inline void StringData::setSize(int64_t len) {
  assertx(!isImmutable() && !hasMultipleRefs());
  assertx(len >= 0 && len <= capacity());
  mutableData()[len] = 0;
  m_lenAndHash = len;
  assertx(m_hash == 0);
  assertx(checkSane());
}