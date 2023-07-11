  LiteralString(std::string &&s, bool ignore_case)
      : lit_(s), ignore_case_(ignore_case),
        is_word_(false) {}