GF_Err Media_GetESD(GF_MediaBox *mdia, u32 sampleDescIndex, GF_ESD **out_esd, Bool true_desc_only)
{
	u32 type;
	GF_ESD *esd;
	GF_MPEGSampleEntryBox *entry = NULL;
	GF_ESDBox *ESDa;
	GF_ProtectionSchemeInfoBox *sinf;
	GF_SampleDescriptionBox *stsd = mdia->information->sampleTable->SampleDescription;

	*out_esd = NULL;
	if (!stsd || !stsd->child_boxes || !sampleDescIndex || (sampleDescIndex > gf_list_count(stsd->child_boxes)) )
		return GF_BAD_PARAM;

	esd = NULL;
	entry = (GF_MPEGSampleEntryBox*)gf_list_get(stsd->child_boxes, sampleDescIndex - 1);
	if (! entry) return GF_ISOM_INVALID_MEDIA;

	*out_esd = NULL;
	ESDa = NULL;
	type = entry->type;
	switch (type) {
	case GF_ISOM_BOX_TYPE_ENCV:
	case GF_ISOM_BOX_TYPE_ENCA:
	case GF_ISOM_BOX_TYPE_ENCS:
	case GF_ISOM_BOX_TYPE_ENCF:
	case GF_ISOM_BOX_TYPE_ENCM:
	case GF_ISOM_BOX_TYPE_ENCT:
		sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_SINF);
		if (sinf && sinf->original_format) {
			type = sinf->original_format->data_format;
		}
		break;
	case GF_ISOM_BOX_TYPE_RESV:
		sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_RINF);
		if (sinf && sinf->original_format) {
			type = sinf->original_format->data_format;
		}
		break;
	}

	switch (type) {
	case GF_ISOM_BOX_TYPE_MP4V:
		ESDa = ((GF_MPEGVisualSampleEntryBox*)entry)->esd;
		if (ESDa) esd = (GF_ESD *) ESDa->desc;
		/*avc1 encrypted*/
		else esd = ((GF_MPEGVisualSampleEntryBox*) entry)->emul_esd;
		break;
	case GF_ISOM_BOX_TYPE_AVC1:
	case GF_ISOM_BOX_TYPE_AVC2:
	case GF_ISOM_BOX_TYPE_AVC3:
	case GF_ISOM_BOX_TYPE_AVC4:
	case GF_ISOM_BOX_TYPE_HVC1:
	case GF_ISOM_BOX_TYPE_HEV1:
	case GF_ISOM_BOX_TYPE_HVC2:
	case GF_ISOM_BOX_TYPE_HEV2:
	case GF_ISOM_BOX_TYPE_HVT1:
	case GF_ISOM_BOX_TYPE_264B:
	case GF_ISOM_BOX_TYPE_265B:
	case GF_ISOM_BOX_TYPE_DVHE:
	case GF_ISOM_BOX_TYPE_VVC1:
	case GF_ISOM_BOX_TYPE_VVI1:
		esd = ((GF_MPEGVisualSampleEntryBox*) entry)->emul_esd;
		break;
	case GF_ISOM_BOX_TYPE_SVC1:
	case GF_ISOM_BOX_TYPE_MVC1:
		if ((mdia->mediaTrack->extractor_mode & 0x0000FFFF) != GF_ISOM_NALU_EXTRACT_INSPECT)
			AVC_RewriteESDescriptorEx((GF_MPEGVisualSampleEntryBox*) entry, mdia);
		else
			AVC_RewriteESDescriptorEx((GF_MPEGVisualSampleEntryBox*) entry, NULL);
		esd = ((GF_MPEGVisualSampleEntryBox*) entry)->emul_esd;
		break;
	case GF_ISOM_BOX_TYPE_LHE1:
	case GF_ISOM_BOX_TYPE_LHV1:
		if ((mdia->mediaTrack->extractor_mode & 0x0000FFFF) != GF_ISOM_NALU_EXTRACT_INSPECT)
			HEVC_RewriteESDescriptorEx((GF_MPEGVisualSampleEntryBox*) entry, mdia);
		else
			HEVC_RewriteESDescriptorEx((GF_MPEGVisualSampleEntryBox*) entry, NULL);
		esd = ((GF_MPEGVisualSampleEntryBox*) entry)->emul_esd;
		break;
	case GF_ISOM_BOX_TYPE_AV01:
		AV1_RewriteESDescriptorEx((GF_MPEGVisualSampleEntryBox*)entry, mdia);
		esd = ((GF_MPEGVisualSampleEntryBox*)entry)->emul_esd;
		break;
	case GF_ISOM_BOX_TYPE_VP08:
	case GF_ISOM_BOX_TYPE_VP09:
		VP9_RewriteESDescriptorEx((GF_MPEGVisualSampleEntryBox*)entry, mdia);
		esd = ((GF_MPEGVisualSampleEntryBox*)entry)->emul_esd;
		break;
	case GF_ISOM_BOX_TYPE_MP4A:
        {
            GF_MPEGAudioSampleEntryBox *ase = (GF_MPEGAudioSampleEntryBox*)entry;
            ESDa = ase->esd;
            if (ESDa) {
				esd = (GF_ESD *) ESDa->desc;
            } else if (!true_desc_only) {
				Bool make_mp4a = GF_FALSE;
				sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_SINF);

				if (sinf && sinf->original_format) {
					if (sinf->original_format->data_format==GF_ISOM_BOX_TYPE_MP4A) {
						make_mp4a = GF_TRUE;
					}
				} else {
					// Assuming that if no ESD is provided the stream is Basic MPEG-4 AAC LC
					make_mp4a = GF_TRUE;
				}
				if (make_mp4a) {
					GF_M4ADecSpecInfo aacinfo;
					memset(&aacinfo, 0, sizeof(GF_M4ADecSpecInfo));
					aacinfo.nb_chan = ase->channel_count;
					aacinfo.base_object_type = GF_M4A_AAC_LC;
					aacinfo.base_sr = ase->samplerate_hi;
					*out_esd = gf_odf_desc_esd_new(0);
					(*out_esd)->decoderConfig->streamType = GF_STREAM_AUDIO;
					(*out_esd)->decoderConfig->objectTypeIndication = GF_CODECID_AAC_MPEG4;
					gf_m4a_write_config(&aacinfo, &(*out_esd)->decoderConfig->decoderSpecificInfo->data, &(*out_esd)->decoderConfig->decoderSpecificInfo->dataLength);
				}
            }
        }
		break;
	case GF_ISOM_BOX_TYPE_MP4S:
		if (entry->internal_type==GF_ISOM_SAMPLE_ENTRY_MP4S) {
			ESDa = entry->esd;
			if (ESDa) esd = (GF_ESD *) ESDa->desc;
		}
		break;
