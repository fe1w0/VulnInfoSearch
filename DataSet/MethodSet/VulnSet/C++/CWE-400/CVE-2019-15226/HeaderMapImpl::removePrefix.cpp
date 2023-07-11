void HeaderMapImpl::removePrefix(const LowerCaseString& prefix) {
  headers_.remove_if([&](const HeaderEntryImpl& entry) {
    bool to_remove = absl::StartsWith(entry.key().getStringView(), prefix.get());
    if (to_remove) {
      // If this header should be removed, make sure any references in the
      // static lookup table are cleared as well.
      EntryCb cb = ConstSingleton<StaticLookupTable>::get().find(entry.key().getStringView());
      if (cb) {
        StaticLookupResponse ref_lookup_response = cb(*this);
        if (ref_lookup_response.entry_) {
          *ref_lookup_response.entry_ = nullptr;
        }
      }
    }
    return to_remove;
  });
}