bool logToUSDT(const Array& bt) {
  std::lock_guard<std::mutex> lock(usdt_mutex);

  memset(&bt_slab, 0, sizeof(bt_slab));

  int i = 0;
  IterateVNoInc(
    bt.get(),
    [&](TypedValue tv) -> bool {

      if (i >= strobelight::kMaxStackframes) {
        return true;
      }

      assertx(isArrayLikeType(type(tv)));
      ArrayData* bt_frame = val(tv).parr;
      strobelight::backtrace_frame_t* frame = &bt_slab.frames[i];

      auto const line = bt_frame->get(s_line.get());
      if (line.is_init()) {
        assertx(isIntType(type(line)));
        frame->line = val(line).num;
      }

      auto const file_name = bt_frame->get(s_file.get());
      if (file_name.is_init()) {
        assertx(isStringType(type(file_name)));
        strncpy(frame->file_name,
                val(file_name).pstr->data(),
                std::min<int64_t>(
                  val(file_name).pstr->size(),
                  strobelight::kFileNameMax
                ));
        frame->file_name[strobelight::kFileNameMax - 1] = '\0';
      }

      auto const class_name = bt_frame->get(s_class.get());
      if (class_name.is_init()) {
        assertx(isStringType(type(class_name)));
        strncpy(frame->class_name,
                val(class_name).pstr->data(),
                std::min<int64_t>(
                  val(class_name).pstr->size(),
                  strobelight::kClassNameMax
                ));
        frame->class_name[strobelight::kClassNameMax - 1] = '\0';
      }

      auto const function_name = bt_frame->get(s_function.get());
      if (function_name.is_init()) {
        assertx(isStringType(type(function_name)));
        strncpy(frame->function,
                val(function_name).pstr->data(),
                std::min<int64_t>(
                  val(function_name).pstr->size(),
                  strobelight::kFunctionMax
                ));
        frame->function[strobelight::kFunctionMax - 1] = '\0';
      }

      i++;
      return false;
    }
  );
  bt_slab.len = i;

  // Allow BPF to read the now-formatted stacktrace
  FOLLY_SDT_WITH_SEMAPHORE(hhvm, hhvm_stack, &bt_slab);

  return true;
}