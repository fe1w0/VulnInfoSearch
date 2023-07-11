static inline item* limited_get(char *key, size_t nkey, conn *c) {
    item *it = item_get(key, nkey, c, DO_UPDATE);
    if (it && it->refcount > IT_REFCOUNT_LIMIT) {
        item_remove(it);
        it = NULL;
    }
    return it;
}