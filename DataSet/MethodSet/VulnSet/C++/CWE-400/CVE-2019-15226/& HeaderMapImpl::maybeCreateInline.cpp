HeaderMapImpl::HeaderEntryImpl& HeaderMapImpl::maybeCreateInline(HeaderEntryImpl** entry,
                                                                 const LowerCaseString& key) {
  if (*entry) {
    return **entry;
  }

  std::list<HeaderEntryImpl>::iterator i = headers_.insert(key);
  i->entry_ = i;
  *entry = &(*i);
  return **entry;
}