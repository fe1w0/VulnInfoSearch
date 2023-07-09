static void ram_block_add(struct uc_struct *uc, RAMBlock *new_block)
{
    RAMBlock *block;
    RAMBlock *last_block = NULL;

    new_block->offset = find_ram_offset(uc, new_block->max_length);

    if (!new_block->host) {
        new_block->host = phys_mem_alloc(uc, new_block->max_length,
                &new_block->mr->align);
        if (!new_block->host) {
            // error_setg_errno(errp, errno,
            //         "cannot set up guest memory '%s'",
            //         memory_region_name(new_block->mr));
            return;
        }
        // memory_try_enable_merging(new_block->host, new_block->max_length);
    }

    /* Keep the list sorted from biggest to smallest block.  Unlike QTAILQ,
     * QLIST (which has an RCU-friendly variant) does not have insertion at
     * tail, so save the last element in last_block.
     */
    RAMBLOCK_FOREACH(block) {
        last_block = block;
        if (block->max_length < new_block->max_length) {
            break;
        }
    }
    if (block) {
        QLIST_INSERT_BEFORE(block, new_block, next);
    } else if (last_block) {
        QLIST_INSERT_AFTER(last_block, new_block, next);
    } else { /* list is empty */
        QLIST_INSERT_HEAD(&uc->ram_list.blocks, new_block, next);
    }
    uc->ram_list.mru_block = NULL;

    /* Write list before version */
    //smp_wmb();

    cpu_physical_memory_set_dirty_range(new_block->offset,
                                        new_block->used_length,
                                        DIRTY_CLIENTS_ALL);

}