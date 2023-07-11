bool MemFile::seek(int64_t offset, int whence /* = SEEK_SET */) {
  assertx(m_len != -1);
  if (whence == SEEK_CUR) {
    if (offset >= 0 && offset < bufferedLen()) {
      setReadPosition(getReadPosition() + offset);
      setPosition(getPosition() + offset);
      return true;
    }
    offset += getPosition();
    whence = SEEK_SET;
  }

  // invalidate the current buffer
  setWritePosition(0);
  setReadPosition(0);
  if (whence == SEEK_SET) {
    if (offset < 0) return false;
    m_cursor = offset;
  } else if (whence == SEEK_END) {
    if (m_len + offset < 0) return false;
    m_cursor = m_len + offset;
  } else {
    return false;
  }
  setPosition(m_cursor);
  return true;
}