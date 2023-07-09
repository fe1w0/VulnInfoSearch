static GF_Err gf_media_export_filters(GF_MediaExporter *dumper)
{
	char *args, szSubArgs[1024], szExt[30];
	GF_Filter *file_out, *reframer, *remux=NULL, *src_filter;
	GF_FilterSession *fsess;
	GF_Err e = GF_OK;
	u32 codec_id=0;
	u32 sample_count=0;
	Bool skip_write_filter = GF_FALSE;
	Bool ext_forced = GF_FALSE;
	Bool use_dynext = GF_FALSE;

	args = NULL;
	strcpy(szExt, "");
	if (dumper->trackID && dumper->file) {
		u32 msubtype = 0;
		u32 mtype = 0;
		u32 afmt = 0;
		GF_PixelFormat pfmt = 0;
		GF_ESD *esd;
		const char *export_ext = dumper->out_name ? gf_file_ext_start(dumper->out_name) : NULL;
		u32 track_num = gf_isom_get_track_by_id(dumper->file, dumper->trackID);
		if (!track_num) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] No tracks with ID %d in file\n", dumper->trackID));
			return GF_BAD_PARAM;
		}
		esd = gf_media_map_esd(dumper->file, track_num, 0);
		sample_count = gf_isom_get_sample_count(dumper->file, dumper->trackID);
		if (esd) {
			if (esd->decoderConfig->objectTypeIndication<GF_CODECID_LAST_MPEG4_MAPPING) {
				codec_id = gf_codecid_from_oti(esd->decoderConfig->streamType, esd->decoderConfig->objectTypeIndication);
#ifndef GPAC_DISABLE_AV_PARSERS
				if (esd->decoderConfig->decoderSpecificInfo && (codec_id==GF_CODECID_AAC_MPEG4)) {
					GF_M4ADecSpecInfo acfg;
					gf_m4a_get_config(esd->decoderConfig->decoderSpecificInfo->data, esd->decoderConfig->decoderSpecificInfo->dataLength, &acfg);
					if (acfg.base_object_type == GF_M4A_USAC)
						codec_id = GF_CODECID_USAC;
				}
#endif
			} else {
				codec_id = esd->decoderConfig->objectTypeIndication;
			}
		}
		if (!codec_id) {
			msubtype = gf_isom_get_media_subtype(dumper->file, track_num, 1);
			codec_id = gf_codec_id_from_isobmf(msubtype);
		}
		mtype = gf_isom_get_media_type(dumper->file, track_num);
		if (!codec_id) {
			pfmt = gf_pixel_fmt_from_qt_type(msubtype);
			if (pfmt) codec_id = GF_CODECID_RAW;
		}

		if (!codec_id) {
			strcpy(szExt, gf_4cc_to_str(msubtype));
			ext_forced = GF_TRUE;
		} else if (codec_id==GF_CODECID_RAW) {
			switch (mtype) {
			case GF_ISOM_MEDIA_VISUAL:
			case GF_ISOM_MEDIA_AUXV:
			case GF_ISOM_MEDIA_PICT:
				if (pfmt)
					strcpy(szExt, gf_pixel_fmt_sname(pfmt));
				break;
			case GF_ISOM_MEDIA_AUDIO:
				afmt = gf_audio_fmt_from_isobmf(msubtype);
				if (afmt)
					strcpy(szExt, gf_audio_fmt_name(afmt));
				break;
			default:
				strcpy(szExt, gf_4cc_to_str(msubtype));
				break;
			}
		} else {
			const char *sname = gf_codecid_file_ext(codec_id);
			if (export_ext && strstr(sname, export_ext+1)) {
				szExt[0]=0;
			} else {
				char *sep;
				strncpy(szExt, sname, 29);
				szExt[29]=0;
				sep = strchr(szExt, '|');
				if (sep) sep[0] = 0;
			}
		}
		switch (mtype) {
		case GF_ISOM_MEDIA_VISUAL:
		case GF_ISOM_MEDIA_AUXV:
		case GF_ISOM_MEDIA_PICT:
		case GF_ISOM_MEDIA_AUDIO:
			skip_write_filter = codec_id ? GF_TRUE : GF_FALSE;
			break;
		default:
			switch (codec_id) {
			case GF_CODECID_WEBVTT:
				skip_write_filter = GF_TRUE;
				break;
			case GF_CODECID_META_TEXT:
			case GF_CODECID_META_XML:
			case GF_CODECID_SUBS_TEXT:
			case GF_CODECID_SUBS_XML:
			case GF_CODECID_SIMPLE_TEXT:
				//use dynamic extension
				szExt[0] = 0;
				use_dynext = GF_TRUE;
				break;
			}
			break;
		}
		//TODO, move these two to filters one of these days
		if ((codec_id==GF_CODECID_VORBIS) || (codec_id==GF_CODECID_THEORA) || (codec_id==GF_CODECID_OPUS)) {
			char *outname = dumper->out_name;
			if (outname && !strcmp(outname, "std")) outname=NULL;
			if (esd) gf_odf_desc_del((GF_Descriptor *) esd);
#ifndef GPAC_DISABLE_AV_PARSERS
			return gf_dump_to_ogg(dumper, outname, track_num);
#else
			return GF_NOT_SUPPORTED;
#endif

		}
		if (codec_id==GF_CODECID_SUBPIC) {
#ifndef GPAC_DISABLE_AV_PARSERS
			char *dsi = NULL;
			u32 dsi_size = 0;
			if (esd && esd->decoderConfig && esd->decoderConfig->decoderSpecificInfo) {
				dsi = esd->decoderConfig->decoderSpecificInfo->data;
				dsi_size = esd->decoderConfig->decoderSpecificInfo->dataLength;
			}
			e = gf_dump_to_vobsub(dumper, dumper->out_name, track_num, dsi, dsi_size);
#else
			e = GF_NOT_SUPPORTED;
#endif
			if (esd) gf_odf_desc_del((GF_Descriptor *) esd);
			return e;
		}
		if (esd) gf_odf_desc_del((GF_Descriptor *) esd);
	} else {
		const char *export_ext = dumper->out_name ? gf_file_ext_start(dumper->out_name) : NULL;
		skip_write_filter = GF_TRUE;
		if (!export_ext)
			use_dynext = GF_TRUE;
	}

	fsess = gf_fs_new_defaults(0);
	if (!fsess) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Failed to create filter session\n"));
		return GF_OUT_OF_MEM;
	}
	file_out = NULL;
	args = NULL;

	if (dumper->flags & GF_EXPORT_REMUX) {
		file_out = gf_fs_load_destination(fsess, dumper->out_name, NULL, NULL, &e);
		if (!file_out) {
			gf_fs_del(fsess);
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot open destination %s\n", dumper->out_name));
			return e;
		}
	}
	//except in nhml inband file dump, create a sink filter
	else if (!dumper->dump_file) {
		Bool no_ext = (dumper->flags & GF_EXPORT_NO_FILE_EXT) ? GF_TRUE : GF_FALSE;
		char *ext = gf_file_ext_start(dumper->out_name);
		//mux args, for now we only dump to file
		e = gf_dynstrcat(&args, "fout:dst=", NULL);
		e |= gf_dynstrcat(&args, dumper->out_name, NULL);

		if (dumper->flags & GF_EXPORT_NHNT) {
			strcpy(szExt, "nhnt");
			e |= gf_dynstrcat(&args, ":clone", NULL);
			no_ext = GF_TRUE;
			if (!ext)
				e |= gf_dynstrcat(&args, ":dynext", NULL);
		} else if (dumper->flags & GF_EXPORT_NHML) {
			strcpy(szExt, "nhml");
			e |= gf_dynstrcat(&args, ":clone", NULL);
			no_ext = GF_TRUE;
			if (!ext)
				e |= gf_dynstrcat(&args, ":dynext", NULL);
		}

		if (dumper->flags & GF_EXPORT_RAW_SAMPLES) {
			if (!dumper->sample_num) {

				ext = gf_file_ext_start(args);
				if (ext) ext[0] = 0;
				if (sample_count>=1000) {
					e |= gf_dynstrcat(&args, "_$num%08d$", NULL);
				} else if (sample_count) {
					e |= gf_dynstrcat(&args, "_$num%03d$", NULL);
				} else {
					e |= gf_dynstrcat(&args, "_$num$", NULL);
				}
				ext = gf_file_ext_start(dumper->out_name);
				if (ext) e |= gf_dynstrcat(&args, ext, NULL);
			}
			e |= gf_dynstrcat(&args, ":dynext", NULL);
		} else if (dumper->trackID && strlen(szExt) ) {
			if (!no_ext && !gf_file_ext_start(dumper->out_name)) {
				if (args) gf_free(args);
				args=NULL;
				e = gf_dynstrcat(&args, "fout:dst=", NULL);
				e |= gf_dynstrcat(&args, dumper->out_name, NULL);
				e |= gf_dynstrcat(&args, szExt, ".");
			} else {
				e |= gf_dynstrcat(&args, ":ext=", NULL);
				e |= gf_dynstrcat(&args, szExt, NULL);
			}
		} else if ((dumper->trackID || dumper->track_type) && use_dynext) {
			e |= gf_dynstrcat(&args, ":dynext", NULL);
		}
		if (e) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot load arguments for output file dumper\n"));
			if (args) gf_free(args);
			gf_fs_del(fsess);
			return e;
		}

		file_out = gf_fs_load_filter(fsess, args, &e);
		if (!file_out) {
			gf_fs_del(fsess);
			if (args) gf_free(args);
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot load output file dumper\n"));
			return e;
		}
	}
	if (args) gf_free(args);
	args = NULL;

	//raw sample frame, force loading filter generic write in frame mode
	if (dumper->flags & GF_EXPORT_RAW_SAMPLES) {
		e = gf_dynstrcat(&args, "writegen:frame", NULL);
		if (dumper->sample_num) {
			sprintf(szSubArgs, ":sstart=%d:send=%d", dumper->sample_num, dumper->sample_num);
			e |= gf_dynstrcat(&args, szSubArgs, NULL);
		}
		remux = e ? NULL : gf_fs_load_filter(fsess, args, &e);
		if (!remux || e) {
			gf_fs_del(fsess);
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot load stream->file filter\n"));
			if (args) gf_free(args);
			return e ? e : GF_FILTER_NOT_FOUND;
		}
	}
	else if (dumper->flags & GF_EXPORT_NHNT) {
		remux = gf_fs_load_filter(fsess, "nhntw:exporter", &e);
		if (!remux) {
			gf_fs_del(fsess);
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot load NHNT write filter\n"));
			return e;
		}
	}
	else if (dumper->flags & GF_EXPORT_NHML) {
		e = gf_dynstrcat(&args, "nhmlw:exporter:name=", NULL);
		e |= gf_dynstrcat(&args, dumper->out_name, NULL);
		if (dumper->flags & GF_EXPORT_NHML_FULL)
			e |= gf_dynstrcat(&args, ":pckp", NULL);
		if (dumper->dump_file) {
			sprintf(szSubArgs, ":nhmlonly:filep=%p", dumper->dump_file);
			e |= gf_dynstrcat(&args, szSubArgs, NULL);
		}
		remux = e ? NULL : gf_fs_load_filter(fsess, args, &e);
		if (!remux || e) {
			gf_fs_del(fsess);
			if (args) gf_free(args);
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot load NHML write filter\n"));
			return e ? e : GF_FILTER_NOT_FOUND;
		}
	} else if (!skip_write_filter) {
		e = gf_dynstrcat(&args, "writegen:exporter", NULL);
		//extension has been forced, override ext at output of writegen
		if (ext_forced) {
			e |= gf_dynstrcat(&args, ":#Extension=", NULL);
			e |= gf_dynstrcat(&args, szExt, NULL);
		}

		remux = e ? NULL : gf_fs_load_filter(fsess, args, &e);
		if (!remux) {
			gf_fs_del(fsess);
			if (args) gf_free(args);
			GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot load stream->file filter\n"));
			return e;
		}
	}
	if (args) gf_free(args);
	args = NULL;

	//force a reframer filter, connected to our input
	e = gf_dynstrcat(&args, "reframer:SID=1", NULL);
	if (dumper->trackID) {
		sprintf(szSubArgs, "#PID=%d", dumper->trackID);
		e |= gf_dynstrcat(&args, szSubArgs, NULL);
	}
	e |= gf_dynstrcat(&args, ":exporter", NULL);
	if (dumper->flags & GF_EXPORT_SVC_LAYER)
		e |= gf_dynstrcat(&args, ":extract=layer", NULL);
	if (dumper->flags & GF_EXPORT_WEBVTT_NOMERGE)
		e |= gf_dynstrcat(&args, ":merge", NULL);

	reframer = gf_fs_load_filter(fsess, args, &e);
	if (!reframer || e) {
		gf_fs_del(fsess);
		if (args) gf_free(args);
		GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot load reframer filter\n"));
		return e ? e : GF_FILTER_NOT_FOUND;
	}
	if (args) gf_free(args);
	args = NULL;

	//we already have the file loaded, directly load the mp4dmx filter with this file
	if (dumper->file) {
		//we want to expose every track
		e = gf_dynstrcat(&args, "mp4dmx:FID=1:noedit:alltk:allt", NULL);
		if (!e) {
			sprintf(szSubArgs, ":mov=%p", dumper->file);
			e = gf_dynstrcat(&args, szSubArgs, NULL);
		}
		
		//we want to expose every track
		src_filter = gf_fs_load_filter(fsess, args, &e);

		gf_free(args);
		args = NULL;
	} else {
		//we want to expose every track
		src_filter = gf_fs_load_source(fsess, dumper->in_name, "FID=1:noedit:alltk:allt", NULL, &e);
	}

	if (!src_filter || e) {
		gf_fs_del(fsess);
		GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("[Exporter] Cannot load filter for input file \"%s\": %s\n", dumper->in_name, gf_error_to_string(e) ));
		return e;
	}

	if (dumper->track_type) {
		const char *mtype = (dumper->track_type==1) ? "video" : "audio";
		if (dumper->trackID) {
			sprintf(szSubArgs, "%s%d", mtype, dumper->trackID);
		} else {
			sprintf(szSubArgs, "%s", mtype);
		}
	}
	else if (dumper->trackID) {
		sprintf(szSubArgs, "PID=%d", dumper->trackID);
	}
	if (remux) {
		gf_filter_set_source(file_out, remux, (dumper->trackID || dumper->track_type) ? szSubArgs : NULL);
		gf_filter_set_source(remux, reframer, (dumper->trackID || dumper->track_type) ? szSubArgs : NULL);
	} else {
		gf_filter_set_source(file_out, reframer, (dumper->trackID || dumper->track_type) ? szSubArgs : NULL);
	}

	e = gf_fs_run(fsess);
	if (e>GF_OK) e = GF_OK;
	if (!e) e = gf_fs_get_last_connect_error(fsess);
	if (!e) e = gf_fs_get_last_process_error(fsess);

	if (!e) {
		if (dumper->file)
			gf_fs_print_unused_args(fsess, NULL);
		else
			gf_fs_print_unused_args(fsess, "alltk,allt,noedit");
	}
	gf_fs_print_non_connected(fsess);
	if (dumper->print_stats_graph & 1) gf_fs_print_stats(fsess);
	if (dumper->print_stats_graph & 2) gf_fs_print_connections(fsess);
	gf_fs_del(fsess);
	return e;
}