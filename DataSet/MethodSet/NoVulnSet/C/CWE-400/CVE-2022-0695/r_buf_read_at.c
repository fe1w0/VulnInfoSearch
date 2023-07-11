R_API st64 r_buf_read_at(RBuffer *b, ut64 addr, ut8 *buf, ut64 len) {
	r_return_val_if_fail (b && buf, -1);
	st64 o_addr = r_buf_seek (b, 0, R_BUF_CUR);
	st64 r = r_buf_seek (b, addr, R_BUF_SET);
	if (r < 0) {
		return r;
	}
	r = r_buf_read (b, buf, len);
	r_buf_seek (b, o_addr, R_BUF_SET);
	return r;
}