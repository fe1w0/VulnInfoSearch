static inline struct f2fs_sb_info *F2FS_P_SB(struct page *page)
{
	return F2FS_M_SB(page_file_mapping(page));
}