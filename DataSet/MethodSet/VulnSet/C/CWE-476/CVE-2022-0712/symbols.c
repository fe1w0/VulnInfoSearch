static RList *symbols(RBinFile *bf) {
	RList *res = r_list_newf ((RListFree)r_bin_symbol_free);
	r_return_val_if_fail (res && bf->o && bf->o->bin_obj, res);
	RCoreSymCacheElement *element = bf->o->bin_obj;
	size_t i;
	HtUU *hash = ht_uu_new0 ();
	if (!hash) {
		return res;
	}
	bool found = false;
	for (i = 0; i < element->hdr->n_lined_symbols; i++) {
		RCoreSymCacheElementSymbol *sym = (RCoreSymCacheElementSymbol *)&element->lined_symbols[i];
		ht_uu_find (hash, sym->paddr, &found);
		if (found) {
			continue;
		}
		RBinSymbol *s = bin_symbol_from_symbol (element, sym);
		if (s) {
			r_list_append (res, s);
			ht_uu_insert (hash, sym->paddr, 1);
		}
	}
	if (element->symbols) {
		for (i = 0; i < element->hdr->n_symbols; i++) {
			RCoreSymCacheElementSymbol *sym = &element->symbols[i];
			ht_uu_find (hash, sym->paddr, &found);
			if (found) {
				continue;
			}
			RBinSymbol *s = bin_symbol_from_symbol (element, sym);
			if (s) {
				r_list_append (res, s);
			}
		}
	}
	ht_uu_free (hash);
	return res;
}