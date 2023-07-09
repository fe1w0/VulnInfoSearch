GF_Err latm_dmx_process(GF_Filter *filter)
{
	GF_LATMDmxCtx *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck, *dst_pck;
	u32 pos;
	u8 *data, *output;
	u32 pck_size, prev_pck_size;
	u64 cts = GF_FILTER_NO_TS;

	if (ctx->in_error)
		return ctx->in_error;

	//always reparse duration
	if (!ctx->duration.num)
		latm_dmx_check_dur(filter, ctx);

	if (ctx->opid && !ctx->is_playing)
		return GF_OK;

	pck = gf_filter_pid_get_packet(ctx->ipid);
	if (!pck) {
		if (gf_filter_pid_is_eos(ctx->ipid)) {
			if (!ctx->latm_buffer_size) {
				if (ctx->opid)
					gf_filter_pid_set_eos(ctx->opid);
				if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
				ctx->src_pck = NULL;
				return GF_EOS;
			}
		} else {
			return GF_OK;
		}
	}

	data = (char *) gf_filter_pck_get_data(pck, &pck_size);

	//input pid sets some timescale - we flushed pending data , update cts
	if (ctx->timescale && pck) {
		cts = gf_filter_pck_get_cts(pck);
	}

	prev_pck_size = ctx->latm_buffer_size;

	if (pck && !ctx->resume_from) {
		if (ctx->latm_buffer_size + pck_size > ctx->latm_buffer_alloc) {
			ctx->latm_buffer_alloc = ctx->latm_buffer_size + pck_size;
			ctx->latm_buffer = gf_realloc(ctx->latm_buffer, ctx->latm_buffer_alloc);
		}
		memcpy(ctx->latm_buffer + ctx->latm_buffer_size, data, pck_size);
		ctx->latm_buffer_size += pck_size;
	}

	if (!ctx->bs) ctx->bs = gf_bs_new(ctx->latm_buffer, ctx->latm_buffer_size, GF_BITSTREAM_READ);
	else gf_bs_reassign_buffer(ctx->bs, ctx->latm_buffer, ctx->latm_buffer_size);

	if (ctx->resume_from) {
		gf_bs_seek(ctx->bs, ctx->resume_from-1);
		ctx->resume_from = 0;
	}

	if (cts == GF_FILTER_NO_TS)
		prev_pck_size = 0;


	while (1) {
		pos = (u32) gf_bs_get_position(ctx->bs);
		u8 latm_buffer[4096];
		u32 latm_frame_size = 4096;
		if (!latm_dmx_sync_frame_bs(ctx->bs,&ctx->acfg, &latm_frame_size, latm_buffer, NULL)) break;

		if (ctx->in_seek) {
			u64 nb_samples_at_seek = (u64) (ctx->start_range * GF_M4ASampleRates[ctx->sr_idx]);
			if (ctx->cts + ctx->dts_inc >= nb_samples_at_seek) {
				//u32 samples_to_discard = (ctx->cts + ctx->dts_inc) - nb_samples_at_seek;
				ctx->in_seek = GF_FALSE;
			}
		}

		latm_dmx_check_pid(filter, ctx);

		if (!ctx->is_playing) {
			ctx->resume_from = pos+1;
			return GF_OK;
		}

		if (!ctx->in_seek) {
			GF_FilterSAPType sap = GF_FILTER_SAP_1;

			dst_pck = gf_filter_pck_new_alloc(ctx->opid, latm_frame_size, &output);
			if (ctx->src_pck) gf_filter_pck_merge_properties(ctx->src_pck, dst_pck);

			memcpy(output, latm_buffer, latm_frame_size);

			gf_filter_pck_set_cts(dst_pck, ctx->cts);
			gf_filter_pck_set_duration(dst_pck, ctx->dts_inc);
			gf_filter_pck_set_framing(dst_pck, GF_TRUE, GF_TRUE);

			/*xHE-AAC, check RAP*/
			if (ctx->acfg.base_object_type==GF_CODECID_USAC) {
				if (latm_frame_size && (output[0] & 0x80) && !ctx->prev_sap) {
					sap = GF_FILTER_SAP_1;
					ctx->prev_sap = GF_TRUE;
				} else {
					sap = GF_FILTER_SAP_NONE;
					ctx->prev_sap = GF_FALSE;
				}
			}
			gf_filter_pck_set_sap(dst_pck, sap);

			gf_filter_pck_send(dst_pck);
		}
		latm_dmx_update_cts(ctx);

		if (prev_pck_size) {
			pos = (u32) gf_bs_get_position(ctx->bs);
			if (prev_pck_size<=pos) {
				prev_pck_size=0;
				if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
				ctx->src_pck = pck;
				if (pck)
					gf_filter_pck_ref_props(&ctx->src_pck);
			}
		}
	}

	if (pck) {
		pos = (u32) gf_bs_get_position(ctx->bs);
		assert(ctx->latm_buffer_size >= pos);
		memmove(ctx->latm_buffer, ctx->latm_buffer+pos, ctx->latm_buffer_size - pos);
		ctx->latm_buffer_size -= pos;
		gf_filter_pid_drop_packet(ctx->ipid);
		assert(!ctx->resume_from);
	} else {
		ctx->latm_buffer_size = 0;
		return latm_dmx_process(filter);
	}
	return GF_OK;
}