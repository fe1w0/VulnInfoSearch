void AV1_RewriteESDescriptorEx(GF_MPEGVisualSampleEntryBox *av1, GF_MediaBox *mdia)
{
	GF_BitRateBox *btrt = gf_isom_sample_entry_get_bitrate((GF_SampleEntryBox *)av1, GF_FALSE);

	if (av1->emul_esd) gf_odf_desc_del((GF_Descriptor *)av1->emul_esd);
	av1->emul_esd = gf_odf_desc_esd_new(2);
	av1->emul_esd->decoderConfig->streamType = GF_STREAM_VISUAL;
	av1->emul_esd->decoderConfig->objectTypeIndication = GF_CODECID_AV1;

	if (btrt) {
		av1->emul_esd->decoderConfig->bufferSizeDB = btrt->bufferSizeDB;
		av1->emul_esd->decoderConfig->avgBitrate = btrt->avgBitrate;
		av1->emul_esd->decoderConfig->maxBitrate = btrt->maxBitrate;
	}
	if (av1->av1_config) {
		GF_AV1Config *av1_cfg = AV1_DuplicateConfig(av1->av1_config->config);
		if (av1_cfg) {
			gf_odf_av1_cfg_write(av1_cfg, &av1->emul_esd->decoderConfig->decoderSpecificInfo->data, &av1->emul_esd->decoderConfig->decoderSpecificInfo->dataLength);
			gf_odf_av1_cfg_del(av1_cfg);
		}
	}
}