GF_Err MergeTrack(GF_TrackBox *trak, GF_TrackFragmentBox *traf, GF_MovieFragmentBox *moof_box, u64 moof_offset, s32 compressed_diff, u64 *cumulated_offset, Bool is_first_merge)
{
	u32 i, j, chunk_size, track_num;
	u64 base_offset, data_offset, traf_duration;
	u32 def_duration, DescIndex, def_size, def_flags;
	u32 duration, size, flags, prev_trun_data_offset, sample_index;
	u8 pad, sync;
	u16 degr;
	Bool first_samp_in_traf=GF_TRUE;
	Bool store_traf_map=GF_FALSE;
	u8 *moof_template=NULL;
	u32 moof_template_size=0;
	Bool is_seg_start = GF_FALSE;
	u64 seg_start=0, sidx_start=0, sidx_end=0, frag_start=0, last_dts=0;
	GF_TrackFragmentRunBox *trun;
	GF_TrunEntry *ent;
#ifdef GF_ENABLE_CTRN
	GF_TrackFragmentBox *traf_ref = NULL;
#endif

	GF_Err stbl_AppendTime(GF_SampleTableBox *stbl, u32 duration, u32 nb_pack);
	GF_Err stbl_AppendSize(GF_SampleTableBox *stbl, u32 size, u32 nb_pack);
	GF_Err stbl_AppendChunk(GF_SampleTableBox *stbl, u64 offset);
	GF_Err stbl_AppendSampleToChunk(GF_SampleTableBox *stbl, u32 DescIndex, u32 samplesInChunk);
	GF_Err stbl_AppendCTSOffset(GF_SampleTableBox *stbl, s32 CTSOffset);
	GF_Err stbl_AppendRAP(GF_SampleTableBox *stbl, u8 isRap);
	GF_Err stbl_AppendPadding(GF_SampleTableBox *stbl, u8 padding);
	GF_Err stbl_AppendDegradation(GF_SampleTableBox *stbl, u16 DegradationPriority);

	if (trak->Header->trackID != traf->tfhd->trackID) return GF_OK;
	if (!trak->Media->information->sampleTable
		|| !trak->Media->information->sampleTable->SampleSize
		|| !trak->Media->information->sampleTable->TimeToSample
		|| !trak->Media->information->sampleTable->SampleToChunk
		|| !trak->Media->information->sampleTable->ChunkOffset
	) {
		return GF_ISOM_INVALID_FILE;
	}

	if (!traf->trex->track)
		traf->trex->track = trak;

	//setup all our defaults
	DescIndex = (traf->tfhd->flags & GF_ISOM_TRAF_SAMPLE_DESC) ? traf->tfhd->sample_desc_index : traf->trex->def_sample_desc_index;
	if (!DescIndex) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] default sample description set to 0, likely broken ! Fixing to 1\n" ));
		DescIndex = 1;
	} else if (DescIndex > gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] default sample description set to %d but only %d sample description(s), likely broken ! Fixing to 1\n", DescIndex, gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes)));
		DescIndex = 1;
	}
#ifdef GF_ENABLE_CTRN
	if (traf->trex->inherit_from_traf_id) {
		u32 traf_count = gf_list_count(moof_box->TrackList);
		for (i=0; i<traf_count; i++) {
			GF_TrackFragmentBox *atraf = gf_list_get(moof_box->TrackList, i);
			if (atraf->tfhd && atraf->tfhd->trackID==traf->trex->inherit_from_traf_id) {
				traf_ref = atraf;
				break;
			}
		}
	}
