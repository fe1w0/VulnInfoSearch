static ut64 estimate_slide(RBinFile *bf, RDyldCache *cache, ut64 value_mask, ut64 value_add) {
	ut64 slide = 0;
	if (cache->n_hdr > 1) {
		return slide;
	}
	ut64 *classlist = malloc (64);
	if (!classlist) {
		goto beach;
	}

	RListIter *iter;
	RDyldBinImage *bin;
	r_list_foreach (cache->bins, iter, bin) {
		bool found_sample = false;

		struct MACH0_(opts_t) opts = {0};
		opts.verbose = bf->rbin->verbose;
		opts.header_at = bin->header_at;
		opts.symbols_off = 0;

		struct MACH0_(obj_t) *mach0 = MACH0_(new_buf) (cache->buf, &opts);
		if (!mach0) {
			goto beach;
		}

		struct section_t *sections = NULL;
		if (!(sections = MACH0_(get_sections) (mach0))) {
			MACH0_(mach0_free) (mach0);
			goto beach;
		}

		int i;
		int incomplete = 2;
		int classlist_idx = 0, data_idx = 0;
		for (i = 0; !sections[i].last && incomplete; i++) {
			if (sections[i].size == 0) {
				continue;
			}
			if (strstr (sections[i].name, "__objc_classlist")) {
				incomplete--;
				classlist_idx = i;
				continue;
			}
			if (strstr (sections[i].name, "__objc_data")) {
				incomplete--;
				data_idx = i;
				continue;
			}
		}

		if (incomplete) {
			goto next_bin;
		}

		int classlist_sample_size = R_MIN (64, sections[classlist_idx].size);
		int n_classes = classlist_sample_size / 8;
		ut64 sect_offset = sections[classlist_idx].offset + bin->hdr_offset;

		if (r_buf_fread_at (cache->buf, sect_offset, (ut8*) classlist, "l", n_classes) < classlist_sample_size) {
			goto next_bin;
		}

		ut64 data_addr = sections[data_idx].addr;
		ut64 data_tail = data_addr & 0xfff;
		ut64 data_tail_end = (data_addr + sections[data_idx].size) & 0xfff;
		for (i = 0; i < n_classes; i++) {
			ut64 cl_addr = (classlist[i] & value_mask) + value_add;
			ut64 cl_tail = cl_addr & 0xfff;
			if (cl_tail >= data_tail && cl_tail < data_tail_end) {
				ut64 off = cl_tail - data_tail;
				slide = ((cl_addr - off) & value_mask) - (data_addr & value_mask);
				found_sample = true;
				break;
			}
		}

next_bin:
		MACH0_(mach0_free) (mach0);
		R_FREE (sections);

		if (found_sample) {
			break;
		}
	}

beach:
	R_FREE (classlist);
	return slide;
}