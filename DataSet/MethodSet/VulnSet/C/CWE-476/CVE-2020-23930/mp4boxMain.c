int mp4boxMain(int argc, char **argv)
{
	u32 i, j;
	const char *gpac_profile = "0";
	GF_Err e = GF_OK;
	nb_tsel_acts = nb_add = nb_cat = nb_track_act = nb_sdp_ex = max_ptime = nb_meta_act = rtp_rate = major_brand = nb_alt_brand_add = nb_alt_brand_rem = car_dur = minor_version = 0;

	split_duration = 0.0;
	split_start = -1.0;
	interleaving_time = 0;
	dash_duration = dash_subduration = 0.0;
	import_fps.num = import_fps.den = 0;
	import_flags = 0;
	split_size = 0;
	movie_time = 0;
	dump_nal = dump_saps = dump_saps_mode = force_new = 0;
	FullInter = HintInter = encode = do_scene_log = old_interleave = do_saf = do_hash = verbose = do_mpd_rip = merge_vtt_cues = get_nb_tracks = GF_FALSE;
#ifndef GPAC_DISABLE_SCENE_DUMP
	dump_mode = GF_SM_DUMP_NONE;
#endif
	Frag = force_ocr = remove_sys_tracks = agg_samples = remove_hint = keep_sys_tracks = remove_root_od = single_group = clean_groups = compress_moov = GF_FALSE;
	conv_type = HintIt = needSave = print_sdp = regular_iod = dump_std = open_edit = dump_rtp = dump_cr = dump_srt = dump_ttxt = dump_m2ts = dump_cart = import_subtitle = force_cat = pack_wgt = dash_live = GF_FALSE;
	no_fragments_defaults = GF_FALSE;
	single_traf_per_moof = hls_clock = GF_FALSE;
    tfdt_per_traf = GF_FALSE;
	dump_nal_type = 0;
	dump_isom = 0;
	print_info = 0;
	/*align cat is the new default behaviour for -cat*/
	align_cat = GF_TRUE;
	subsegs_per_sidx = 0;
	track_dump_type = 0;
	crypt = 0;
	time_shift_depth = 0;
	file = NULL;
	itunes_tags = pes_dump = NULL;
	seg_name = dash_ctx_file = NULL;
	compress_top_boxes = NULL;
	initial_moof_sn = 0;
	initial_tfdt = 0;

#ifndef GPAC_DISABLE_SCENE_ENCODER
	memset(&smenc_opts, 0, sizeof(smenc_opts));
#endif

	trackID = stat_level = hint_flags = 0;
	program_number = 0;
	info_track_id = 0;
	do_flat = 0;
	inName = outName = mediaSource = input_ctx = output_ctx = drm_file = avi2raw = cprt = chap_file = pack_file = raw_cat = high_dynamc_range_filename = use_init_seg = box_patch_filename = NULL;

#ifndef GPAC_DISABLE_SWF_IMPORT
	swf_flags = GF_SM_SWF_SPLIT_TIMELINE;
#endif
	swf_flatten_angle = 0.0f;
	tmpdir = NULL;

	for (i = 1; i < (u32) argc ; i++) {
		if (!strcmp(argv[i], "-mem-track") || !strcmp(argv[i], "-mem-track-stack")) {
#ifdef GPAC_MEMORY_TRACKING
            mem_track = !strcmp(argv[i], "-mem-track-stack") ? GF_MemTrackerBackTrace : GF_MemTrackerSimple;
#else
			fprintf(stderr, "WARNING - GPAC not compiled with Memory Tracker - ignoring \"%s\"\n", argv[i]);
#endif
			break;
		}
		else if (!strcmp(argv[i], "-p")) {
			if (i+1<(u32) argc)
				gpac_profile = argv[i+1];
			else {
				fprintf(stderr, "Bad argument for -p, expecting profile name but no more args\n");
				return 1;
			}
		}
		else if (!strncmp(argv[i], "-p=", 3))
			gpac_profile = argv[i]+3;
	}

#ifdef _TWO_DIGIT_EXPONENT
	_set_output_format(_TWO_DIGIT_EXPONENT);
#endif

	/*init libgpac*/
	gf_sys_init(mem_track, gpac_profile);
	if (argc < 2) {
		fprintf(stderr, "Not enough arguments - check usage with -h\n"
			"MP4Box - GPAC version %s\n"
	        "%s\n", gf_gpac_version(), gf_gpac_copyright());
		gf_sys_close();
		return 0;
	}

	helpout = stdout;

	i = mp4box_parse_args(argc, argv);
	if (i) {
		return mp4box_cleanup(i - 1);
	}

	if (!inName && dump_std)
		inName = "std";

	if (!inName) {
		if (has_next_arg) {
			fprintf(stderr, "Broken argument specifier or file name missing - check usage with -h\n");
		} else {
			PrintUsage();
		}
		return mp4box_cleanup(1);
	}
	if (!strcmp(inName, "std")) dump_std = 2;
	if (!strcmp(inName, "stdb")) {
		inName = "std";
		dump_std = 1;
	}

	if (!interleaving_time) {
		/*by default use single fragment per dash segment*/
		if (dash_duration)
			interleaving_time = dash_duration;
		else if (!do_flat) {
			interleaving_time = DEFAULT_INTERLEAVING_IN_SEC;
		}
	}

	if (dump_std)
		outName = "std";

	if (dump_std==2) {
#ifdef WIN32
		if ( _setmode(_fileno(stdout), _O_BINARY) == -1 )
#else
		if ( freopen(NULL, "wb", stdout) == NULL)
#endif
		{
			fprintf(stderr, "Fatal error: cannot reopen stdout in binary mode.\n");
			return mp4box_cleanup(1);
		}
	}

#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
	if (live_scene) {
		int ret = live_session(argc, argv);
		return mp4box_cleanup(ret);
	}
#endif

	GF_LOG_Level level = verbose ? GF_LOG_DEBUG : GF_LOG_INFO;
	gf_log_set_tool_level(GF_LOG_CONTAINER, level);
	gf_log_set_tool_level(GF_LOG_SCENE, level);
	gf_log_set_tool_level(GF_LOG_PARSER, level);
	gf_log_set_tool_level(GF_LOG_AUTHOR, level);
	gf_log_set_tool_level(GF_LOG_CODING, level);
	gf_log_set_tool_level(GF_LOG_DASH, level);
#ifdef GPAC_MEMORY_TRACKING
	if (mem_track)
		gf_log_set_tool_level(GF_LOG_MEMORY, level);
#endif

	e = gf_sys_set_args(argc, (const char **) argv);
	if (e) {
		fprintf(stderr, "Error assigning libgpac arguments: %s\n", gf_error_to_string(e) );
		return mp4box_cleanup(1);
	}

	if (raw_cat) {
		char chunk[4096];
		FILE *fin, *fout;
		s64 to_copy, done;
		fin = gf_fopen(raw_cat, "rb");
		if (!fin) return mp4box_cleanup(1);

		fout = gf_fopen(inName, "a+b");
		if (!fout) {
			gf_fclose(fin);
			return mp4box_cleanup(1);
		}
		gf_fseek(fin, 0, SEEK_END);
		to_copy = gf_ftell(fin);
		gf_fseek(fin, 0, SEEK_SET);
		done = 0;
		while (1) {
			u32 nb_bytes = (u32) gf_fread(chunk, 4096, fin);
			gf_fwrite(chunk, nb_bytes, fout);
			done += nb_bytes;
			fprintf(stderr, "Appending file %s - %02.2f done\r", raw_cat, 100.0*done/to_copy);
			if (done >= to_copy) break;
		}
		gf_fclose(fin);
		gf_fclose(fout);
		return mp4box_cleanup(0);
	}
	if (compress_top_boxes) {
		if (size_top_box) {
			u64 top_size = do_size_top_boxes(inName, compress_top_boxes, size_top_box);
			fprintf(stdout, LLU"\n", top_size);
			return mp4box_cleanup(e ? 1 : 0);
		} else {
			e = do_compress_top_boxes(inName, outName, compress_top_boxes, comp_top_box_version, comp_lzma);
			return mp4box_cleanup(e ? 1 : 0);
		}
	}

	if (do_mpd_rip) {
		e = rip_mpd(inName, outName);
		return mp4box_cleanup(e ? 1 : 0);
	}

#ifndef GPAC_DISABLE_CORE_TOOLS
	if (do_wget != NULL) {
		e = gf_dm_wget(do_wget, inName, 0, 0, NULL);
		if (e != GF_OK) {
			fprintf(stderr, "Cannot retrieve %s: %s\n", do_wget, gf_error_to_string(e) );
			return mp4box_cleanup(1);
		}
		return mp4box_cleanup(0);
	}
#endif

	if (udp_dest) {
		GF_Socket *sock = gf_sk_new(GF_SOCK_TYPE_UDP);
		u16 port = 2345;
		char *sep = strrchr(udp_dest, ':');
		if (sep) {
			sep[0] = 0;
			port = atoi(sep+1);
		}
		e = gf_sk_bind( sock, "127.0.0.1", 0, udp_dest, port, 0);
		if (sep) sep[0] = ':';
		if (e) fprintf(stderr, "Failed to bind socket to %s: %s\n", udp_dest, gf_error_to_string(e) );
		else {
			e = gf_sk_send(sock, (u8 *) inName, (u32)strlen(inName));
			if (e) fprintf(stderr, "Failed to send datagram: %s\n", gf_error_to_string(e) );
		}
		gf_sk_del(sock);
		return 0;
	}

#ifndef GPAC_DISABLE_MPD
	if (do_mpd) {
		Bool remote = GF_FALSE;
		GF_MPD *mpd;
		char *mpd_base_url = NULL;
		if (!strnicmp(inName, "http://", 7) || !strnicmp(inName, "https://", 8)) {
#if !defined(GPAC_DISABLE_CORE_TOOLS)
			e = gf_dm_wget(inName, "tmp_main.m3u8", 0, 0, &mpd_base_url);
			if (e != GF_OK) {
				fprintf(stderr, "Cannot retrieve M3U8 (%s): %s\n", inName, gf_error_to_string(e));
				if (mpd_base_url) gf_free(mpd_base_url);
				return mp4box_cleanup(1);
			}
			remote = GF_TRUE;
#else
			gf_free(mpd_base_url);
			fprintf(stderr, "HTTP Downloader disabled in this build\n");
			return mp4box_cleanup(1);
#endif

			if (outName)
				strcpy(outfile, outName);
			else {
				const char *sep = gf_file_basename(inName);
				char *ext = gf_file_ext_start(sep);
				if (ext) ext[0] = 0;
				sprintf(outfile, "%s.mpd", sep);
				if (ext) ext[0] = '.';
			}
		} else {
			if (outName)
				strcpy(outfile, outName);
			else {
				char *dst = strdup(inName);
				char *ext = strstr(dst, ".m3u8");
				if (ext) ext[0] = 0;
				sprintf(outfile, "%s.mpd", dst);
				gf_free(dst);
			}
		}

		mpd = gf_mpd_new();
		if (!mpd) {
			e = GF_OUT_OF_MEM;
			fprintf(stderr, "[DASH] Error: MPD creation problem %s\n", gf_error_to_string(e));
			mp4box_cleanup(1);
		}
		FILE *f = gf_fopen(remote ? "tmp_main.m3u8" : inName, "r");
		u32 manif_type = 0;
		if (f) {
			char szDATA[1000];
			s32 read;
			szDATA[999]=0;
			read = (s32) gf_fread(szDATA, 999, f);
			if (read<0) read = 0;
			szDATA[read]=0;
			gf_fclose(f);
			if (strstr(szDATA, "SmoothStreamingMedia"))
				manif_type = 2;
			else if (strstr(szDATA, "#EXTM3U"))
				manif_type = 1;
		}

		if (manif_type==1) {
			e = gf_m3u8_to_mpd(remote ? "tmp_main.m3u8" : inName, mpd_base_url ? mpd_base_url : inName, outfile, 0, "video/mp2t", GF_TRUE, use_url_template, segment_timeline, NULL, mpd, GF_TRUE, GF_TRUE);
		} else if (manif_type==2) {
			e = gf_mpd_smooth_to_mpd(remote ? "tmp_main.m3u8" : inName, mpd, mpd_base_url ? mpd_base_url : inName);
		} else {
			e = GF_NOT_SUPPORTED;
		}
		if (!e)
			gf_mpd_write_file(mpd, outfile);

		if (mpd)
			gf_mpd_del(mpd);
		if (mpd_base_url)
			gf_free(mpd_base_url);

		if (remote) {
			gf_file_delete("tmp_main.m3u8");
		}
		if (e != GF_OK) {
			fprintf(stderr, "Error converting %s (%s) to MPD (%s): %s\n", (manif_type==1) ? "HLS" : "Smooth",  inName, outfile, gf_error_to_string(e));
			return mp4box_cleanup(1);
		} else {
			fprintf(stderr, "Done converting %s (%s) to MPD (%s)\n", (manif_type==1) ? "HLS" : "Smooth",  inName, outfile);
			return mp4box_cleanup(0);
		}
	}
#endif
	if (dash_duration && !nb_dash_inputs) {
		dash_inputs = set_dash_input(dash_inputs, inName, &nb_dash_inputs);
	}


	if (do_saf && !encode) {
		switch (get_file_type_by_ext(inName)) {
		case GF_FILE_TYPE_BT_WRL_X3DV:
		case GF_FILE_TYPE_XMT_X3D:
		case GF_FILE_TYPE_SVG:
			encode = GF_TRUE;
			break;
		case GF_FILE_TYPE_NOT_SUPPORTED:
		case GF_FILE_TYPE_ISO_MEDIA:
		case GF_FILE_TYPE_SWF:
		case GF_FILE_TYPE_LSR_SAF:
			break;
		}
	}

#ifndef GPAC_DISABLE_SCENE_DUMP
	if (dump_mode == GF_SM_DUMP_SVG) {
		if (strstr(inName, ".srt") || strstr(inName, ".ttxt")) import_subtitle = 2;
	}
#endif


	if (import_subtitle && !trackID) {
		/* We import the subtitle file,
		   i.e. we parse it and store the content as samples of a 3GPP Timed Text track in an ISO file,
		   possibly for later export (e.g. when converting SRT to TTXT, ...) */
#ifndef GPAC_DISABLE_MEDIA_IMPORT
		GF_MediaImporter import;
		/* Prepare the importer */
		file = gf_isom_open("ttxt_convert", GF_ISOM_OPEN_WRITE, NULL);
		if (timescale && file) gf_isom_set_timescale(file, timescale);

		memset(&import, 0, sizeof(GF_MediaImporter));
		import.dest = file;
		import.in_name = inName;
		/* Start the import */
		e = gf_media_import(&import);
		if (e) {
			fprintf(stderr, "Error importing %s: %s\n", inName, gf_error_to_string(e));
			gf_isom_delete(file);
			gf_file_delete("ttxt_convert");
			return mp4box_cleanup(1);
		}
		/* Prepare the export */
		strcpy(outfile, inName);
		if (strchr(outfile, '.')) {
			while (outfile[strlen(outfile)-1] != '.') outfile[strlen(outfile)-1] = 0;
			outfile[strlen(outfile)-1] = 0;
		}
#ifndef GPAC_DISABLE_ISOM_DUMP
		/* Start the export of the track #1, in the appropriate dump type, indicating it's a conversion */
		dump_isom_timed_text(file, gf_isom_get_track_id(file, 1),
							  dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE,
		                      GF_TRUE,
		                      (import_subtitle==2) ? GF_TEXTDUMPTYPE_SVG : (dump_srt ? GF_TEXTDUMPTYPE_SRT : GF_TEXTDUMPTYPE_TTXT));
#endif
		/* Clean the importer */
		gf_isom_delete(file);
		gf_file_delete("ttxt_convert");
		if (e) {
			fprintf(stderr, "Error converting %s: %s\n", inName, gf_error_to_string(e));
			return mp4box_cleanup(1);
		}
		return mp4box_cleanup(0);
#else
		fprintf(stderr, "Feature not supported\n");
		return mp4box_cleanup(1);
#endif
	}

#if !defined(GPAC_DISABLE_MEDIA_IMPORT) && !defined(GPAC_DISABLE_ISOM_WRITE)
	if (nb_add || nb_cat) {
		u32 ipass, nb_pass = 1;
		char *mux_args=NULL;
		GF_FilterSession *fs = NULL;
		if (nb_add) {

			GF_ISOOpenMode open_mode = GF_ISOM_OPEN_EDIT;
			if (force_new) {
				open_mode = (do_flat || (force_new==2)) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
			} else {
				FILE *test = gf_fopen(inName, "rb");
				if (!test) {
					open_mode = (do_flat) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
					if (!outName) outName = inName;
				} else {
					gf_fclose(test);
					if (! gf_isom_probe_file(inName) ) {
						open_mode = (do_flat) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
						if (!outName) outName = inName;
					}
				}
			}
			open_edit = do_flat ? GF_FALSE : GF_TRUE;
			file = gf_isom_open(inName, open_mode, tmpdir);
			if (!file) {
				fprintf(stderr, "Cannot open destination file %s: %s\n", inName, gf_error_to_string(gf_isom_last_error(NULL)) );
				return mp4box_cleanup(1);
			}

			if (freeze_box_order)
				gf_isom_freeze_order(file);
		}

		if (do_flat && interleaving_time) {
			char szSubArg[100];
			gf_isom_set_storage_mode(file, GF_ISOM_STORE_FASTSTART);
			do_flat = 2;
			nb_pass = 2;
			fs = gf_fs_new_defaults(0);
			if (!fs) {
				fprintf(stderr, "Error creating filter session\n");
				gf_isom_delete(file);
				return mp4box_cleanup(1);
			}

			//mux args
			gf_dynstrcat(&mux_args, "mp4mx:importer:store=fstart", ":");

			sprintf(szSubArg, "file=%p", file);
			gf_dynstrcat(&mux_args, szSubArg, ":");
			sprintf(szSubArg, "cdur=%g", interleaving_time);
			gf_dynstrcat(&mux_args, szSubArg, ":");
		}

		for (ipass=0; ipass<nb_pass; ipass++) {
			u32 tk_idx = 1;
			for (i=0; i<(u32) argc; i++) {
				char *margs=NULL;
				if (!strcmp(argv[i], "-add")) {
					char *src = argv[i+1];

					while (src) {
						char *loc_src = src;
						char *sep = NULL;
						while (1) {
							char *opt_sep;
							sep = strchr(loc_src, '+');
							if (!sep) break;

							sep[0] = 0;
							if (strstr(src, "://"))
								break;

							opt_sep = gf_url_colon_suffix(src);
							if (opt_sep)
								opt_sep[0] = 0;
							if (gf_file_exists(src)) {
								if (opt_sep)
									opt_sep[0] = ':';
								break;
							}
							if (opt_sep)
								opt_sep[0] = ':';

							sep[0] = '+';
							loc_src = sep+1;
						}

						e = import_file(file, src, import_flags, import_fps, agg_samples, fs, (fs && (ipass==0)) ? &margs : NULL, tk_idx);
						tk_idx++;

						if (margs) {
							gf_dynstrcat(&mux_args, margs, ":");
							gf_free(margs);
						}

						if (e) {
							fprintf(stderr, "Error importing %s: %s\n", argv[i+1], gf_error_to_string(e));
							gf_isom_delete(file);
							if (fs)
								gf_fs_del(fs);
							return mp4box_cleanup(1);
						}
						if (sep) {
							sep[0] = '+';
							src = sep+1;
						} else {
							break;
						}
					}
					i++;
				} else if (!strcmp(argv[i], "-cat") || !strcmp(argv[i], "-catx") || !strcmp(argv[i], "-catpl")) {
					if (nb_pass == 2) {
						fprintf(stderr, "Cannot cat files when using -newfs mode\n");
						return mp4box_cleanup(1);
					}
					if (!file) {
						u8 open_mode = GF_ISOM_OPEN_EDIT;
						if (force_new) {
							open_mode = (do_flat) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
						} else {
							FILE *test = gf_fopen(inName, "rb");
							if (!test) {
								open_mode = (do_flat) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
								if (!outName) outName = inName;
							}
							else gf_fclose(test);
						}

						open_edit = GF_TRUE;
						file = gf_isom_open(inName, open_mode, tmpdir);
						if (!file) {
							fprintf(stderr, "Cannot open destination file %s: %s\n", inName, gf_error_to_string(gf_isom_last_error(NULL)) );
							return mp4box_cleanup(1);
						}
					}

					e = cat_isomedia_file(file, argv[i+1], import_flags, import_fps, agg_samples, tmpdir, force_cat, align_cat, !strcmp(argv[i], "-catx") ? GF_TRUE : GF_FALSE, !strcmp(argv[i], "-catpl") ? GF_TRUE : GF_FALSE);
					if (e) {
						fprintf(stderr, "Error appending %s: %s\n", argv[i+1], gf_error_to_string(e));
						gf_isom_delete(file);
						return mp4box_cleanup(1);
					}
					i++;
				}
			}
			if ((nb_pass == 2) && !ipass) {
				GF_Filter *mux_filter = gf_fs_load_filter(fs, mux_args, NULL);
				gf_free(mux_args);
				if (!mux_filter) {
					fprintf(stderr, "Error loadin isobmff mux filter\n");
					gf_isom_delete(file);
					gf_fs_del(fs);
					return mp4box_cleanup(1);
				}

				e = gf_fs_run(fs);
				if (e==GF_EOS) e = GF_OK;

				if (!e) e = gf_fs_get_last_connect_error(fs);
				if (!e) e = gf_fs_get_last_process_error(fs);

				if (e) {
					fprintf(stderr, "Error importing sources: %s\n", gf_error_to_string(e));
					gf_isom_delete(file);
					gf_fs_del(fs);
					return mp4box_cleanup(1);
				}
			}
		}
		if (fs) {
			if (fs_dump_flags & 1) gf_fs_print_stats(fs);
			if (fs_dump_flags & 2) gf_fs_print_connections(fs);
			gf_fs_del(fs);
		}

		/*unless explicitly asked, remove all systems tracks*/
#ifndef GPAC_DISABLE_AV_PARSERS
		if (!keep_sys_tracks) remove_systems_tracks(file);
#endif
		needSave = GF_TRUE;
	}
#endif /*!GPAC_DISABLE_MEDIA_IMPORT && !GPAC_DISABLE_ISOM_WRITE*/

#if !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_SCENE_ENCODER) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
	else if (chunk_mode) {
		if (!inName) {
			fprintf(stderr, "chunk encoding syntax: [-outctx outDump] -inctx inScene auFile\n");
			return mp4box_cleanup(1);
		}
		e = EncodeFileChunk(inName, outName ? outName : inName, input_ctx, output_ctx, tmpdir);
		if (e) {
			fprintf(stderr, "Error encoding chunk file %s\n", gf_error_to_string(e));
			return mp4box_cleanup(1);
		}
		goto exit;
	}
#endif // !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_SCENE_ENCODER) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
	else if (encode) {
#if !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_SCENE_ENCODER) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
		FILE *logs = NULL;
		if (do_scene_log) {
			char alogfile[GF_MAX_PATH];
			strcpy(alogfile, inName);
			if (strchr(alogfile, '.')) {
				while (alogfile[strlen(alogfile)-1] != '.') alogfile[strlen(alogfile)-1] = 0;
				alogfile[strlen(alogfile)-1] = 0;
			}
			strcat(alogfile, "_enc.logs");
			logs = gf_fopen(alogfile, "wt");
		}
		strcpy(outfile, outName ? outName : inName);
		if (strchr(outfile, '.')) {
			while (outfile[strlen(outfile)-1] != '.') outfile[strlen(outfile)-1] = 0;
			outfile[strlen(outfile)-1] = 0;
		}
		strcat(outfile, ".mp4");
		file = gf_isom_open(outfile, GF_ISOM_WRITE_EDIT, tmpdir);
		smenc_opts.mediaSource = mediaSource ? mediaSource : outfile;
		e = EncodeFile(inName, file, &smenc_opts, logs);
		if (logs) gf_fclose(logs);
		if (e) goto err_exit;
		needSave = GF_TRUE;
		if (do_saf) {
			needSave = GF_FALSE;
			open_edit = GF_FALSE;
		}
#endif //!defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_SCENE_ENCODER) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
	}

#ifndef GPAC_DISABLE_ISOM_WRITE
	else if (pack_file) {
		char *fileName = gf_url_colon_suffix(pack_file);
		if (fileName && ((fileName - pack_file)==4)) {
			fileName[0] = 0;
			file = package_file(fileName + 1, pack_file, tmpdir, pack_wgt);
			fileName[0] = ':';
		} else {
			file = package_file(pack_file, NULL, tmpdir, pack_wgt);
			if (!file) {
				fprintf(stderr, "Failed to package file\n");
				return mp4box_cleanup(1);
			}
		}
		if (!outName) outName = inName;
		needSave = GF_TRUE;
		open_edit = GF_TRUE;
	}
#endif //GPAC_DISABLE_ISOM_WRITE

	if (dash_duration) {
		Bool del_file = GF_FALSE;
		char szMPD[GF_MAX_PATH], *sep;
		char szStateFile[GF_MAX_PATH];
		Bool dyn_state_file = GF_FALSE;
		u32 do_abort = 0;
		GF_DASHSegmenter *dasher=NULL;

		if (crypt) {
			fprintf(stderr, "MP4Box cannot crypt and DASH on the same pass. Please encrypt your content first.\n");
			return mp4box_cleanup(1);
		}

		strcpy(outfile, outName ? outName : gf_url_get_resource_name(inName) );
		sep = strrchr(outfile, '.');
		if (sep) sep[0] = 0;
		if (!outName) strcat(outfile, "_dash");
		strcpy(szMPD, outfile);
		if (outName && sep) {
			sep[0] = '.';
			strcat(szMPD, sep);
		} else {
			strcat(szMPD, ".mpd");
		}
		
		if ((dash_subduration>0) && (dash_duration > dash_subduration)) {
			fprintf(stderr, "Warning: -subdur parameter (%g s) should be greater than segment duration (%g s), using segment duration instead\n", dash_subduration, dash_duration);
			dash_subduration = dash_duration;
		}

		if (dash_mode && dash_live)
			fprintf(stderr, "Live DASH-ing - press 'q' to quit, 's' to save context and quit\n");

		if (!dash_ctx_file && dash_live) {
			u32 r1;
			u64 add = (u64) (intptr_t) &dasher;
			add ^= gf_net_get_utc();
			r1 = (u32) add ^ (u32) (add/0xFFFFFFFF);
			r1 ^= gf_rand();
 			sprintf(szStateFile, "%s/dasher_%X.xml", gf_get_default_cache_directory(), r1 );
			dash_ctx_file = szStateFile;
			dyn_state_file = GF_TRUE;
		} else if (dash_ctx_file) {
			if (force_new)
				gf_file_delete(dash_ctx_file);
		}

		if (dash_profile==GF_DASH_PROFILE_AUTO)
			dash_profile = dash_mode ? GF_DASH_PROFILE_LIVE : GF_DASH_PROFILE_FULL;

		if (!dash_mode) {
			time_shift_depth = 0;
			mpd_update_time = 0;
		} else if ((dash_profile>=GF_DASH_PROFILE_MAIN) && !use_url_template && !mpd_update_time) {
			/*use a default MPD update of dash_duration sec*/
			mpd_update_time = (Double) (dash_subduration ? dash_subduration : dash_duration);
			fprintf(stderr, "Using default MPD refresh of %g seconds\n", mpd_update_time);
		}

		if (file && needSave) {
			gf_isom_close(file);
			file = NULL;
			del_file = GF_TRUE;
		}

		/*setup dash*/
		dasher = gf_dasher_new(szMPD, dash_profile, tmpdir, dash_scale, dash_ctx_file);
		if (!dasher) {
			return mp4box_cleanup(1);
		}
		e = gf_dasher_set_info(dasher, dash_title, cprt, dash_more_info, dash_source, NULL);
		if (e) {
			fprintf(stderr, "DASH Error: %s\n", gf_error_to_string(e));
			gf_dasher_del(dasher);
			return mp4box_cleanup(1);
		}

		gf_dasher_set_start_date(dasher, dash_start_date);
		gf_dasher_set_location(dasher, dash_source);
		for (i=0; i < nb_mpd_base_urls; i++) {
			e = gf_dasher_add_base_url(dasher, mpd_base_urls[i]);
			if (e) {
				fprintf(stderr, "DASH Error: %s\n", gf_error_to_string(e));
				gf_dasher_del(dasher);
				return mp4box_cleanup(1);
			}
		}

		if (segment_timeline && !use_url_template) {
			fprintf(stderr, "DASH Warning: using -segment-timeline with no -url-template. Forcing URL template.\n");
			use_url_template = GF_TRUE;
		}

		e = gf_dasher_enable_url_template(dasher, (Bool) use_url_template, seg_name, seg_ext, init_seg_ext);
		if (!e) e = gf_dasher_enable_segment_timeline(dasher, segment_timeline);
		if (!e) e = gf_dasher_enable_single_segment(dasher, single_segment);
		if (!e) e = gf_dasher_enable_single_file(dasher, single_file);
		if (!e) e = gf_dasher_set_switch_mode(dasher, bitstream_switching_mode);
		if (!e) e = gf_dasher_set_durations(dasher, dash_duration, interleaving_time, dash_subduration);
		if (!e) e = gf_dasher_enable_rap_splitting(dasher, seg_at_rap, frag_at_rap);
		if (!e) e = gf_dasher_set_segment_marker(dasher, segment_marker);
		if (!e) e = gf_dasher_enable_sidx(dasher, (subsegs_per_sidx>=0) ? 1 : 0, (u32) subsegs_per_sidx, daisy_chain_sidx, use_ssix);
		if (!e) e = gf_dasher_set_dynamic_mode(dasher, dash_mode, mpd_update_time, time_shift_depth, mpd_live_duration);
		if (!e) e = gf_dasher_set_min_buffer(dasher, min_buffer);
		if (!e) e = gf_dasher_set_ast_offset(dasher, ast_offset_ms);
		if (!e) e = gf_dasher_enable_memory_fragmenting(dasher, memory_frags);
		if (!e) e = gf_dasher_set_initial_isobmf(dasher, initial_moof_sn, initial_tfdt);
		if (!e) e = gf_dasher_configure_isobmf_default(dasher, no_fragments_defaults, pssh_mode, samplegroups_in_traf, single_traf_per_moof, tfdt_per_traf, mvex_after_traks, sdtp_in_traf);
		if (!e) e = gf_dasher_enable_utc_ref(dasher, insert_utc);
		if (!e) e = gf_dasher_enable_real_time(dasher, frag_real_time);
		if (!e) e = gf_dasher_set_content_protection_location_mode(dasher, cp_location_mode);
		if (!e) e = gf_dasher_set_profile_extension(dasher, dash_profile_extension);
		if (!e) e = gf_dasher_enable_cached_inputs(dasher, no_cache);
		if (!e) e = gf_dasher_enable_loop_inputs(dasher, ! no_loop);
		if (!e) e = gf_dasher_set_split_mode(dasher, dash_split_mode);
		if (!e) e = gf_dasher_set_hls_clock(dasher, hls_clock);
		if (!e && dash_cues) e = gf_dasher_set_cues(dasher, dash_cues, strict_cues);
		if (!e && fs_dump_flags) e = gf_dasher_print_session_info(dasher, fs_dump_flags);

		for (i=0; i < nb_dash_inputs; i++) {
			if (!e) e = gf_dasher_add_input(dasher, &dash_inputs[i]);
		}
		if (e) {
			fprintf(stderr, "DASH Setup Error: %s\n", gf_error_to_string(e));
			gf_dasher_del(dasher);
			return mp4box_cleanup(1);
		}

		dash_cumulated_time=0;

		while (1) {
			if (run_for && (dash_cumulated_time >= run_for)) {
				fprintf(stderr, "Done running, computing static MPD\n");
				do_abort = 3;
			}

			dash_prev_time=gf_sys_clock();
			if (do_abort>=2) {
				e = gf_dasher_set_dynamic_mode(dasher, GF_DASH_DYNAMIC_LAST, 0, time_shift_depth, mpd_live_duration);
			}

			if (!e) e = gf_dasher_process(dasher);
			if (!dash_live && (e==GF_EOS) ) {
				fprintf(stderr, "Nothing to dash, too early ...\n");
				e = GF_OK;
			}

			if (do_abort)
				break;

			//this happens when reading file while writing them (local playback of the live session ...)
			if (dash_live && (e==GF_IO_ERR) ) {
				fprintf(stderr, "Error dashing file (%s) but continuing ...\n", gf_error_to_string(e) );
				e = GF_OK;
			}

			if (e) break;

			if (dash_live) {
				u64 ms_in_session=0;
				u32 slept = gf_sys_clock();
				u32 sleep_for = gf_dasher_next_update_time(dasher, &ms_in_session);
				fprintf(stderr, "Next generation scheduled in %u ms (DASH time "LLU" ms)\r", sleep_for, ms_in_session);
				if (run_for && (ms_in_session>=run_for)) {
					dash_cumulated_time = 1+run_for;
					continue;
				}

				while (1) {
					if (gf_prompt_has_input()) {
						char c = (char) gf_prompt_get_char();
						if (c=='X') {
							do_abort = 1;
							break;
						}
						if (c=='q') {
							do_abort = 2;
							break;
						}
						if (c=='s') {
							do_abort = 3;
							break;
						}
					}

					if (dash_mode == GF_DASH_DYNAMIC_DEBUG) {
						break;
					}
					if (!sleep_for) break;

					gf_sleep(sleep_for/10);
					sleep_for = gf_dasher_next_update_time(dasher, NULL);
					if (sleep_for<=1) {
						dash_now_time=gf_sys_clock();
						dash_cumulated_time+=(dash_now_time-dash_prev_time);
						fprintf(stderr, "Slept for %d ms before generation, dash cumulated time %d\n", dash_now_time - slept, dash_cumulated_time);
						break;
					}
				}
			} else {
				break;
			}
		}

		gf_dasher_del(dasher);

		if (!run_for && dash_ctx_file && (do_abort==3) && (dyn_state_file) && !gf_sys_is_test_mode() ) {
			char szName[1024];
			fprintf(stderr, "Enter file name to save dash context:\n");
			if (scanf("%1023s", szName) == 1) {
				gf_file_move(dash_ctx_file, szName);
			}
		}
		if (e) fprintf(stderr, "Error DASHing file: %s\n", gf_error_to_string(e));
		if (file) gf_isom_delete(file);
		if (del_file)
			gf_file_delete(inName);

		if (e) return mp4box_cleanup(1);
		goto exit;
	}

	else if (!file && !do_hash
#ifndef GPAC_DISABLE_MEDIA_EXPORT
	         && !(track_dump_type & GF_EXPORT_AVI_NATIVE)
#endif
	        ) {
		FILE *st = gf_fopen(inName, "rb");
		Bool file_exists = 0;
		GF_ISOOpenMode omode;
		if (st) {
			file_exists = 1;
			gf_fclose(st);
		}
		switch (get_file_type_by_ext(inName)) {
		case 1:
			omode =  (u8) (force_new ? GF_ISOM_WRITE_EDIT : (open_edit ? GF_ISOM_OPEN_EDIT : ( ((dump_isom>0) || print_info) ? GF_ISOM_OPEN_READ_DUMP : GF_ISOM_OPEN_READ) ) );

			if (crypt) {
				//keep fragment signaling in moov
				omode = GF_ISOM_OPEN_READ;
				if (use_init_seg)
					file = gf_isom_open(use_init_seg, GF_ISOM_OPEN_READ, tmpdir);
			}
			if (!crypt && use_init_seg) {
				file = gf_isom_open(use_init_seg, GF_ISOM_OPEN_READ_DUMP, tmpdir);
				if (file) {
					e = gf_isom_open_segment(file, inName, 0, 0, 0);
					if (e) {
						fprintf(stderr, "Error opening segment %s: %s\n", inName, gf_error_to_string(e) );
						gf_isom_delete(file);
						file = NULL;
					}
				}
			}
			if (!file)
				file = gf_isom_open(inName, omode, tmpdir);

			if (!file && (gf_isom_last_error(NULL) == GF_ISOM_INCOMPLETE_FILE) && !open_edit) {
				u64 missing_bytes;
				e = gf_isom_open_progressive(inName, 0, 0, GF_FALSE, &file, &missing_bytes);
				fprintf(stderr, "Truncated file - missing "LLD" bytes\n", missing_bytes);
			}

			if (!file) {
				if (open_edit && nb_meta_act) {
					file = gf_isom_open(inName, GF_ISOM_WRITE_EDIT, tmpdir);
					if (!outName && file) outName = inName;
				}

				if (!file) {
					fprintf(stderr, "Error opening file %s: %s\n", inName, gf_error_to_string(gf_isom_last_error(NULL)));
					return mp4box_cleanup(1);
				}
			}
			if (freeze_box_order)
				gf_isom_freeze_order(file);
			break;
		/*allowed for bt<->xmt*/
		case 2:
		case 3:
		/*allowed for svg->lsr**/
		case 4:
		/*allowed for swf->bt, swf->xmt, swf->svg*/
		case 5:
			break;
		/*used for .saf / .lsr dump*/
		case 6:
#ifndef GPAC_DISABLE_SCENE_DUMP
			if ((dump_mode==GF_SM_DUMP_LASER) || (dump_mode==GF_SM_DUMP_SVG)) {
				break;
			}
#endif

		default:
			if (!open_edit && file_exists && !gf_isom_probe_file(inName) && track_dump_type) {
			}
#ifndef GPAC_DISABLE_ISOM_WRITE
			else if (!open_edit && file_exists /* && !gf_isom_probe_file(inName) */
#ifndef GPAC_DISABLE_SCENE_DUMP
			         && dump_mode == GF_SM_DUMP_NONE
#endif //GPAC_DISABLE_SCENE_DUMP
			        ) {
				/*************************************************************************************************/
#ifndef GPAC_DISABLE_MEDIA_IMPORT
				if(dvbhdemux)
				{
					GF_MediaImporter import;
					file = gf_isom_open("ttxt_convert", GF_ISOM_OPEN_WRITE, NULL);
					memset(&import, 0, sizeof(GF_MediaImporter));
					import.dest = file;
					import.in_name = inName;
					import.flags = GF_IMPORT_MPE_DEMUX;
					e = gf_media_import(&import);
					if (e) {
						fprintf(stderr, "Error importing %s: %s\n", inName, gf_error_to_string(e));
						gf_isom_delete(file);
						gf_file_delete("ttxt_convert");
						return mp4box_cleanup(1);
					}
				}
#endif /*GPAC_DISABLE_MEDIA_IMPORT*/

				if (dump_m2ts) {
#ifndef GPAC_DISABLE_MPEG2TS
					dump_mpeg2_ts(inName, pes_dump, program_number);
#endif
				} else if (dump_timestamps) {
#ifndef GPAC_DISABLE_MPEG2TS
					dump_mpeg2_ts(inName, pes_dump, program_number);
#endif
#ifndef GPAC_DISABLE_CORE_TOOLS
				} else if (do_bin_xml) {
					xml_bs_to_bin(inName, outName, dump_std);
#endif
				} else if (do_hash) {
					hash_file(inName, dump_std);
				} else if (print_info) {
#ifndef GPAC_DISABLE_MEDIA_IMPORT
					convert_file_info(inName, info_track_id);
#endif
				} else {
					fprintf(stderr, "Input %s is not an MP4 file, operation not allowed\n", inName);
					return mp4box_cleanup(1);
				}
				goto exit;
			}
#endif /*GPAC_DISABLE_ISOM_WRITE*/
			else if (open_edit) {
				file = gf_isom_open(inName, GF_ISOM_WRITE_EDIT, tmpdir);
				if (!outName && file) outName = inName;
			} else if (!file_exists) {
				fprintf(stderr, "Error creating file %s: %s\n", inName, gf_error_to_string(GF_URL_ERROR));
				return mp4box_cleanup(1);
			} else {
				fprintf(stderr, "Cannot open %s - extension not supported\n", inName);
				return mp4box_cleanup(1);
			}
		}
	}

	if (high_dynamc_range_filename) {
		e = parse_high_dynamc_range_xml_desc(file, high_dynamc_range_filename);
		if (e) goto err_exit;
	}

	if (file && keep_utc && open_edit) {
		gf_isom_keep_utc_times(file, 1);
	}

	strcpy(outfile, outName ? outName : inName);
	{

		char *szExt = gf_file_ext_start(outfile);

		if (szExt)
		{
			/*turn on 3GP saving*/
			if (!stricmp(szExt, ".3gp") || !stricmp(szExt, ".3gpp") || !stricmp(szExt, ".3g2"))
				conv_type = GF_ISOM_CONV_TYPE_3GPP;
			else if (!stricmp(szExt, ".m4a") || !stricmp(szExt, ".m4v"))
				conv_type = GF_ISOM_CONV_TYPE_IPOD;
			else if (!stricmp(szExt, ".psp"))
				conv_type = GF_ISOM_CONV_TYPE_PSP;
			else if (!stricmp(szExt, ".mov") || !stricmp(szExt, ".qt"))
				conv_type = GF_ISOM_CONV_TYPE_MOV;

			//remove extension from outfile
			*szExt = 0;
		}
	}

#ifndef GPAC_DISABLE_MEDIA_EXPORT
	if (track_dump_type & GF_EXPORT_AVI_NATIVE) {
		char szFile[GF_MAX_PATH+24];
		GF_MediaExporter mdump;
		memset(&mdump, 0, sizeof(mdump));
		mdump.in_name = inName;
		mdump.flags = GF_EXPORT_AVI_NATIVE;
		mdump.trackID = trackID;
		if (dump_std) {
			mdump.out_name = "std";
		} else if (outName) {
			mdump.out_name = outName;
		} else if (trackID>2) {
			sprintf(szFile, "%s_audio%d", outfile, trackID-1);
			mdump.out_name = szFile;
		} else {
			sprintf(szFile, "%s_%s", outfile, (trackID==1) ? "video" : "audio");
			mdump.out_name = szFile;
		}

		mdump.print_stats_graph = fs_dump_flags;
		e = gf_media_export(&mdump);
		if (e) goto err_exit;
		goto exit;
	}
	if (!open_edit && track_dump_type && !gf_isom_probe_file(inName)) {
		GF_MediaExporter mdump;
		char szFile[GF_MAX_PATH+24];
		for (i=0; i<nb_track_act; i++) {
			TrackAction *tka = &tracks[i];
			if (tka->act_type != TRAC_ACTION_RAW_EXTRACT) continue;
			memset(&mdump, 0, sizeof(mdump));
			mdump.in_name = inName;
			mdump.flags = tka->dump_type;
			mdump.trackID = tka->trackID;
			mdump.sample_num = tka->sample_num;
			if (outName) {
				mdump.out_name = outName;
				mdump.flags |= GF_EXPORT_MERGE;
			} else if (nb_track_act>1) {
				sprintf(szFile, "%s_track%d", outfile, mdump.trackID);
				mdump.out_name = szFile;
			} else {
				mdump.out_name = outfile;
			}
			mdump.print_stats_graph = fs_dump_flags;
			e = gf_media_export(&mdump);
			if (e) goto err_exit;
		}
		goto exit;
	}

#endif /*GPAC_DISABLE_MEDIA_EXPORT*/

#ifndef GPAC_DISABLE_SCENE_DUMP
	if (dump_mode != GF_SM_DUMP_NONE) {
		e = dump_isom_scene(inName, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_mode, do_scene_log, no_odf_conf);
		if (e) goto err_exit;
	}
#endif

#ifndef GPAC_DISABLE_SCENE_STATS
	if (stat_level) dump_isom_scene_stats(inName, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, stat_level);
#endif

#ifndef GPAC_DISABLE_ISOM_HINTING
	if (!HintIt && print_sdp) dump_isom_sdp(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
#endif
	if (get_nb_tracks) {
		fprintf(stdout, "%d\n", gf_isom_get_track_count(file));
	}
	if (print_info) {
		if (!file) {
			fprintf(stderr, "Cannot print info on a non ISOM file (%s)\n", inName);
		} else {
			if (info_track_id) DumpTrackInfo(file, info_track_id, 1, (print_info==2) ? GF_TRUE : GF_FALSE);
			else DumpMovieInfo(file);
		}
	}
#ifndef GPAC_DISABLE_ISOM_DUMP
	if (dump_isom) {
		e = dump_isom_xml(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, (dump_isom==2) ? GF_TRUE : GF_FALSE, merge_vtt_cues, use_init_seg ? GF_TRUE : GF_FALSE, (dump_isom==3) ? GF_TRUE : GF_FALSE);
		if (e) goto err_exit;
	}
	if (dump_cr) dump_isom_ismacryp(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
	if ((dump_ttxt || dump_srt) && trackID) {

		if (trackID == (u32)-1) {
			for (j=0; j<gf_isom_get_track_count(file); j++) {
				trackID = gf_isom_get_track_id(file, j+1);
				dump_isom_timed_text(file, trackID, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE,
									GF_FALSE, dump_srt ? GF_TEXTDUMPTYPE_SRT : GF_TEXTDUMPTYPE_TTXT);
			}

		}
		else {
			dump_isom_timed_text(file, trackID, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE,
								GF_FALSE, dump_srt ? GF_TEXTDUMPTYPE_SRT : GF_TEXTDUMPTYPE_TTXT);
		}
	}

#ifndef GPAC_DISABLE_ISOM_HINTING
	if (dump_rtp) dump_isom_rtp(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
#endif

#endif

	if (dump_timestamps) dump_isom_timestamps(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_timestamps);
	if (dump_nal) dump_isom_nal(file, dump_nal, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_nal_type);
	if (dump_saps) dump_isom_saps(file, dump_saps, dump_saps_mode, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);

	if (do_hash) {
		e = hash_file(inName, dump_std);
		if (e) goto err_exit;
	}
#ifndef GPAC_DISABLE_CORE_TOOLS
	if (do_bin_xml) {
		e = xml_bs_to_bin(inName, outName, dump_std);
		if (e) goto err_exit;
	}
#endif

	if (dump_cart) dump_isom_cover_art(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
	if (dump_chap) dump_isom_chapters(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_chap);
	if (dump_udta_type) dump_isom_udta(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_udta_type, dump_udta_track);

	if (dump_iod) {
		GF_InitialObjectDescriptor *iod = (GF_InitialObjectDescriptor *)gf_isom_get_root_od(file);
		if (!iod) {
			fprintf(stderr, "File %s has no IOD", inName);
		} else {
			char szName[GF_MAX_PATH+10];
			FILE *iodf;
			sprintf(szName, "%s.iod", outfile);
			iodf = gf_fopen(szName, "wb");
			if (!iodf) {
				fprintf(stderr, "Cannot open destination %s\n", szName);
			} else {
				u8 *desc;
				u32 size;
				GF_BitStream *bs = gf_bs_from_file(iodf, GF_BITSTREAM_WRITE);
				if (gf_odf_desc_write((GF_Descriptor *)iod, &desc, &size)==GF_OK) {
					gf_fwrite(desc, size, iodf);
					gf_free(desc);
				} else {
					fprintf(stderr, "Error writing IOD %s\n", szName);
				}
				gf_fclose(iodf);
				gf_bs_del(bs);
			}
			gf_odf_desc_del((GF_Descriptor*)iod);
		}
	}

#if !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
	if (split_duration || split_size || split_range_str) {
		split_isomedia_file(file, split_duration, split_size, inName, interleaving_time, split_start, adjust_split_end, outName, tmpdir, seg_at_rap, split_range_str);
		/*never save file when splitting is desired*/
		open_edit = GF_FALSE;
		needSave = GF_FALSE;
	}
#endif // !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_MEDIA_IMPORT)

#ifndef GPAC_DISABLE_MEDIA_EXPORT
	if (track_dump_type) {
		char szFile[GF_MAX_PATH+24];
		GF_MediaExporter mdump;
		for (i=0; i<nb_track_act; i++) {
			TrackAction *tka = &tracks[i];
			if (tka->act_type != TRAC_ACTION_RAW_EXTRACT) continue;
			memset(&mdump, 0, sizeof(mdump));
			mdump.file = file;
			mdump.flags = tka->dump_type;
			mdump.trackID = tka->trackID;
			mdump.sample_num = tka->sample_num;
			if (tka->out_name) {
				mdump.out_name = tka->out_name;
			} else if (outName) {
				mdump.out_name = outName;
				mdump.flags |= GF_EXPORT_MERGE;
				/*don't infer extension on user-given filename*/
				mdump.flags |= GF_EXPORT_NO_FILE_EXT;
			} else if (mdump.trackID) {
				sprintf(szFile, "%s_track%d", outfile, mdump.trackID);
				mdump.out_name = szFile;
			} else {
				sprintf(szFile, "%s_export", outfile);
				mdump.out_name = szFile;
			}
			if (tka->trackID==(u32) -1) {
				for (j=0; j<gf_isom_get_track_count(file); j++) {
					mdump.trackID = gf_isom_get_track_id(file, j+1);
					sprintf(szFile, "%s_track%d", outfile, mdump.trackID);
					mdump.out_name = szFile;
					mdump.print_stats_graph = fs_dump_flags;
					e = gf_media_export(&mdump);
					if (e) goto err_exit;
				}
			} else {
				mdump.print_stats_graph = fs_dump_flags;
				e = gf_media_export(&mdump);
				if (e) goto err_exit;
			}
		}
	} else if (do_saf) {
		GF_MediaExporter mdump;
		memset(&mdump, 0, sizeof(mdump));
		mdump.file = file;
		mdump.flags = GF_EXPORT_SAF;
		mdump.out_name = outfile;
		mdump.print_stats_graph = fs_dump_flags;
		e = gf_media_export(&mdump);
		if (e) goto err_exit;
	}
#endif

	for (i=0; i<nb_meta_act; i++) {
		u32 tk = 0;
#ifndef GPAC_DISABLE_ISOM_WRITE
		Bool self_ref;
#endif
		MetaAction *meta = &metas[i];

		if (meta->trackID) tk = gf_isom_get_track_by_id(file, meta->trackID);

		switch (meta->act_type) {
#ifndef GPAC_DISABLE_ISOM_WRITE
		case META_ACTION_SET_TYPE:
			/*note: we don't handle file brand modification, this is an author stuff and cannot be guessed from meta type*/
			e = gf_isom_set_meta_type(file, meta->root_meta, tk, meta->meta_4cc);
			gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_ISO2, GF_TRUE);
			needSave = GF_TRUE;
			break;
		case META_ACTION_ADD_ITEM:
			self_ref = !stricmp(meta->szPath, "NULL") || !stricmp(meta->szPath, "this") || !stricmp(meta->szPath, "self");
			e = gf_isom_add_meta_item(file, meta->root_meta, tk, self_ref, self_ref ? NULL : meta->szPath,
			                          meta->szName,
			                          meta->item_id,
									  meta->item_type,
			                          meta->mime_type,
			                          meta->enc_type,
			                          meta->use_dref ? meta->szPath : NULL,  NULL,
			                          meta->image_props);
			if (meta->ref_type) {
				e = gf_isom_meta_add_item_ref(file, meta->root_meta, tk, meta->item_id, meta->ref_item_id, meta->ref_type, NULL);
			}
			needSave = GF_TRUE;
			break;
		case META_ACTION_ADD_IMAGE_ITEM:
		{
			u32 old_tk_count = gf_isom_get_track_count(file);
			GF_Fraction _frac = {0,0};
			e = import_file(file, meta->szPath, 0, _frac, 0, NULL, NULL, 0);
			if (e == GF_OK) {
				u32 meta_type = gf_isom_get_meta_type(file, meta->root_meta, tk);
				if (!meta_type) {
					e = gf_isom_set_meta_type(file, meta->root_meta, tk, GF_META_ITEM_TYPE_PICT);
				} else {
					if (meta_type != GF_META_ITEM_TYPE_PICT) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("Warning: file already has a root 'meta' box of type %s\n", gf_4cc_to_str(meta_type)));
						e = GF_BAD_PARAM;
					}
				}
				if (e == GF_OK) {
					if (!meta->item_id) {
						e = gf_isom_meta_get_next_item_id(file, meta->root_meta, tk, &meta->item_id);
					}
					if (e == GF_OK) {
						e = gf_isom_iff_create_image_item_from_track(file, meta->root_meta, tk, 1,
								meta->szName,
								meta->item_id,
								meta->image_props, NULL);
						if (e == GF_OK && meta->primary) {
							e = gf_isom_set_meta_primary_item(file, meta->root_meta, tk, meta->item_id);
						}
						if (e == GF_OK && meta->ref_type) {
							e = gf_isom_meta_add_item_ref(file, meta->root_meta, tk, meta->item_id, meta->ref_item_id, meta->ref_type, NULL);
						}
					}
				}
			}
			gf_isom_remove_track(file, old_tk_count+1);
			needSave = GF_TRUE;
		}
			break;
		case META_ACTION_REM_ITEM:
			e = gf_isom_remove_meta_item(file, meta->root_meta, tk, meta->item_id);
			needSave = GF_TRUE;
			break;
		case META_ACTION_SET_PRIMARY_ITEM:
			e = gf_isom_set_meta_primary_item(file, meta->root_meta, tk, meta->item_id);
			needSave = GF_TRUE;
			break;
		case META_ACTION_SET_XML:
		case META_ACTION_SET_BINARY_XML:
			e = gf_isom_set_meta_xml(file, meta->root_meta, tk, meta->szPath, NULL, 0, (meta->act_type==META_ACTION_SET_BINARY_XML) ? 1 : 0);
			needSave = GF_TRUE;
			break;
		case META_ACTION_REM_XML:
			if (gf_isom_get_meta_item_count(file, meta->root_meta, tk)) {
				e = gf_isom_remove_meta_xml(file, meta->root_meta, tk);
				needSave = GF_TRUE;
			} else {
				fprintf(stderr, "No meta box in input file\n");
			}
			break;
		case META_ACTION_DUMP_ITEM:
			if (gf_isom_get_meta_item_count(file, meta->root_meta, tk)) {
				e = gf_isom_extract_meta_item(file, meta->root_meta, tk, meta->item_id, strlen(meta->szPath) ? meta->szPath : NULL);
			} else {
				fprintf(stderr, "No meta box in input file\n");
			}
			break;
#endif // GPAC_DISABLE_ISOM_WRITE

		case META_ACTION_DUMP_XML:
			if (gf_isom_has_meta_xml(file, meta->root_meta, tk)) {
				e = gf_isom_extract_meta_xml(file, meta->root_meta, tk, meta->szPath, NULL);
			} else {
				fprintf(stderr, "No meta box in input file\n");
			}
			break;
		default:
			break;
		}
		if (meta->image_props) {
			gf_free(meta->image_props);
			meta->image_props = NULL;
		}
		if (e) goto err_exit;
	}
	if (!open_edit && !needSave) {
		if (file) gf_isom_delete(file);
		goto exit;
	}


#ifndef GPAC_DISABLE_ISOM_WRITE
	if (clean_groups) {
		e = gf_isom_reset_switch_parameters(file);
		if (e) goto err_exit;
		needSave = GF_TRUE;
	}

	for (i=0; i<nb_tsel_acts; i++) {
		switch (tsel_acts[i].act_type) {
		case TSEL_ACTION_SET_PARAM:
			e = gf_isom_set_track_switch_parameter(file,
			                                       gf_isom_get_track_by_id(file, tsel_acts[i].trackID),
			                                       tsel_acts[i].refTrackID ? gf_isom_get_track_by_id(file, tsel_acts[i].refTrackID) : 0,
			                                       tsel_acts[i].is_switchGroup ? 1 : 0,
			                                       &tsel_acts[i].switchGroupID,
			                                       tsel_acts[i].criteria, tsel_acts[i].nb_criteria);
			if (e == GF_BAD_PARAM) {
				u32 alternateGroupID, nb_groups;
				gf_isom_get_track_switch_group_count(file, gf_isom_get_track_by_id(file, tsel_acts[i].trackID), &alternateGroupID, &nb_groups);
				if (alternateGroupID)
					fprintf(stderr, "Hint: for adding more tracks to group, using: -group-add -refTrack=ID1:[criteria:]trackID=ID2\n");
				else
					fprintf(stderr, "Hint: for creates a new grouping information, using -group-add -trackID=ID1:[criteria:]trackID=ID2\n");
			}
			if (e) goto err_exit;
			needSave = GF_TRUE;
			break;
		case TSEL_ACTION_REMOVE_TSEL:
			e = gf_isom_reset_track_switch_parameter(file, gf_isom_get_track_by_id(file, tsel_acts[i].trackID), 0);
			if (e) goto err_exit;
			needSave = GF_TRUE;
			break;
		case TSEL_ACTION_REMOVE_ALL_TSEL_IN_GROUP:
			e = gf_isom_reset_track_switch_parameter(file, gf_isom_get_track_by_id(file, tsel_acts[i].trackID), 1);
			if (e) goto err_exit;
			needSave = GF_TRUE;
			break;
		default:
			break;
		}
	}

	if (remove_sys_tracks) {
#ifndef GPAC_DISABLE_AV_PARSERS
		remove_systems_tracks(file);
#endif
		needSave = GF_TRUE;
		if (conv_type < GF_ISOM_CONV_TYPE_ISMA_EX) conv_type = 0;
	}
	if (remove_root_od) {
		gf_isom_remove_root_od(file);
		needSave = GF_TRUE;
	}
#ifndef GPAC_DISABLE_ISOM_HINTING
	if (remove_hint) {
		for (i=0; i<gf_isom_get_track_count(file); i++) {
			if (gf_isom_get_media_type(file, i+1) == GF_ISOM_MEDIA_HINT) {
				fprintf(stderr, "Removing hint track ID %d\n", gf_isom_get_track_id(file, i+1));
				gf_isom_remove_track(file, i+1);
				i--;
			}
		}
		gf_isom_sdp_clean(file);
		needSave = GF_TRUE;
	}
#endif // GPAC_DISABLE_ISOM_HINTING

	if (timescale && (timescale != gf_isom_get_timescale(file))) {
		gf_isom_set_timescale(file, timescale);
		needSave = GF_TRUE;
	}

	if (!encode) {
		if (!file) {
			fprintf(stderr, "Nothing to do - exiting\n");
			goto exit;
		}
		if (outName) {
			strcpy(outfile, outName);
		} else {
			char *rel_name = strrchr(inName, GF_PATH_SEPARATOR);
			if (!rel_name) rel_name = strrchr(inName, '/');

			strcpy(outfile, "");
			if (tmpdir) {
				strcpy(outfile, tmpdir);
				if (!strchr("\\/", tmpdir[strlen(tmpdir)-1])) strcat(outfile, "/");
			}
			if (!pack_file) strcat(outfile, "out_");
			strcat(outfile, rel_name ? rel_name + 1 : inName);

			if (pack_file) {
				strcpy(outfile, rel_name ? rel_name + 1 : inName);
				rel_name = strrchr(outfile, '.');
				if (rel_name) rel_name[0] = 0;
				strcat(outfile, ".m21");
			}
		}
#ifndef GPAC_DISABLE_MEDIA_IMPORT
		if ((conv_type == GF_ISOM_CONV_TYPE_ISMA) || (conv_type == GF_ISOM_CONV_TYPE_ISMA_EX)) {
			fprintf(stderr, "Converting to ISMA Audio-Video MP4 file...\n");
			/*keep ESIDs when doing ISMACryp*/
			e = gf_media_make_isma(file, crypt ? 1 : 0, GF_FALSE, (conv_type==GF_ISOM_CONV_TYPE_ISMA_EX) ? 1 : 0);
			if (e) goto err_exit;
			needSave = GF_TRUE;
		}
		if (conv_type == GF_ISOM_CONV_TYPE_3GPP) {
			fprintf(stderr, "Converting to 3GP file...\n");
			e = gf_media_make_3gpp(file);
			if (e) goto err_exit;
			needSave = GF_TRUE;
		}
		if (conv_type == GF_ISOM_CONV_TYPE_PSP) {
			fprintf(stderr, "Converting to PSP file...\n");
			e = gf_media_make_psp(file);
			if (e) goto err_exit;
			needSave = GF_TRUE;
		}
		if (conv_type == GF_ISOM_CONV_TYPE_MOV) {
			e = gf_media_check_qt_prores(file);
			if (e) goto err_exit;
			needSave = GF_TRUE;
			if (interleaving_time) interleaving_time = 0.5;
		}
#endif /*GPAC_DISABLE_MEDIA_IMPORT*/
		if (conv_type == GF_ISOM_CONV_TYPE_IPOD) {
			u32 ipod_major_brand = 0;
			fprintf(stderr, "Setting up iTunes/iPod file...\n");

			for (i=0; i<gf_isom_get_track_count(file); i++) {
				u32 mType = gf_isom_get_media_type(file, i+1);
				switch (mType) {
				case GF_ISOM_MEDIA_VISUAL:
                case GF_ISOM_MEDIA_AUXV:
                case GF_ISOM_MEDIA_PICT:
					ipod_major_brand = GF_ISOM_BRAND_M4V;
					gf_isom_set_ipod_compatible(file, i+1);
					break;
				case GF_ISOM_MEDIA_AUDIO:
					if (!ipod_major_brand) ipod_major_brand = GF_ISOM_BRAND_M4A;
					else gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_M4A, GF_TRUE);
					break;
				case GF_ISOM_MEDIA_TEXT:
					/*this is a text track track*/
					if (gf_isom_get_media_subtype(file, i+1, 1) == GF_ISOM_SUBTYPE_TX3G) {
						Bool is_chap = 0;
						for (j=0; j<gf_isom_get_track_count(file); j++) {
							s32 count = gf_isom_get_reference_count(file, j+1, GF_ISOM_REF_CHAP);
							if (count>0) {
								u32 tk, k;
								for (k=0; k<(u32) count; k++) {
									gf_isom_get_reference(file, j+1, GF_ISOM_REF_CHAP, k+1, &tk);
									if (tk==i+1) {
										is_chap = 1;
										break;
									}
								}
								if (is_chap) break;
							}
							if (is_chap) break;
						}
						/*this is a subtitle track*/
						if (!is_chap)
							gf_isom_set_media_type(file, i+1, GF_ISOM_MEDIA_SUBT);
					}
					break;
				}
			}
			gf_isom_set_brand_info(file, ipod_major_brand, 1);
			gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_MP42, GF_TRUE);
			needSave = GF_TRUE;
		}

	} else if (outName) {
		strcpy(outfile, outName);
	}

	for (j=0; j<nb_track_act; j++) {
		TrackAction *tka = &tracks[j];
		u32 track = tka->trackID ? gf_isom_get_track_by_id(file, tka->trackID) : 0;

		timescale = gf_isom_get_timescale(file);
		switch (tka->act_type) {
		case TRAC_ACTION_REM_TRACK:
			e = gf_isom_remove_track(file, track);
			if (e) {
				fprintf(stderr, "Error Removing track ID %d: %s\n", tka->trackID, gf_error_to_string(e));
			} else {
				fprintf(stderr, "Removing track ID %d\n", tka->trackID);
			}
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_SET_LANGUAGE:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				e = gf_isom_set_media_language(file, i+1, tka->lang);
				if (e) goto err_exit;
				needSave = GF_TRUE;
			}
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_SET_KIND:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				e = gf_isom_add_track_kind(file, i+1, tka->kind_scheme, tka->kind_value);
				if (e) goto err_exit;
				needSave = GF_TRUE;
			}
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_REM_KIND:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				e = gf_isom_remove_track_kind(file, i+1, tka->kind_scheme, tka->kind_value);
				if (e) goto err_exit;
				needSave = GF_TRUE;
			}
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_SET_DELAY:
			if (tka->delay_ms) {
				u64 tk_dur;

				gf_isom_remove_edits(file, track);
				tk_dur = gf_isom_get_track_duration(file, track);
				if (gf_isom_get_edits_count(file, track))
					needSave = GF_TRUE;
				if (tka->delay_ms>0) {
					gf_isom_append_edit(file, track, (timescale*tka->delay_ms)/1000, 0, GF_ISOM_EDIT_EMPTY);
					gf_isom_append_edit(file, track, tk_dur, 0, GF_ISOM_EDIT_NORMAL);
					needSave = GF_TRUE;
				} else {
					u64 to_skip = (timescale*(-tka->delay_ms))/1000;
					if (to_skip<tk_dur) {
						u64 media_time = (-tka->delay_ms)*gf_isom_get_media_timescale(file, track) / 1000;
						gf_isom_append_edit(file, track, tk_dur-to_skip, media_time, GF_ISOM_EDIT_NORMAL);
						needSave = GF_TRUE;
					} else {
						fprintf(stderr, "Warning: request negative delay longer than track duration - ignoring\n");
					}
				}
			} else if (gf_isom_get_edits_count(file, track)) {
				gf_isom_remove_edits(file, track);
				needSave = GF_TRUE;
			}
			break;
		case TRAC_ACTION_SET_KMS_URI:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				if (!gf_isom_is_media_encrypted(file, i+1, 1)) continue;
				if (!gf_isom_is_ismacryp_media(file, i+1, 1)) continue;
				e = gf_isom_change_ismacryp_protection(file, i+1, 1, NULL, (char *) tka->kms);
				if (e) goto err_exit;
				needSave = GF_TRUE;
			}
			break;
		case TRAC_ACTION_SET_ID:
			if (!tka->trackID && (gf_isom_get_track_count(file) == 1)) {
				fprintf(stderr, "Warning: track id is not specified, but file has only one track - assume that you want to change id for this track\n");
				track = 1;
			}
			if (track) {
				u32 newTrack;
				newTrack = gf_isom_get_track_by_id(file, tka->newTrackID);
				if (newTrack != 0) {
					fprintf(stderr, "Error: Cannot set track id with value %d because a track already exists - ignoring", tka->newTrackID);
				} else {
					e = gf_isom_set_track_id(file, track, tka->newTrackID);
					needSave = GF_TRUE;
				}
			} else {
				fprintf(stderr, "Error: Cannot change id for track %d because it does not exist - ignoring", tka->trackID);
			}
			break;
		case TRAC_ACTION_SWAP_ID:
			if (track) {
				u32 tk1, tk2;
				tk1 = gf_isom_get_track_by_id(file, tka->trackID);
				tk2 = gf_isom_get_track_by_id(file, tka->newTrackID);
				if (!tk1 || !tk2) {
					fprintf(stderr, "Error: Cannot swap track IDs because not existing - ignoring");
				} else {
					e = gf_isom_set_track_id(file, tk2, 0);
					if (!e) e = gf_isom_set_track_id(file, tk1, tka->newTrackID);
					if (!e) e = gf_isom_set_track_id(file, tk2, tka->trackID);
					needSave = GF_TRUE;
				}
			} else {
				fprintf(stderr, "Error: Cannot change id for track %d because it does not exist - ignoring", tka->trackID);
			}
			break;
		case TRAC_ACTION_SET_PAR:
			e = gf_media_change_par(file, track, tka->par_num, tka->par_den, tka->force_par, tka->rewrite_bs);
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_SET_CLAP:
			e = gf_isom_set_clean_aperture(file, track, 1, tka->clap_wnum, tka->clap_wden, tka->clap_hnum, tka->clap_hden, tka->clap_honum, tka->clap_hoden, tka->clap_vonum, tka->clap_voden);
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_SET_MX:
			e = gf_isom_set_track_matrix(file, track, tka->mx);
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_SET_HANDLER_NAME:
			e = gf_isom_set_handler_name(file, track, tka->hdl_name);
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_ENABLE:
			if (!gf_isom_is_track_enabled(file, track)) {
				e = gf_isom_set_track_enabled(file, track, GF_TRUE);
				needSave = GF_TRUE;
			}
			break;
		case TRAC_ACTION_DISABLE:
			if (gf_isom_is_track_enabled(file, track)) {
				e = gf_isom_set_track_enabled(file, track, GF_FALSE);
				needSave = GF_TRUE;
			}
			break;
		case TRAC_ACTION_REFERENCE:
			e = gf_isom_set_track_reference(file, track, GF_4CC(tka->lang[0], tka->lang[1], tka->lang[2], tka->lang[3]), (u32) tka->delay_ms);
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_REM_NON_RAP:
			fprintf(stderr, "Removing non-rap samples from track %d\n", tka->trackID);
			e = gf_media_remove_non_rap(file, track, GF_FALSE);
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_REM_NON_REFS:
			fprintf(stderr, "Removing non-reference samples from track %d\n", tka->trackID);
			e = gf_media_remove_non_rap(file, track, GF_TRUE);
			needSave = GF_TRUE;
			break;
		case TRAC_ACTION_SET_UDTA:
			fprintf(stderr, "Assigning udta box\n");
			e = set_file_udta(file, track, tka->udta_type, tka->src_name, tka->sample_num ? GF_TRUE : GF_FALSE);
			if (e) goto err_exit;
			needSave = GF_TRUE;
			break;
		default:
			break;
		}
		if (e) goto err_exit;
	}

	if (itunes_tags) {
		char *tags = itunes_tags;

		while (tags) {
			char *val;
			char *sep = gf_url_colon_suffix(tags);
			u32 tlen, itag = 0;
			if (sep) {
				while (sep) {
					for (itag=0; itag<nb_itunes_tags; itag++) {
						if (!strnicmp(sep+1, itags[itag].name, strlen(itags[itag].name))) break;
					}
					if (itag<nb_itunes_tags) {
						break;
					}
					sep = gf_url_colon_suffix(sep+1);
				}
				if (sep) sep[0] = 0;
			}
			for (itag=0; itag<nb_itunes_tags; itag++) {
				if (!strnicmp(tags, itags[itag].name, strlen(itags[itag].name))) {
					break;
				}
			}
			if (itag==nb_itunes_tags) {
				fprintf(stderr, "Invalid iTune tag format \"%s\" - ignoring\n", tags);
				tags = NULL;
				continue;
			}
			itag = itags[itag].code;

			val = strchr(tags, '=');
			if (!val) {
				fprintf(stderr, "Invalid iTune tag format \"%s\" (expecting '=') - ignoring\n", tags);
				tags = NULL;
				continue;
			}
			val ++;
			if ((val[0]==':') || !val[0] || !stricmp(val, "NULL") ) val = NULL;

			tlen = val ? (u32) strlen(val) : 0;
			switch (itag) {
			case GF_ISOM_ITUNE_COVER_ART:
			{
				u8 *d=NULL;
				e = GF_OK;
				if (val) {
					char *ext;
					e = gf_file_load_data(val, (u8 **) &d, &tlen);

					ext = strrchr(val, '.');
					if (!stricmp(ext, ".png")) tlen |= 0x80000000;
				}
				if (!e)
					e = gf_isom_apple_set_tag(file, GF_ISOM_ITUNE_COVER_ART, d, tlen);

				if (d) gf_free(d);
			}
			break;
			case GF_ISOM_ITUNE_TEMPO:
				gf_isom_apple_set_tag(file, itag, NULL, val ? atoi(val) : 0);
				break;
			case GF_ISOM_ITUNE_GENRE:
			{
				u8 _v = gf_id3_get_genre_tag(val);
				if (_v) {
					gf_isom_apple_set_tag(file, itag, NULL, _v);
				} else {
					if (!val) val="";
					gf_isom_apple_set_tag(file, itag, (u8 *) val, (u32) strlen(val) );
				}
			}
			break;
			case GF_ISOM_ITUNE_DISK:
			case GF_ISOM_ITUNE_TRACKNUMBER:
			{
				u32 n, t;
				char _t[8];
				n = t = 0;
				if (val) {
					memset(_t, 0, sizeof(char) * 8);
					tlen = (itag == GF_ISOM_ITUNE_DISK) ? 6 : 8;
					if (sscanf(val, "%u/%u", &n, &t) == 2) {
						_t[3] = n;
						_t[2] = n >> 8;
						_t[5] = t;
						_t[4] = t >> 8;
					}
					else if (sscanf(val, "%u", &n) == 1) {
						_t[3] = n;
						_t[2] = n >> 8;
					}
					else tlen = 0;
				}
				if (!val || tlen) gf_isom_apple_set_tag(file, itag, val ? (u8 *)_t : NULL, tlen);
			}
			break;
			case GF_ISOM_ITUNE_GAPLESS:
			case GF_ISOM_ITUNE_COMPILATION:
			{
				u8 _t[1];
				if (val && !stricmp(val, "yes")) _t[0] = 1;
				else  _t[0] = 0;
				gf_isom_apple_set_tag(file, itag, _t, 1);
			}
			break;
			default:
				gf_isom_apple_set_tag(file, itag, (u8 *)val, tlen);
				break;
			}
			needSave = GF_TRUE;

			if (sep) {
				sep[0] = ':';
				tags = sep+1;
			} else {
				tags = NULL;
			}
		}
	}

	if (movie_time) {
		gf_isom_set_creation_time(file, movie_time);
		for (i=0; i<gf_isom_get_track_count(file); i++) {
			gf_isom_set_track_creation_time(file, i+1, movie_time);
		}
		needSave = GF_TRUE;
	}

	if (cprt) {
		e = gf_isom_set_copyright(file, "und", cprt);
		needSave = GF_TRUE;
		if (e) goto err_exit;
	}
	if (chap_file) {
#ifndef GPAC_DISABLE_MEDIA_IMPORT
		e = gf_media_import_chapters(file, chap_file, import_fps, chap_qt);
		needSave = GF_TRUE;
#else
		fprintf(stderr, "Warning: GPAC compiled without Media Import, chapters can't be imported\n");
		e = GF_NOT_SUPPORTED;
#endif
		if (e) goto err_exit;
	}

	if (major_brand) {
		gf_isom_set_brand_info(file, major_brand, minor_version);
		needSave = GF_TRUE;
	}
	for (i=0; i<nb_alt_brand_add; i++) {
		gf_isom_modify_alternate_brand(file, brand_add[i], GF_TRUE);
		needSave = GF_TRUE;
	}
	for (i=0; i<nb_alt_brand_rem; i++) {
		gf_isom_modify_alternate_brand(file, brand_rem[i], GF_FALSE);
		needSave = GF_TRUE;
	}
	if (box_patch_filename) {
		e = gf_isom_apply_box_patch(file, box_patch_trackID, box_patch_filename, GF_FALSE);
		if (e) {
			fprintf(stderr, "Failed to apply box patch %s: %s\n", box_patch_filename, gf_error_to_string(e) );
			goto err_exit;
		}
		needSave = GF_TRUE;
	}

