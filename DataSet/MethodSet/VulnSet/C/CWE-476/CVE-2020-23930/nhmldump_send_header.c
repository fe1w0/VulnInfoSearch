static void nhmldump_send_header(GF_NHMLDumpCtx *ctx)
{
	GF_FilterPacket *dst_pck;
	char nhml[1024];
	u32 size;
	u8 *output;
	const GF_PropertyValue *p;

	ctx->szRootName = "NHNTStream";
	if (ctx->dims) {
		ctx->szRootName = "DIMSStream";
	}

	if (!ctx->filep) {
		sprintf(nhml, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
	}

	/*write header*/
	sprintf(nhml, "<%s version=\"1.0\" ", ctx->szRootName);
	gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));


	NHML_PRINT_UINT(GF_PROP_PID_ID, NULL, "trackID")
	NHML_PRINT_UINT(GF_PROP_PID_TIMESCALE, NULL, "timeScale")

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_IN_IOD);
	if (p && p->value.boolean) {
		sprintf(nhml, "inRootOD=\"yes\" ");
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
	}

	if (ctx->oti && (ctx->oti<GF_CODECID_LAST_MPEG4_MAPPING)) {
		sprintf(nhml, "streamType=\"%d\" objectTypeIndication=\"%d\" ", ctx->streamtype, ctx->oti);
		gf_bs_write_data(ctx->bs_w, nhml, (u32)strlen(nhml));
	} else {
		p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_SUBTYPE);
		if (p) {
			sprintf(nhml, "%s=\"%s\" ", "mediaType", gf_4cc_to_str(p->value.uint));
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));

			NHML_PRINT_4CC(GF_PROP_PID_ISOM_SUBTYPE, "mediaSubType", "mediaSubType")
		} else {
			NHML_PRINT_4CC(GF_PROP_PID_CODECID, NULL, "codecID")
		}
	}

	if (ctx->w && ctx->h) {
		//compatibility with old arch, we might want to remove this
		switch (ctx->streamtype) {
		case GF_STREAM_VISUAL:
		case GF_STREAM_SCENE:
			sprintf(nhml, "width=\"%d\" height=\"%d\" ", ctx->w, ctx->h);
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
			break;
		default:
			break;
		}
	}
	else if (ctx->sr && ctx->chan) {
		sprintf(nhml, "sampleRate=\"%d\" numChannels=\"%d\" ", ctx->sr, ctx->chan);
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		sprintf(nhml, "sampleRate=\"%d\" numChannels=\"%d\" ", ctx->sr, ctx->chan);
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_AUDIO_FORMAT);
		sprintf(nhml, "bitsPerSample=\"%d\" ", gf_audio_fmt_bit_depth(p->value.uint));
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
	}

	NHML_PRINT_4CC(0, "codec_vendor", "codecVendor")
	NHML_PRINT_UINT(0, "codec_version", "codecVersion")
	NHML_PRINT_UINT(0, "codec_revision", "codecRevision")
	NHML_PRINT_STRING(0, "compressor_name", "compressorName")
	NHML_PRINT_UINT(0, "temporal_quality", "temporalQuality")
	NHML_PRINT_UINT(0, "spatial_quality", "spatialQuality")
	NHML_PRINT_UINT(0, "hres", "horizontalResolution")
	NHML_PRINT_UINT(0, "vres", "verticalResolution")
	NHML_PRINT_UINT(GF_PROP_PID_BIT_DEPTH_Y, NULL, "bitDepth")

	NHML_PRINT_STRING(0, "meta:xmlns", "xml_namespace")
	NHML_PRINT_STRING(0, "meta:schemaloc", "xml_schema_location")
	NHML_PRINT_STRING(0, "meta:mime", "mime_type")

	NHML_PRINT_STRING(0, "meta:config", "config")
	NHML_PRINT_STRING(0, "meta:aux_mimes", "aux_mime_type")

	if (ctx->codecid == GF_CODECID_DIMS) {
		if (gf_filter_pid_get_property_str(ctx->ipid, "meta:xmlns")==NULL) {
			sprintf(nhml, "xmlns=\"http://www.3gpp.org/richmedia\" ");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}

		NHML_PRINT_UINT(0, "dims:profile", "profile")
		NHML_PRINT_UINT(0, "dims:level", "level")
		NHML_PRINT_UINT(0, "dims:pathComponents", "pathComponents")

		p = gf_filter_pid_get_property_str(ctx->ipid, "dims:fullRequestHost");
		if (p) {
			sprintf(nhml, "useFullRequestHost=\"%s\" ", p->value.boolean ? "yes" : "no");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		p = gf_filter_pid_get_property_str(ctx->ipid, "dims:streamType");
		if (p) {
			sprintf(nhml, "stream_type=\"%s\" ", p->value.boolean ? "primary" : "secondary");
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		p = gf_filter_pid_get_property_str(ctx->ipid, "dims:redundant");
		if (p) {
			sprintf(nhml, "contains_redundant=\"%s\" ", (p->value.uint==1) ? "main" : ((p->value.uint==1) ? "redundant" : "main+redundant") );
			gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
		}
		NHML_PRINT_UINT(0, "dims:scriptTypes", "scriptTypes")
	}

	//send DCD
	if (ctx->opid_info) {
		sprintf(nhml, "specificInfoFile=\"%s\" ", gf_file_basename(ctx->info_file) );
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));

		dst_pck = gf_filter_pck_new_shared(ctx->opid_info, ctx->dcfg, ctx->dcfg_size, NULL);
		gf_filter_pck_set_framing(dst_pck, GF_TRUE, GF_TRUE);
		gf_filter_pck_set_readonly(dst_pck);
		gf_filter_pck_send(dst_pck);
	}

	NHML_PRINT_STRING(0, "meta:encoding", "encoding")
	NHML_PRINT_STRING(0, "meta:contentEncoding", "content_encoding")
	ctx->uncompress = GF_FALSE;
	if (p) {
		if (!strcmp(p->value.string, "deflate")) ctx->uncompress = GF_TRUE;
		else {
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[NHMLMx] content_encoding %s not supported\n", p->value.string ));
		}
	}

	if (ctx->opid_mdia) {
		sprintf(nhml, "baseMediaFile=\"%s\" ", gf_file_basename(ctx->media_file) );
		gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));
	}
	sprintf(nhml, ">\n");
	gf_bs_write_data(ctx->bs_w, nhml, (u32) strlen(nhml));

	gf_bs_get_content_no_truncate(ctx->bs_w, &ctx->nhml_buffer, &size, &ctx->nhml_buffer_size);

	if (ctx->filep) {
		gf_fwrite(ctx->nhml_buffer, size, ctx->filep);
		return;
	}

	dst_pck = gf_filter_pck_new_alloc(ctx->opid_nhml, size, &output);
	memcpy(output, ctx->nhml_buffer, size);
	gf_filter_pck_set_framing(dst_pck, GF_TRUE, GF_FALSE);
	gf_filter_pck_send(dst_pck);
}