R_API st64 r_buf_fread_at(RBuffer *b, ut64 addr, ut8 *buf, const char *fmt, int n) {
	r_return_val_if_fail (b && buf && fmt, -1);
	st64 o_addr = r_buf_seek (b, 0, R_BUF_CUR);
	st64 r = r_buf_seek (b, addr, R_BUF_SET);
	if (r < 0) {
		return r;
	}
	r = r_buf_fread (b, buf, fmt, n);
	r_buf_seek (b, o_addr, R_BUF_SET);
	return r;
}