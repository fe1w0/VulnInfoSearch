  std::shared_ptr<Grammar> perform_core(const char *s, size_t n,
                                        const Rules &rules, std::string &start,
                                        Log log) {
    Data data;
    any dt = &data;
    auto r = g["Grammar"].parse(s, n, dt);

    if (!r.ret) {
      if (log) {
        if (r.message_pos) {
          auto line = line_info(s, r.message_pos);
          log(line.first, line.second, r.message);
        } else {
          auto line = line_info(s, r.error_pos);
          log(line.first, line.second, "syntax error");
        }
      }
      return nullptr;
    }

    auto &grammar = *data.grammar;

    // User provided rules
    for (const auto &x : rules) {
      auto name = x.first;
      bool ignore = false;
      if (!name.empty() && name[0] == '~') {
        ignore = true;
        name.erase(0, 1);
      }
      if (!name.empty()) {
        auto &rule = grammar[name];
        rule <= x.second;
        rule.name = name;
        rule.ignoreSemanticValue = ignore;
      }
    }

    // Check duplicated definitions
    bool ret = data.duplicates.empty();

    for (const auto &x : data.duplicates) {
      if (log) {
        const auto &name = x.first;
        auto ptr = x.second;
        auto line = line_info(s, ptr);
        log(line.first, line.second, "'" + name + "' is already defined.");
      }
    }

    // Check missing definitions
    for (auto &x : grammar) {
      auto &rule = x.second;

      ReferenceChecker vis(*data.grammar, rule.params);
      rule.accept(vis);
      for (const auto &y : vis.error_s) {
        const auto &name = y.first;
        const auto ptr = y.second;
        if (log) {
          auto line = line_info(s, ptr);
          log(line.first, line.second, vis.error_message[name]);
        }
        ret = false;
      }
    }

    if (!ret) { return nullptr; }

    // Link references
    for (auto &x : grammar) {
      auto &rule = x.second;
      LinkReferences vis(*data.grammar, rule.params);
      rule.accept(vis);
    }

    // Check left recursion
    ret = true;

    for (auto &x : grammar) {
      const auto &name = x.first;
      auto &rule = x.second;

      DetectLeftRecursion vis(name);
      rule.accept(vis);
      if (vis.error_s) {
        if (log) {
          auto line = line_info(s, vis.error_s);
          log(line.first, line.second, "'" + name + "' is left recursive.");
        }
        ret = false;
      }
    }

    if (!ret) { return nullptr; }

    // Set root definition
    auto &start_rule = (*data.grammar)[data.start];

    // Check infinite loop
    {
      DetectInfiniteLoop vis(data.start_pos, data.start);
      start_rule.accept(vis);
      if (vis.has_error) {
        if (log) {
          auto line = line_info(s, vis.error_s);
          log(line.first, line.second,
              "infinite loop is detected in '" + vis.error_name + "'.");
        }
        return nullptr;
      }
    }

    // Automatic whitespace skipping
    if (grammar.count(WHITESPACE_DEFINITION_NAME)) {
      for (auto &x : grammar) {
        auto &rule = x.second;
        auto ope = rule.get_core_operator();
        if (IsLiteralToken::check(*ope)) { rule <= tok(ope); }
      }

      start_rule.whitespaceOpe =
          wsp((*data.grammar)[WHITESPACE_DEFINITION_NAME].get_core_operator());
    }

    // Word expression
    if (grammar.count(WORD_DEFINITION_NAME)) {
      start_rule.wordOpe =
          (*data.grammar)[WORD_DEFINITION_NAME].get_core_operator();
    }

    // Apply instructions
    for (const auto &item : data.instructions) {
      const auto &name = item.first;
      const auto &instruction = item.second;
      auto &rule = grammar[name];

      if (instruction.type == "precedence") {
        const auto &info =
            any_cast<PrecedenceClimbing::BinOpeInfo>(instruction.data);

        if (!apply_precedence_instruction(rule, info, s, log)) {
          return nullptr;
        }
      }
    }

    // Set root definition
    start = data.start;

    return data.grammar;
  }