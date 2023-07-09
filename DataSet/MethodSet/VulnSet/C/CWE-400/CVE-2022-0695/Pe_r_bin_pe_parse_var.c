static Var* Pe_r_bin_pe_parse_var(RBinPEObj* pe, PE_DWord* curAddr) {
	Var* var = calloc (1, sizeof (*var));
	if (!var) {
		pe_printf ("Warning: calloc (Var)\n");
		return NULL;
	}
	if ((var->wLength = r_buf_read_le16_at (pe->b, *curAddr)) == UT16_MAX) {
		pe_printf ("Warning: read (Var wLength)\n");
		free_Var (var);
		return NULL;
	}
	*curAddr += sizeof (var->wLength);
	if ((var->wValueLength = r_buf_read_le16_at (pe->b, *curAddr)) == UT16_MAX) {
		pe_printf ("Warning: read (Var wValueLength)\n");
		free_Var (var);
		return NULL;
	}
	*curAddr += sizeof (var->wValueLength);
	if ((var->wType = r_buf_read_le16_at (pe->b, *curAddr)) == UT16_MAX) {
		pe_printf ("Warning: read (Var wType)\n");
		free_Var (var);
		return NULL;
	}
	*curAddr += sizeof (var->wType);
	if (var->wType != 0 && var->wType != 1) {
		pe_printf ("Warning: check (Var wType)\n");
		free_Var (var);
		return NULL;
	}

	var->szKey = (ut16*) malloc (UT16_ALIGN (TRANSLATION_UTF_16_LEN));  //L"Translation"
	if (!var->szKey) {
		pe_printf ("Warning: malloc (Var szKey)\n");
		free_Var (var);
		return NULL;
	}
	if (r_buf_read_at (pe->b, *curAddr, (ut8*) var->szKey, TRANSLATION_UTF_16_LEN) < 1) {
		pe_printf ("Warning: read (Var szKey)\n");
		free_Var (var);
		return NULL;
	}
	*curAddr += TRANSLATION_UTF_16_LEN;
	if (memcmp (var->szKey, TRANSLATION_UTF_16, TRANSLATION_UTF_16_LEN)) {
		pe_printf ("Warning: check (Var szKey)\n");
		free_Var (var);
		return NULL;
	}
	align32 (*curAddr);
	var->numOfValues = var->wValueLength / 4;
	if (!var->numOfValues) {
		pe_printf ("Warning: check (Var numOfValues)\n");
		free_Var (var);
		return NULL;
	}
	var->Value = (ut32*) malloc (var->wValueLength);
	if (!var->Value) {
		pe_printf ("Warning: malloc (Var Value)\n");
		free_Var (var);
		return NULL;
	}
	if (r_buf_read_at (pe->b, *curAddr, (ut8*) var->Value, var->wValueLength) != var->wValueLength) {
		pe_printf ("Warning: read (Var Value)\n");
		free_Var (var);
		return NULL;
	}
	*curAddr += var->wValueLength;
	return var;
}