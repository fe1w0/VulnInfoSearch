static GF_Err gf_isom_parse_movie_boxes_internal(GF_ISOFile *mov, u32 *boxType, u64 *bytesMissing, Bool progressive_mode)
{
	GF_Box *a;
	u64 totSize, mdat_end=0;
	GF_Err e = GF_OK;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (mov->single_moof_mode && mov->single_moof_state == 2) {
		return e;
	}

	/*restart from where we stopped last*/
	totSize = mov->current_top_box_start;
	if (mov->bytes_removed) {
		assert(totSize >= mov->bytes_removed);
		totSize -= mov->bytes_removed;
	}
	gf_bs_seek(mov->movieFileMap->bs, totSize);
#endif


	/*while we have some data, parse our boxes*/
	while (gf_bs_available(mov->movieFileMap->bs)) {
		*bytesMissing = 0;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
		mov->current_top_box_start = gf_bs_get_position(mov->movieFileMap->bs) + mov->bytes_removed;
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[iso file] Parsing a top-level box at position %d\n", mov->current_top_box_start));
#endif

		e = gf_isom_parse_root_box(&a, mov->movieFileMap->bs, boxType, bytesMissing, progressive_mode);

		if (e >= 0) {

		} else if (e == GF_ISOM_INCOMPLETE_FILE) {
			/*our mdat is uncomplete, only valid for READ ONLY files...*/
			if (mov->openMode != GF_ISOM_OPEN_READ) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Incomplete MDAT while file is not read-only\n"));
				return GF_ISOM_INVALID_FILE;
			}
			if ((mov->openMode == GF_ISOM_OPEN_READ) && !progressive_mode) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Incomplete file while reading for dump - aborting parsing\n"));
				break;
			}
			return e;
		} else {
			return e;
		}

		switch (a->type) {
		/*MOOV box*/
		case GF_ISOM_BOX_TYPE_MOOV:
			if (mov->moov) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Duplicate MOOV detected!\n"));
				gf_isom_box_del(a);
				return GF_ISOM_INVALID_FILE;
			}
			mov->moov = (GF_MovieBox *)a;
			mov->original_moov_offset = mov->current_top_box_start;
			/*set our pointer to the movie*/
			mov->moov->mov = mov;
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
			if (mov->moov->mvex) mov->moov->mvex->mov = mov;

#ifdef GF_ENABLE_CTRN
			if (! (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG)) {
				gf_isom_setup_traf_inheritance(mov);
			}
#endif

#endif
			e = gf_list_add(mov->TopBoxes, a);
			if (e) return e;

			totSize += a->size;

            if (!mov->moov->mvhd) {
                GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MovieHeaderBox\n"));
                return GF_ISOM_INVALID_FILE;
            }

            if (mov->meta) {
				gf_isom_meta_restore_items_ref(mov, mov->meta);
			}

			//dump senc info in dump mode
			if (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG) {
				u32 k;
				for (k=0; k<gf_list_count(mov->moov->trackList); k++) {
					GF_TrackBox *trak = (GF_TrackBox *)gf_list_get(mov->moov->trackList, k);

					if (trak->sample_encryption) {
						e = senc_Parse(mov->movieFileMap->bs, trak, NULL, trak->sample_encryption);
						if (e) return e;
					}
				}
			} else {
				u32 k;
				for (k=0; k<gf_list_count(mov->moov->trackList); k++) {
					GF_TrackBox *trak = (GF_TrackBox *)gf_list_get(mov->moov->trackList, k);
					if (trak->Media->information->sampleTable->sampleGroups) {
						convert_compact_sample_groups(trak->Media->information->sampleTable->child_boxes, trak->Media->information->sampleTable->sampleGroups);
					}
				}
			}

            if (mdat_end && mov->signal_frag_bounds && !(mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG) ) {
                gf_isom_push_mdat_end(mov, mdat_end);
                mdat_end=0;
            }
			break;

		/*META box*/
		case GF_ISOM_BOX_TYPE_META:
			if (mov->meta) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Duplicate META detected!\n"));
				gf_isom_box_del(a);
				return GF_ISOM_INVALID_FILE;
			}
			mov->meta = (GF_MetaBox *)a;
			mov->original_meta_offset = mov->current_top_box_start;
			e = gf_list_add(mov->TopBoxes, a);
			if (e) {
				return e;
			}
			totSize += a->size;
			gf_isom_meta_restore_items_ref(mov, mov->meta);
			break;

		/*we only keep the MDAT in READ for dump purposes*/
		case GF_ISOM_BOX_TYPE_MDAT:
			if (!mov->first_data_toplevel_offset) {
				mov->first_data_toplevel_offset = mov->current_top_box_start;
				mov->first_data_toplevel_size = a->size;
			}
			totSize += a->size;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
			if (mov->emsgs) {
				gf_isom_box_array_del(mov->emsgs);
				mov->emsgs = NULL;
			}