#ifndef GPAC_DISABLE_TTXT
	case GF_ISOM_BOX_TYPE_TX3G:
	case GF_ISOM_BOX_TYPE_TEXT:
		if (!true_desc_only && mdia->mediaTrack->moov->mov->convert_streaming_text) {
			GF_Err e = gf_isom_get_ttxt_esd(mdia, out_esd);
			if (e) return e;
			break;
		}
		else
			return GF_ISOM_INVALID_MEDIA;
#endif
#ifndef GPAC_DISABLE_VTT
	case GF_ISOM_BOX_TYPE_WVTT:
	{
		GF_WebVTTSampleEntryBox*vtte = (GF_WebVTTSampleEntryBox*)entry;
		esd =  gf_odf_desc_esd_new(2);
		*out_esd = esd;
		esd->decoderConfig->streamType = GF_STREAM_TEXT;
		esd->decoderConfig->objectTypeIndication = GF_CODECID_WEBVTT;
		if (vtte->config) {
			esd->decoderConfig->decoderSpecificInfo->dataLength = (u32) strlen(vtte->config->string);
			esd->decoderConfig->decoderSpecificInfo->data = gf_malloc(sizeof(char)*esd->decoderConfig->decoderSpecificInfo->dataLength);
			memcpy(esd->decoderConfig->decoderSpecificInfo->data, vtte->config->string, esd->decoderConfig->decoderSpecificInfo->dataLength);
		}
	}
		break;
	case GF_ISOM_BOX_TYPE_STPP:
	case GF_ISOM_BOX_TYPE_SBTT:
	case GF_ISOM_BOX_TYPE_STXT:
		break;
