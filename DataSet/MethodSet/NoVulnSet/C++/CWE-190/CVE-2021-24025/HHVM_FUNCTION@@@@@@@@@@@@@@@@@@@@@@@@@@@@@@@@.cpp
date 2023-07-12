Variant HHVM_FUNCTION(mcrypt_generic_init, const Resource& td,
                                           const String& key,
                                           const String& iv) {
  auto pm = get_valid_mcrypt_resource(td);
  if (!pm) {
    return false;
  }

  int max_key_size = mcrypt_enc_get_key_size(pm->m_td);
  int iv_size = mcrypt_enc_get_iv_size(pm->m_td);

  if (key.empty()) {
    raise_warning("Key size is 0");
  }

  unsigned char *key_s = (unsigned char *)malloc(key.size());
  memset(key_s, 0, key.size());

  unsigned char *iv_s = (unsigned char *)malloc(iv_size + 1);
  memset(iv_s, 0, iv_size + 1);

  int key_size;
  if (key.size() > max_key_size) {
    raise_warning("Key size too large; supplied length: %ld, max: %d",
                    key.size(), max_key_size);
    key_size = max_key_size;
  } else {
    key_size = key.size();
  }
  memcpy(key_s, key.data(), key.size());

  if (iv.size() != iv_size) {
    raise_warning("Iv size incorrect; supplied length: %ld, needed: %d",
                    iv.size(), iv_size);
  }
  memcpy(iv_s, iv.data(), std::min<int64_t>(iv_size, iv.size()));

  mcrypt_generic_deinit(pm->m_td);
  int result = mcrypt_generic_init(pm->m_td, key_s, key_size, iv_s);

  /* If this function fails, close the mcrypt module to prevent crashes
   * when further functions want to access this resource */
  if (result < 0) {
    pm->close();
    switch (result) {
    case -3:
      raise_warning("Key length incorrect");
      break;
    case -4:
      raise_warning("Memory allocation error");
      break;
    case -1:
    default:
      raise_warning("Unknown error");
      break;
    }
  } else {
    pm->m_init = true;
  }

  free(iv_s);
  free(key_s);
  return result;
}