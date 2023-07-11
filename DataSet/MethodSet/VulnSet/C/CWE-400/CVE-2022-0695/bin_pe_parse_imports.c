static int bin_pe_parse_imports(RBinPEObj* pe,
                                struct r_bin_pe_import_t** importp, int* nimp,
                                const char* dll_name,
                                PE_DWord OriginalFirstThunk,
                                PE_DWord FirstThunk) {
	char import_name[PE_NAME_LENGTH + 1];
	char name[PE_NAME_LENGTH + 1];
	PE_Word import_hint, import_ordinal = 0;
	PE_DWord import_table = 0, off = 0;
	int i = 0, len;
	Sdb* db = NULL;
	char* sdb_module = NULL;
	char* symname = NULL;
	char* symdllname = NULL;

	if (!dll_name || !*dll_name || *dll_name == '0') {
		return 0;
	}

	if (!(off = PE_(va2pa) (pe, OriginalFirstThunk)) &&
	!(off = PE_(va2pa) (pe, FirstThunk))) {
		return 0;
	}
	do {
		if (import_ordinal >= UT16_MAX) {
			break;
		}
		if (off + i * sizeof (PE_DWord) > pe->size) {
			break;
		}
		import_table = R_BUF_READ_PE_DWORD_AT (pe->b, off + i * sizeof (PE_DWord));
		if (import_table == PE_DWORD_MAX) {
			pe_printf ("Warning: read (import table)\n");
			goto error;
		} else if (import_table) {
			if (import_table & ILT_MASK1) {
				import_ordinal = import_table & ILT_MASK2;
				import_hint = 0;
				snprintf (import_name, PE_NAME_LENGTH, "Ordinal_%i", import_ordinal);
				free (symdllname);
				strncpy (name, dll_name, sizeof (name) - 1);
				name[sizeof (name) - 1] = 0;
				symdllname = strdup (name);

				// remove the trailling ".dll"
				size_t len = strlen (symdllname);
				r_str_case (symdllname, 0);
				len = len < 4? 0: len - 4;
				symdllname[len] = 0;

				char* filename = NULL;
				if (!sdb_module || strcmp (symdllname, sdb_module)) {
					sdb_free (db);
					db = NULL;
					free (sdb_module);
					sdb_module = strdup (symdllname);
					filename = r_str_newf ("%s.sdb", symdllname);
					if (filename && r_file_exists (filename)) {
						db = sdb_new (NULL, filename, 0);
					} else {
						const char *dirPrefix = r_sys_prefix (NULL);
						char *lower_symdllname = strdup (symdllname);
						r_str_case (lower_symdllname, false);
						filename = r_str_newf (R_JOIN_4_PATHS ("%s", R2_SDB_FORMAT, "dll", "%s.sdb"),
							dirPrefix, lower_symdllname);
						free (lower_symdllname);
						if (r_file_exists (filename)) {
							db = sdb_new (NULL, filename, 0);
						}
					}
				}
				if (db) {
					symname = resolveModuleOrdinal (db, symdllname, import_ordinal);
					if (symname) {
						snprintf (import_name, PE_NAME_LENGTH, "%s", symname);
						R_FREE (symname);
					}
				} else {
					pe_printf ("Cannot find %s\n", filename);
				}
				free (filename);
			} else {
				import_ordinal++;
				const ut64 off = PE_(va2pa) (pe, import_table);
				if (off > pe->size || (off + sizeof (PE_Word)) > pe->size) {
					pe_printf ("Warning: off > pe->size\n");
					goto error;
				}
				import_hint = r_buf_read_le16_at (pe->b, off);
				if (import_hint == UT16_MAX) {
					pe_printf ("Warning: read import hint at 0x%08"PFMT64x "\n", off);
					goto error;
				}
				name[0] = '\0';
				len = r_buf_read_at (pe->b, off + sizeof (PE_Word), (ut8*) name, PE_NAME_LENGTH);
				if (len < 1) {
					pe_printf ("Warning: read (import name)\n");
					goto error;
				} else if (!*name) {
					break;
				}
				name[PE_NAME_LENGTH] = '\0';
				int len = snprintf (import_name, sizeof (import_name), "%s" , name);
				if (len >= sizeof (import_name)) {
					eprintf ("Import name '%s' has been truncated.\n", import_name);
				}
			}
			struct r_bin_pe_import_t *new_importp = realloc (*importp, (*nimp + 1) * sizeof (struct r_bin_pe_import_t));
			if (!new_importp) {
				r_sys_perror ("realloc (import)");
				goto error;
			}
			*importp = new_importp;
			memcpy ((*importp)[*nimp].name, import_name, PE_NAME_LENGTH);
			(*importp)[*nimp].name[PE_NAME_LENGTH] = '\0';
			memcpy ((*importp)[*nimp].libname, dll_name, PE_NAME_LENGTH);
			(*importp)[*nimp].libname[PE_NAME_LENGTH] = '\0';
			(*importp)[*nimp].vaddr = bin_pe_rva_to_va (pe, FirstThunk + i * sizeof (PE_DWord));
			(*importp)[*nimp].paddr = PE_(va2pa) (pe, FirstThunk) + i * sizeof (PE_DWord);
			(*importp)[*nimp].hint = import_hint;
			(*importp)[*nimp].ordinal = import_ordinal;
			(*importp)[*nimp].last = 0;
			(*nimp)++;
			i++;
		}
	} while (import_table);

	if (db) {
		sdb_free (db);
		db = NULL;
	}
	free (symdllname);
	free (sdb_module);
	return i;

error:
	if (db) {
		sdb_free (db);
		db = NULL;
	}
	free (symdllname);
	free (sdb_module);
	return false;
}