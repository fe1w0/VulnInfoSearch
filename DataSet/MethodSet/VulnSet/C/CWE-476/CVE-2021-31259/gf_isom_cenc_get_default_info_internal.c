void gf_isom_cenc_get_default_info_internal(GF_TrackBox *trak, u32 sampleDescriptionIndex, u32 *container_type, Bool *default_IsEncrypted, u8 *crypt_byte_block, u8 *skip_byte_block, const u8 **key_info, u32 *key_info_size)
{
	GF_ProtectionSchemeInfoBox *sinf;


	//setup all default as not encrypted
	if (default_IsEncrypted) *default_IsEncrypted = GF_FALSE;
	if (crypt_byte_block) *crypt_byte_block = 0;
	if (skip_byte_block) *skip_byte_block = 0;
	if (container_type) *container_type = 0;
	if (key_info) *key_info = NULL;
	if (key_info_size) *key_info_size = 0;

	sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_CENC_SCHEME, NULL);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_CBC_SCHEME, NULL);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_CENS_SCHEME, NULL);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_CBCS_SCHEME, NULL);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_PIFF_SCHEME, NULL);

	if (!sinf) {
		u32 i, nb_stsd = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
		for (i=0; i<nb_stsd; i++) {
			GF_ProtectionSchemeInfoBox *a_sinf;
			GF_SampleEntryBox *sentry=NULL;
			if (i+1==sampleDescriptionIndex) continue;
			sentry = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, i);
			a_sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(sentry->child_boxes, GF_ISOM_BOX_TYPE_SINF);
			if (!a_sinf) continue;
			//signal default (not encrypted)
			return;
		}
	}

	if (sinf && sinf->info && sinf->info->tenc) {
		if (default_IsEncrypted) *default_IsEncrypted = sinf->info->tenc->isProtected;
		if (crypt_byte_block) *crypt_byte_block = sinf->info->tenc->crypt_byte_block;
		if (skip_byte_block) *skip_byte_block = sinf->info->tenc->skip_byte_block;
		if (key_info) *key_info = sinf->info->tenc->key_info;
		if (key_info_size) {
			*key_info_size = 20;
			if (!sinf->info->tenc->key_info[3])
				*key_info_size += 1 + sinf->info->tenc->key_info[20];
		}

		//set default value, overwritten below
		if (container_type) *container_type = GF_ISOM_BOX_TYPE_SENC;
	} else if (sinf && sinf->info && sinf->info->piff_tenc) {
		if (default_IsEncrypted) *default_IsEncrypted = GF_TRUE;
		if (key_info) *key_info = sinf->info->piff_tenc->key_info;
		if (key_info_size) *key_info_size = 19;
		//set default value, overwritten below
		if (container_type) *container_type = GF_ISOM_BOX_UUID_PSEC;
	} else {
		u32 i, count = 0;
		GF_CENCSampleEncryptionGroupEntry *seig_entry = NULL;

		if (!trak->moov->mov->is_smooth)
			count = gf_list_count(trak->Media->information->sampleTable->sampleGroupsDescription);

		for (i=0; i<count; i++) {
			GF_SampleGroupDescriptionBox *sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroupsDescription, i);
			if (sgdesc->grouping_type!=GF_ISOM_SAMPLE_GROUP_SEIG) continue;
			if (sgdesc->default_description_index)
				seig_entry = gf_list_get(sgdesc->group_descriptions, sgdesc->default_description_index-1);
			else
				seig_entry = gf_list_get(sgdesc->group_descriptions, 0);
			if (!seig_entry->key_info[0])
				seig_entry = NULL;
			break;
		}
		if (seig_entry) {
			if (default_IsEncrypted) *default_IsEncrypted = seig_entry->IsProtected;
			if (crypt_byte_block) *crypt_byte_block = seig_entry->crypt_byte_block;
			if (skip_byte_block) *skip_byte_block = seig_entry->skip_byte_block;
			if (key_info) *key_info = seig_entry->key_info;
			if (key_info_size) *key_info_size = seig_entry->key_info_size;
			if (container_type) *container_type = GF_ISOM_BOX_TYPE_SENC;
		} else {
			if (! trak->moov->mov->is_smooth ) {
				trak->moov->mov->is_smooth = GF_TRUE;
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] senc box without tenc, assuming MS smooth+piff\n"));
			}
			if (default_IsEncrypted) *default_IsEncrypted = GF_TRUE;
			//set default value, overwritten below
			if (container_type) *container_type = GF_ISOM_BOX_UUID_PSEC;
		}
	}

	if (container_type && trak->sample_encryption) {
		if (trak->sample_encryption->type == GF_ISOM_BOX_TYPE_SENC) *container_type = GF_ISOM_BOX_TYPE_SENC;
		else if (trak->sample_encryption->type == GF_ISOM_BOX_TYPE_UUID) *container_type = ((GF_UUIDBox*)trak->sample_encryption)->internal_4cc;
	}
}