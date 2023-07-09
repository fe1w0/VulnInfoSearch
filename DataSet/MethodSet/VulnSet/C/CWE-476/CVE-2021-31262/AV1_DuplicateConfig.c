static GF_AV1Config* AV1_DuplicateConfig(GF_AV1Config const * const cfg) {
	u32 i = 0;
	GF_AV1Config *out = gf_malloc(sizeof(GF_AV1Config));

	out->marker = cfg->marker;
	out->version = cfg->version;
	out->seq_profile = cfg->seq_profile;
	out->seq_level_idx_0 = cfg->seq_level_idx_0;
	out->seq_tier_0 = cfg->seq_tier_0;
	out->high_bitdepth = cfg->high_bitdepth;
	out->twelve_bit = cfg->twelve_bit;
	out->monochrome = cfg->monochrome;
	out->chroma_subsampling_x = cfg->chroma_subsampling_x;
	out->chroma_subsampling_y = cfg->chroma_subsampling_y;
	out->chroma_sample_position = cfg->chroma_sample_position;

	out->initial_presentation_delay_present = cfg->initial_presentation_delay_present;
	out->initial_presentation_delay_minus_one = cfg->initial_presentation_delay_minus_one;
	out->obu_array = gf_list_new();
	for (i = 0; i<gf_list_count(cfg->obu_array); ++i) {
		GF_AV1_OBUArrayEntry *dst = gf_malloc(sizeof(GF_AV1_OBUArrayEntry)), *src = gf_list_get(cfg->obu_array, i);
		dst->obu_length = src->obu_length;
		dst->obu_type = src->obu_type;
		dst->obu = gf_malloc((size_t)dst->obu_length);
		memcpy(dst->obu, src->obu, (size_t)src->obu_length);
		gf_list_add(out->obu_array, dst);
	}
	return out;
}