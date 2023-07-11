void * CAPSTONE_API cs_winkernel_malloc(size_t size)
{
	// Disallow zero length allocation because they waste pool header space and,
	// in many cases, indicate a potential validation issue in the calling code.
	NT_ASSERT(size);

	// FP; a use of NonPagedPool is required for Windows 7 support
#pragma prefast(suppress : 30030)		// Allocating executable POOL_TYPE memory
	CS_WINKERNEL_MEMBLOCK *block = (CS_WINKERNEL_MEMBLOCK *)ExAllocatePoolWithTag(
			NonPagedPool, size + sizeof(CS_WINKERNEL_MEMBLOCK), CS_WINKERNEL_POOL_TAG);
	if (!block) {
		return NULL;
	}
	block->size = size;

	return block->data;
}