  void writeStats(Array& /*ret*/) override {
    fprintf(stderr, "writeStats start\n");
    // RetSame: the return value is the same instance every time
    // HasThis: call has a this argument
    // AllSame: all returns were the same data even though args are different
    // MemberCount: number of different arg sets (including this)
    fprintf(stderr, "Count Function MinSerLen MaxSerLen RetSame HasThis "
            "AllSame MemberCount\n");
    for (auto& me : m_memos) {
      if (me.second.m_ignore) continue;
      if (me.second.m_count == 1) continue;
      int min_ser_len = 999999999;
      int max_ser_len = 0;
      int count = 0;
      int member_count = 0;
      bool all_same = true;
      if (me.second.m_has_this) {
        bool any_multiple = false;
        auto& fr = me.second.m_member_memos.begin()->second.m_return_value;
        member_count = me.second.m_member_memos.size();
        for (auto& mme : me.second.m_member_memos) {
          if (mme.second.m_return_value != fr) all_same = false;
          count += mme.second.m_count;
          auto ser_len = mme.second.m_return_value.length();
          min_ser_len = std::min<int64_t>(min_ser_len, ser_len);
          max_ser_len = std::max<int64_t>(max_ser_len, ser_len);
          if (mme.second.m_count > 1) any_multiple = true;
        }
        if (!any_multiple && !all_same) continue;
      } else {
        min_ser_len = max_ser_len = me.second.m_return_value.length();
        count = me.second.m_count;
        all_same = me.second.m_ret_tv_same;
      }
      fprintf(stderr, "%d %s %d %d %s %s %s %d\n",
              count, me.first.data(),
              min_ser_len, max_ser_len,
              me.second.m_ret_tv_same ? " true" : "false",
              me.second.m_has_this ? " true" : "false",
              all_same ? " true" : "false",
              member_count
             );
    }
    fprintf(stderr, "writeStats end\n");
  }