#endif

	def_duration = (traf->tfhd->flags & GF_ISOM_TRAF_SAMPLE_DUR) ? traf->tfhd->def_sample_duration : traf->trex->def_sample_duration;
	def_size = (traf->tfhd->flags & GF_ISOM_TRAF_SAMPLE_SIZE) ? traf->tfhd->def_sample_size : traf->trex->def_sample_size;
	def_flags = (traf->tfhd->flags & GF_ISOM_TRAF_SAMPLE_FLAGS) ? traf->tfhd->def_sample_flags : traf->trex->def_sample_flags;

	//locate base offset, by default use moof (dash-like)
	base_offset = moof_offset;
	//explicit base offset, use it
	if (traf->tfhd->flags & GF_ISOM_TRAF_BASE_OFFSET)
		base_offset = traf->tfhd->base_data_offset;
	//no moof offset and no explicit offset, the offset is the end of the last written chunk of
	//the previous traf. For the first traf, *cumulated_offset is actually moof offset
	else if (!(traf->tfhd->flags & GF_ISOM_MOOF_BASE_OFFSET))
		base_offset = *cumulated_offset;

	chunk_size = 0;
	prev_trun_data_offset = 0;
	data_offset = 0;
	traf_duration = 0;

	/*in playback mode*/
	if (traf->tfdt && is_first_merge) {
#ifndef GPAC_DISABLE_LOG
		if (trak->moov->mov->NextMoofNumber && trak->present_in_scalable_segment && trak->sample_count_at_seg_start && (trak->dts_at_seg_start != traf->tfdt->baseMediaDecodeTime)) {
			s32 drift = (s32) ((s64) traf->tfdt->baseMediaDecodeTime - (s64)trak->dts_at_seg_start);
			if (drift<0)  {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Warning: TFDT timing "LLD" less than cumulated timing "LLD" - using tfdt\n", traf->tfdt->baseMediaDecodeTime, trak->dts_at_seg_start ));
			} else {
				GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("[iso file] TFDT timing "LLD" higher than cumulated timing "LLD" (last sample got extended in duration)\n", traf->tfdt->baseMediaDecodeTime, trak->dts_at_seg_start ));
			}
		}