#endif

			if (mov->openMode == GF_ISOM_OPEN_READ) {
				if (!mov->mdat) {
					mov->mdat = (GF_MediaDataBox *) a;
					e = gf_list_add(mov->TopBoxes, mov->mdat);
					if (e) {
						return e;
					}
				}
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
				else if (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG) gf_list_add(mov->TopBoxes, a);
#endif
				else gf_isom_box_del(a); //in other modes we don't care


				if (mov->signal_frag_bounds && !(mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG) ) {
                    mdat_end = gf_bs_get_position(mov->movieFileMap->bs);
                    if (mov->moov) {
                        gf_isom_push_mdat_end(mov, mdat_end);
                        mdat_end=0;
                    }
				}
			}
			/*if we don't have any MDAT yet, create one (edit-write mode)
			We only work with one mdat, but we're puting it at the place
			of the first mdat found when opening a file for editing*/
			else if (!mov->mdat && (mov->openMode != GF_ISOM_OPEN_READ) && (mov->openMode != GF_ISOM_OPEN_KEEP_FRAGMENTS)) {
				gf_isom_box_del(a);
				mov->mdat = (GF_MediaDataBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MDAT);
				if (!mov->mdat) return GF_OUT_OF_MEM;
				e = gf_list_add(mov->TopBoxes, mov->mdat);
				if (e) {
					return e;
				}
			} else {
				gf_isom_box_del(a);
			}
			break;
		case GF_ISOM_BOX_TYPE_FTYP:
			/*ONE AND ONLY ONE FTYP*/
			if (mov->brand) {
				gf_isom_box_del(a);
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Duplicate 'ftyp' detected!\n"));
				return GF_ISOM_INVALID_FILE;
			}
			mov->brand = (GF_FileTypeBox *)a;
			totSize += a->size;
			e = gf_list_add(mov->TopBoxes, a);
			if (e) return e;
			break;

		case GF_ISOM_BOX_TYPE_OTYP:
			/*ONE AND ONLY ONE FTYP*/
			if (mov->otyp) {
				gf_isom_box_del(a);
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Duplicate 'otyp' detected!\n"));
				return GF_ISOM_INVALID_FILE;
			}

			if (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG) {
				mov->otyp = (GF_Box *)a;
				totSize += a->size;
				e = gf_list_add(mov->TopBoxes, a);
				if (e) return e;
			} else {
				GF_FileTypeBox *brand = (GF_FileTypeBox *) gf_isom_box_find_child(a->child_boxes, GF_ISOM_BOX_TYPE_FTYP);
				if (brand) {
					s32 pos;
					gf_list_del_item(a->child_boxes, brand);
					pos = gf_list_del_item(mov->TopBoxes, mov->brand);
					gf_isom_box_del((GF_Box *) mov->brand);
					mov->brand = brand;
					if (pos<0) pos=0;
					gf_list_insert(mov->TopBoxes, brand, pos);
				}
				gf_isom_box_del(a);
			}
			break;

		case GF_ISOM_BOX_TYPE_PDIN:
			/*ONE AND ONLY ONE PDIN*/
			if (mov->pdin) {
				gf_isom_box_del(a);
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Duplicate 'pdin'' detected!\n"));
				return GF_ISOM_INVALID_FILE;
			}
			mov->pdin = (GF_ProgressiveDownloadBox *) a;
			totSize += a->size;
			e = gf_list_add(mov->TopBoxes, a);
			if (e) return e;
			break;


