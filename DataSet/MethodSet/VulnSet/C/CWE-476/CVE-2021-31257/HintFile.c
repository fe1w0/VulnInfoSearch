GF_Err HintFile(GF_ISOFile *file, u32 MTUSize, u32 max_ptime, u32 rtp_rate, u32 base_flags, Bool copy_data, Bool interleave, Bool regular_iod, Bool single_group, Bool hint_no_offset)
{
	GF_ESD *esd;
	GF_InitialObjectDescriptor *iod;
	u32 i, val, res, streamType;
	u32 sl_mode, prev_ocr, single_ocr, nb_done, tot_bw, bw, flags, spec_type;
	GF_Err e;
	char szPayload[30];
	GF_RTPHinter *hinter;
	Bool copy, has_iod, single_av;
	u8 init_payt = BASE_PAYT;
	u32 mtype;
	GF_SDP_IODProfile iod_mode = GF_SDP_IOD_NONE;
	u32 media_group = 0;
	u8 media_prio = 0;

	tot_bw = 0;
	prev_ocr = 0;
	single_ocr = 1;

	has_iod = 1;
	iod = (GF_InitialObjectDescriptor *) gf_isom_get_root_od(file);
	if (!iod) has_iod = 0;
	else {
		if (!gf_list_count(iod->ESDescriptors)) has_iod = 0;
		gf_odf_desc_del((GF_Descriptor *) iod);
	}

	spec_type = gf_isom_guess_specification(file);
	single_av = single_group ? 1 : gf_isom_is_single_av(file);

	/*first make sure we use a systems track as base OCR*/
	for (i=0; i<gf_isom_get_track_count(file); i++) {
		res = gf_isom_get_media_type(file, i+1);
		if ((res==GF_ISOM_MEDIA_SCENE) || (res==GF_ISOM_MEDIA_OD)) {
			if (gf_isom_is_track_in_root_od(file, i+1)) {
				gf_isom_set_default_sync_track(file, i+1);
				break;
			}
		}
	}

	nb_done = 0;
	for (i=0; i<gf_isom_get_track_count(file); i++) {
		sl_mode = base_flags;
		copy = copy_data;
		/*skip emty tracks (mainly MPEG-4 interaction streams...*/
		if (!gf_isom_get_sample_count(file, i+1)) continue;
		if (!gf_isom_is_track_enabled(file, i+1)) {
			M4_LOG(GF_LOG_INFO, ("Track ID %d disabled - skipping hint\n", gf_isom_get_track_id(file, i+1) ));
			continue;
		}

		mtype = gf_isom_get_media_type(file, i+1);
		switch (mtype) {
		case GF_ISOM_MEDIA_VISUAL:
			if (single_av) {
				media_group = 2;
				media_prio = 2;
			}
			break;
        case GF_ISOM_MEDIA_AUXV:
            if (single_av) {
                media_group = 2;
                media_prio = 3;
            }
            break;
        case GF_ISOM_MEDIA_PICT:
            if (single_av) {
                media_group = 2;
                media_prio = 4;
            }
            break;
		case GF_ISOM_MEDIA_AUDIO:
			if (single_av) {
				media_group = 2;
				media_prio = 1;
			}
			break;
		case GF_ISOM_MEDIA_HINT:
			continue;
		default:
			/*no hinting of systems track on isma*/
			if (spec_type==GF_ISOM_BRAND_ISMA) continue;
		}
		mtype = gf_isom_get_media_subtype(file, i+1, 1);
		if ((mtype==GF_ISOM_SUBTYPE_MPEG4) || (mtype==GF_ISOM_SUBTYPE_MPEG4_CRYP) ) mtype = gf_isom_get_mpeg4_subtype(file, i+1, 1);

		if (!single_av) {
			/*one media per group only (we should prompt user for group selection)*/
			media_group ++;
			media_prio = 1;
		}

		streamType = 0;
		esd = gf_isom_get_esd(file, i+1, 1);
		if (esd) {
			streamType = esd->decoderConfig->streamType;
			if (!prev_ocr) {
				prev_ocr = esd->OCRESID;
				if (!esd->OCRESID) prev_ocr = esd->ESID;
			} else if (esd->OCRESID && prev_ocr != esd->OCRESID) {
				single_ocr = 0;
			}
			/*OD MUST BE WITHOUT REFERENCES*/
			if (streamType==1) copy = 1;
		}
		gf_odf_desc_del((GF_Descriptor *) esd);

		if (!regular_iod && gf_isom_is_track_in_root_od(file, i+1)) {
			/*single AU - check if base64 would fit in ESD (consider 33% overhead of base64), otherwise stream*/
			if (gf_isom_get_sample_count(file, i+1)==1) {
				GF_ISOSample *samp = gf_isom_get_sample(file, i+1, 1, &val);
				if (streamType) {
					res = gf_hinter_can_embbed_data(samp->data, samp->dataLength, streamType);
				} else {
					/*not a system track, we shall hint it*/
					res = 0;
				}
				if (samp) gf_isom_sample_del(&samp);
				if (res) continue;
			}
		}
		if (interleave) sl_mode |= GP_RTP_PCK_USE_INTERLEAVING;

		hinter = gf_hinter_track_new(file, i+1, MTUSize, max_ptime, rtp_rate, sl_mode, init_payt, copy, media_group, media_prio, &e);

		if (!hinter) {
			if (e) {
				M4_LOG(nb_done ? GF_LOG_WARNING : GF_LOG_ERROR, ("Cannot create hinter (%s)\n", gf_error_to_string(e) ));
				if (!nb_done) return e;
			}
			continue;
		}

		if (hint_no_offset)
			gf_hinter_track_force_no_offsets(hinter);

		bw = gf_hinter_track_get_bandwidth(hinter);
		tot_bw += bw;
		flags = gf_hinter_track_get_flags(hinter);

		//set extraction mode for AVC/SVC
		gf_isom_set_nalu_extract_mode(file, i+1, GF_ISOM_NALU_EXTRACT_LAYER_ONLY);

		gf_hinter_track_get_payload_name(hinter, szPayload);
		M4_LOG(GF_LOG_INFO, ("Hinting track ID %d - Type \"%s:%s\" (%s) - BW %d kbps\n", gf_isom_get_track_id(file, i+1), gf_4cc_to_str(mtype), gf_4cc_to_str(mtype), szPayload, bw));
		if (flags & GP_RTP_PCK_SYSTEMS_CAROUSEL) M4_LOG(GF_LOG_INFO, ("\tMPEG-4 Systems stream carousel enabled\n"));
		e = gf_hinter_track_process(hinter);

		if (!e) e = gf_hinter_track_finalize(hinter, has_iod);
		gf_hinter_track_del(hinter);

		if (e) {
			M4_LOG(GF_LOG_ERROR, ("Error while hinting (%s)\n", gf_error_to_string(e)));
			if (!nb_done) return e;
		}
		init_payt++;
		nb_done ++;
	}

	if (has_iod) {
		iod_mode = GF_SDP_IOD_ISMA;
		if (regular_iod) iod_mode = GF_SDP_IOD_REGULAR;
	} else {
		iod_mode = GF_SDP_IOD_NONE;
	}
	gf_hinter_finalize(file, iod_mode, tot_bw);

	if (!single_ocr)
		M4_LOG(GF_LOG_WARNING, ("Warning: at least 2 timelines found in the file\nThis may not be supported by servers/players\n\n"));

	return GF_OK;
}