static s32 gf_media_vvc_read_vps_bs_internal(GF_BitStream *bs, VVCState *vvc, Bool stop_at_vps_ext)
{
	u32 i, j;
	s32 vps_id;
	VVC_VPS *vps;
	Bool vps_default_ptl_dpb_hrd_max_tid_flag=0;

	//nalu header already parsed
	vps_id = gf_bs_read_int_log(bs, 4, "vps_id");
	if (vps_id >= 16) return -1;
	if (!vps_id) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] VPS ID 0 is forbidden\n"));
		return -1;
	}
	vps = &vvc->vps[vps_id];
	if (!vps->state) {
		vps->id = vps_id;
		vps->state = 1;
	}
	vps->max_layers = 1 + gf_bs_read_int_log(bs, 6, "max_layers");
	if (vps->max_layers > MAX_LHVC_LAYERS) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] sorry, %d layers in VPS but only %d supported\n", vps->max_layers, MAX_LHVC_LAYERS));
		return -1;
	}
	vps->max_sub_layers = gf_bs_read_int_log(bs, 3, "max_sub_layers_minus1") + 1;

	if ((vps->max_layers>1) && (vps->max_sub_layers>1))
		vps_default_ptl_dpb_hrd_max_tid_flag = gf_bs_read_int_log(bs, 1, "vps_default_ptl_dpb_hrd_max_tid_flag");

	if (vps->max_layers>1)
		vps->all_layers_independent = gf_bs_read_int_log(bs, 1, "all_layers_independent");

	for (i=0; i<vps->max_layers; i++) {
		u32 layer_id = gf_bs_read_int_log_idx(bs, 6, "layer_id", i);
		if (layer_id>vps->max_layer_id) vps->max_layer_id = layer_id;
		if (i && !vps->all_layers_independent) {
			Bool layer_indep = gf_bs_read_int_log_idx(bs, 1, "layer_independent", i);
			if (!layer_indep) {
				Bool vps_max_tid_ref_present_flag = gf_bs_read_int_log_idx(bs, 1, "vps_max_tid_ref_present_flag", i);
				for (j=0; j<i; j++) {
					Bool vps_direct_ref_layer_flag = gf_bs_read_int_log_idx2(bs, 1, "vps_direct_ref_layer_flag", i, j);
					if (vps_max_tid_ref_present_flag && vps_direct_ref_layer_flag) {
						gf_bs_read_int_log_idx2(bs, 3, "vps_max_tid_il_ref_pics_plus1", i, j);
					}
				}
			}
		}
	}
	vps->num_ptl = 1;
	if (vps->max_layers > 1) {
		if (vps->all_layers_independent) {
			vps->each_layer_is_ols = gf_bs_read_int_log(bs, 1, "each_layer_is_ols");
		}
		if (!vps->each_layer_is_ols) {
			u32 vps_ols_mode_idc = 2;
			if (!vps->all_layers_independent) {
				vps_ols_mode_idc = gf_bs_read_int_log(bs, 2, "vps_ols_mode_idc");
			}
			if (vps_ols_mode_idc==2) {
				u8 vps_num_output_layer_sets = 2 + gf_bs_read_int_log(bs, 8, "vps_num_output_layer_sets_minus2");
				for (i=0; i<vps_num_output_layer_sets; i++) {
					for (j=0; j<vps->max_layers; j++) {
						gf_bs_read_int_log_idx2(bs, 1, "vps_ols_output_layer_flag", i, j);
					}
				}
			}
		}
		vps->num_ptl = 1 + gf_bs_read_int_log(bs, 8, "num_ptl_minus1");
	}
	vps->ptl[0].pt_present = 1;
	for (i=0; i<vps->num_ptl; i++) {
		if (i)
			vps->ptl[i].pt_present = gf_bs_read_int_log_idx(bs, 1, "pt_present", i);
		if (!vps_default_ptl_dpb_hrd_max_tid_flag)
			vps->ptl[i].ptl_max_tid = gf_bs_read_int_log_idx(bs, 3, "ptl_max_tid", i);
		else
			vps->ptl[i].ptl_max_tid = vps->max_sub_layers - 1;;
	}
	//align
	gf_bs_align(bs);

	for (i=0; i<vps->num_ptl; i++) {
		vvc_profile_tier_level(bs, &vps->ptl[i], i);
	}

	//TODO, parse multilayer stuff
	return vps_id;
}