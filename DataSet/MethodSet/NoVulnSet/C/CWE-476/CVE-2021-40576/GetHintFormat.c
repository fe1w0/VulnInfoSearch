u32 GetHintFormat(GF_TrackBox *trak)
{
	GF_HintMediaHeaderBox *hmhd = (GF_HintMediaHeaderBox *)trak->Media->information->InfoHeader;
	if (!hmhd || (hmhd->type != GF_ISOM_BOX_TYPE_HMHD))
		return 0;
		
	if (!hmhd || !hmhd->subType) {
		GF_Box *a = (GF_Box *)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, 0);
		if (!hmhd) return a ? a->type : 0;
		if (a) hmhd->subType = a->type;
		return hmhd->subType;
	}
	return hmhd->subType;
}