static ram_addr_t find_ram_offset(struct uc_struct *uc, ram_addr_t size)
{
    RAMBlock *block, *next_block;
    ram_addr_t offset = RAM_ADDR_MAX, mingap = RAM_ADDR_MAX;

    assert(size != 0); /* it would hand out same offset multiple times */

    if (QLIST_EMPTY(&uc->ram_list.blocks)) {
        return 0;
    }

    RAMBLOCK_FOREACH(block) {
        ram_addr_t candidate, next = RAM_ADDR_MAX;

        /* Align blocks to start on a 'long' in the bitmap
         * which makes the bitmap sync'ing take the fast path.
         */
        candidate = block->offset + block->max_length;
        candidate = ROUND_UP(candidate, BITS_PER_LONG << TARGET_PAGE_BITS);

        /* Search for the closest following block
         * and find the gap.
         */
        RAMBLOCK_FOREACH(next_block) {
            if (next_block->offset >= candidate) {
                next = MIN(next, next_block->offset);
            }
        }

        /* If it fits remember our place and remember the size
         * of gap, but keep going so that we might find a smaller
         * gap to fill so avoiding fragmentation.
         */
        if (next - candidate >= size && next - candidate < mingap) {
            offset = candidate;
            mingap = next - candidate;
        }
    }

    if (offset == RAM_ADDR_MAX) {
        fprintf(stderr, "Failed to find gap of requested size: %" PRIu64 "\n",
                (uint64_t)size);
        abort();
    }

    return offset;
}