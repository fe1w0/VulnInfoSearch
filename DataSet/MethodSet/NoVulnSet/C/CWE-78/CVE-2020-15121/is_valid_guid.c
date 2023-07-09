static bool is_valid_guid(const char *guid) {
	if (!guid) {
		return false;
	}
	size_t i;
	for (i = 0; guid[i]; i++) {
		if (!isxdigit (guid[i])) {
			return false;
		}
	}
	return i >= 33; // len of GUID and age
}