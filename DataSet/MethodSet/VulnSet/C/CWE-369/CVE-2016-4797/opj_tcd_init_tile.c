static INLINE OPJ_BOOL opj_tcd_init_tile(opj_tcd_t *p_tcd, OPJ_UINT32 p_tile_no, OPJ_BOOL isEncoder, OPJ_FLOAT32 fraction, OPJ_SIZE_T sizeof_block, opj_event_mgr_t* manager)
{
	OPJ_UINT32 (*l_gain_ptr)(OPJ_UINT32) = 00;
	OPJ_UINT32 compno, resno, bandno, precno, cblkno;
	opj_tcp_t * l_tcp = 00;
	opj_cp_t * l_cp = 00;
	opj_tcd_tile_t * l_tile = 00;
	opj_tccp_t *l_tccp = 00;
	opj_tcd_tilecomp_t *l_tilec = 00;
	opj_image_comp_t * l_image_comp = 00;
	opj_tcd_resolution_t *l_res = 00;
	opj_tcd_band_t *l_band = 00;
	opj_stepsize_t * l_step_size = 00;
	opj_tcd_precinct_t *l_current_precinct = 00;
	opj_image_t *l_image = 00;
	OPJ_UINT32 p,q;
	OPJ_UINT32 l_level_no;
	OPJ_UINT32 l_pdx, l_pdy;
	OPJ_UINT32 l_gain;
	OPJ_INT32 l_x0b, l_y0b;
	OPJ_UINT32 l_tx0, l_ty0;
	/* extent of precincts , top left, bottom right**/
	OPJ_INT32 l_tl_prc_x_start, l_tl_prc_y_start, l_br_prc_x_end, l_br_prc_y_end;
	/* number of precinct for a resolution */
	OPJ_UINT32 l_nb_precincts;
	/* room needed to store l_nb_precinct precinct for a resolution */
	OPJ_UINT32 l_nb_precinct_size;
	/* number of code blocks for a precinct*/
	OPJ_UINT32 l_nb_code_blocks;
	/* room needed to store l_nb_code_blocks code blocks for a precinct*/
	OPJ_UINT32 l_nb_code_blocks_size;
	/* size of data for a tile */
	OPJ_UINT32 l_data_size;
	
	l_cp = p_tcd->cp;
	l_tcp = &(l_cp->tcps[p_tile_no]);
	l_tile = p_tcd->tcd_image->tiles;
	l_tccp = l_tcp->tccps;
	l_tilec = l_tile->comps;
	l_image = p_tcd->image;
	l_image_comp = p_tcd->image->comps;
	
	p = p_tile_no % l_cp->tw;       /* tile coordinates */
	q = p_tile_no / l_cp->tw;
	/*fprintf(stderr, "Tile coordinate = %d,%d\n", p, q);*/
	
	/* 4 borders of the tile rescale on the image if necessary */
	l_tx0 = l_cp->tx0 + p * l_cp->tdx; /* can't be greater than l_image->x1 so won't overflow */
	l_tile->x0 = (OPJ_INT32)opj_uint_max(l_tx0, l_image->x0);
	l_tile->x1 = (OPJ_INT32)opj_uint_min(opj_uint_adds(l_tx0, l_cp->tdx), l_image->x1);
	l_ty0 = l_cp->ty0 + q * l_cp->tdy; /* can't be greater than l_image->y1 so won't overflow */
	l_tile->y0 = (OPJ_INT32)opj_uint_max(l_ty0, l_image->y0);
	l_tile->y1 = (OPJ_INT32)opj_uint_min(opj_uint_adds(l_ty0, l_cp->tdy), l_image->y1);

	/* testcase 1888.pdf.asan.35.988 */
	if (l_tccp->numresolutions == 0) {
		opj_event_msg(manager, EVT_ERROR, "tiles require at least one resolution\n");
		return OPJ_FALSE;
	}
	/*fprintf(stderr, "Tile border = %d,%d,%d,%d\n", l_tile->x0, l_tile->y0,l_tile->x1,l_tile->y1);*/
	
	/*tile->numcomps = image->numcomps; */
	for (compno = 0; compno < l_tile->numcomps; ++compno) {
		/*fprintf(stderr, "compno = %d/%d\n", compno, l_tile->numcomps);*/
		l_image_comp->resno_decoded = 0;
		/* border of each l_tile component (global) */
		l_tilec->x0 = opj_int_ceildiv(l_tile->x0, (OPJ_INT32)l_image_comp->dx);
		l_tilec->y0 = opj_int_ceildiv(l_tile->y0, (OPJ_INT32)l_image_comp->dy);
		l_tilec->x1 = opj_int_ceildiv(l_tile->x1, (OPJ_INT32)l_image_comp->dx);
		l_tilec->y1 = opj_int_ceildiv(l_tile->y1, (OPJ_INT32)l_image_comp->dy);
		/*fprintf(stderr, "\tTile compo border = %d,%d,%d,%d\n", l_tilec->x0, l_tilec->y0,l_tilec->x1,l_tilec->y1);*/
		
		/* compute l_data_size with overflow check */
		l_data_size = (OPJ_UINT32)(l_tilec->x1 - l_tilec->x0);
		if ((((OPJ_UINT32)-1) / l_data_size) < (OPJ_UINT32)(l_tilec->y1 - l_tilec->y0)) {
			opj_event_msg(manager, EVT_ERROR, "Not enough memory for tile data\n");
			return OPJ_FALSE;
		}
		l_data_size = l_data_size * (OPJ_UINT32)(l_tilec->y1 - l_tilec->y0);
		
		if ((((OPJ_UINT32)-1) / (OPJ_UINT32)sizeof(OPJ_UINT32)) < l_data_size) {
			opj_event_msg(manager, EVT_ERROR, "Not enough memory for tile data\n");
			return OPJ_FALSE;
		}
		l_data_size = l_data_size * (OPJ_UINT32)sizeof(OPJ_UINT32);
		l_tilec->numresolutions = l_tccp->numresolutions;
		if (l_tccp->numresolutions < l_cp->m_specific_param.m_dec.m_reduce) {
			l_tilec->minimum_num_resolutions = 1;
		}
		else {
			l_tilec->minimum_num_resolutions = l_tccp->numresolutions - l_cp->m_specific_param.m_dec.m_reduce;
		}
		
		l_tilec->data_size_needed = l_data_size;
		if (p_tcd->m_is_decoder && !opj_alloc_tile_component_data(l_tilec)) {
			opj_event_msg(manager, EVT_ERROR, "Not enough memory for tile data\n");
			return OPJ_FALSE;
		}
		
		l_data_size = l_tilec->numresolutions * (OPJ_UINT32)sizeof(opj_tcd_resolution_t);
		
		if (l_tilec->resolutions == 00) {
			l_tilec->resolutions = (opj_tcd_resolution_t *) opj_malloc(l_data_size);
			if (! l_tilec->resolutions ) {
				return OPJ_FALSE;
			}
			/*fprintf(stderr, "\tAllocate resolutions of tilec (opj_tcd_resolution_t): %d\n",l_data_size);*/
			l_tilec->resolutions_size = l_data_size;
			memset(l_tilec->resolutions,0,l_data_size);
		}
		else if (l_data_size > l_tilec->resolutions_size) {
			opj_tcd_resolution_t* new_resolutions = (opj_tcd_resolution_t *) opj_realloc(l_tilec->resolutions, l_data_size);
			if (! new_resolutions) {
				opj_event_msg(manager, EVT_ERROR, "Not enough memory for tile resolutions\n");
				opj_free(l_tilec->resolutions);
				l_tilec->resolutions = NULL;
				l_tilec->resolutions_size = 0;
				return OPJ_FALSE;
			}
			l_tilec->resolutions = new_resolutions;
			/*fprintf(stderr, "\tReallocate data of tilec (int): from %d to %d x OPJ_UINT32\n", l_tilec->resolutions_size, l_data_size);*/
			memset(((OPJ_BYTE*) l_tilec->resolutions)+l_tilec->resolutions_size,0,l_data_size - l_tilec->resolutions_size);
			l_tilec->resolutions_size = l_data_size;
		}
		
		l_level_no = l_tilec->numresolutions;
		l_res = l_tilec->resolutions;
		l_step_size = l_tccp->stepsizes;
		if (l_tccp->qmfbid == 0) {
			l_gain_ptr = &opj_dwt_getgain_real;
		}
		else {
			l_gain_ptr  = &opj_dwt_getgain;
		}
		/*fprintf(stderr, "\tlevel_no=%d\n",l_level_no);*/
		
		for (resno = 0; resno < l_tilec->numresolutions; ++resno) {
			/*fprintf(stderr, "\t\tresno = %d/%d\n", resno, l_tilec->numresolutions);*/
			OPJ_INT32 tlcbgxstart, tlcbgystart /*, brcbgxend, brcbgyend*/;
			OPJ_UINT32 cbgwidthexpn, cbgheightexpn;
			OPJ_UINT32 cblkwidthexpn, cblkheightexpn;
			
			--l_level_no;
			
			/* border for each resolution level (global) */
			l_res->x0 = opj_int_ceildivpow2(l_tilec->x0, (OPJ_INT32)l_level_no);
			l_res->y0 = opj_int_ceildivpow2(l_tilec->y0, (OPJ_INT32)l_level_no);
			l_res->x1 = opj_int_ceildivpow2(l_tilec->x1, (OPJ_INT32)l_level_no);
			l_res->y1 = opj_int_ceildivpow2(l_tilec->y1, (OPJ_INT32)l_level_no);
			/*fprintf(stderr, "\t\t\tres_x0= %d, res_y0 =%d, res_x1=%d, res_y1=%d\n", l_res->x0, l_res->y0, l_res->x1, l_res->y1);*/
			/* p. 35, table A-23, ISO/IEC FDIS154444-1 : 2000 (18 august 2000) */
			l_pdx = l_tccp->prcw[resno];
			l_pdy = l_tccp->prch[resno];
			/*fprintf(stderr, "\t\t\tpdx=%d, pdy=%d\n", l_pdx, l_pdy);*/
			/* p. 64, B.6, ISO/IEC FDIS15444-1 : 2000 (18 august 2000)  */
			l_tl_prc_x_start = opj_int_floordivpow2(l_res->x0, (OPJ_INT32)l_pdx) << l_pdx;
			l_tl_prc_y_start = opj_int_floordivpow2(l_res->y0, (OPJ_INT32)l_pdy) << l_pdy;
			l_br_prc_x_end = opj_int_ceildivpow2(l_res->x1, (OPJ_INT32)l_pdx) << l_pdx;
			l_br_prc_y_end = opj_int_ceildivpow2(l_res->y1, (OPJ_INT32)l_pdy) << l_pdy;
			/*fprintf(stderr, "\t\t\tprc_x_start=%d, prc_y_start=%d, br_prc_x_end=%d, br_prc_y_end=%d \n", l_tl_prc_x_start, l_tl_prc_y_start, l_br_prc_x_end ,l_br_prc_y_end );*/
			
			l_res->pw = (l_res->x0 == l_res->x1) ? 0 : (OPJ_UINT32)((l_br_prc_x_end - l_tl_prc_x_start) >> l_pdx);
			l_res->ph = (l_res->y0 == l_res->y1) ? 0 : (OPJ_UINT32)((l_br_prc_y_end - l_tl_prc_y_start) >> l_pdy);
			/*fprintf(stderr, "\t\t\tres_pw=%d, res_ph=%d\n", l_res->pw, l_res->ph );*/
			
			l_nb_precincts = l_res->pw * l_res->ph;
			l_nb_precinct_size = l_nb_precincts * (OPJ_UINT32)sizeof(opj_tcd_precinct_t);
			if (resno == 0) {
				tlcbgxstart = l_tl_prc_x_start;
				tlcbgystart = l_tl_prc_y_start;
				/*brcbgxend = l_br_prc_x_end;*/
				/* brcbgyend = l_br_prc_y_end;*/
				cbgwidthexpn = l_pdx;
				cbgheightexpn = l_pdy;
				l_res->numbands = 1;
			}
			else {
				tlcbgxstart = opj_int_ceildivpow2(l_tl_prc_x_start, 1);
				tlcbgystart = opj_int_ceildivpow2(l_tl_prc_y_start, 1);
				/*brcbgxend = opj_int_ceildivpow2(l_br_prc_x_end, 1);*/
				/*brcbgyend = opj_int_ceildivpow2(l_br_prc_y_end, 1);*/
				cbgwidthexpn = l_pdx - 1;
				cbgheightexpn = l_pdy - 1;
				l_res->numbands = 3;
			}
			
			cblkwidthexpn = opj_uint_min(l_tccp->cblkw, cbgwidthexpn);
			cblkheightexpn = opj_uint_min(l_tccp->cblkh, cbgheightexpn);
			l_band = l_res->bands;
			
			for (bandno = 0; bandno < l_res->numbands; ++bandno) {
				OPJ_INT32 numbps;
				/*fprintf(stderr, "\t\t\tband_no=%d/%d\n", bandno, l_res->numbands );*/
				
				if (resno == 0) {
					l_band->bandno = 0 ;
					l_band->x0 = opj_int_ceildivpow2(l_tilec->x0, (OPJ_INT32)l_level_no);
					l_band->y0 = opj_int_ceildivpow2(l_tilec->y0, (OPJ_INT32)l_level_no);
					l_band->x1 = opj_int_ceildivpow2(l_tilec->x1, (OPJ_INT32)l_level_no);
					l_band->y1 = opj_int_ceildivpow2(l_tilec->y1, (OPJ_INT32)l_level_no);
				}
				else {
					l_band->bandno = bandno + 1;
					/* x0b = 1 if bandno = 1 or 3 */
					l_x0b = l_band->bandno&1;
					/* y0b = 1 if bandno = 2 or 3 */
					l_y0b = (OPJ_INT32)((l_band->bandno)>>1);
					/* l_band border (global) */
					l_band->x0 = opj_int64_ceildivpow2(l_tilec->x0 - ((OPJ_INT64)l_x0b << l_level_no), (OPJ_INT32)(l_level_no + 1));
					l_band->y0 = opj_int64_ceildivpow2(l_tilec->y0 - ((OPJ_INT64)l_y0b << l_level_no), (OPJ_INT32)(l_level_no + 1));
					l_band->x1 = opj_int64_ceildivpow2(l_tilec->x1 - ((OPJ_INT64)l_x0b << l_level_no), (OPJ_INT32)(l_level_no + 1));
					l_band->y1 = opj_int64_ceildivpow2(l_tilec->y1 - ((OPJ_INT64)l_y0b << l_level_no), (OPJ_INT32)(l_level_no + 1));
				}
				
				/** avoid an if with storing function pointer */
				l_gain = (*l_gain_ptr) (l_band->bandno);
				numbps = (OPJ_INT32)(l_image_comp->prec + l_gain);
				l_band->stepsize = (OPJ_FLOAT32)(((1.0 + l_step_size->mant / 2048.0) * pow(2.0, (OPJ_INT32) (numbps - l_step_size->expn)))) * fraction;
				l_band->numbps = l_step_size->expn + (OPJ_INT32)l_tccp->numgbits - 1;      /* WHY -1 ? */
				
				if (!l_band->precincts && (l_nb_precincts > 0U)) {
					l_band->precincts = (opj_tcd_precinct_t *) opj_malloc( /*3 * */ l_nb_precinct_size);
					if (! l_band->precincts) {
						return OPJ_FALSE;
					}
					/*fprintf(stderr, "\t\t\t\tAllocate precincts of a band (opj_tcd_precinct_t): %d\n",l_nb_precinct_size);     */
					memset(l_band->precincts,0,l_nb_precinct_size);
					l_band->precincts_data_size = l_nb_precinct_size;
				}
				else if (l_band->precincts_data_size < l_nb_precinct_size) {
					
					opj_tcd_precinct_t * new_precincts = (opj_tcd_precinct_t *) opj_realloc(l_band->precincts,/*3 * */ l_nb_precinct_size);
					if (! new_precincts) {
						opj_event_msg(manager, EVT_ERROR, "Not enough memory to handle band precints\n");
						opj_free(l_band->precincts);
						l_band->precincts = NULL;
						l_band->precincts_data_size = 0;
						return OPJ_FALSE;
					}
					l_band->precincts = new_precincts;
					/*fprintf(stderr, "\t\t\t\tReallocate precincts of a band (opj_tcd_precinct_t): from %d to %d\n",l_band->precincts_data_size, l_nb_precinct_size);*/
					memset(((OPJ_BYTE *) l_band->precincts) + l_band->precincts_data_size,0,l_nb_precinct_size - l_band->precincts_data_size);
					l_band->precincts_data_size = l_nb_precinct_size;
				}
				
				l_current_precinct = l_band->precincts;
				for (precno = 0; precno < l_nb_precincts; ++precno) {
					OPJ_INT32 tlcblkxstart, tlcblkystart, brcblkxend, brcblkyend;
					OPJ_INT32 cbgxstart = tlcbgxstart + (OPJ_INT32)(precno % l_res->pw) * (1 << cbgwidthexpn);
					OPJ_INT32 cbgystart = tlcbgystart + (OPJ_INT32)(precno / l_res->pw) * (1 << cbgheightexpn);
					OPJ_INT32 cbgxend = cbgxstart + (1 << cbgwidthexpn);
					OPJ_INT32 cbgyend = cbgystart + (1 << cbgheightexpn);
					/*fprintf(stderr, "\t precno=%d; bandno=%d, resno=%d; compno=%d\n", precno, bandno , resno, compno);*/
					/*fprintf(stderr, "\t tlcbgxstart(=%d) + (precno(=%d) percent res->pw(=%d)) * (1 << cbgwidthexpn(=%d)) \n",tlcbgxstart,precno,l_res->pw,cbgwidthexpn);*/
					
					/* precinct size (global) */
					/*fprintf(stderr, "\t cbgxstart=%d, l_band->x0 = %d \n",cbgxstart, l_band->x0);*/
					
					l_current_precinct->x0 = opj_int_max(cbgxstart, l_band->x0);
					l_current_precinct->y0 = opj_int_max(cbgystart, l_band->y0);
					l_current_precinct->x1 = opj_int_min(cbgxend, l_band->x1);
					l_current_precinct->y1 = opj_int_min(cbgyend, l_band->y1);
					/*fprintf(stderr, "\t prc_x0=%d; prc_y0=%d, prc_x1=%d; prc_y1=%d\n",l_current_precinct->x0, l_current_precinct->y0 ,l_current_precinct->x1, l_current_precinct->y1);*/
					
					tlcblkxstart = opj_int_floordivpow2(l_current_precinct->x0, (OPJ_INT32)cblkwidthexpn) << cblkwidthexpn;
					/*fprintf(stderr, "\t tlcblkxstart =%d\n",tlcblkxstart );*/
					tlcblkystart = opj_int_floordivpow2(l_current_precinct->y0, (OPJ_INT32)cblkheightexpn) << cblkheightexpn;
					/*fprintf(stderr, "\t tlcblkystart =%d\n",tlcblkystart );*/
					brcblkxend = opj_int_ceildivpow2(l_current_precinct->x1, (OPJ_INT32)cblkwidthexpn) << cblkwidthexpn;
					/*fprintf(stderr, "\t brcblkxend =%d\n",brcblkxend );*/
					brcblkyend = opj_int_ceildivpow2(l_current_precinct->y1, (OPJ_INT32)cblkheightexpn) << cblkheightexpn;
					/*fprintf(stderr, "\t brcblkyend =%d\n",brcblkyend );*/
					l_current_precinct->cw = (OPJ_UINT32)((brcblkxend - tlcblkxstart) >> cblkwidthexpn);
					l_current_precinct->ch = (OPJ_UINT32)((brcblkyend - tlcblkystart) >> cblkheightexpn);
					
					l_nb_code_blocks = l_current_precinct->cw * l_current_precinct->ch;
					/*fprintf(stderr, "\t\t\t\t precinct_cw = %d x recinct_ch = %d\n",l_current_precinct->cw, l_current_precinct->ch);      */
					l_nb_code_blocks_size = l_nb_code_blocks * (OPJ_UINT32)sizeof_block;
					
					if (!l_current_precinct->cblks.blocks && (l_nb_code_blocks > 0U)) {
						l_current_precinct->cblks.blocks = opj_malloc(l_nb_code_blocks_size);
						if (! l_current_precinct->cblks.blocks ) {
							return OPJ_FALSE;
						}
						/*fprintf(stderr, "\t\t\t\tAllocate cblks of a precinct (opj_tcd_cblk_dec_t): %d\n",l_nb_code_blocks_size);*/
						
						memset(l_current_precinct->cblks.blocks,0,l_nb_code_blocks_size);
						
						l_current_precinct->block_size = l_nb_code_blocks_size;
					}
					else if (l_nb_code_blocks_size > l_current_precinct->block_size) {
						void *new_blocks = opj_realloc(l_current_precinct->cblks.blocks, l_nb_code_blocks_size);
						if (! new_blocks) {
							opj_free(l_current_precinct->cblks.blocks);
							l_current_precinct->cblks.blocks = NULL;
							l_current_precinct->block_size = 0;
							opj_event_msg(manager, EVT_ERROR, "Not enough memory for current precinct codeblock element\n");
							return OPJ_FALSE;
						}
						l_current_precinct->cblks.blocks = new_blocks;
						/*fprintf(stderr, "\t\t\t\tReallocate cblks of a precinct (opj_tcd_cblk_dec_t): from %d to %d\n",l_current_precinct->block_size, l_nb_code_blocks_size);     */
						
						memset(((OPJ_BYTE *) l_current_precinct->cblks.blocks) + l_current_precinct->block_size
									 ,0
									 ,l_nb_code_blocks_size - l_current_precinct->block_size);
						
						l_current_precinct->block_size = l_nb_code_blocks_size;
					}
					
					if (! l_current_precinct->incltree) {
						l_current_precinct->incltree = opj_tgt_create(l_current_precinct->cw, l_current_precinct->ch, manager);
					}
					else{
						l_current_precinct->incltree = opj_tgt_init(l_current_precinct->incltree, l_current_precinct->cw, l_current_precinct->ch, manager);
					}

					if (! l_current_precinct->incltree)     {
						opj_event_msg(manager, EVT_WARNING, "No incltree created.\n");
						/*return OPJ_FALSE;*/
					}

					if (! l_current_precinct->imsbtree) {
						l_current_precinct->imsbtree = opj_tgt_create(l_current_precinct->cw, l_current_precinct->ch, manager);
					}
					else {
						l_current_precinct->imsbtree = opj_tgt_init(l_current_precinct->imsbtree, l_current_precinct->cw, l_current_precinct->ch, manager);
					}

					if (! l_current_precinct->imsbtree) {
						opj_event_msg(manager, EVT_WARNING, "No imsbtree created.\n");
						/*return OPJ_FALSE;*/
					}

					for (cblkno = 0; cblkno < l_nb_code_blocks; ++cblkno) {
						OPJ_INT32 cblkxstart = tlcblkxstart + (OPJ_INT32)(cblkno % l_current_precinct->cw) * (1 << cblkwidthexpn);
						OPJ_INT32 cblkystart = tlcblkystart + (OPJ_INT32)(cblkno / l_current_precinct->cw) * (1 << cblkheightexpn);
						OPJ_INT32 cblkxend = cblkxstart + (1 << cblkwidthexpn);
						OPJ_INT32 cblkyend = cblkystart + (1 << cblkheightexpn);
						
						if (isEncoder) {
							opj_tcd_cblk_enc_t* l_code_block = l_current_precinct->cblks.enc + cblkno;
							
							if (! opj_tcd_code_block_enc_allocate(l_code_block)) {
								return OPJ_FALSE;
							}
							/* code-block size (global) */
							l_code_block->x0 = opj_int_max(cblkxstart, l_current_precinct->x0);
							l_code_block->y0 = opj_int_max(cblkystart, l_current_precinct->y0);
							l_code_block->x1 = opj_int_min(cblkxend, l_current_precinct->x1);
							l_code_block->y1 = opj_int_min(cblkyend, l_current_precinct->y1);
							
							if (! opj_tcd_code_block_enc_allocate_data(l_code_block)) {
								return OPJ_FALSE;
							}
						} else {
							opj_tcd_cblk_dec_t* l_code_block = l_current_precinct->cblks.dec + cblkno;
							
							if (! opj_tcd_code_block_dec_allocate(l_code_block)) {
								return OPJ_FALSE;
							}
							/* code-block size (global) */
							l_code_block->x0 = opj_int_max(cblkxstart, l_current_precinct->x0);
							l_code_block->y0 = opj_int_max(cblkystart, l_current_precinct->y0);
							l_code_block->x1 = opj_int_min(cblkxend, l_current_precinct->x1);
							l_code_block->y1 = opj_int_min(cblkyend, l_current_precinct->y1);
						}
					}
					++l_current_precinct;
				} /* precno */
				++l_band;
				++l_step_size;
			} /* bandno */
			++l_res;
		} /* resno */
		++l_tccp;
		++l_tilec;
		++l_image_comp;
	} /* compno */
	return OPJ_TRUE;
}