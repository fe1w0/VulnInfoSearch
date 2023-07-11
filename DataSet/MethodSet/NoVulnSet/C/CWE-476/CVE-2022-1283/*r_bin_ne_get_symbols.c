RList *r_bin_ne_get_symbols(r_bin_ne_obj_t *bin) {
	RBinSymbol *sym;
	ut16 off = bin->ne_header->ResidNamTable + bin->header_offset;
	RList *symbols = r_list_newf (free);
	if (!symbols) {
		return NULL;
	}
	RList *entries = r_bin_ne_get_entrypoints (bin);
	bool resident = true, first = true;
	while (entries) {
		ut8 sz = r_buf_read8_at (bin->buf, off);
		if (!sz) {
			first = true;
			if (resident) {
				resident = false;
				off = bin->ne_header->OffStartNonResTab;
				sz = r_buf_read8_at (bin->buf, off);
				if (!sz) {
					break;
				}
			} else {
				break;
			}
		}
		char *name = malloc ((ut64)sz + 1);
		if (!name) {
			break;
		}
		off++;
		r_buf_read_at (bin->buf, off, (ut8 *)name, sz);
		name[sz] = '\0';
		off += sz;
		sym = R_NEW0 (RBinSymbol);
		if (!sym) {
			break;
		}
		sym->name = name;
		if (!first) {
			sym->bind = R_BIN_BIND_GLOBAL_STR;
		}
		ut16 entry_off = r_buf_read_le16_at (bin->buf, off);
		off += 2;
		RBinAddr *entry = r_list_get_n (entries, entry_off);
		if (entry) {
			sym->paddr = entry->paddr;
		} else {
			sym->paddr = -1;
		}
		sym->ordinal = entry_off;
		r_list_append (symbols, sym);
		first = false;
	}
	RListIter *it;
	RBinAddr *en;
	int i = 1;
	r_list_foreach (entries, it, en) {
		if (!r_list_find (symbols, &en->paddr, __find_symbol_by_paddr)) {
			sym = R_NEW0 (RBinSymbol);
			if (!sym) {
				break;
			}
			sym->name = r_str_newf ("entry%d", i - 1);
			sym->paddr = en->paddr;
			sym->bind = R_BIN_BIND_GLOBAL_STR;
			sym->ordinal = i;
			r_list_append (symbols, sym);
		}
		i++;
	}
	bin->symbols = symbols;
	return symbols;
}