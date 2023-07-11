void HeaderMapImpl::remove(const LowerCaseString& key) {
  EntryCb cb = ConstSingleton<StaticLookupTable>::get().find(key.get());
  if (cb) {
    StaticLookupResponse ref_lookup_response = cb(*this);
    removeInline(ref_lookup_response.entry_);
  } else {
    for (auto i = headers_.begin(); i != headers_.end();) {
      if (i->key() == key.get().c_str()) {
        i = headers_.erase(i);
      } else {
        ++i;
      }
    }
  }
}