struct r_bin_pe_addr_t *PE_(check_mingw)(RBinPEObj *pe) {
	struct r_bin_pe_addr_t* entry;
	bool sw = false;
	ut8 b[1024];
	size_t n = 0;
	if (!pe || !pe->b) {
		return 0LL;
	}
	entry = PE_(r_bin_pe_get_entrypoint) (pe);
	ZERO_FILL (b);
	if (r_buf_read_at (pe->b, entry->paddr, b, sizeof (b)) < 0) {
		pe_printf ("Warning: Cannot read entry at 0x%08"PFMT64x "\n", entry->paddr);
		free (entry);
		return NULL;
	}
	// mingw
	//55                                         push    ebp
	//89 E5                                      mov     ebp, esp
	//83 EC 08                                   sub     esp, 8
	//C7 04 24 01 00 00 00                       mov     dword ptr[esp], 1
	//FF 15 C8 63 41 00                          call    ds : __imp____set_app_type
	//E8 B8 FE FF FF                             call    ___mingw_CRTStartup
	if (b[0] == 0x55 && b[1] == 0x89 && b[3] == 0x83 && b[6] == 0xc7 && b[13] == 0xff && b[19] == 0xe8) {
		sw = follow_offset (entry, pe->b, b, sizeof (b), pe->big_endian, 19);
	}
	//83 EC 1C                                   sub     esp, 1Ch
	//C7 04 24 01 00 00 00                       mov[esp + 1Ch + var_1C], 1
	//FF 15 F8 60 40 00                          call    ds : __imp____set_app_type
	//E8 6B FD FF FF                             call    ___mingw_CRTStartup
	if (b[0] == 0x83 && b[3] == 0xc7 && b[10] == 0xff && b[16] == 0xe8) {
		sw = follow_offset (entry, pe->b, b, sizeof (b), pe->big_endian, 16);
	}
	//83 EC 0C                                            sub     esp, 0Ch
	//C7 05 F4 0A 81 00 00 00 00 00                       mov     ds : _mingw_app_type, 0
	//ED E8 3E AD 24 00                                      call    ___security_init_cookie
	//F2 83 C4 0C                                            add     esp, 0Ch
	//F5 E9 86 FC FF FF                                      jmp     ___tmainCRTStartup
	if (b[0] == 0x83 && b[3] == 0xc7 && b[13] == 0xe8 && b[18] == 0x83 && b[21] == 0xe9) {
		sw = follow_offset (entry, pe->b, b, sizeof (b), pe->big_endian, 21);
	}
	if (sw) {
		// case1:
		// from des address of call search for a1 xx xx xx xx 89 xx xx e8 xx xx xx xx
		//A1 04 50 44 00                             mov     eax, ds:dword_445004
		//89 04 24                                   mov[esp + 28h + lpTopLevelExceptionFilter], eax
		//E8 A3 01 00 00                             call    sub_4013EE
		for (n = 0; n < sizeof (b) - 12; n++) {
			if (b[n] == 0xa1 && b[n + 5] == 0x89 && b[n + 8] == 0xe8) {
				sw = follow_offset (entry, pe->b, b, sizeof (b), pe->big_endian, n + 8);
				return entry;
			}
		}
	}
	free (entry);
	return NULL;
}