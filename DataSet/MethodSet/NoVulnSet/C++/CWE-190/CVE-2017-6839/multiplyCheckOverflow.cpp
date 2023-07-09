int multiplyCheckOverflow(int a, int b, int *result)
{
#if (defined __GNUC__ && __GNUC__ >= 5) || ( __clang__ && __has_builtin(__builtin_mul_overflow))
	return __builtin_mul_overflow(a, b, result);
#else
	if (firstBitSet(a)+firstBitSet(b)>31) // int is signed, so we can't use 32 bits
		return true;
	*result = a * b;
	return false;
#endif
}