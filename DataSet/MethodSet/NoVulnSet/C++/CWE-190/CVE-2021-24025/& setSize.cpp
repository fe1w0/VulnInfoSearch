  const String& setSize(int64_t len) {
    assertx(m_str);
    m_str->setSize(len);
    return *this;
  }