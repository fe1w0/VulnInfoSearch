REDIS_STATIC int _quicklistNodeAllowInsert(const quicklistNode *node,
                                           const int fill, const size_t sz) {
    if (unlikely(!node))
        return 0;

    int ziplist_overhead;
    /* size of previous offset */
    if (sz < 254)
        ziplist_overhead = 1;
    else
        ziplist_overhead = 5;

    /* size of forward offset */
    if (sz < 64)
        ziplist_overhead += 1;
    else if (likely(sz < 16384))
        ziplist_overhead += 2;
    else
        ziplist_overhead += 5;

    /* new_sz overestimates if 'sz' encodes to an integer type */
    unsigned int new_sz = node->sz + sz + ziplist_overhead;
    if (likely(_quicklistNodeSizeMeetsOptimizationRequirement(new_sz, fill)))
        return 1;
    else if (!sizeMeetsSafetyLimit(new_sz))
        return 0;
    else if ((int)node->count < fill)
        return 1;
    else
        return 0;
}