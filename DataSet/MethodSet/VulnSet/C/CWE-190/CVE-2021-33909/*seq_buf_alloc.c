static void *seq_buf_alloc(unsigned long size)
{
	return kvmalloc(size, GFP_KERNEL_ACCOUNT);
}