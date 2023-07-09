static inline ut16 r_read_at_le16(const void *src, size_t offset) {
	const ut8 *s = (const ut8*)src + offset;
	return r_read_le16 (s);
}