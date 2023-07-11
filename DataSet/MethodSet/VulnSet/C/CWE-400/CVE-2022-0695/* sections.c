static RList* sections(RBinFile* bf) {
	RList* ret = NULL;
	RBinSection* sect = NULL;
	psxexe_header psxheader = {0};
	ut64 sz = 0;

	if (!(ret = r_list_new ())) {
		return NULL;
	}

	if (!(sect = R_NEW0 (RBinSection))) {
		r_list_free (ret);
		return NULL;
	}

	if (r_buf_fread_at (bf->buf, 0, (ut8*)&psxheader, "8c17i", 1) < sizeof (psxexe_header)) {
		eprintf ("Truncated Header\n");
		free (sect);
		r_list_free (ret);
		return NULL;
	}

	sz = r_buf_size (bf->buf);

	sect->name = strdup ("TEXT");
	sect->paddr = PSXEXE_TEXTSECTION_OFFSET;
	sect->size = sz - PSXEXE_TEXTSECTION_OFFSET;
	sect->vaddr = psxheader.t_addr;
	sect->vsize = psxheader.t_size;
	sect->perm = R_PERM_RX;
	sect->add = true;
	sect->has_strings = true;

	r_list_append (ret, sect);
	return ret;
}