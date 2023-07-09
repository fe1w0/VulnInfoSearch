static int lmf_header_load(lmf_header *lmfh, RBuffer *buf, Sdb *db) {
	if (r_buf_size (buf) < sizeof (lmf_header)) {
		return false;
	}
	if (r_buf_fread_at (buf, QNX_HEADER_ADDR, (ut8 *) lmfh, "iiiiiiiicccciiiicc", 1) < QNX_HDR_SIZE) {
		return false;
	}
	r_strf_buffer (32);
	sdb_set (db, "qnx.version", r_strf ("0x%xH", lmfh->version), 0);
	sdb_set (db, "qnx.cflags", r_strf ("0x%xH", lmfh->cflags), 0);
	sdb_set (db, "qnx.cpu", r_strf ("0x%xH", lmfh->cpu), 0);
	sdb_set (db, "qnx.fpu", r_strf ("0x%xH", lmfh->fpu), 0);
	sdb_set (db, "qnx.code_index", r_strf ("0x%x", lmfh->code_index), 0);
	sdb_set (db, "qnx.stack_index", r_strf ("0x%x", lmfh->stack_index), 0);
	sdb_set (db, "qnx.heap_index", r_strf ("0x%x", lmfh->heap_index), 0);
	sdb_set (db, "qnx.argv_index", r_strf ("0x%x", lmfh->argv_index), 0);
	sdb_set (db, "qnx.code_offset", r_strf ("0x%x", lmfh->code_offset), 0);
	sdb_set (db, "qnx.stack_nbytes", r_strf ("0x%x", lmfh->stack_nbytes), 0);
	sdb_set (db, "qnx.heap_nbytes", r_strf ("0x%x", lmfh->heap_nbytes), 0);
	sdb_set (db, "qnx.image_base", r_strf ("0x%x", lmfh->image_base), 0);
	return true;
}