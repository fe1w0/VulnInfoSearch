void qemu_ram_free(struct uc_struct *uc, RAMBlock *block)
{
    if (!block) {
        return;
    }

    //if (block->host) {
    //    ram_block_notify_remove(block->host, block->max_length);
    //}

    QLIST_REMOVE(block, next);
    uc->ram_list.mru_block = NULL;
    /* Write list before version */
    //smp_wmb();
    // call_rcu(block, reclaim_ramblock, rcu);
    reclaim_ramblock(uc, block);
}