#ifndef GPAC_DISABLE_CRYPTO
	if (crypt) {
		if (!drm_file) {
			fprintf(stderr, "Missing DRM file location - usage '-%s drm_file input_file\n", (crypt==1) ? "crypt" : "decrypt");
			e = GF_BAD_PARAM;
			goto err_exit;
		}
		if (get_file_type_by_ext(inName) != GF_FILE_TYPE_ISO_MEDIA) {
			fprintf(stderr, "MP4Box can crypt only ISOMedia File\n");
			e = GF_BAD_PARAM;
			goto err_exit;
		}
		if (crypt == 1) {
			if (use_init_seg) {
				e = gf_crypt_fragment(file, drm_file, outfile, inName, fs_dump_flags);
			} else {
				e = gf_crypt_file(file, drm_file, outfile, interleaving_time, fs_dump_flags);
			}
		} else if (crypt ==2) {
			if (use_init_seg) {
				e = gf_decrypt_fragment(file, drm_file, outfile, inName, fs_dump_flags);
			} else {
				e = gf_decrypt_file(file, drm_file, outfile, interleaving_time, fs_dump_flags);
			}
		}
		if (e) goto err_exit;
		needSave = outName ? GF_FALSE : GF_TRUE;

		if (!Frag && !HintIt && !FullInter && !force_co64) {
			char szName[GF_MAX_PATH];
			strcpy(szName, gf_isom_get_filename(file) );
			gf_isom_delete(file);
			file = NULL;
			if (!outName) {
				e = gf_file_move(outfile, szName);
				if (e) goto err_exit;
			}
			goto exit;
		}
	}
