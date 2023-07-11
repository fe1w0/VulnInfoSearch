static s32 gf_media_vvc_read_pps_bs_internal(GF_BitStream *bs, VVCState *vvc)
{
	u32 i;
	s32 pps_id;
	VVC_PPS *pps;

	//NAL header already read
	pps_id = gf_bs_read_int_log(bs, 6, "pps_id");

	if ((pps_id < 0) || (pps_id >= 64)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] wrong PPS ID %d in PPS\n", pps_id));
		return -1;
	}
	pps = &vvc->pps[pps_id];

	if (!pps->state) {
		pps->id = pps_id;
		pps->state = 1;
	}
	pps->sps_id = gf_bs_read_int_log(bs, 4, "sps_id");
	if (pps->sps_id >= 16) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] wrong SPS ID %d in PPS\n", pps->sps_id));
		pps->sps_id=0;
		return -1;
	}
	vvc->sps_active_idx = pps->sps_id; /*set active sps*/
	pps->mixed_nal_types = gf_bs_read_int_log(bs, 1, "mixed_nal_types");
	pps->width = gf_bs_read_ue_log(bs, "width");
	pps->height = gf_bs_read_ue_log(bs, "height");
	pps->conf_window = gf_bs_read_int_log(bs, 1, "conformance_window_flag");
	if (pps->conf_window) {
		pps->cw_left = gf_bs_read_ue_log(bs, "conf_win_left_offset");
		pps->cw_right = gf_bs_read_ue_log(bs, "conf_win_right_offset");
		pps->cw_top = gf_bs_read_ue_log(bs, "conf_win_top_offset");
		pps->cw_bottom = gf_bs_read_ue_log(bs, "conf_win_bottom_offset");
	}
	//scaling window
	if (gf_bs_read_int_log(bs, 1, "scaling_window_explicit_signalling_flag")) {
		gf_bs_read_se_log(bs, "scaling_win_left_offset");
		gf_bs_read_se_log(bs, "scaling_win_right_offset");
		gf_bs_read_se_log(bs, "scaling_win_top_offset");
		gf_bs_read_se_log(bs, "scaling_win_bottom_offset");
	}
	pps->output_flag_present_flag = gf_bs_read_int_log(bs, 1, "output_flag_present_flag");
	pps->no_pic_partition_flag = gf_bs_read_int_log(bs, 1, "no_pic_partition_flag");
	pps->subpic_id_mapping_present_flag = gf_bs_read_int_log(bs, 1, "subpic_id_mapping_present_flag");
	if (pps->subpic_id_mapping_present_flag) {
		u32 pps_subpic_id_len, pps_num_subpics=0;
		if (!pps->no_pic_partition_flag) {
			pps_num_subpics = 1+gf_bs_read_ue_log(bs, "pps_num_subpics_minus1");
		}
		pps_subpic_id_len = 1 + gf_bs_read_ue(bs);
		for (i=0; i<pps_num_subpics; i++) {
			gf_bs_read_int_log_idx(bs, pps_subpic_id_len, "subpic_id", i);
		}
	}
	if (!pps->no_pic_partition_flag) {
		gf_bs_read_int_log(bs, 2, "pps_log2_ctu_size_minus5");
		u32 num_exp_tile_columns = 1 + gf_bs_read_ue_log(bs, "num_exp_tile_columns_minus1");
		u32 num_exp_tile_rows = 1 + gf_bs_read_ue_log(bs, "num_exp_tile_rows_minus1");
		for (i=0; i<num_exp_tile_columns; i++)
			gf_bs_read_ue_log_idx(bs, "tile_column_width_minus1", i);
		for (i=0; i<num_exp_tile_rows; i++)
			gf_bs_read_ue_log_idx(bs, "tile_row_height_minus1", i);

		//todo parse the rest
		return pps_id;
	}


	//todo parse the rest

	return pps_id;
}