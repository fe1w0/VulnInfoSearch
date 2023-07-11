int64_t MemFile::readImpl(char *buffer, int64_t length) {
  assertx(m_len != -1);
  assertx(length > 0);
  assertx(m_cursor >= 0);
  int64_t remaining = m_len - m_cursor;
  if (remaining < length) length = remaining;
  if (length > 0) {
    memcpy(buffer, (const void *)(m_data + m_cursor), length);
    m_cursor += length;
    return length;
  }
  return 0;
}