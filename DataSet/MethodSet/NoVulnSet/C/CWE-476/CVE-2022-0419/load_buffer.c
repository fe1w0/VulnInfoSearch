static bool load_buffer(RBinFile *bf, void **bin_obj, RBuffer *buf, ut64 loadaddr, Sdb *sdb) {
	RBuffer *fbuf = r_buf_ref (buf);
	struct MACH0_(opts_t) opts;
	MACH0_(opts_set_default) (&opts, bf);
	struct MACH0_(obj_t) *main_mach0 = MACH0_(new_buf) (fbuf, &opts);
	if (!main_mach0) {
		return false;
	}

	RRebaseInfo *rebase_info = r_rebase_info_new_from_mach0 (fbuf, main_mach0);
	RKernelCacheObj *obj = NULL;

	RPrelinkRange *prelink_range = get_prelink_info_range_from_mach0 (main_mach0);
	if (!prelink_range) {
		goto beach;
	}

	obj = R_NEW0 (RKernelCacheObj);
	if (!obj) {
		R_FREE (prelink_range);
		goto beach;
	}

	RCFValueDict *prelink_info = NULL;
	if (main_mach0->hdr.filetype != MH_FILESET && prelink_range->range.size) {
		prelink_info = r_cf_value_dict_parse (fbuf, prelink_range->range.offset,
				prelink_range->range.size, R_CF_OPTION_SKIP_NSDATA);
		if (!prelink_info) {
			R_FREE (prelink_range);
			R_FREE (obj);
			goto beach;
		}
	}

	if (!pending_bin_files) {
		pending_bin_files = r_list_new ();
		if (!pending_bin_files) {
			R_FREE (prelink_range);
			R_FREE (obj);
			R_FREE (prelink_info);
			goto beach;
		}
	}

	obj->mach0 = main_mach0;
	obj->rebase_info = rebase_info;
	obj->prelink_info = prelink_info;
	obj->cache_buf = fbuf;
	obj->pa2va_exec = prelink_range->pa2va_exec;
	obj->pa2va_data = prelink_range->pa2va_data;

	R_FREE (prelink_range);

	*bin_obj = obj;

	r_list_push (pending_bin_files, bf);

	if (rebase_info || main_mach0->chained_starts) {
		RIO *io = bf->rbin->iob.io;
		swizzle_io_read (obj, io);
	}

	return true;

beach:
	r_buf_free (fbuf);
	if (obj) {
		obj->cache_buf = NULL;
	}
	MACH0_(mach0_free) (main_mach0);
	return false;
}