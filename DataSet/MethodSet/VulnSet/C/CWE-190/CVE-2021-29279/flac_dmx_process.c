GF_Err flac_dmx_process(GF_Filter *filter)
{
	GF_FLACDmxCtx *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck, *dst_pck;
	u8 *output;
	u8 *start;
	Bool final_flush=GF_FALSE;
	u32 pck_size, remain, prev_pck_size;
	u64 cts = GF_FILTER_NO_TS;
	FLACHeader hdr;

	//always reparse duration
	if (!ctx->duration.num)
		flac_dmx_check_dur(filter, ctx);

	if (ctx->opid && !ctx->is_playing)
		return GF_OK;

	pck = gf_filter_pid_get_packet(ctx->ipid);
	if (!pck) {
		if (gf_filter_pid_is_eos(ctx->ipid)) {
			if (!ctx->flac_buffer_size) {
				if (ctx->opid)
					gf_filter_pid_set_eos(ctx->opid);
				if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
				ctx->src_pck = NULL;
				return GF_EOS;
			}
			final_flush = GF_TRUE;
		} else {
			return GF_OK;
		}
	}

	prev_pck_size = ctx->flac_buffer_size;
	if (pck && !ctx->resume_from) {
		u8 *data = (u8 *) gf_filter_pck_get_data(pck, &pck_size);

		if (ctx->byte_offset != GF_FILTER_NO_BO) {
			u64 byte_offset = gf_filter_pck_get_byte_offset(pck);
			if (!ctx->flac_buffer_size) {
				ctx->byte_offset = byte_offset;
			} else if (ctx->byte_offset + ctx->flac_buffer_size != byte_offset) {
				ctx->byte_offset = GF_FILTER_NO_BO;
				if ((byte_offset != GF_FILTER_NO_BO) && (byte_offset>ctx->flac_buffer_size) ) {
					ctx->byte_offset = byte_offset - ctx->flac_buffer_size;
				}
			}
		}

		if (ctx->flac_buffer_size + pck_size > ctx->flac_buffer_alloc) {
			ctx->flac_buffer_alloc = ctx->flac_buffer_size + pck_size;
			ctx->flac_buffer = gf_realloc(ctx->flac_buffer, ctx->flac_buffer_alloc);
		}
		memcpy(ctx->flac_buffer + ctx->flac_buffer_size, data, pck_size);
		ctx->flac_buffer_size += pck_size;
	}

	//input pid sets some timescale - we flushed pending data , update cts
	if (ctx->timescale && pck) {
		cts = gf_filter_pck_get_cts(pck);
	}

	if (cts == GF_FILTER_NO_TS) {
		//avoids updating cts
		prev_pck_size = 0;
	}

	remain = ctx->flac_buffer_size;
	start = ctx->flac_buffer;

	if (ctx->resume_from) {
		start += ctx->resume_from - 1;
		remain -= ctx->resume_from - 1;
		ctx->resume_from = 0;
	}

	while (remain>2) {
		u32 next_frame=0, nb_samp;
		u32 cur_size = remain-2;
		u8 *cur_buf = start+2;
		u8 *hdr_start = NULL;

		if (final_flush) {
			next_frame = remain;
		} else {
			while (cur_size) {
				//wait till we have a frame header
				hdr_start = memchr(cur_buf, 0xFF, cur_size);
				if (!hdr_start) break;
				next_frame = (u32) (hdr_start-start);
				if (next_frame == remain)
					break;

				if ((hdr_start[1]&0xFC) == 0xF8) {
					if (flac_parse_header(ctx, hdr_start, (u32) remain - next_frame, &hdr))
						break;
				}
				cur_buf = hdr_start+1;
				cur_size = (u32) (cur_buf - start);
				assert(cur_size<=remain);
				cur_size = remain - cur_size;
				hdr_start = NULL;
			}
			if (!hdr_start) break;
			if (next_frame == remain)
				break;
		}


		if (!ctx->initialized) {
			u32 size = next_frame;
			u32 dsi_end = 0;
			//we have a header
			gf_bs_reassign_buffer(ctx->bs, ctx->flac_buffer, size);
			u32 magic = gf_bs_read_u32(ctx->bs);
			if (magic != GF_4CC('f','L','a','C')) {

			}
			while (gf_bs_available(ctx->bs)) {
				Bool last = gf_bs_read_int(ctx->bs, 1);
				u32 type = gf_bs_read_int(ctx->bs, 7);
				u32 len = gf_bs_read_int(ctx->bs, 24);

				if (type==0) {
					u16 min_block_size = gf_bs_read_u16(ctx->bs);
					u16 max_block_size = gf_bs_read_u16(ctx->bs);
					/*u32 min_frame_size = */gf_bs_read_u24(ctx->bs);
					/*u32 max_frame_size = */gf_bs_read_u24(ctx->bs);
					ctx->sample_rate = gf_bs_read_int(ctx->bs, 20);
					ctx->nb_channels = 1 + gf_bs_read_int(ctx->bs, 3);
					ctx->bits_per_sample = 1 + gf_bs_read_int(ctx->bs, 5);
					if (min_block_size==max_block_size) ctx->block_size = min_block_size;
					else ctx->block_size = 0;

					ctx->duration.num = gf_bs_read_long_int(ctx->bs, 36);
					ctx->duration.den = ctx->sample_rate;
					//ignore the rest
					gf_bs_skip_bytes(ctx->bs, 16);
					dsi_end = (u32) gf_bs_get_position(ctx->bs);

				} else {
					//ignore the rest for now
					//TODO: expose metadata, pictures and co
					gf_bs_skip_bytes(ctx->bs, len);
				}
				if (last) break;
			}
			flac_dmx_check_pid(filter, ctx, ctx->flac_buffer+4, dsi_end-4);
			remain -= size;
			start += size;
			ctx->initialized = GF_TRUE;
			if (!ctx->is_playing) break;
			continue;
		}

		//we have a next frame, check we are synchronize
		if ((start[0] != 0xFF) && ((start[1]&0xFC) != 0xF8)) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[FLACDmx] invalid frame, droping %d bytes and resyncing\n", next_frame));
			start += next_frame;
			remain -= next_frame;
			continue;
		}

		flac_parse_header(ctx,start, next_frame, &hdr);
		if (hdr.sample_rate != ctx->sample_rate) {
			ctx->sample_rate = hdr.sample_rate;
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_SAMPLE_RATE, & PROP_UINT(ctx->sample_rate));
		}

		nb_samp = hdr.block_size;

		if (ctx->in_seek) {
			u64 nb_samples_at_seek = (u64) (ctx->start_range * ctx->sample_rate);
			if (ctx->cts + nb_samp >= nb_samples_at_seek) {
				//u32 samples_to_discard = (ctx->cts + nb_samp ) - nb_samples_at_seek;
				ctx->in_seek = GF_FALSE;
			}
		}

		if (ctx->timescale && !prev_pck_size && (cts != GF_FILTER_NO_TS) ) {
			ctx->cts = cts;
			cts = GF_FILTER_NO_TS;
		}

		if (!ctx->in_seek) {
			dst_pck = gf_filter_pck_new_alloc(ctx->opid, next_frame, &output);
			memcpy(output, start, next_frame);

			gf_filter_pck_set_cts(dst_pck, ctx->cts);
			if (!ctx->timescale || (ctx->timescale==ctx->sample_rate) )
				gf_filter_pck_set_duration(dst_pck, nb_samp);
			else {
				gf_filter_pck_set_duration(dst_pck, (nb_samp * ctx->timescale) / ctx->sample_rate);
			}
			gf_filter_pck_set_sap(dst_pck, GF_FILTER_SAP_1);
			gf_filter_pck_set_framing(dst_pck, GF_TRUE, GF_TRUE);

			if (ctx->byte_offset != GF_FILTER_NO_BO) {
				gf_filter_pck_set_byte_offset(dst_pck, ctx->byte_offset);
			}
			gf_filter_pck_send(dst_pck);
		}
		flac_dmx_update_cts(ctx, nb_samp);

		assert (start[0] == 0xFF);
		assert((start[1]&0xFC) == 0xF8);

		start += next_frame;
		assert(remain >= next_frame);
		remain -= next_frame;

	}

	if (!pck) {
		ctx->flac_buffer_size = 0;
		return flac_dmx_process(filter);
	} else {
		if (remain < ctx->flac_buffer_size) {
			memmove(ctx->flac_buffer, start, remain);
		}
		ctx->flac_buffer_size = remain;
		gf_filter_pid_drop_packet(ctx->ipid);
	}
	return GF_OK;
}