#endif
		trak->dts_at_seg_start = traf->tfdt->baseMediaDecodeTime;
	}
	else if (traf->tfxd) {
		trak->dts_at_seg_start = traf->tfxd->absolute_time_in_track_timescale;
	}

	if (traf->tfxd) {
		trak->last_tfxd_value = traf->tfxd->absolute_time_in_track_timescale;
		trak->last_tfxd_value += traf->tfxd->fragment_duration_in_track_timescale;
	}
	if (traf->tfrf) {
		if (trak->tfrf) gf_isom_box_del_parent(&trak->child_boxes, (GF_Box *)trak->tfrf);
		trak->tfrf = traf->tfrf;
		gf_list_del_item(traf->child_boxes, traf->tfrf);
		gf_list_add(trak->child_boxes, trak->tfrf);
	}

	if (trak->moov->mov->signal_frag_bounds) {
		store_traf_map = GF_TRUE;
		if (is_first_merge) {
			GF_MovieFragmentBox *moof_clone = NULL;
			gf_isom_box_freeze_order((GF_Box *)moof_box);
			gf_isom_clone_box((GF_Box *)moof_box, (GF_Box **)&moof_clone);

			if (moof_clone) {
				GF_BitStream *bs;
				for (i=0; i<gf_list_count(moof_clone->TrackList); i++) {
					GF_TrackFragmentBox *traf_clone = gf_list_get(moof_clone->TrackList, i);
					gf_isom_box_array_reset_parent(&traf_clone->child_boxes, traf_clone->TrackRuns);
					gf_isom_box_array_reset_parent(&traf_clone->child_boxes, traf_clone->sampleGroups);
					gf_isom_box_array_reset_parent(&traf_clone->child_boxes, traf_clone->sampleGroupsDescription);
					gf_isom_box_array_reset_parent(&traf_clone->child_boxes, traf_clone->sub_samples);
					gf_isom_box_array_reset_parent(&traf_clone->child_boxes, traf_clone->sai_offsets);
					gf_isom_box_array_reset_parent(&traf_clone->child_boxes, traf_clone->sai_sizes);
					if (traf_clone->sample_encryption) {
						gf_isom_box_del_parent(&traf_clone->child_boxes, (GF_Box *) traf_clone->sample_encryption);
						traf_clone->sample_encryption = NULL;
					}
					if (traf_clone->sdtp) {
						gf_isom_box_del_parent(&traf_clone->child_boxes, (GF_Box *) traf_clone->sdtp);
						traf_clone->sdtp = NULL;
					}
				}
				gf_isom_box_size((GF_Box *)moof_clone);
				bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

				if (trak->moov->mov->seg_styp) {
					gf_isom_box_size(trak->moov->mov->seg_styp);
					gf_isom_box_write(trak->moov->mov->seg_styp, bs);
				}
				if (trak->moov->mov->root_sidx) {
					gf_isom_box_size((GF_Box *)trak->moov->mov->root_sidx);
					gf_isom_box_write((GF_Box *)trak->moov->mov->root_sidx, bs);
				}
				if (trak->moov->mov->seg_ssix) {
					gf_isom_box_size(trak->moov->mov->seg_ssix);
					gf_isom_box_write(trak->moov->mov->seg_ssix, bs);
				}
				gf_isom_box_write((GF_Box *)moof_clone, bs);
				gf_isom_box_del((GF_Box*)moof_clone);

				gf_bs_get_content(bs, &moof_template, &moof_template_size);
				gf_bs_del(bs);
			}
		}
		if (trak->moov->mov->seg_styp) {
			is_seg_start = GF_TRUE;
			seg_start = trak->moov->mov->styp_start_offset;
		}
		if (trak->moov->mov->root_sidx) {
			is_seg_start = GF_TRUE;
			sidx_start = trak->moov->mov->sidx_start_offset;
			sidx_end = trak->moov->mov->sidx_end_offset;
			if (! seg_start || (sidx_start<seg_start))
				seg_start = sidx_start;
		}
		frag_start = trak->moov->mov->current_top_box_start;
	}
	else if (trak->moov->mov->store_traf_map) {
		store_traf_map = GF_TRUE;
	}


	sample_index = 0;
	i=0;
	while ((trun = (GF_TrackFragmentRunBox *)gf_list_enum(traf->TrackRuns, &i))) {
		//merge the run
		for (j=0; j<trun->sample_count; j++) {
			GF_Err e;
			s32 cts_offset=0;
			if (j<trun->nb_samples) {
				ent = &trun->samples[j];
			} else {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Track %d doesn't have enough trun entries (%d) compared to sample count (%d) in run\n", traf->trex->trackID, trun->nb_samples, trun->sample_count ));
				break;
			}
			size = def_size;
			duration = def_duration;
			flags = def_flags;

			//CTS - if flag not set (trun or ctrn) defaults to 0 which is the base value after alloc
			//we just need to overrite its value if inherited
			cts_offset = ent->CTS_Offset;

#ifdef GF_ENABLE_CTRN
			if (trun->use_ctrn) {
				if (!j && (trun->ctrn_flags & GF_ISOM_CTRN_FIRST_SAMPLE) ) {
					if (trun->ctrn_first_dur) duration = ent->Duration;
					if (trun->ctrn_first_size) size = ent->size;
					if (trun->ctrn_first_ctts) flags = ent->flags;
				} else {
					if (trun->ctrn_dur) duration = ent->Duration;
					if (trun->ctrn_size) size = ent->size;
					if (trun->ctrn_sample_flags) flags = ent->flags;
				}
				/*re-override*/
				if (trun->ctrn_flags & 0xF0) {
					GF_TrunEntry *ref_entry;
					if (!traf_ref) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Track %d use traf inheritance to track ID %d but reference traf not found\n", traf->trex->trackID, traf->trex->inherit_from_traf_id ));
						break;
					}
					ref_entry = traf_get_sample_entry(traf_ref, sample_index);
					if (!ref_entry) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Track %d use traf inheritance but sample %d not found in reference traf\n", traf->trex->trackID, sample_index+1 ));
						break;
					}
					if (trun->ctrn_flags & GF_ISOM_CTRN_INHERIT_DUR)
						duration = ref_entry->Duration;
					if (trun->ctrn_flags & GF_ISOM_CTRN_INHERIT_SIZE)
						size = ref_entry->size;
					if (trun->ctrn_flags & GF_ISOM_CTRN_INHERIT_FLAGS)
						flags = ref_entry->flags;
					if (trun->ctrn_flags & GF_ISOM_CTRN_INHERIT_CTSO)
						cts_offset = ref_entry->CTS_Offset;
				}

			} else
