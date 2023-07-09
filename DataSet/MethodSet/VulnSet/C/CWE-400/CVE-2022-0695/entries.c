static RList* entries(RBinFile* bf) {
	RList* ret = NULL;
	RBinAddr* addr = NULL;
	psxexe_header psxheader;

	if (!(ret = r_list_new ())) {
		return NULL;
	}

	if (!(addr = R_NEW0 (RBinAddr))) {
		r_list_free (ret);
		return NULL;
	}

	if (r_buf_fread_at (bf->buf, 0, (ut8*)&psxheader, "8c17i", 1) < sizeof (psxexe_header)) {
		eprintf ("PSXEXE Header truncated\n");
		r_list_free (ret);
		free (addr);
		return NULL;
	}

	addr->paddr = (psxheader.pc0 - psxheader.t_addr) + PSXEXE_TEXTSECTION_OFFSET;
	addr->vaddr = psxheader.pc0;

	r_list_append (ret, addr);
	return ret;
}