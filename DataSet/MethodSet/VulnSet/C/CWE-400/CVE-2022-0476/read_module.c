static void read_module(RBuffer *b, ut64 addr, struct minidump_module *module) {
	st64 o_addr = r_buf_seek (b, 0, R_BUF_CUR);
	r_buf_seek (b, addr, R_BUF_SET);
	module->base_of_image = r_buf_read_le64 (b);
	module->size_of_image = r_buf_read_le32 (b);
	module->check_sum = r_buf_read_le32 (b);
	module->time_date_stamp = r_buf_read_le32 (b);
	module->module_name_rva = r_buf_read_le32 (b);
	module->version_info.dw_signature = r_buf_read_le32 (b);
	module->version_info.dw_struc_version = r_buf_read_le32 (b);
	module->version_info.dw_file_version_ms = r_buf_read_le32 (b);
	module->version_info.dw_file_version_ls = r_buf_read_le32 (b);
	module->version_info.dw_product_version_ms = r_buf_read_le32 (b);
	module->version_info.dw_product_version_ls = r_buf_read_le32 (b);
	module->version_info.dw_file_flags_mask = r_buf_read_le32 (b);
	module->version_info.dw_file_flags = r_buf_read_le32 (b);
	module->version_info.dw_file_os = r_buf_read_le32 (b);
	module->version_info.dw_file_type = r_buf_read_le32 (b);
	module->version_info.dw_file_subtype = r_buf_read_le32 (b);
	module->version_info.dw_file_date_ms = r_buf_read_le32 (b);
	module->version_info.dw_file_date_ls = r_buf_read_le32 (b);
	module->cv_record.data_size = r_buf_read_le32 (b);
	module->cv_record.rva = r_buf_read_le32 (b);
	module->misc_record.data_size = r_buf_read_le32 (b);
	module->misc_record.rva = r_buf_read_le32 (b);
	module->reserved_0 = r_buf_read_le64 (b);
	module->reserved_1 = r_buf_read_le64 (b);
	r_buf_seek (b, o_addr, R_BUF_SET);
}