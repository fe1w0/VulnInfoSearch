size_t jsuGetFreeStack() {
#ifdef ARM
  void *frame = __builtin_frame_address(0);
  size_t stackPos = (size_t)((char*)frame);
  size_t stackEnd = (size_t)((char*)&LINKER_END_VAR);
  if (stackPos < stackEnd) return 0; // should never happen, but just in case of overflow!
  return  stackPos - stackEnd;
#elif defined(LINUX)
  // On linux, we set STACK_BASE from `main`.
  char ptr; // this is on the stack
  extern void *STACK_BASE;
  uint32_t count =  (uint32_t)((size_t)STACK_BASE - (size_t)&ptr);
  return 1000000 - count; // give it 1 megabyte of stack
#else
  // stack depth seems pretty platform-specific :( Default to a value that disables it
  return 1000000; // no stack depth check on this platform
#endif
}