#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
		case GF_ISOM_BOX_TYPE_STYP:
		{
			u32 brand = ((GF_FileTypeBox *)a)->majorBrand;
			switch (brand) {
			case GF_ISOM_BRAND_SISX:
			case GF_ISOM_BRAND_RISX:
			case GF_ISOM_BRAND_SSSS:
				mov->is_index_segment = GF_TRUE;
				break;
			default:
				break;
			}
		}
		/*fall-through*/

		case GF_ISOM_BOX_TYPE_SIDX:
		case GF_ISOM_BOX_TYPE_SSIX:
			if (mov->moov && !mov->first_data_toplevel_offset) {
				mov->first_data_toplevel_offset = mov->current_top_box_start;
				mov->first_data_toplevel_size = a->size;
			}
			totSize += a->size;
			if (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG) {
				e = gf_list_add(mov->TopBoxes, a);
				if (e) return e;
			} else if (mov->signal_frag_bounds && !(mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG)  && (mov->openMode!=GF_ISOM_OPEN_KEEP_FRAGMENTS)
			) {
				if (a->type==GF_ISOM_BOX_TYPE_SIDX) {
					if (mov->root_sidx) gf_isom_box_del( (GF_Box *) mov->root_sidx);
					mov->root_sidx = (GF_SegmentIndexBox *) a;
					mov->sidx_start_offset = mov->current_top_box_start;
					mov->sidx_end_offset = gf_bs_get_position(mov->movieFileMap->bs);

				}
				else if (a->type==GF_ISOM_BOX_TYPE_STYP) {
					mov->styp_start_offset = mov->current_top_box_start;

					if (mov->seg_styp) gf_isom_box_del(mov->seg_styp);
					mov->seg_styp = a;
				} else if (a->type==GF_ISOM_BOX_TYPE_SSIX) {
					if (mov->seg_ssix) gf_isom_box_del(mov->seg_ssix);
					mov->seg_ssix = a;
				} else {
					gf_isom_box_del(a);
				}
				gf_isom_push_mdat_end(mov, mov->current_top_box_start);
			} else if (!mov->NextMoofNumber && (a->type==GF_ISOM_BOX_TYPE_SIDX)) {
				if (mov->main_sidx) gf_isom_box_del( (GF_Box *) mov->main_sidx);
				mov->main_sidx = (GF_SegmentIndexBox *) a;
				mov->main_sidx_end_pos = mov->current_top_box_start + a->size;
			} else {
				gf_isom_box_del(a);
			}
			break;

		case GF_ISOM_BOX_TYPE_MOOF:
			//no support for inplace rewrite for fragmented files
			gf_isom_disable_inplace_rewrite(mov);
			if (!mov->moov) {
				GF_LOG(mov->moof ? GF_LOG_DEBUG : GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Movie fragment but no moov (yet) - possibly broken parsing!\n"));
			}
			if (mov->single_moof_mode) {
				mov->single_moof_state++;
				if (mov->single_moof_state > 1) {
					gf_isom_box_del(a);
					return GF_OK;
				}
			}
			((GF_MovieFragmentBox *)a)->mov = mov;

			totSize += a->size;
			mov->moof = (GF_MovieFragmentBox *) a;

			/*some smooth streaming streams contain a SDTP under the TRAF: this is incorrect, convert it*/
			FixTrackID(mov);
			if (! (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG)) {
				FixSDTPInTRAF(mov->moof);
			} else {
				u32 k;
				for (k=0; k<gf_list_count(mov->moof->TrackList); k++) {
					GF_TrackFragmentBox *traf = (GF_TrackFragmentBox *)gf_list_get(mov->moof->TrackList, k);
					if (traf->sampleGroups) {
						convert_compact_sample_groups(traf->child_boxes, traf->sampleGroups);
					}
				}
			}

			/*read & debug: store at root level*/
			if (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG) {
				u32 k;
				gf_list_add(mov->TopBoxes, a);
				/*also update pointers to trex for debug*/
				if (mov->moov) {
					for (k=0; k<gf_list_count(mov->moof->TrackList); k++) {
						GF_TrackFragmentBox *traf = gf_list_get(mov->moof->TrackList, k);
						if (traf->tfhd && mov->moov->mvex && mov->moov->mvex->TrackExList) {
							GF_TrackBox *trak = gf_isom_get_track_from_id(mov->moov, traf->tfhd->trackID);
							u32 j=0;
							while ((traf->trex = (GF_TrackExtendsBox*)gf_list_enum(mov->moov->mvex->TrackExList, &j))) {
								if (traf->trex->trackID == traf->tfhd->trackID) {
									if (!traf->trex->track) traf->trex->track = trak;
									break;
								}
								traf->trex = NULL;
							}
						}
						//we should only parse senc/psec when no saiz/saio is present, otherwise we fetch the info directly
						if (traf->trex && traf->tfhd && traf->trex->track && traf->sample_encryption) {
							GF_TrackBox *trak = GetTrackbyID(mov->moov, traf->tfhd->trackID);
							if (trak) {
								trak->current_traf_stsd_idx = traf->tfhd->sample_desc_index ? traf->tfhd->sample_desc_index : traf->trex->def_sample_desc_index;
								e = senc_Parse(mov->movieFileMap->bs, trak, traf, traf->sample_encryption);
								if (e) return e;
								trak->current_traf_stsd_idx = 0;
							}
						}
					}
				} else {
					for (k=0; k<gf_list_count(mov->moof->TrackList); k++) {
						GF_TrackFragmentBox *traf = gf_list_get(mov->moof->TrackList, k);
						if (traf->sample_encryption) {
							e = senc_Parse(mov->movieFileMap->bs, NULL, traf, traf->sample_encryption);
							if (e) return e;
						}
					}

				}
			} else if (mov->openMode==GF_ISOM_OPEN_KEEP_FRAGMENTS) {
				mov->NextMoofNumber = mov->moof->mfhd->sequence_number+1;
				mov->moof = NULL;
				gf_isom_box_del(a);
			} else {
				/*merge all info*/
				e = MergeFragment((GF_MovieFragmentBox *)a, mov);
				gf_isom_box_del(a);
				if (e) return e;
			}

			//done with moov
			if (mov->root_sidx) {
				gf_isom_box_del((GF_Box *) mov->root_sidx);
				mov->root_sidx = NULL;
			}
			if (mov->root_ssix) {
				gf_isom_box_del(mov->seg_ssix);
				mov->root_ssix = NULL;
			}
			if (mov->seg_styp) {
				gf_isom_box_del(mov->seg_styp);
				mov->seg_styp = NULL;
			}
			mov->sidx_start_offset = 0;
			mov->sidx_end_offset = 0;
			mov->styp_start_offset = 0;
			break;
#endif
		case GF_ISOM_BOX_TYPE_UNKNOWN:
		{
			GF_UnknownBox *box = (GF_UnknownBox*)a;
			if (box->original_4cc == GF_ISOM_BOX_TYPE_JP) {
				u8 *c = (u8 *) box->data;
				if ((box->dataSize==4) && (GF_4CC(c[0],c[1],c[2],c[3])==(u32)0x0D0A870A))
					mov->is_jp2 = 1;
				gf_isom_box_del(a);
			} else {
				e = gf_list_add(mov->TopBoxes, a);
				if (e) return e;
			}
		}
		break;

		case GF_ISOM_BOX_TYPE_PRFT:
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
			if (!(mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG)) {
				//keep the last one read
				if (mov->last_producer_ref_time)
					gf_isom_box_del(a);
				else
					mov->last_producer_ref_time = (GF_ProducerReferenceTimeBox *)a;
				break;
			}
#endif
		//fallthrough
		case GF_ISOM_BOX_TYPE_EMSG:
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
			if (! (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG)) {
				if (!mov->emsgs) mov->emsgs = gf_list_new();
				gf_list_add(mov->emsgs, a);
				break;
			}
#endif
		case GF_ISOM_BOX_TYPE_MFRA:
		case GF_ISOM_BOX_TYPE_MFRO:
			//only keep for dump mode, otherwise we ignore these boxes and we don't want to carry them over in non-fragmented file
			if (! (mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG)) {
				totSize += a->size;
				gf_isom_box_del(a);
				break;
			}
		default:
			totSize += a->size;
			e = gf_list_add(mov->TopBoxes, a);
			if (e) return e;
			break;
		}

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
		/*remember where we left, in case we append an entire number of movie fragments*/
		mov->current_top_box_start = gf_bs_get_position(mov->movieFileMap->bs) + mov->bytes_removed;
