static GF_Err av1dmx_parse_flush_sample(GF_Filter *filter, GF_AV1DmxCtx *ctx)
{
	u32 pck_size;
	GF_FilterPacket *pck;
	u8 *output;

	gf_bs_get_content_no_truncate(ctx->state.bs, &ctx->state.frame_obus, &pck_size, &ctx->state.frame_obus_alloc);

	if (!pck_size) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[AV1Dmx] no frame OBU, skipping OBU\n"));
		return GF_OK;
	}

	pck = gf_filter_pck_new_alloc(ctx->opid, pck_size, &output);
	if (ctx->src_pck) gf_filter_pck_merge_properties(ctx->src_pck, pck);

	gf_filter_pck_set_cts(pck, ctx->cts);
	gf_filter_pck_set_sap(pck, ctx->state.frame_state.key_frame ? GF_FILTER_SAP_1 : 0);

	memcpy(output, ctx->state.frame_obus, pck_size);

	if (ctx->deps) {
		u8 flags = 0;
		//dependsOn
		flags = ( ctx->state.frame_state.key_frame) ? 2 : 1;
		flags <<= 2;
		//dependedOn
	 	flags |= ctx->state.frame_state.refresh_frame_flags ? 1 : 2;
		flags <<= 2;
		//hasRedundant
	 	//flags |= ctx->has_redundant ? 1 : 2;
	 	gf_filter_pck_set_dependency_flags(pck, flags);
	}

	gf_filter_pck_send(pck);

	av1dmx_update_cts(ctx);
	gf_av1_reset_state(&ctx->state, GF_FALSE);

	return GF_OK;

}