#endif
			{
				if (trun->flags & GF_ISOM_TRUN_DURATION) duration = ent->Duration;
				if (trun->flags & GF_ISOM_TRUN_SIZE) size = ent->size;
				if (trun->flags & GF_ISOM_TRUN_FLAGS) {
					flags = ent->flags;
				} else if (!j && (trun->flags & GF_ISOM_TRUN_FIRST_FLAG)) {
					flags = trun->first_sample_flags;
				}
			}
			sample_index++;
			/*store the resolved value in case we have inheritance*/
			ent->size = size;
			ent->Duration = duration;
			ent->flags = flags;
			ent->CTS_Offset = cts_offset;

			last_dts += duration;

			//add size first
			if (!trak->Media->information->sampleTable->SampleSize) {
				trak->Media->information->sampleTable->SampleSize = (GF_SampleSizeBox *) gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_STSZ);
				if (!trak->Media->information->sampleTable->SampleSize)
					return GF_OUT_OF_MEM;
			}
			e = stbl_AppendSize(trak->Media->information->sampleTable, size, ent->nb_pack);
			if (e) return e;

			//then TS
			if (!trak->Media->information->sampleTable->TimeToSample) {
				trak->Media->information->sampleTable->TimeToSample = (GF_TimeToSampleBox *) gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_STTS);
				if (!trak->Media->information->sampleTable->TimeToSample)
					return GF_OUT_OF_MEM;
			}
			e = stbl_AppendTime(trak->Media->information->sampleTable, duration, ent->nb_pack);
			if (e) return e;

			//add chunk on first sample
			if (!j) {
				u64 final_offset;
				data_offset = base_offset;
				//we have an explicit data offset for this trun
				if (trun->flags & GF_ISOM_TRUN_DATA_OFFSET) {
					data_offset += trun->data_offset;
					/*reset chunk size since data is now relative to this trun*/
					chunk_size = 0;
					/*remember this data offset for following trun*/
					prev_trun_data_offset = trun->data_offset;
					/*if mdat is located after the moof, and the moof was compressed, adjust offset
					otherwise the offset does not need adjustment*/
					if (trun->data_offset>=0) {
						data_offset -= compressed_diff;
						prev_trun_data_offset -= compressed_diff;
					}
				}
				//we had an explicit data offset for the previous trun, use it + chunk size
				else if (prev_trun_data_offset) {
					/*data offset is previous chunk size plus previous offset of the trun*/
					data_offset += prev_trun_data_offset + chunk_size;
				}
				//no explicit data offset, continuous data after last data in previous chunk
				else {
					data_offset += chunk_size;
					//data offset of first trun in first traf, adjust if compressed moof
					if ((i==1) && (trun->data_offset>=0)) {
						data_offset -= compressed_diff;
					}
				}

				final_offset = data_offset;
				//adjust offset if moov was also compressed and we are still in the same file
				//so that later call to gf_isom_get_sample properly adjust back the offset
				if (trak->moov->compressed_diff) {
					final_offset += trak->moov->compressed_diff;
				}

				if (!trak->Media->information->sampleTable->ChunkOffset) {
					trak->Media->information->sampleTable->ChunkOffset = gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_STCO);
					if (!trak->Media->information->sampleTable->ChunkOffset)
						return GF_OUT_OF_MEM;
				}
				e = stbl_AppendChunk(trak->Media->information->sampleTable, final_offset);
				if (e) return e;
				//then sampleToChunk
				if (!trak->Media->information->sampleTable->SampleToChunk) {
					trak->Media->information->sampleTable->SampleToChunk = (GF_SampleToChunkBox *) gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_STSC);
					if (!trak->Media->information->sampleTable->SampleToChunk)
						return GF_OUT_OF_MEM;
				}
				e = stbl_AppendSampleToChunk(trak->Media->information->sampleTable,
				                         DescIndex, trun->sample_count);
				if (e) return e;
			}
			chunk_size += size;

			if (store_traf_map && first_samp_in_traf) {
				first_samp_in_traf = GF_FALSE;
				e = stbl_AppendTrafMap(trak->Media->information->sampleTable, is_seg_start, seg_start, frag_start, moof_template, moof_template_size, sidx_start, sidx_end);
				if (e) return e;
				//do not deallocate, the memory is now owned by traf map
				moof_template = NULL;
				moof_template_size = 0;
			}
			if (ent->nb_pack>1) {
				j+= ent->nb_pack-1;
				traf_duration += ent->nb_pack*duration;
				continue;
			}

			traf_duration += duration;

			e = stbl_AppendCTSOffset(trak->Media->information->sampleTable, cts_offset);
			if (e) return e;
			//flags
			sync = GF_ISOM_GET_FRAG_SYNC(flags);
			if (trak->Media->information->sampleTable->no_sync_found && sync) {
				trak->Media->information->sampleTable->no_sync_found = 0;
			}
			e = stbl_AppendRAP(trak->Media->information->sampleTable, sync);
			if (e) return e;
			pad = GF_ISOM_GET_FRAG_PAD(flags);
			if (pad) {
				e = stbl_AppendPadding(trak->Media->information->sampleTable, pad);
				if (e) return e;
			}
			degr = GF_ISOM_GET_FRAG_DEG(flags);
			if (degr) {
				e = stbl_AppendDegradation(trak->Media->information->sampleTable, degr);
				if (e) return e;
			}
			e = stbl_AppendDependencyType(trak->Media->information->sampleTable, GF_ISOM_GET_FRAG_LEAD(flags), GF_ISOM_GET_FRAG_DEPENDS(flags), GF_ISOM_GET_FRAG_DEPENDED(flags), GF_ISOM_GET_FRAG_REDUNDANT(flags));
			if (e) return e;
		}
	}

	if (trak->moov->mov->is_smooth && !traf->tfdt && !traf->tfxd) {
		if (is_first_merge)
			trak->dts_at_seg_start = trak->dts_at_next_seg_start;
		trak->dts_at_next_seg_start += last_dts;
	}
	if (traf_duration && trak->editBox && trak->editBox->editList) {
		for (i=0; i<gf_list_count(trak->editBox->editList->entryList); i++) {
			GF_EdtsEntry *edts_e = gf_list_get(trak->editBox->editList->entryList, i);
			if (edts_e->was_empty_dur) {
				u64 extend_dur = traf_duration;
				extend_dur *= trak->moov->mvhd->timeScale;
				extend_dur /= trak->Media->mediaHeader->timeScale;
				edts_e->segmentDuration += extend_dur;
			}
			else if (!edts_e->segmentDuration) {
				edts_e->was_empty_dur = GF_TRUE;
				if ((s64) traf_duration > edts_e->mediaTime)
					traf_duration -= edts_e->mediaTime;
				else
					traf_duration = 0;

				edts_e->segmentDuration = traf_duration;
				edts_e->segmentDuration *= trak->moov->mvhd->timeScale;
				edts_e->segmentDuration /= trak->Media->mediaHeader->timeScale;
			}

		}
	}

	//in any case, update the cumulated offset
	//this will handle hypothetical files mixing MOOF offset and implicit non-moof offset
	*cumulated_offset = data_offset + chunk_size;

	/*merge sample groups*/
	if (traf->sampleGroups) {
		GF_List *groups;
		GF_List *groupDescs;
		Bool is_identical_sgpd = GF_TRUE;
		u32 *new_idx = NULL, new_idx_count=0;

		if (!trak->Media->information->sampleTable->sampleGroups)
			trak->Media->information->sampleTable->sampleGroups = gf_list_new();

		if (!trak->Media->information->sampleTable->sampleGroupsDescription)
			trak->Media->information->sampleTable->sampleGroupsDescription = gf_list_new();

		groupDescs = trak->Media->information->sampleTable->sampleGroupsDescription;
		for (i=0; i<gf_list_count(traf->sampleGroupsDescription); i++) {
			GF_SampleGroupDescriptionBox *new_sgdesc = NULL;
			GF_SampleGroupDescriptionBox *sgdesc = gf_list_get(traf->sampleGroupsDescription, i);
			for (j=0; j<gf_list_count(groupDescs); j++) {
				new_sgdesc = gf_list_get(groupDescs, j);
				if (new_sgdesc->grouping_type==sgdesc->grouping_type) break;
				new_sgdesc = NULL;
			}
			/*new description, move it to our sample table*/
			if (!new_sgdesc) {
				gf_list_add(groupDescs, sgdesc);
				gf_list_add(trak->Media->information->sampleTable->child_boxes, sgdesc);
				gf_list_rem(traf->sampleGroupsDescription, i);
				gf_list_del_item(traf->child_boxes, sgdesc);
				i--;
			}
			/*merge descriptions*/
			else {
				u32 count;

				is_identical_sgpd = gf_isom_is_identical_sgpd(new_sgdesc, sgdesc, 0);
				if (is_identical_sgpd)
					continue;

				new_idx_count = gf_list_count(sgdesc->group_descriptions);
				new_idx = (u32 *)gf_malloc(new_idx_count * sizeof(u32));
				if (!new_idx) return GF_OUT_OF_MEM;

				count = 0;
				while (gf_list_count(sgdesc->group_descriptions)) {
					void *sgpd_entry = gf_list_get(sgdesc->group_descriptions, 0);
					Bool new_entry = GF_TRUE;

					for (j = 0; j < gf_list_count(new_sgdesc->group_descriptions); j++) {
						void *ptr = gf_list_get(new_sgdesc->group_descriptions, j);
						if (gf_isom_is_identical_sgpd(sgpd_entry, ptr, new_sgdesc->grouping_type)) {
							new_idx[count] = j + 1;
							count ++;
							new_entry = GF_FALSE;
							gf_free(sgpd_entry);
							break;
						}
					}

					if (new_entry) {
						gf_list_add(new_sgdesc->group_descriptions, sgpd_entry);
						new_idx[count] = gf_list_count(new_sgdesc->group_descriptions);
						count ++;
					}

					gf_list_rem(sgdesc->group_descriptions, 0);
				}
			}
		}

		groups = trak->Media->information->sampleTable->sampleGroups;
		for (i=0; i<gf_list_count(traf->sampleGroups); i++) {
			GF_SampleGroupBox *stbl_group = NULL;
			GF_SampleGroupBox *frag_group = gf_list_get(traf->sampleGroups, i);


			for (j=0; j<gf_list_count(groups); j++) {
				stbl_group = gf_list_get(groups, j);
				if ((frag_group->grouping_type==stbl_group->grouping_type) && (frag_group->grouping_type_parameter==stbl_group->grouping_type_parameter))
					break;
				stbl_group = NULL;
			}
			if (!stbl_group) {
				stbl_group = (GF_SampleGroupBox *) gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_SBGP);
				if (!stbl_group) return GF_OUT_OF_MEM;
				stbl_group->grouping_type = frag_group->grouping_type;
				stbl_group->grouping_type_parameter = frag_group->grouping_type_parameter;
				stbl_group->version = frag_group->version;
				gf_list_add(groups, stbl_group);
			}

			if (is_identical_sgpd) {
				//adjust sgpd index: in traf index start at 0x1001
				for (j = 0; j < frag_group->entry_count; j++)
					frag_group->sample_entries[j].group_description_index &= 0x0FFFF;
				if (frag_group->entry_count && stbl_group->entry_count &&
				        (frag_group->sample_entries[0].group_description_index==stbl_group->sample_entries[stbl_group->entry_count-1].group_description_index)
				   ) {
					stbl_group->sample_entries[stbl_group->entry_count - 1].sample_count += frag_group->sample_entries[0].sample_count;
					if (frag_group->entry_count>1) {
						stbl_group->sample_entries = gf_realloc(stbl_group->sample_entries, sizeof(GF_SampleGroupEntry) * (stbl_group->entry_count + frag_group->entry_count - 1));
						memcpy(&stbl_group->sample_entries[stbl_group->entry_count], &frag_group->sample_entries[1], sizeof(GF_SampleGroupEntry) * (frag_group->entry_count - 1));
						stbl_group->entry_count += frag_group->entry_count - 1;
					}
				} else {
					stbl_group->sample_entries = gf_realloc(stbl_group->sample_entries, sizeof(GF_SampleGroupEntry) * (stbl_group->entry_count + frag_group->entry_count));
					memcpy(&stbl_group->sample_entries[stbl_group->entry_count], &frag_group->sample_entries[0], sizeof(GF_SampleGroupEntry) * frag_group->entry_count);
					stbl_group->entry_count += frag_group->entry_count;
				}
			} else {
				stbl_group->sample_entries = gf_realloc(stbl_group->sample_entries, sizeof(GF_SampleGroupEntry) * (stbl_group->entry_count + frag_group->entry_count));
				//adjust sgpd index
				for (j = 0; j < frag_group->entry_count; j++) {
					u32 sgidx = frag_group->sample_entries[j].group_description_index;
					if (sgidx > 0x10000) {
						sgidx -= 0x10001;
						if (sgidx>=new_idx_count) {
							GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[isobmf] corrupted sample group index in fragment %d but only %d group descriptions in fragment\n", sgidx, new_idx_count));
						} else {
							frag_group->sample_entries[j].group_description_index = new_idx[sgidx];
						}
					}
				}
				memcpy(&stbl_group->sample_entries[stbl_group->entry_count], &frag_group->sample_entries[0], sizeof(GF_SampleGroupEntry) * frag_group->entry_count);
				stbl_group->entry_count += frag_group->entry_count;
			}
		}

		if (new_idx) gf_free(new_idx);
	}

	/*content is encrypted*/
	track_num = gf_isom_get_tracknum_from_id(trak->moov, trak->Header->trackID);
	if (gf_isom_is_cenc_media(trak->moov->mov, track_num, DescIndex)
		|| traf->sample_encryption) {
		/*Merge sample auxiliary encryption information*/
		GF_SampleEncryptionBox *senc = NULL;
		u32 scheme_type;
		gf_isom_get_cenc_info(trak->moov->mov, track_num, DescIndex, NULL, &scheme_type, NULL);

		if (traf->sample_encryption) {
			for (i = 0; i < gf_list_count(trak->Media->information->sampleTable->child_boxes); i++) {
				GF_Box *a = (GF_Box *)gf_list_get(trak->Media->information->sampleTable->child_boxes, i);
				if (a->type != traf->sample_encryption->type) continue;

				if ((a->type ==GF_ISOM_BOX_TYPE_UUID) && (((GF_UUIDBox *)a)->internal_4cc == GF_ISOM_BOX_UUID_PSEC)) {
					senc = (GF_SampleEncryptionBox *)a;
					break;
				}
				else if (a->type ==GF_ISOM_BOX_TYPE_SENC) {
					senc = (GF_SampleEncryptionBox *)a;
					break;
				}
			}
			if (!senc && trak->sample_encryption)
				senc = trak->sample_encryption;

			if (!senc) {
				if (traf->sample_encryption->piff_type==1) {
					senc = (GF_SampleEncryptionBox *)gf_isom_create_piff_psec_box(1, 0x2, 0, 0, NULL);
				} else {
					senc = gf_isom_create_samp_enc_box(1, 0x2);
				}

				if (!trak->Media->information->sampleTable->child_boxes) trak->Media->information->sampleTable->child_boxes = gf_list_new();

				trak->sample_encryption = senc;
				if (!trak->child_boxes) trak->child_boxes = gf_list_new();
				gf_list_add(trak->child_boxes, senc);
			}
		}

		/*get sample auxiliary information by saiz/saio rather than by parsing senc box*/
		if (gf_isom_cenc_has_saiz_saio_traf(traf, scheme_type)) {
			u32 nb_saio;
			u32 aux_info_type;
			u64 offset;
			GF_Err e;
			Bool is_encrypted;
			GF_SampleAuxiliaryInfoOffsetBox *saio = NULL;
			GF_SampleAuxiliaryInfoSizeBox *saiz = NULL;

			offset = nb_saio = 0;

			for (i = 0; i < gf_list_count(traf->sai_offsets); i++) {
				saio = (GF_SampleAuxiliaryInfoOffsetBox *)gf_list_get(traf->sai_offsets, i);
				aux_info_type = saio->aux_info_type;
				if (!aux_info_type) aux_info_type = scheme_type;

				/*if we have only 1 sai_offsets, assume that its type is cenc*/
				if ((aux_info_type == GF_ISOM_CENC_SCHEME) || (aux_info_type == GF_ISOM_CBC_SCHEME) ||
					(aux_info_type == GF_ISOM_CENS_SCHEME) || (aux_info_type == GF_ISOM_CBCS_SCHEME) ||
					(gf_list_count(traf->sai_offsets) == 1)) {
					offset = saio->offsets[0] + moof_offset;
					nb_saio = saio->entry_count;
					break;
				}
			}
			for (i = 0; i < gf_list_count(traf->sai_sizes); i++) {
				saiz = (GF_SampleAuxiliaryInfoSizeBox *)gf_list_get(traf->sai_sizes, i);
				aux_info_type = saiz->aux_info_type;
				if (!aux_info_type) aux_info_type = scheme_type;
				/*if we have only 1 sai_sizes, assume that its type is cenc*/
				if ((aux_info_type == GF_ISOM_CENC_SCHEME) || (aux_info_type == GF_ISOM_CBC_SCHEME) ||
					(aux_info_type == GF_ISOM_CENS_SCHEME) || (aux_info_type == GF_ISOM_CBCS_SCHEME) ||
					(gf_list_count(traf->sai_sizes) == 1)) {
					break;
				}
			}
			if (saiz && saio && senc) {
				for (i = 0; i < saiz->sample_count; i++) {
					GF_CENCSampleAuxInfo *sai;
					const u8 *key_info=NULL;
					u32 key_info_size;
					u64 cur_position;
					if (nb_saio != 1)
						offset = saio->offsets[i] + moof_offset;
					size = saiz->default_sample_info_size ? saiz->default_sample_info_size : saiz->sample_info_size[i];

					cur_position = gf_bs_get_position(trak->moov->mov->movieFileMap->bs);
					gf_bs_seek(trak->moov->mov->movieFileMap->bs, offset);

					GF_SAFEALLOC(sai, GF_CENCSampleAuxInfo);
					if (!sai) return GF_OUT_OF_MEM;

					e = gf_isom_get_sample_cenc_info_internal(trak, traf, senc, i+1, &is_encrypted, NULL, NULL, &key_info, &key_info_size);
					if (e) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[isobmf] could not get cenc info for sample %d: %s\n", i+1, gf_error_to_string(e) ));
						return e;
					}

					if (is_encrypted) {
						sai->cenc_data_size = size;
						sai->cenc_data = gf_malloc(sizeof(u8)*size);
						if (!sai->cenc_data) return GF_OUT_OF_MEM;
						gf_bs_read_data(trak->moov->mov->movieFileMap->bs, sai->cenc_data, sai->cenc_data_size);
					} else {
						sai->isNotProtected=1;
					}

					if (key_info) {
						//not multikey
						if (!key_info[0]) {
							//size greater than IV
							if (size > key_info[3])
								senc->flags = 0x00000002;
						}
						//multikey, always use subsamples
						else {
							senc->flags = 0x00000002;
						}
					}


					gf_bs_seek(trak->moov->mov->movieFileMap->bs, cur_position);

					gf_list_add(senc->samp_aux_info, sai);

					e = gf_isom_cenc_merge_saiz_saio(senc, trak->Media->information->sampleTable, offset, size);
					if (e) return e;
					if (nb_saio == 1)
						offset += size;
				}
			}
		} else if (traf->sample_encryption) {
			senc_Parse(trak->moov->mov->movieFileMap->bs, trak, traf, traf->sample_encryption);
			trak->sample_encryption->AlgorithmID = traf->sample_encryption->AlgorithmID;
			if (!trak->sample_encryption->IV_size)
				trak->sample_encryption->IV_size = traf->sample_encryption->IV_size;
			if (!trak->sample_encryption->samp_aux_info) trak->sample_encryption->samp_aux_info = gf_list_new();
			gf_list_transfer(trak->sample_encryption->samp_aux_info, traf->sample_encryption->samp_aux_info);
			if (traf->sample_encryption->flags & 0x00000002)
				trak->sample_encryption->flags |= 0x00000002;
		}
	}
	return GF_OK;
}