#endif

	case GF_ISOM_SUBTYPE_3GP_AMR:
	case GF_ISOM_SUBTYPE_3GP_AMR_WB:
	case GF_ISOM_SUBTYPE_3GP_EVRC:
	case GF_ISOM_SUBTYPE_3GP_QCELP:
	case GF_ISOM_SUBTYPE_3GP_SMV:
		if (!true_desc_only) {
			GF_Err e = gf_isom_get_3gpp_audio_esd(mdia->information->sampleTable, type, (GF_GenericAudioSampleEntryBox*)entry, out_esd);
			if (e) return e;
			break;
		} else return GF_ISOM_INVALID_MEDIA;

	case GF_ISOM_SUBTYPE_OPUS: {
		GF_OpusSpecificBox *e = ((GF_MPEGAudioSampleEntryBox*)entry)->cfg_opus;
		GF_BitStream *bs_out;
		if (!e) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("ESD not found for Opus\n)"));
			break;
		}

		*out_esd = gf_odf_desc_esd_new(2);
		(*out_esd)->decoderConfig->streamType = GF_STREAM_AUDIO;
		(*out_esd)->decoderConfig->objectTypeIndication = GF_CODECID_OPUS;

		//serialize box with header - compatibility with ffmpeg
		bs_out = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
		gf_isom_box_size((GF_Box *) e);
		gf_isom_box_write((GF_Box *) e, bs_out);
		gf_bs_get_content(bs_out, & (*out_esd)->decoderConfig->decoderSpecificInfo->data, & (*out_esd)->decoderConfig->decoderSpecificInfo->dataLength);
		gf_bs_del(bs_out);
		break;
	}
	case GF_ISOM_SUBTYPE_3GP_H263:
		if (true_desc_only) {
			return GF_ISOM_INVALID_MEDIA;
		} else {
			esd =  gf_odf_desc_esd_new(2);
			*out_esd = esd;
			esd->decoderConfig->streamType = GF_STREAM_VISUAL;
			esd->decoderConfig->objectTypeIndication = GF_CODECID_H263;
			break;
		}

	case GF_ISOM_SUBTYPE_MP3:
		if (true_desc_only) {
			return GF_ISOM_INVALID_MEDIA;
		} else {
			esd =  gf_odf_desc_esd_new(2);
			*out_esd = esd;
			esd->decoderConfig->streamType = GF_STREAM_AUDIO;
			esd->decoderConfig->objectTypeIndication = GF_CODECID_MPEG_AUDIO;
			break;
		}

	case GF_ISOM_SUBTYPE_LSR1:
		if (true_desc_only) {
			return GF_ISOM_INVALID_MEDIA;
		} else {
			GF_LASeRSampleEntryBox*ptr = (GF_LASeRSampleEntryBox*)entry;
			esd =  gf_odf_desc_esd_new(2);
			*out_esd = esd;
			esd->decoderConfig->streamType = GF_STREAM_SCENE;
			esd->decoderConfig->objectTypeIndication = GF_CODECID_LASER;
			esd->decoderConfig->decoderSpecificInfo->dataLength = ptr->lsr_config->hdr_size;
			esd->decoderConfig->decoderSpecificInfo->data = gf_malloc(sizeof(char)*ptr->lsr_config->hdr_size);
			if (!esd->decoderConfig->decoderSpecificInfo->data) return GF_OUT_OF_MEM;
			memcpy(esd->decoderConfig->decoderSpecificInfo->data, ptr->lsr_config->hdr, sizeof(char)*ptr->lsr_config->hdr_size);
			break;
		}
	case GF_ISOM_SUBTYPE_MH3D_MHA1:
	case GF_ISOM_SUBTYPE_MH3D_MHA2:
	case GF_ISOM_SUBTYPE_MH3D_MHM1:
	case GF_ISOM_SUBTYPE_MH3D_MHM2:
		if (true_desc_only) {
			return GF_ISOM_INVALID_MEDIA;
		} else {
			GF_MPEGAudioSampleEntryBox*ptr = (GF_MPEGAudioSampleEntryBox*)entry;
			esd = gf_odf_desc_esd_new(2);
			*out_esd = esd;
			esd->decoderConfig->streamType = GF_STREAM_AUDIO;
			if ((type==GF_ISOM_SUBTYPE_MH3D_MHA1) || (type==GF_ISOM_SUBTYPE_MH3D_MHA2))
				esd->decoderConfig->objectTypeIndication = GF_CODECID_MPHA;
			else
				esd->decoderConfig->objectTypeIndication = GF_CODECID_MHAS;
			if (ptr->cfg_mha) {
				GF_BitStream *bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

				gf_bs_write_u8(bs, ptr->cfg_mha->configuration_version);
				gf_bs_write_u8(bs, ptr->cfg_mha->mha_pl_indication);
				gf_bs_write_u8(bs, ptr->cfg_mha->reference_channel_layout);
				gf_bs_write_u16(bs, ptr->cfg_mha->mha_config ? ptr->cfg_mha->mha_config_size : 0);
				if (ptr->cfg_mha->mha_config && ptr->cfg_mha->mha_config_size)
					gf_bs_write_data(bs, ptr->cfg_mha->mha_config, ptr->cfg_mha->mha_config_size);

				gf_bs_get_content(bs, &esd->decoderConfig->decoderSpecificInfo->data, &esd->decoderConfig->decoderSpecificInfo->dataLength);
				gf_bs_del(bs);
			}
		}
		break;

	default:
		return GF_ISOM_INVALID_MEDIA;
	}

	if (true_desc_only) {
		if (!esd) return GF_ISOM_INVALID_MEDIA;
		*out_esd = esd;
		return GF_OK;
	} else {
		if (!esd && !*out_esd) return GF_ISOM_INVALID_MEDIA;
		if (*out_esd == NULL) return gf_odf_desc_copy((GF_Descriptor *)esd, (GF_Descriptor **)out_esd);
	}
	return GF_OK;
}