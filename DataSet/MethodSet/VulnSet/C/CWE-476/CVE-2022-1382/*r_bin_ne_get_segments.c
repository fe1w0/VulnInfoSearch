RList *r_bin_ne_get_segments(r_bin_ne_obj_t *bin) {
	int i;
	if (!bin) {
		return NULL;
	}
	RList *segments = r_list_newf (free);
	for (i = 0; i < bin->ne_header->SegCount; i++) {
		RBinSection *bs = R_NEW0 (RBinSection);
		if (!bs) {
			return segments;
		}
		NE_image_segment_entry *se = &bin->segment_entries[i];
		bs->size = se->length;
		bs->vsize = se->minAllocSz ? se->minAllocSz : 64000;
		bs->bits = R_SYS_BITS_16;
		bs->is_data = se->flags & IS_DATA;
		bs->perm = __translate_perms (se->flags);
		bs->paddr = (ut64)se->offset * bin->alignment;
		bs->name = r_str_newf ("%s.%" PFMT64d, se->flags & IS_MOVEABLE ? "MOVEABLE" : "FIXED", bs->paddr);
		bs->is_segment = true;
		r_list_append (segments, bs);
	}
	bin->segments = segments;
	return segments;
}