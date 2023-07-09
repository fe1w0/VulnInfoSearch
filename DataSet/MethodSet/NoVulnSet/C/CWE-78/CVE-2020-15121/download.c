static int download(struct SPDBDownloader *pd) {
	SPDBDownloaderOpt *opt = pd->opt;
	int res = 0;
	int cmd_ret;

	if (!opt->dbg_file || !*opt->dbg_file) {
		// no pdb debug file
		return 0;
	}

	char *abspath_to_file = r_str_newf ("%s%s%s%s%s%s%s",
		opt->symbol_store_path, R_SYS_DIR,
		opt->dbg_file, R_SYS_DIR,
		opt->guid, R_SYS_DIR,
		opt->dbg_file);

	if (r_file_exists (abspath_to_file)) {
		eprintf ("File already downloaded.\n");
		free (abspath_to_file);
		return 1;
	}

	if (checkExtract () || opt->extract == 0) {
		char *extractor_cmd = NULL;
		char *archive_name = strdup (opt->dbg_file);
		archive_name[strlen (archive_name) - 1] = '_';
		char *abspath_to_archive = r_str_newf ("%s%s%s%s%s%s%s",
			opt->symbol_store_path, R_SYS_DIR,
			opt->dbg_file, R_SYS_DIR,
			opt->guid, R_SYS_DIR,
			archive_name);

		eprintf ("Attempting to download compressed pdb in %s\n", abspath_to_archive);
		char *abs_arch_esc = r_str_escape_sh (abspath_to_archive);
#if __WINDOWS__
		char *abs_file_esc = r_str_escape_sh (abspath_to_file);
		// expand %1 %2
		// %1 - absolute path to archive
		// %2 - absolute path to file that will be dearchive
		extractor_cmd = r_str_newf ("expand \"%s\" \"%s\"", abs_arch_esc, abs_file_esc);
		free (abs_file_esc);
#else
		char *abspath_to_dir = r_file_dirname (abspath_to_archive);
		char *abs_dir_esc = r_str_escape_sh (abspath_to_dir);
		// cabextract -d %1 %2
		// %1 - path to directory where to extract all files from cab archive
		// %2 - absolute path to cab archive
		extractor_cmd = r_str_newf ("cabextract -d \"%s\" \"%s\"", abs_arch_esc, abs_dir_esc);
		free (abs_dir_esc);
		free (abspath_to_dir);
#endif
		free (abs_arch_esc);
		res = download_and_write (opt, archive_name);

		if (opt->extract > 0 && res) {
			eprintf ("Attempting to decompress pdb\n");
			if (res && ((cmd_ret = r_sys_cmd (extractor_cmd)) != 0)) {
				eprintf ("cab extractor exited with error %d\n", cmd_ret);
				res = 0;
			}
			r_file_rm (abspath_to_archive);
		}
		free (archive_name);
		free (abspath_to_archive);
	}
	if (res == 0) {
		eprintf ("Falling back to uncompressed pdb\n");
		eprintf ("Attempting to download uncompressed pdb in %s\n", abspath_to_file);
		res = download_and_write (opt, opt->dbg_file);
	}
	free (abspath_to_file);
	return res;
}