static inline ut32 r_read_le32(const void *src) {
	const ut8 *s = (const ut8*)src;
	return (((ut32)s[3]) << 24) | (((ut32)s[2]) << 16) |
		(((ut32)s[1]) << 8) | (((ut32)s[0]) << 0);
}