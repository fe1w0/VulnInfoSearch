R_API int r_io_bank_read_from_submap_at(RIO *io, const ut32 bankid, ut64 addr, ut8 *buf, int len) {
	r_return_val_if_fail (io, -1);
	RIOBank *bank = r_io_bank_get (io, bankid);
	if (!bank) {
		return 0;
	}
	if (!len) {
		return 0;
	}
	RRBNode *node;
	if (bank->last_used && r_io_submap_contain (((RIOSubMap *)bank->last_used->data), addr)) {
		node = bank->last_used;
	} else {
		node = r_crbtree_find_node (bank->submaps, &addr, _find_sm_by_vaddr_cb, NULL);
		if (!node) {
			return 0;
		}
		bank->last_used = node;
	}
	RIOSubMap *sm = (RIOSubMap *)node->data;
	if (!r_io_submap_contain (sm, addr)) {
		return 0;
	}
	RIOMap *map = r_io_map_get_by_ref (io, &sm->mapref);
	if (!map || !(map->perm & R_PERM_R)) {
		return -1;
	}
	const int read_len = R_MIN (len, r_io_submap_to (sm) - addr + 1);
	if (map->perm & R_PERM_RELOC) {
		return map->reloc_map->read (io, map, addr, buf, read_len);
	}
	const ut64 paddr = addr - r_io_map_from (map) + map->delta;
	return r_io_fd_read_at (io, map->fd, paddr, buf, read_len);
}