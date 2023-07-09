static inline ut16 r_read_le16(const void *src) {
	const ut8 *s = (const ut8*)src;
	return (((ut16)s[1]) << 8) | (((ut16)s[0]) << 0);
}