#endif
	}

	/*we need at least moov or meta*/
	if (!mov->moov && !mov->meta
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	        && !mov->moof && !mov->is_index_segment
#endif
	   ) {
		return GF_ISOM_INCOMPLETE_FILE;
	}
	/*we MUST have movie header*/
	if (!gf_opts_get_bool("core", "no-check")) {
		if (mov->moov && !mov->moov->mvhd) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MVHD in MOOV!\n"));
			return GF_ISOM_INVALID_FILE;
		}

		/*we MUST have meta handler*/
		if (mov->meta && !mov->meta->handler) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing handler in META!\n"));
			return GF_ISOM_INVALID_FILE;
		}
	}

#ifndef GPAC_DISABLE_ISOM_WRITE

	if (mov->moov) {
		/*set the default interleaving time*/
		mov->interleavingTime = mov->moov->mvhd->timeScale;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
		/*in edit mode with successfully loaded fragments, delete all fragment signaling since
		file is no longer fragmented*/
		if ((mov->openMode > GF_ISOM_OPEN_READ) && (mov->openMode != GF_ISOM_OPEN_KEEP_FRAGMENTS) && mov->moov->mvex) {
			gf_isom_box_del_parent(&mov->moov->child_boxes, (GF_Box *)mov->moov->mvex);
			mov->moov->mvex = NULL;
		}
#endif

	}

	//create a default mdat if none was found
	if (!mov->mdat && (mov->openMode != GF_ISOM_OPEN_READ) && (mov->openMode != GF_ISOM_OPEN_KEEP_FRAGMENTS)) {
		mov->mdat = (GF_MediaDataBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MDAT);
		if (!mov->mdat) return GF_OUT_OF_MEM;
		e = gf_list_add(mov->TopBoxes, mov->mdat);
		if (e) return e;
	}
#endif /*GPAC_DISABLE_ISOM_WRITE*/

	return GF_OK;
}