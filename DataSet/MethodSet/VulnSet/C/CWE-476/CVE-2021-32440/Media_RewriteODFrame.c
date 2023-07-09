GF_Err Media_RewriteODFrame(GF_MediaBox *mdia, GF_ISOSample *sample)
{
	GF_Err e;
	GF_ODCodec *ODdecode;
	GF_ODCodec *ODencode;
	GF_ODCom *com;

	//the commands we proceed
	GF_ESDUpdate *esdU, *esdU2;
	GF_ESDRemove *esdR, *esdR2;
	GF_ODUpdate *odU, *odU2;

	//the desc they contain
	GF_ObjectDescriptor *od;
	GF_IsomObjectDescriptor *isom_od;
	GF_ESD *esd;
	GF_ES_ID_Ref *ref;
	GF_Descriptor *desc;
	GF_TrackReferenceTypeBox *mpod;
	u32 i, j, skipped;

	if (!mdia || !sample || !sample->data || !sample->dataLength) return GF_BAD_PARAM;

	mpod = NULL;
	e = Track_FindRef(mdia->mediaTrack, GF_ISOM_BOX_TYPE_MPOD, &mpod);
	if (e) return e;
	//no references, nothing to do...
	if (!mpod || !mpod->trackIDs) return GF_OK;

	ODdecode = gf_odf_codec_new();
	if (!ODdecode) return GF_OUT_OF_MEM;
	ODencode = gf_odf_codec_new();
	if (!ODencode) {
		gf_odf_codec_del(ODdecode);
		return GF_OUT_OF_MEM;
	}
	e = gf_odf_codec_set_au(ODdecode, sample->data, sample->dataLength);
	if (e) goto err_exit;
	e = gf_odf_codec_decode(ODdecode);
	if (e) goto err_exit;

	while (1) {
		com = gf_odf_codec_get_com(ODdecode);
		if (!com) break;

		//we only need to rewrite commands with ESDs inside: ESDUpdate and ODUpdate
		switch (com->tag) {
		case GF_ODF_OD_UPDATE_TAG:
			odU = (GF_ODUpdate *) com;
			odU2 = (GF_ODUpdate *) gf_odf_com_new(GF_ODF_OD_UPDATE_TAG);

			i=0;
			while ((desc = (GF_Descriptor*)gf_list_enum(odU->objectDescriptors, &i))) {
				switch (desc->tag) {
				case GF_ODF_OD_TAG:
				case GF_ODF_ISOM_OD_TAG:
				//IOD can be used in OD streams
				case GF_ODF_ISOM_IOD_TAG:
					break;
				default:
					return GF_ISOM_INVALID_FILE;
				}
				e = gf_odf_desc_copy(desc, (GF_Descriptor **)&isom_od);
				if (e) goto err_exit;

				//create our OD...
				if (desc->tag == GF_ODF_ISOM_IOD_TAG) {
					od = (GF_ObjectDescriptor *) gf_malloc(sizeof(GF_InitialObjectDescriptor));
				} else {
					od = (GF_ObjectDescriptor *) gf_malloc(sizeof(GF_ObjectDescriptor));
				}
				if (!od) {
					e = GF_OUT_OF_MEM;
					goto err_exit;
				}
				od->ESDescriptors = gf_list_new();
				//and duplicate...
				od->objectDescriptorID = isom_od->objectDescriptorID;
				od->tag = GF_ODF_OD_TAG;
				od->URLString = isom_od->URLString;
				isom_od->URLString = NULL;
				od->extensionDescriptors = isom_od->extensionDescriptors;
				isom_od->extensionDescriptors = NULL;
				od->IPMP_Descriptors = isom_od->IPMP_Descriptors;
				isom_od->IPMP_Descriptors = NULL;
				od->OCIDescriptors = isom_od->OCIDescriptors;
				isom_od->OCIDescriptors = NULL;

				//init as IOD
				if (isom_od->tag == GF_ODF_ISOM_IOD_TAG) {
					((GF_InitialObjectDescriptor *)od)->audio_profileAndLevel = ((GF_IsomInitialObjectDescriptor *)isom_od)->audio_profileAndLevel;
					((GF_InitialObjectDescriptor *)od)->inlineProfileFlag = ((GF_IsomInitialObjectDescriptor *)isom_od)->inlineProfileFlag;
					((GF_InitialObjectDescriptor *)od)->graphics_profileAndLevel = ((GF_IsomInitialObjectDescriptor *)isom_od)->graphics_profileAndLevel;
					((GF_InitialObjectDescriptor *)od)->OD_profileAndLevel = ((GF_IsomInitialObjectDescriptor *)isom_od)->OD_profileAndLevel;
					((GF_InitialObjectDescriptor *)od)->scene_profileAndLevel = ((GF_IsomInitialObjectDescriptor *)isom_od)->scene_profileAndLevel;
					((GF_InitialObjectDescriptor *)od)->visual_profileAndLevel = ((GF_IsomInitialObjectDescriptor *)isom_od)->visual_profileAndLevel;
					((GF_InitialObjectDescriptor *)od)->IPMPToolList = ((GF_IsomInitialObjectDescriptor *)isom_od)->IPMPToolList;
					((GF_IsomInitialObjectDescriptor *)isom_od)->IPMPToolList = NULL;
				}

				//then rewrite the ESDesc
				j=0;
				while ((ref = (GF_ES_ID_Ref*)gf_list_enum(isom_od->ES_ID_RefDescriptors, &j))) {
					//if the ref index is not valid, skip this desc...
					if (!mpod->trackIDs || gf_isom_get_track_from_id(mdia->mediaTrack->moov, mpod->trackIDs[ref->trackRef - 1]) == NULL) continue;
					//OK, get the esd
					e = GetESDForTime(mdia->mediaTrack->moov, mpod->trackIDs[ref->trackRef - 1], sample->DTS, &esd);
					if (!e) e = gf_odf_desc_add_desc((GF_Descriptor *) od, (GF_Descriptor *) esd);
					if (e) {
						gf_odf_desc_del((GF_Descriptor *)od);
						gf_odf_com_del((GF_ODCom **)&odU2);
						gf_odf_desc_del((GF_Descriptor *)isom_od);
						gf_odf_com_del((GF_ODCom **)&odU);
						goto err_exit;
					}

				}
				//delete our desc
				gf_odf_desc_del((GF_Descriptor *)isom_od);
				gf_list_add(odU2->objectDescriptors, od);
			}
			//clean a bit
			gf_odf_com_del((GF_ODCom **)&odU);
			gf_odf_codec_add_com(ODencode, (GF_ODCom *)odU2);
			break;

		case GF_ODF_ESD_UPDATE_TAG:
			esdU = (GF_ESDUpdate *) com;
			esdU2 = (GF_ESDUpdate *) gf_odf_com_new(GF_ODF_ESD_UPDATE_TAG);
			esdU2->ODID = esdU->ODID;
			i=0;
			while ((ref = (GF_ES_ID_Ref*)gf_list_enum(esdU->ESDescriptors, &i))) {
				//if the ref index is not valid, skip this desc...
				if (gf_isom_get_track_from_id(mdia->mediaTrack->moov, mpod->trackIDs[ref->trackRef - 1]) == NULL) continue;
				//OK, get the esd
				e = GetESDForTime(mdia->mediaTrack->moov, mpod->trackIDs[ref->trackRef - 1], sample->DTS, &esd);
				if (e) goto err_exit;
				gf_list_add(esdU2->ESDescriptors, esd);
			}
			gf_odf_com_del((GF_ODCom **)&esdU);
			gf_odf_codec_add_com(ODencode, (GF_ODCom *)esdU2);
			break;

		//brand new case: the ESRemove follows the same principle according to the spec...
		case GF_ODF_ESD_REMOVE_REF_TAG:
			//both commands have the same structure, only the tags change
			esdR = (GF_ESDRemove *) com;
			esdR2 = (GF_ESDRemove *) gf_odf_com_new(GF_ODF_ESD_REMOVE_TAG);
			esdR2->ODID = esdR->ODID;
			esdR2->NbESDs = esdR->NbESDs;
			//alloc our stuff
			esdR2->ES_ID = (unsigned short*)gf_malloc(sizeof(u32) * esdR->NbESDs);
			if (!esdR2->ES_ID) {
				e = GF_OUT_OF_MEM;
				goto err_exit;
			}
			skipped = 0;
			//get the ES_ID in the mpod indicated in the ES_ID[]
			for (i = 0; i < esdR->NbESDs; i++) {
				//if the ref index is not valid, remove this desc...
				if (gf_isom_get_track_from_id(mdia->mediaTrack->moov, mpod->trackIDs[esdR->ES_ID[i] - 1]) == NULL) {
					skipped ++;
				} else {
					//the command in the file has the ref index of the trackID in the mpod
					esdR2->ES_ID[i - skipped] = mpod->trackIDs[esdR->ES_ID[i] - 1];
				}
			}
			//gf_realloc...
			if (skipped && (skipped != esdR2->NbESDs) ) {
				esdR2->NbESDs -= skipped;
				esdR2->ES_ID = (unsigned short*)gf_realloc(esdR2->ES_ID, sizeof(u32) * esdR2->NbESDs);
			}
			gf_odf_com_del((GF_ODCom **)&esdR);
			gf_odf_codec_add_com(ODencode, (GF_ODCom *)esdR2);
			break;

		default:
			e = gf_odf_codec_add_com(ODencode, com);
			if (e) goto err_exit;
		}
	}
	//encode our new AU
	e = gf_odf_codec_encode(ODencode, 1);
	if (e) goto err_exit;

	//and set the buffer in the sample
	gf_free(sample->data);
	sample->data = NULL;
	sample->dataLength = 0;
	e = gf_odf_codec_get_au(ODencode, &sample->data, &sample->dataLength);

err_exit:
	gf_odf_codec_del(ODdecode);
	gf_odf_codec_del(ODencode);
	return e;
}