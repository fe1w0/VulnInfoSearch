static bool set_sockaddr(sockaddr_storage &sa_storage, req::ptr<Socket> sock,
                         const String& addr, int port,
                         struct sockaddr *&sa_ptr, size_t &sa_size) {
  // Always zero it out:
  // - fields are added over time; zeroing it out is future-proofing; for
  //   example, sockaddr_in6 did not originally include sin6_scope_id or
  //   sin6_flowinfo.
  // - required for all on MacOS for correct behavior
  // - on Linux, required for sockaddr_un to deal with buggy sun_path readers
  //   (they should look at the length)
  memset(&sa_storage, 0, sizeof(struct sockaddr_storage));
  struct sockaddr *sock_type = (struct sockaddr*) &sa_storage;
  switch (sock->getType()) {
  case AF_UNIX:
    {
#ifdef _MSC_VER
      return false;
#else
      struct sockaddr_un *sa = (struct sockaddr_un *)sock_type;
      sa->sun_family = AF_UNIX;
      if (addr.length() > sizeof(sa->sun_path)) {
        raise_warning(
          "Unix socket path length (%ld) is larger than system limit (%lu)",
          addr.length(),
          sizeof(sa->sun_path)
        );
        return false;
      }
      memcpy(sa->sun_path, addr.data(), addr.length());
      sa_ptr = (struct sockaddr *)sa;
      sa_size = offsetof(struct sockaddr_un, sun_path) + addr.length();
#ifdef __linux__
      if (addr.length() == 0) {
        // Linux supports 3 kinds of unix sockets; behavior of this struct
        // is in `man 7 unix`; relevant parts:
        // - unnamed: 0-length path. As paths are not required to be
        //   null-terminated, this needs to be undicated by the size.
        //   These might be created by `socketpair()`, for eaxmple.
        // - pathname (common): nothing strange. struct size technically
        //   indicates length, but null terminators are usually set. This
        //   does matter if addr.length() == size of the char array though
        // - abstract: these have a meaningful name, but start with `\0`
        //
        // Setting sa_size to indicate a 0-length path is required to
        // distinguish between unnamed and abstract.
        sa_size = offsetof(struct sockaddr_un, sun_path);
      }
#endif

#endif // ifdef _MSC_VER
    }
    break;
  case AF_INET:
    {
      struct sockaddr_in *sa = (struct sockaddr_in *)sock_type;
      sa->sin_family = AF_INET;
      sa->sin_port = htons((unsigned short) port);
      if (!php_set_inet_addr(sa, addr.c_str(), sock)) {
        return false;
      }
      sa_ptr = (struct sockaddr *)sa;
      sa_size = sizeof(struct sockaddr_in);
    }
    break;
  case AF_INET6:
    {
      struct sockaddr_in6 *sa = (struct sockaddr_in6 *)sock_type;
      sa->sin6_family = AF_INET6;
      sa->sin6_port = htons((unsigned short) port);
      if (!php_set_inet6_addr(sa, addr.c_str(), sock)) {
        return false;
      }
      sa_ptr = (struct sockaddr *)sa;
      sa_size = sizeof(struct sockaddr_in6);
    }
    break;
  default:
    raise_warning("unsupported socket type '%d', must be "
                    "AF_UNIX, AF_INET, or AF_INET6", sock->getType());
    return false;
  }
#ifdef __APPLE__
  // This field is not in the relevant standards, not defined on Linux, but is
  // technically required on MacOS (and other BSDs) according to the man pages:
  // - `man 4 netintro` covers the base sa_len
  // - `man 4 unix` and `man 4 inet6` cover AF_UNIX sun_len and AF_INET6
  //    sin6_len
  // - ... At least MacOS Catalina includes the wrong `man 4 inet`. Look at the
  //   (Net|Free|Open)BSD `man 4 inet` instead.
  //   The MacOS man page says it starts with `sin_family`, which would conflict
  //   with the base sockaddr definition. `sin_len` is actually the first field
  //   in the header file, matching `sa_len`.
  sa_ptr->sa_len = sa_size;
#endif
  return true;
}