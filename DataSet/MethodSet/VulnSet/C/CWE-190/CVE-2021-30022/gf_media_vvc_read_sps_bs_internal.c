static s32 gf_media_vvc_read_sps_bs_internal(GF_BitStream *bs, VVCState *vvc, u8 layer_id, u32 *vui_flag_pos)
{
	s32 vps_id, sps_id;
	u32 i, CtbSizeY;
	VVC_SPS *sps;
	u8 sps_ptl_dpb_hrd_params_present_flag;

	if (vui_flag_pos) *vui_flag_pos = 0;

	sps_id = gf_bs_read_int_log(bs, 4, "sps_id");
	if (sps_id >= 16) {
		return -1;
	}
	vps_id = gf_bs_read_int_log(bs, 4, "vps_id");
	if (vps_id >= 16) {
		return -1;
	}
	if (!vps_id && !vvc->vps[0].state) {
		vvc->vps[0].state = 1;
		vvc->vps[0].num_ptl = 1;
		vvc->vps[0].max_layers = 1;
		vvc->vps[0].all_layers_independent = 1;
	}

	sps = &vvc->sps[sps_id];
	if (!sps->state) {
		sps->state = 1;
		sps->id = sps_id;
		sps->vps_id = vps_id;
	}
	sps->max_sublayers = 1 + gf_bs_read_int_log(bs, 3, "max_sublayers_minus1");
	sps->chroma_format_idc = gf_bs_read_int_log(bs, 2, "chroma_format_idc");
	sps->log2_ctu_size = 5 + gf_bs_read_int_log(bs, 2, "log2_ctu_size_minus5");
	CtbSizeY = 1<<sps->log2_ctu_size;

	sps_ptl_dpb_hrd_params_present_flag = gf_bs_read_int_log(bs, 1, "sps_ptl_dpb_hrd_params_present_flag");
	if (sps_ptl_dpb_hrd_params_present_flag) {
		VVC_ProfileTierLevel ptl, *p_ptl;
		if (sps->vps_id) {
			p_ptl = &ptl;
		} else {
			p_ptl = &vvc->vps[0].ptl[0];
		}
		memset(p_ptl, 0, sizeof(VVC_ProfileTierLevel));
		p_ptl->pt_present = 1;
		p_ptl->ptl_max_tid = sps->max_sublayers;
		vvc_profile_tier_level(bs, p_ptl, 0);
	}
	sps->gdr_enabled = gf_bs_read_int_log(bs, 1, "gdr_enabled");
	sps->ref_pic_resampling = gf_bs_read_int_log(bs, 1, "ref_pic_resampling");
	if (sps->ref_pic_resampling)
		sps->res_change_in_clvs = gf_bs_read_int_log(bs, 1, "res_change_in_clvs");
	sps->width = gf_bs_read_ue_log(bs, "width");
	sps->height = gf_bs_read_ue_log(bs, "height");
	sps->conf_window = gf_bs_read_int_log(bs, 1, "conformance_window_present_flag");
	if (sps->conf_window) {
		sps->cw_left = gf_bs_read_ue_log(bs, "conformance_window_left");
		sps->cw_right = gf_bs_read_ue_log(bs, "conformance_window_right");
		sps->cw_top = gf_bs_read_ue_log(bs, "conformance_window_top");
		sps->cw_bottom = gf_bs_read_ue_log(bs, "conformance_window_bottom");
	}
	sps->subpic_info_present = gf_bs_read_int_log(bs, 1, "subpic_info_present");
	if (sps->subpic_info_present) {
		sps->nb_subpics = 1 + gf_bs_read_ue_log(bs, "nb_subpics_minus1");
		if (sps->nb_subpics>1) {
			u32 tmpWidthVal, tmpHeightVal;
			sps->independent_subpic_flags = gf_bs_read_int_log(bs, 1, "independent_subpic_flags");
			sps->subpic_same_size = gf_bs_read_int_log(bs, 1, "subpic_same_size");

			tmpWidthVal = (sps->width + CtbSizeY-1) / CtbSizeY;
			tmpWidthVal = gf_get_bit_size(tmpWidthVal);
			tmpHeightVal = (sps->height + CtbSizeY-1) / CtbSizeY;
			tmpHeightVal = gf_get_bit_size(tmpHeightVal);

			for (i=0; i<sps->nb_subpics; i++) {
				if( !sps->subpic_same_size || !i) {
					if (i && (sps->width > CtbSizeY))
						gf_bs_read_int_log(bs, tmpWidthVal, "subpic_ctu_top_left_x");
					if (i && (sps->height > CtbSizeY))
						gf_bs_read_int_log(bs, tmpHeightVal, "subpic_ctu_top_left_y");
					if ((i+1 < sps->nb_subpics) && (sps->width > CtbSizeY))
						gf_bs_read_int_log(bs, tmpWidthVal, "subpic_width_minus1");
					if ((i+1 < sps->nb_subpics) && (sps->height > CtbSizeY))
						gf_bs_read_int_log(bs, tmpHeightVal, "subpic_height_minus1");
				}
				if (!sps->independent_subpic_flags) {
					gf_bs_read_int_log(bs, 1, "subpic_treated_as_pic_flag");
					gf_bs_read_int_log(bs, 1, "loop_filter_across_subpic_enabled_flag");
				}
			}
			sps->subpicid_len = gf_bs_read_ue_log(bs, "subpic_id_len_minus1") + 1;
			sps->subpicid_mapping_explicit = gf_bs_read_int_log(bs, 1, "subpic_id_mapping_explicitly_signalled_flag");
			if (sps->subpicid_mapping_explicit) {
				sps->subpicid_mapping_present = gf_bs_read_int_log(bs, 1, "subpic_id_mapping_present_flag");
				if (sps->subpicid_mapping_present) {
					for (i=0; i<sps->nb_subpics; i++) {
						gf_bs_read_ue_log(bs, "subpic_id");
					}
				}
			}
		}
	}
	sps->bitdepth = gf_bs_read_ue_log(bs, "bitdepth_minus8") + 8;
	gf_bs_read_int_log(bs, 1, "entropy_coding_sync_enabled_flag");
	gf_bs_read_int_log(bs, 1, "entry_point_offsets_present_flag");
	sps->log2_max_poc_lsb = 4 + gf_bs_read_int_log(bs, 4, "log2_max_poc_lsb_minus4");
	if ((sps->poc_msb_cycle_flag = gf_bs_read_int_log(bs, 1, "poc_msb_cycle_flag")))
		sps->poc_msb_cycle_len = 1 + gf_bs_read_ue_log(bs, "poc_msb_cycle_len_minus1");

	u8 sps_num_extra_ph_bits = 8 * gf_bs_read_int_log(bs, 2, "sps_num_extra_ph_bytes");
	for (i=0; i<sps_num_extra_ph_bits; i++) {
		if (gf_bs_read_int_log_idx(bs, 1, "extra_ph_bit_present_flag", 1))
			sps->ph_num_extra_bits++;
	}
	u8 sps_num_extra_sh_bits = 8 * gf_bs_read_int_log(bs, 2, "num_extra_sh_bytes");
	for (i=0; i<sps_num_extra_sh_bits; i++) {
		if (gf_bs_read_int_log_idx(bs, 1, "extra_sh_bit_present_flag", i))
			sps->sh_num_extra_bits++;
	}

	if (sps_ptl_dpb_hrd_params_present_flag) {
		u8 sps_sublayer_dpb_params_flag = 0;
		if (sps->max_sublayers>1) {
			sps_sublayer_dpb_params_flag = gf_bs_read_int_log(bs, 1, "sps_sublayer_dpb_params_flag");
		}
		for (i=(sps_sublayer_dpb_params_flag ? 0 : sps->max_sublayers-1); i < sps->max_sublayers; i++ ) {
			gf_bs_read_ue_log_idx(bs, "dpb_max_dec_pic_buffering_minus1", i);
			gf_bs_read_ue_log_idx(bs, "dpb_max_num_reorder_pics", i);
			gf_bs_read_ue_log_idx(bs, "dpb_max_latency_increase_plus1", i);
		}
	}
	gf_bs_read_ue_log(bs, "sps_log2_min_luma_coding_block_size_minus2");
	gf_bs_read_int_log(bs, 1, "sps_partition_constraints_override_enabled_flag");
	gf_bs_read_ue_log(bs, "sps_log2_min_luma_coding_block_size_minus2");
	u8 sps_max_mtt_hierarchy_depth_intra_slice_luma = gf_bs_read_ue_log(bs, "sps_max_mtt_hierarchy_depth_intra_slice_luma");
	if (sps_max_mtt_hierarchy_depth_intra_slice_luma != 0) {
		gf_bs_read_ue_log(bs, "sps_log2_diff_max_bt_min_qt_intra_slice_luma");
		gf_bs_read_ue_log(bs, "sps_log2_diff_max_tt_min_qt_intra_slice_luma");
	}
	u8 sps_qtbtt_dual_tree_intra_flag = 0;
	if (sps->chroma_format_idc) {
		sps_qtbtt_dual_tree_intra_flag = gf_bs_read_int_log(bs, 1, "sps_qtbtt_dual_tree_intra_flag");
	}
	if (sps_qtbtt_dual_tree_intra_flag) {
		gf_bs_read_ue_log(bs, "sps_log2_diff_min_qt_min_cb_intra_slice_chroma");
		u8 sps_max_mtt_hierarchy_depth_intra_slice_chroma = gf_bs_read_ue_log(bs, "sps_max_mtt_hierarchy_depth_intra_slice_chroma");
		if( sps_max_mtt_hierarchy_depth_intra_slice_chroma != 0) {
			gf_bs_read_ue_log(bs, "sps_log2_diff_max_bt_min_qt_intra_slice_chroma");
			gf_bs_read_ue_log(bs, "sps_log2_diff_max_tt_min_qt_intra_slice_chroma");
		}
	}

	gf_bs_read_ue_log(bs, "sps_log2_diff_min_qt_min_cb_inter_slice");
	u8 sps_max_mtt_hierarchy_depth_inter_slice = gf_bs_read_ue_log(bs, "sps_max_mtt_hierarchy_depth_inter_slice");
	if (sps_max_mtt_hierarchy_depth_inter_slice != 0) {
		gf_bs_read_ue_log(bs, "sps_log2_diff_max_bt_min_qt_inter_slice");
		gf_bs_read_ue_log(bs, "sps_log2_diff_max_tt_min_qt_inter_slice");
	}
	//u8 sps_max_luma_transform_size_64_flag = 0;
	if (CtbSizeY > 32) {
		/*sps_max_luma_transform_size_64_flag = */gf_bs_read_int_log(bs, 1, "sps_max_luma_transform_size_64_flag");
	}
	u8 sps_transform_skip_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_transform_skip_enabled_flag");

	if (sps_transform_skip_enabled_flag) {
		gf_bs_read_ue_log(bs, "sps_log2_transform_skip_max_size_minus2");
		gf_bs_read_int_log(bs, 1, "sps_bdpcm_enabled_flag");
	}
	if (gf_bs_read_int_log(bs, 1, "sps_mts_enabled_flag")) {
		gf_bs_read_int_log(bs, 1, "sps_explicit_mts_intra_enabled_flag");
		gf_bs_read_int_log(bs, 1, "sps_explicit_mts_inter_enabled_flag");
	}
	gf_bs_read_int_log(bs, 1, "sps_lfnst_enabled_flag");
	if (sps->chroma_format_idc) {
		u8 sps_joint_cbcr_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_joint_cbcr_enabled_flag");
		u8 sps_same_qp_table_for_chroma_flag = gf_bs_read_int_log(bs, 1, "sps_same_qp_table_for_chroma_flag");
		u32 numQpTables = sps_same_qp_table_for_chroma_flag ? 1 : (sps_joint_cbcr_enabled_flag ? 3 : 2);
		for (i=0; i<numQpTables; i++) {
			gf_bs_read_se_log_idx(bs, "sps_qp_table_start_minus26", i);
			u32 j, sps_num_points_in_qp_table = 1 + gf_bs_read_ue_log_idx(bs, "sps_num_points_in_qp_table_minus1", i);
			for (j=0; j<sps_num_points_in_qp_table; j++) {
				gf_bs_read_ue_log_idx2(bs, "sps_delta_qp_in_val_minus1", i, j);
				gf_bs_read_ue_log_idx2(bs, "sps_delta_qp_diff_val", i, j);
			}
		}
	}
	gf_bs_read_int_log(bs, 1, "sps_sao_enabled_flag");
	sps->alf_enabled_flag = gf_bs_read_int_log(bs, 1, "sps_alf_enabled_flag");
	if (sps->alf_enabled_flag && sps->chroma_format_idc) {
		gf_bs_read_int_log(bs, 1, "sps_ccalf_enabled_flag");
	}
	/*! TODO parse the rest !*/

	return sps_id;
}