static inline struct htx_blk *htx_add_header(struct htx *htx, const struct ist name,
					     const struct ist value)
{
	struct htx_blk *blk;

	if (name.len > 255 || value.len > 1048575)
		return NULL;

	blk = htx_add_blk(htx, HTX_BLK_HDR, name.len + value.len);
	if (!blk)
		return NULL;

	blk->info += (value.len << 8) + name.len;
	ist2bin_lc(htx_get_blk_ptr(htx, blk), name);
	memcpy(htx_get_blk_ptr(htx, blk)  + name.len, value.ptr, value.len);
	return blk;
}