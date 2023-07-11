static mrb_value
mrb_ary_shift_m(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);
  mrb_int len = ARY_LEN(a);
  mrb_int n;
  mrb_value val;

  if (mrb_get_args(mrb, "|i", &n) == 0) {
    return mrb_ary_shift(mrb, self);
  };
  ary_modify_check(mrb, a);
  if (len == 0 || n == 0) return mrb_ary_new(mrb);
  if (n < 0) mrb_raise(mrb, E_ARGUMENT_ERROR, "negative array shift");
  if (n > len) n = len;
  val = mrb_ary_new_from_values(mrb, n, ARY_PTR(a));
  if (ARY_SHARED_P(a)) {
  L_SHIFT:
    a->as.heap.ptr+=n;
    a->as.heap.len-=n;
    return val;
  }
  if (len > ARY_SHIFT_SHARED_MIN) {
    ary_make_shared(mrb, a);
    goto L_SHIFT;
  }
  else if (len == n) {
    ARY_SET_LEN(a, 0);
  }
  else {
    mrb_value *ptr = ARY_PTR(a);
    mrb_int size = len-n;

    while (size--) {
      *ptr = *(ptr+n);
      ++ptr;
    }
    ARY_SET_LEN(a, len-n);
  }
  return val;
}