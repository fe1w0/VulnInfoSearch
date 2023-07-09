RAMBlock *qemu_ram_alloc_from_ptr(struct uc_struct *uc, ram_addr_t size, void *host,
                                   MemoryRegion *mr)
{
    RAMBlock *new_block;
    ram_addr_t max_size = size;

    size = HOST_PAGE_ALIGN(uc, size);
    max_size = HOST_PAGE_ALIGN(uc, max_size);
    new_block = g_malloc0(sizeof(*new_block));
    if (new_block == NULL)
        return NULL;
    new_block->mr = mr;
    new_block->used_length = size;
    new_block->max_length = max_size;
    assert(max_size >= size);
    new_block->page_size = uc->qemu_real_host_page_size;
    new_block->host = host;
    if (host) {
        new_block->flags |= RAM_PREALLOC;
    }
    ram_block_add(mr->uc, new_block);

    return new_block;
}