#endif /*GPAC_DISABLE_CRYPTO*/

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (Frag) {
		if (!interleaving_time) interleaving_time = DEFAULT_INTERLEAVING_IN_SEC;
		if (HintIt) fprintf(stderr, "Warning: cannot hint and fragment - ignoring hint\n");
		fprintf(stderr, "Fragmenting file (%.3f seconds fragments)\n", interleaving_time);
		e = gf_media_fragment_file(file, outfile, interleaving_time, use_mfra);
		if (e) fprintf(stderr, "Error while fragmenting file: %s\n", gf_error_to_string(e));
		if (!e && !outName) {
			if (gf_file_exists(inName) && gf_file_delete(inName)) fprintf(stderr, "Error removing file %s\n", inName);
			else if (gf_file_move(outfile, inName)) fprintf(stderr, "Error renaming file %s to %s\n", outfile, inName);
		}
		if (e) goto err_exit;
		gf_isom_delete(file);
		goto exit;
	}
#endif

#ifndef GPAC_DISABLE_ISOM_HINTING
	if (HintIt) {
		if (force_ocr) SetupClockReferences(file);
		fprintf(stderr, "Hinting file with Path-MTU %d Bytes\n", MTUSize);
		MTUSize -= 12;
		e = HintFile(file, MTUSize, max_ptime, rtp_rate, hint_flags, HintCopy, HintInter, regular_iod, single_group, hint_no_offset);
		if (e) goto err_exit;
		needSave = GF_TRUE;
		if (print_sdp) dump_isom_sdp(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
	}
#endif

#if !defined(GPAC_DISABLE_ISOM_HINTING) && !defined(GPAC_DISABLE_SENG)
	for (i=0; i<nb_sdp_ex; i++) {
		if (sdp_lines[i].trackID) {
			u32 track = gf_isom_get_track_by_id(file, sdp_lines[i].trackID);
			if (gf_isom_get_media_type(file, track)!=GF_ISOM_MEDIA_HINT) {
				s32 ref_count;
				u32 k, count = gf_isom_get_track_count(file);
				for (j=0; j<count; j++) {
					if (gf_isom_get_media_type(file, j+1)!=GF_ISOM_MEDIA_HINT) continue;
					ref_count = gf_isom_get_reference_count(file, j+1, GF_ISOM_REF_HINT);
					if (ref_count<0) continue;
					for (k=0; k<(u32) ref_count; k++) {
						u32 refTk;
						if (gf_isom_get_reference(file, j+1, GF_ISOM_REF_HINT, k+1, &refTk)) continue;
						if (refTk==track) {
							track = j+1;
							j=count;
							break;
						}
					}
				}
			}
			gf_isom_sdp_add_track_line(file, track, sdp_lines[i].line);
			needSave = GF_TRUE;
		} else {
			gf_isom_sdp_add_line(file, sdp_lines[i].line);
			needSave = GF_TRUE;
		}
	}
#endif /*!defined(GPAC_DISABLE_ISOM_HINTING) && !defined(GPAC_DISABLE_SENG)*/

	/*full interleave (sample-based) if just hinted*/
	if (FullInter) {
		e = gf_isom_set_storage_mode(file, GF_ISOM_STORE_TIGHT);
	} else if (do_flat) {
		e = gf_isom_set_storage_mode(file, (do_flat==1) ? GF_ISOM_STORE_FLAT : GF_ISOM_STORE_STREAMABLE);
		needSave = GF_TRUE;
	} else {
		e = gf_isom_make_interleave(file, interleaving_time);
		if (!e && old_interleave) e = gf_isom_set_storage_mode(file, GF_ISOM_STORE_INTERLEAVED);
	}
	if (force_co64)
		gf_isom_force_64bit_chunk_offset(file, GF_TRUE);

	if (compress_moov)
		gf_isom_enable_compression(file, GF_ISO_COMP_MOOV, GF_FALSE);

	if (e) goto err_exit;

	if (!encode && !force_new) gf_isom_set_final_name(file, outfile);
	if (needSave) {

		if (!gf_sys_is_quiet()) {
			if (outName) {
				gf_isom_set_final_name(file, outfile);
			} else if (encode || pack_file) {
				fprintf(stderr, "Saving to %s: ", gf_isom_get_filename(file) );
			} else {
				fprintf(stderr, "Saving %s: ", inName);
			}
			if (HintIt && FullInter) fprintf(stderr, "Hinted file - Full Interleaving\n");
			else if (FullInter) fprintf(stderr, "Full Interleaving\n");
			else if ((force_new==2) && interleaving_time) fprintf(stderr, "Fast-start interleaved storage\n");
			else if (do_flat || !interleaving_time) fprintf(stderr, "Flat storage\n");
			else fprintf(stderr, "%.3f secs Interleaving%s\n", interleaving_time, old_interleave ? " - no drift control" : "");
		} else {
			if (outName)
				gf_isom_set_final_name(file, outfile);
		}

		e = gf_isom_close(file);
		file = NULL;

		if (!e && !outName && !encode && !force_new && !pack_file) {
			if (gf_file_exists(inName)) {
				e = gf_file_delete(inName);
				if (e) {
					fprintf(stderr, "Error removing file %s\n", inName);
				}
			}
			
			e = gf_file_move(outfile, inName);
			if (e) {
				fprintf(stderr, "Error renaming file %s to %s\n", outfile, inName);
			}
		}
	} else {
		gf_isom_delete(file);
	}

	if (e) {
		fprintf(stderr, "Error: %s\n", gf_error_to_string(e));
		goto err_exit;
	}
	goto exit;

#else
	/*close libgpac*/
	gf_isom_delete(file);
	fprintf(stderr, "Error: Read-only version of MP4Box.\n");
	return mp4box_cleanup(1);
#endif //GPAC_DISABLE_ISOM_WRITE


err_exit:
	/*close libgpac*/
	if (file) gf_isom_delete(file);
	fprintf(stderr, "\n\tError: %s\n", gf_error_to_string(e));
	return mp4box_cleanup(1);

exit:
	mp4box_cleanup(0);

#ifdef GPAC_MEMORY_TRACKING
	if (mem_track && (gf_memory_size() || gf_file_handles_count() )) {
		gf_log_set_tool_level(GF_LOG_MEMORY, GF_LOG_INFO);
		gf_memory_print();
		return 2;
	}
#endif
	return 0;
}