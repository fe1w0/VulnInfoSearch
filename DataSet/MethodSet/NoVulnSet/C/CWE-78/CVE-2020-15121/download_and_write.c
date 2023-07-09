static bool download_and_write(SPDBDownloaderOpt *opt, const char *file) {
	char *dir = r_str_newf ("%s%s%s%s%s",
		opt->symbol_store_path, R_SYS_DIR,
		opt->dbg_file, R_SYS_DIR,
		opt->guid);
	if (!r_sys_mkdirp (dir)) {
		free (dir);
		return false;
	}
	char *url = r_str_newf ("%s/%s/%s/%s", opt->symbol_server, opt->dbg_file, opt->guid, file);
	int len;
	char *file_buf = r_socket_http_get (url, NULL, &len);
	free (url);
	if (!len || R_STR_ISEMPTY (file_buf)) {
		free (dir);
		free (file_buf);
		return false;
	}
	char *path = r_str_newf ("%s%s%s", dir, R_SYS_DIR, opt->dbg_file);
	FILE *f = fopen (path, "wb");
	if (f) {
		fwrite (file_buf, sizeof (char), (size_t)len, f);
		fclose (f);
	}
	free (dir);
	free (path);
	free (file_buf);
	return true;
}