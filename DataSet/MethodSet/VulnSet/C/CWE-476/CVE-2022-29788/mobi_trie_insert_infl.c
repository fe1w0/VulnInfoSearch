MOBI_RET mobi_trie_insert_infl(MOBITrie **root, const MOBIIndx *indx, size_t i) {
    MOBIIndexEntry e = indx->entries[i];
    char *inflected = e.label;
    for (size_t j = 0; j < e.tags_count; j++) {
        MOBIIndexTag t = e.tags[j];
        if (t.tagid == INDX_TAGARR_INFL_PARTS_V1) {
            for (size_t k = 0; k < t.tagvalues_count - 1; k += 2) {
                uint32_t len = t.tagvalues[k];
                uint32_t offset = t.tagvalues[k + 1];
                char *base = mobi_get_cncx_string_flat(indx->cncx_record, offset, len);
                if (base == NULL) {
                    return MOBI_MALLOC_FAILED;
                }
                MOBI_RET ret = mobi_trie_insert_reversed(root, base, inflected);
                free(base);
                if (ret != MOBI_SUCCESS) {
                    return ret;
                }
            }
        }
    }
    return MOBI_SUCCESS;
}