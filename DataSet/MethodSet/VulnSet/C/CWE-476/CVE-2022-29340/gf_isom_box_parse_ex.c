GF_Err gf_isom_box_parse_ex(GF_Box **outBox, GF_BitStream *bs, u32 parent_type, Bool is_root_box, u64 parent_size)
{
	u32 type, uuid_type, hdr_size, restore_type;
	u64 size, start, comp_start, end;
	char uuid[16];
	GF_Err e;
	GF_BitStream *uncomp_bs = NULL;
	u8 *uncomp_data = NULL;
	u32 compressed_size=0;
	GF_Box *newBox;
	Bool skip_logs = (gf_bs_get_cookie(bs) & GF_ISOM_BS_COOKIE_NO_LOGS ) ? GF_TRUE : GF_FALSE;
	Bool is_special = GF_TRUE;
	
	if ((bs == NULL) || (outBox == NULL) ) return GF_BAD_PARAM;
	*outBox = NULL;
	if (gf_bs_available(bs) < 8) {
		return GF_ISOM_INCOMPLETE_FILE;
	}

	comp_start = start = gf_bs_get_position(bs);

	uuid_type = 0;
	size = (u64) gf_bs_read_u32(bs);
	hdr_size = 4;
	/*fix for some boxes found in some old hinted files*/
	if ((size >= 2) && (size <= 4)) {
		size = 4;
		type = GF_ISOM_BOX_TYPE_VOID;
	} else {
		type = gf_bs_read_u32(bs);
		hdr_size += 4;
		/*no size means till end of file - EXCEPT FOR some old QuickTime boxes...*/
		if (type == GF_ISOM_BOX_TYPE_TOTL)
			size = 12;
		if (!size) {
			if (is_root_box) {
				if (!skip_logs) {
					GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[iso file] Warning Read Box type %s (0x%08X) size 0 reading till the end of file\n", gf_4cc_to_str(type), type));
				}
				size = gf_bs_available(bs) + 8;
			} else {
				if (!skip_logs) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Read Box type %s (0x%08X) at position "LLU" has size 0 but is not at root/file level. Forbidden, skipping end of parent box !\n", gf_4cc_to_str(type), type, start));
					return GF_SKIP_BOX;
				}
				return GF_OK;
			}
		}
		if (is_root_box && (size>=8)) {
			Bool do_uncompress = GF_FALSE;
			u8 *compb = NULL;
			u32 osize = 0;
			u32 otype = type;
			if (type==GF_4CC('!', 'm', 'o', 'f')) {
				do_uncompress = GF_TRUE;
				type = GF_ISOM_BOX_TYPE_MOOF;
			}
			else if (type==GF_4CC('!', 'm', 'o', 'v')) {
				do_uncompress = GF_TRUE;
				type = GF_ISOM_BOX_TYPE_MOOV;
			}
			else if (type==GF_4CC('!', 's', 'i', 'x')) {
				do_uncompress = GF_TRUE;
				type = GF_ISOM_BOX_TYPE_SIDX;
			}
			else if (type==GF_4CC('!', 's', 's', 'x')) {
				do_uncompress = GF_TRUE;
				type = GF_ISOM_BOX_TYPE_SSIX;
			}

			if (do_uncompress) {
				compb = gf_malloc((u32) (size-8));

				compressed_size = (u32) (size - 8);
				gf_bs_read_data(bs, compb, compressed_size);
				e = gf_gz_decompress_payload(compb, compressed_size, &uncomp_data, &osize);
				if (e) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Failed to uncompress payload for box type %s (0x%08X)\n", gf_4cc_to_str(otype), otype));
					return e;
				}

				//keep size as complete box size for tests below
				size = osize + 8;
				uncomp_bs = gf_bs_new(uncomp_data, osize, GF_BITSTREAM_READ);
				bs = uncomp_bs;
				start = 0;
				gf_free(compb);
			}
		}
	}
	/*handle uuid*/
	memset(uuid, 0, 16);
	if (type == GF_ISOM_BOX_TYPE_UUID ) {
		if (gf_bs_available(bs) < 16) {
			return GF_ISOM_INCOMPLETE_FILE;
		}
		gf_bs_read_data(bs, uuid, 16);
		hdr_size += 16;
		uuid_type = gf_isom_solve_uuid_box(uuid);
	}

	//handle large box
	if (size == 1) {
		if (gf_bs_available(bs) < 8) {
			return GF_ISOM_INCOMPLETE_FILE;
		}
		size = gf_bs_read_u64(bs);
		hdr_size += 8;
	}
	if (!skip_logs)
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[iso file] Read Box type %s size "LLD" start "LLD"\n", gf_4cc_to_str(type), size,  start));

	if ( size < hdr_size ) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Box %s size "LLD" less than box header size %d\n", gf_4cc_to_str(type), size, hdr_size));
		return GF_ISOM_INVALID_FILE;
	}
	//if parent size is given, make sure box fits within parent
	if (parent_size && (parent_size<size)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Box %s size "LLU" is larger than remaining parent size "LLU"\n", gf_4cc_to_str(type), size, parent_size ));
		return GF_ISOM_INVALID_FILE;
	}
	restore_type = 0;
	if ((parent_type==GF_ISOM_BOX_TYPE_STSD) && (type==GF_QT_SUBTYPE_RAW) ) {
		u64 cookie = gf_bs_get_cookie(bs);
		restore_type = type;
		if (cookie & GF_ISOM_BS_COOKIE_VISUAL_TRACK)
			type = GF_QT_SUBTYPE_RAW_VID;
		else
			type = GF_QT_SUBTYPE_RAW_AUD;

	}

	//some special boxes (references and track groups) are handled by a single generic box with an associated ref/group type
	if (parent_type && (parent_type == GF_ISOM_BOX_TYPE_TREF)) {
		newBox = gf_isom_box_new(GF_ISOM_BOX_TYPE_REFT);
		if (!newBox) return GF_OUT_OF_MEM;
		((GF_TrackReferenceTypeBox*)newBox)->reference_type = type;
	} else if (parent_type && (parent_type == GF_ISOM_BOX_TYPE_IREF)) {
		newBox = gf_isom_box_new(GF_ISOM_BOX_TYPE_REFI);
		if (!newBox) return GF_OUT_OF_MEM;
		((GF_ItemReferenceTypeBox*)newBox)->reference_type = type;
	} else if (parent_type && (parent_type == GF_ISOM_BOX_TYPE_TRGR)) {
		newBox = gf_isom_box_new(GF_ISOM_BOX_TYPE_TRGT);
		if (!newBox) return GF_OUT_OF_MEM;
		((GF_TrackGroupTypeBox*)newBox)->group_type = type;
	} else if (parent_type && (parent_type == GF_ISOM_BOX_TYPE_GRPL)) {
		newBox = gf_isom_box_new(GF_ISOM_BOX_TYPE_GRPT);
		if (!newBox) return GF_OUT_OF_MEM;
		((GF_EntityToGroupTypeBox*)newBox)->grouping_type = type;
	} else {
		//OK, create the box based on the type
		is_special = GF_FALSE;
		newBox = gf_isom_box_new_ex(uuid_type ? uuid_type : type, parent_type, skip_logs, is_root_box);
		if (!newBox) return GF_OUT_OF_MEM;
	}

	//OK, init and read this box
	if (type==GF_ISOM_BOX_TYPE_UUID && !is_special) {
		memcpy(((GF_UUIDBox *)newBox)->uuid, uuid, 16);
		((GF_UUIDBox *)newBox)->internal_4cc = uuid_type;
	}

	if (!newBox->type) newBox->type = type;
	if (restore_type)
		newBox->type = restore_type;

	end = gf_bs_available(bs);
	if (size - hdr_size > end ) {
		newBox->size = size - hdr_size - end;
		*outBox = newBox;
		return GF_ISOM_INCOMPLETE_FILE;
	}

	newBox->size = size - hdr_size;

	e = gf_isom_full_box_read(newBox, bs);
	if (!e) e = gf_isom_box_read(newBox, bs);
	if (e) {
		if (gf_opts_get_bool("core", "no-check"))
			e = GF_OK;
	}
	newBox->size = size;
	end = gf_bs_get_position(bs);

	if (uncomp_bs) {
		gf_free(uncomp_data);
		gf_bs_del(uncomp_bs);
		if (e) {
			gf_isom_box_del(newBox);
			*outBox = NULL;
			return e;
		}
		//move size to real bitstream offsets for tests below
		size -= 8;
		//remember compressed vs real size info for moof in order to properly recompute data_offset/base_data_offset
		if (type==GF_ISOM_BOX_TYPE_MOOF) {
			((GF_MovieFragmentBox *)newBox)->compressed_diff = (s32)size - (s32)compressed_size;
		}
		//remember compressed vs real size info for moov in order to properly recompute chunk offset
		else if (type==GF_ISOM_BOX_TYPE_MOOV) {
			((GF_MovieBox *)newBox)->compressed_diff = (s32)size - (s32)compressed_size;
			((GF_MovieBox *)newBox)->file_offset = comp_start;
		}
		//remember compressed vs real size info for dump
		else if (type==GF_ISOM_BOX_TYPE_SIDX) {
			((GF_SegmentIndexBox *)newBox)->compressed_diff = (s32)size - (s32)compressed_size;
		}
		//remember compressed vs real size info for dump
		else if (type==GF_ISOM_BOX_TYPE_SSIX) {
			((GF_SubsegmentIndexBox *)newBox)->compressed_diff = (s32)size - (s32)compressed_size;
		}
		newBox->internal_flags = GF_ISOM_BOX_COMPRESSED;
	}


	if (e && (e != GF_ISOM_INCOMPLETE_FILE)) {
		gf_isom_box_del(newBox);
		*outBox = NULL;

		if (!skip_logs) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Read Box \"%s\" (start "LLU") failed (%s) - skipping\n", gf_4cc_to_str(type), start, gf_error_to_string(e)));
		}
		//we don't try to reparse known boxes that have been failing (too dangerous)
		return e;
	}

	if (end-start > size) {
		if (!skip_logs) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Box \"%s\" size "LLU" (start "LLU") invalid (read "LLU")\n", gf_4cc_to_str(type), size, start, (end-start) ));
		}
		/*let's still try to load the file since no error was notified*/
		gf_bs_seek(bs, start+size);
	} else if (end-start < size) {
		u32 to_skip = (u32) (size-(end-start));
		if (!skip_logs) {
			if ((to_skip!=4) || gf_bs_peek_bits(bs, 32, 0)) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Box \"%s\" (start "LLU") has %u extra bytes\n", gf_4cc_to_str(type), start, to_skip));
				unused_bytes += to_skip;
			}
		}
		gf_bs_skip_bytes(bs, to_skip);
	}
	*outBox = newBox;

	return e;
}