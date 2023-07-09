static void *seq_buf_alloc(unsigned long size)
{
	if (unlikely(size > MAX_RW_COUNT))
		return NULL;

	return kvmalloc(size, GFP_KERNEL_ACCOUNT);
}