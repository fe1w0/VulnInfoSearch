static s32 gf_hevc_read_sps_bs_internal(GF_BitStream *bs, HEVCState *hevc, u8 layer_id, u32 *vui_flag_pos)
{
	s32 vps_id, sps_id = -1;
	u32 i, nb_CTUs, depth;
	HEVC_SPS *sps;
	HEVC_VPS *vps;
	HEVC_ProfileTierLevel ptl;
	Bool multiLayerExtSpsFlag;
	u8 sps_ext_or_max_sub_layers_minus1, max_sub_layers_minus1;

	if (vui_flag_pos) *vui_flag_pos = 0;

	//nalu header already parsed
	vps_id = gf_bs_read_int_log(bs, 4, "vps_id");
	if (vps_id >= 16) {
		return -1;
	}
	memset(&ptl, 0, sizeof(ptl));
	max_sub_layers_minus1 = 0;
	sps_ext_or_max_sub_layers_minus1 = 0;
	if (layer_id == 0)
		max_sub_layers_minus1 = gf_bs_read_int_log(bs, 3, "max_sub_layers_minus1");
	else
		sps_ext_or_max_sub_layers_minus1 = gf_bs_read_int_log(bs, 3, "sps_ext_or_max_sub_layers_minus1");
	multiLayerExtSpsFlag = (layer_id != 0) && (sps_ext_or_max_sub_layers_minus1 == 7);
	if (!multiLayerExtSpsFlag) {
		gf_bs_read_int_log(bs, 1, "temporal_id_nesting_flag");
		hevc_profile_tier_level(bs, 1, max_sub_layers_minus1, &ptl, 0);
	}

	sps_id = gf_bs_read_ue_log(bs, "sps_id");
	if ((sps_id < 0) || (sps_id >= 16)) {
		return -1;
	}

	sps = &hevc->sps[sps_id];
	if (!sps->state) {
		sps->state = 1;
		sps->id = sps_id;
		sps->vps_id = vps_id;
	}
	sps->ptl = ptl;
	vps = &hevc->vps[vps_id];
	sps->max_sub_layers_minus1 = 0;
	sps->sps_ext_or_max_sub_layers_minus1 = 0;

	/* default values */
	sps->colour_primaries = 2;
	sps->transfer_characteristic = 2;
	sps->matrix_coeffs = 2;

	//sps_rep_format_idx = 0;
	if (multiLayerExtSpsFlag) {
		sps->update_rep_format_flag = gf_bs_read_int_log(bs, 1, "update_rep_format_flag");
		if (sps->update_rep_format_flag) {
			sps->rep_format_idx = gf_bs_read_int_log(bs, 8, "rep_format_idx");
		}
		else {
			sps->rep_format_idx = vps->rep_format_idx[layer_id];
		}
		sps->width = vps->rep_formats[sps->rep_format_idx].pic_width_luma_samples;
		sps->height = vps->rep_formats[sps->rep_format_idx].pic_height_luma_samples;
		sps->chroma_format_idc = vps->rep_formats[sps->rep_format_idx].chroma_format_idc;
		sps->bit_depth_luma = vps->rep_formats[sps->rep_format_idx].bit_depth_luma;
		sps->bit_depth_chroma = vps->rep_formats[sps->rep_format_idx].bit_depth_chroma;
		sps->separate_colour_plane_flag = vps->rep_formats[sps->rep_format_idx].separate_colour_plane_flag;

		//TODO this is crude ...
		sps->ptl = vps->ext_ptl[0];
	}
	else {
		sps->chroma_format_idc = gf_bs_read_ue_log(bs, "chroma_format_idc");
		if (sps->chroma_format_idc == 3)
			sps->separate_colour_plane_flag = gf_bs_read_int_log(bs, 1, "separate_colour_plane_flag");
		sps->width = gf_bs_read_ue_log(bs, "width");
		sps->height = gf_bs_read_ue_log(bs, "height");
		if ((sps->cw_flag = gf_bs_read_int_log(bs, 1, "conformance_window_flag"))) {
			u32 SubWidthC, SubHeightC;

			if (sps->chroma_format_idc == 1) {
				SubWidthC = SubHeightC = 2;
			}
			else if (sps->chroma_format_idc == 2) {
				SubWidthC = 2;
				SubHeightC = 1;
			}
			else {
				SubWidthC = SubHeightC = 1;
			}

			sps->cw_left = gf_bs_read_ue_log(bs, "conformance_window_left");
			sps->cw_right = gf_bs_read_ue_log(bs, "conformance_window_right");
			sps->cw_top = gf_bs_read_ue_log(bs, "conformance_window_top");
			sps->cw_bottom = gf_bs_read_ue_log(bs, "conformance_window_bottom");

			sps->width -= SubWidthC * (sps->cw_left + sps->cw_right);
			sps->height -= SubHeightC * (sps->cw_top + sps->cw_bottom);
		}
		sps->bit_depth_luma = 8 + gf_bs_read_ue_log(bs, "bit_depth_luma_minus8");
		sps->bit_depth_chroma = 8 + gf_bs_read_ue_log(bs, "bit_depth_chroma_minus8");
	}

	sps->log2_max_pic_order_cnt_lsb = 4 + gf_bs_read_ue_log(bs, "log2_max_pic_order_cnt_lsb_minus4");

	if (!multiLayerExtSpsFlag) {
		sps->sub_layer_ordering_info_present_flag = gf_bs_read_int_log(bs, 1, "sub_layer_ordering_info_present_flag");
		for (i = sps->sub_layer_ordering_info_present_flag ? 0 : sps->max_sub_layers_minus1; i <= sps->max_sub_layers_minus1; i++) {
			gf_bs_read_ue_log_idx(bs, "max_dec_pic_buffering", i);
			gf_bs_read_ue_log_idx(bs, "num_reorder_pics", i);
			gf_bs_read_ue_log_idx(bs, "max_latency_increase", i);
		}
	}

	sps->log2_min_luma_coding_block_size = 3 + gf_bs_read_ue_log(bs, "log2_min_luma_coding_block_size_minus3");
	sps->log2_diff_max_min_luma_coding_block_size = gf_bs_read_ue_log(bs, "log2_diff_max_min_luma_coding_block_size");
	sps->max_CU_width = (1 << (sps->log2_min_luma_coding_block_size + sps->log2_diff_max_min_luma_coding_block_size));
	sps->max_CU_height = (1 << (sps->log2_min_luma_coding_block_size + sps->log2_diff_max_min_luma_coding_block_size));

	sps->log2_min_transform_block_size = 2 + gf_bs_read_ue_log(bs, "log2_min_transform_block_size_minus2");
	sps->log2_max_transform_block_size = sps->log2_min_transform_block_size  + gf_bs_read_ue_log(bs, "log2_max_transform_block_size");

	depth = 0;
	sps->max_transform_hierarchy_depth_inter = gf_bs_read_ue_log(bs, "max_transform_hierarchy_depth_inter");
	sps->max_transform_hierarchy_depth_intra = gf_bs_read_ue_log(bs, "max_transform_hierarchy_depth_intra");
	while ((u32)(sps->max_CU_width >> sps->log2_diff_max_min_luma_coding_block_size) > (u32)(1 << (sps->log2_min_transform_block_size + depth)))
	{
		depth++;
	}
	sps->max_CU_depth = sps->log2_diff_max_min_luma_coding_block_size + depth;

	nb_CTUs = ((sps->width + sps->max_CU_width - 1) / sps->max_CU_width) * ((sps->height + sps->max_CU_height - 1) / sps->max_CU_height);
	sps->bitsSliceSegmentAddress = 0;
	while (nb_CTUs > (u32)(1 << sps->bitsSliceSegmentAddress)) {
		sps->bitsSliceSegmentAddress++;
	}

	sps->scaling_list_enable_flag = gf_bs_read_int_log(bs, 1, "scaling_list_enable_flag");
	if (sps->scaling_list_enable_flag) {
		sps->infer_scaling_list_flag = 0;
		sps->scaling_list_ref_layer_id = 0;
		if (multiLayerExtSpsFlag) {
			sps->infer_scaling_list_flag = gf_bs_read_int_log(bs, 1, "infer_scaling_list_flag");
		}
		if (sps->infer_scaling_list_flag) {
			sps->scaling_list_ref_layer_id = gf_bs_read_int_log(bs, 6, "scaling_list_ref_layer_id");
		}
		else {
			sps->scaling_list_data_present_flag = gf_bs_read_int_log(bs, 1, "scaling_list_data_present_flag");
			if (sps->scaling_list_data_present_flag) {
				hevc_scaling_list_data(bs);
			}
		}
	}
	sps->asymmetric_motion_partitions_enabled_flag = gf_bs_read_int_log(bs, 1, "asymmetric_motion_partitions_enabled_flag");
	sps->sample_adaptive_offset_enabled_flag = gf_bs_read_int_log(bs, 1, "sample_adaptive_offset_enabled_flag");
	if ( (sps->pcm_enabled_flag = gf_bs_read_int_log(bs, 1, "pcm_enabled_flag")) ) {
		sps->pcm_sample_bit_depth_luma_minus1 = gf_bs_read_int_log(bs, 4, "pcm_sample_bit_depth_luma_minus1");
		sps->pcm_sample_bit_depth_chroma_minus1 = gf_bs_read_int_log(bs, 4, "pcm_sample_bit_depth_chroma_minus1");
		sps->log2_min_pcm_luma_coding_block_size_minus3 = gf_bs_read_ue_log(bs, "log2_min_pcm_luma_coding_block_size_minus3");
		sps->log2_diff_max_min_pcm_luma_coding_block_size = gf_bs_read_ue_log(bs, "log2_diff_max_min_pcm_luma_coding_block_size");
		sps->pcm_loop_filter_disable_flag = gf_bs_read_int_log(bs, 1, "pcm_loop_filter_disable_flag");
	}
	sps->num_short_term_ref_pic_sets = gf_bs_read_ue_log(bs, "num_short_term_ref_pic_sets");
	if (sps->num_short_term_ref_pic_sets > 64) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[HEVC] Invalid number of short term reference picture sets %d\n", sps->num_short_term_ref_pic_sets));
		return -1;
	}

	for (i = 0; i < sps->num_short_term_ref_pic_sets; i++) {
		Bool ret = hevc_parse_short_term_ref_pic_set(bs, sps, i);
		/*cannot parse short_term_ref_pic_set, skip VUI parsing*/
		if (!ret) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[HEVC] Invalid short_term_ref_pic_set\n"));
			return -1;
		}
	}
	sps->long_term_ref_pics_present_flag = gf_bs_read_int_log(bs, 1, "long_term_ref_pics_present_flag");
	if (sps->long_term_ref_pics_present_flag) {
		sps->num_long_term_ref_pic_sps = gf_bs_read_ue_log(bs, "num_long_term_ref_pic_sps");
		for (i = 0; i < sps->num_long_term_ref_pic_sps; i++) {
			gf_bs_read_int_log_idx(bs, sps->log2_max_pic_order_cnt_lsb, "lt_ref_pic_poc_lsb_sps", i);
			gf_bs_read_int_log_idx(bs, 1, "used_by_curr_pic_lt_sps_flag", i);
		}
	}
	sps->temporal_mvp_enable_flag = gf_bs_read_int_log(bs, 1, "temporal_mvp_enable_flag");
	sps->strong_intra_smoothing_enable_flag = gf_bs_read_int_log(bs, 1, "strong_intra_smoothing_enable_flag");

	if (vui_flag_pos)
		*vui_flag_pos = (u32)gf_bs_get_bit_offset(bs);

	if ((sps->vui_parameters_present_flag = gf_bs_read_int_log(bs, 1, "vui_parameters_present_flag")) ) {
		sps->aspect_ratio_info_present_flag = gf_bs_read_int_log(bs, 1, "aspect_ratio_info_present_flag");
		if (sps->aspect_ratio_info_present_flag) {
			sps->sar_idc = gf_bs_read_int_log(bs, 8, "aspect_ratio_idc");
			if (sps->sar_idc == 255) {
				sps->sar_width = gf_bs_read_int_log(bs, 16, "aspect_ratio_width");
				sps->sar_height = gf_bs_read_int_log(bs, 16, "aspect_ratio_height");
			}
			else if (sps->sar_idc < 17) {
				sps->sar_width = hevc_sar[sps->sar_idc].w;
				sps->sar_height = hevc_sar[sps->sar_idc].h;
			}
		}

		if ((sps->overscan_info_present = gf_bs_read_int_log(bs, 1, "overscan_info_present")))
			sps->overscan_appropriate = gf_bs_read_int_log(bs, 1, "overscan_appropriate");

		sps->video_signal_type_present_flag = gf_bs_read_int_log(bs, 1, "video_signal_type_present_flag");
		if (sps->video_signal_type_present_flag) {
			sps->video_format = gf_bs_read_int_log(bs, 3, "video_format");
			sps->video_full_range_flag = gf_bs_read_int_log(bs, 1, "video_full_range_flag");
			if ((sps->colour_description_present_flag = gf_bs_read_int_log(bs, 1, "colour_description_present_flag"))) {
				sps->colour_primaries = gf_bs_read_int_log(bs, 8, "colour_primaries");
				sps->transfer_characteristic = gf_bs_read_int_log(bs, 8, "transfer_characteristic");
				sps->matrix_coeffs = gf_bs_read_int_log(bs, 8, "matrix_coefficients");
			}
		}

		if ((sps->chroma_loc_info_present_flag = gf_bs_read_int_log(bs, 1, "chroma_loc_info_present_flag"))) {
			sps->chroma_sample_loc_type_top_field = gf_bs_read_ue_log(bs, "chroma_sample_loc_type_top_field");
			sps->chroma_sample_loc_type_bottom_field = gf_bs_read_ue_log(bs, "chroma_sample_loc_type_bottom_field");
		}

		sps->neutra_chroma_indication_flag = gf_bs_read_int_log(bs, 1, "neutra_chroma_indication_flag");
		sps->field_seq_flag = gf_bs_read_int_log(bs, 1, "field_seq_flag");
		sps->frame_field_info_present_flag = gf_bs_read_int_log(bs, 1, "frame_field_info_present_flag");

		if ((sps->default_display_window_flag = gf_bs_read_int_log(bs, 1, "default_display_window_flag"))) {
			sps->left_offset = gf_bs_read_ue_log(bs, "display_window_left_offset");
			sps->right_offset = gf_bs_read_ue_log(bs, "display_window_right_offset");
			sps->top_offset = gf_bs_read_ue_log(bs, "display_window_top_offset");
			sps->bottom_offset = gf_bs_read_ue_log(bs, "display_window_bottom_offset");
		}

		sps->has_timing_info = gf_bs_read_int_log(bs, 1, "has_timing_info");
		if (sps->has_timing_info) {
			sps->num_units_in_tick = gf_bs_read_int_log(bs, 32, "num_units_in_tick");
			sps->time_scale = gf_bs_read_int_log(bs, 32, "time_scale");
			sps->poc_proportional_to_timing_flag = gf_bs_read_int_log(bs, 1, "poc_proportional_to_timing_flag");
			if (sps->poc_proportional_to_timing_flag)
				sps->num_ticks_poc_diff_one_minus1 = gf_bs_read_ue_log(bs, "num_ticks_poc_diff_one_minus1");
			if ((sps->hrd_parameters_present_flag = gf_bs_read_int_log(bs, 1, "hrd_parameters_present_flag"))) {
				//				GF_LOG(GF_LOG_INFO, GF_LOG_CODING, ("[HEVC] HRD param parsing not implemented\n"));
				return sps_id;
			}
		}

		if (gf_bs_read_int_log(bs, 1, "bitstream_restriction_flag")) {
			gf_bs_read_int_log(bs, 1, "tiles_fixed_structure_flag");
			gf_bs_read_int_log(bs, 1, "motion_vectors_over_pic_boundaries_flag");
			gf_bs_read_int_log(bs, 1, "restricted_ref_pic_lists_flag");
			gf_bs_read_ue_log(bs, "min_spatial_segmentation_idc");
			gf_bs_read_ue_log(bs, "max_bytes_per_pic_denom");
			gf_bs_read_ue_log(bs, "max_bits_per_min_cu_denom");
			gf_bs_read_ue_log(bs, "log2_max_mv_length_horizontal");
			gf_bs_read_ue_log(bs, "log2_max_mv_length_vertical");
		}
	}

	if (gf_bs_read_int_log(bs, 1, "sps_extension_flag")) {
#if 0
		while (gf_bs_available(bs)) {
			/*sps_extension_data_flag */ gf_bs_read_int(bs, 1);
		}
#endif

	}

	return sps_id;
}