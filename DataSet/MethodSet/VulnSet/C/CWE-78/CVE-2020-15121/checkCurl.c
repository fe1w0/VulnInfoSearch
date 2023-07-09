static bool checkCurl() {
	const char nul[] = R_SYS_DEVNULL;
	if (r_sys_cmdf ("curl --version > %s", nul) != 0) {
		return false;
	}
	return true;
}