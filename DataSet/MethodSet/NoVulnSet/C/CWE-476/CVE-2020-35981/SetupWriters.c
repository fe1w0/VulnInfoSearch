GF_Err SetupWriters(MovieWriter *mw, GF_List *writers, u8 interleaving)
{
	u32 i, trackCount;
	TrackWriter *writer;
	GF_TrackBox *trak;
	GF_ISOFile *movie = mw->movie;

	mw->total_samples = mw->nb_done = 0;
	if (!movie->moov) return GF_OK;

	trackCount = gf_list_count(movie->moov->trackList);
	for (i = 0; i < trackCount; i++) {
		GF_SampleTableBox *stbl;
		trak = gf_isom_get_track(movie->moov, i+1);

		stbl = (trak->Media && trak->Media->information) ? trak->Media->information->sampleTable : NULL;
		if (!stbl || !stbl->SampleSize || !stbl->ChunkOffset || !stbl->SampleToChunk) {
			return GF_ISOM_INVALID_FILE;
		}

		GF_SAFEALLOC(writer, TrackWriter);
		if (!writer) goto exit;
		writer->sampleNumber = 1;
		writer->mdia = trak->Media;
		writer->stbl = trak->Media->information->sampleTable;
		writer->timeScale = trak->Media->mediaHeader->timeScale;
		writer->all_dref_mode = Media_SelfContainedType(writer->mdia);

		if (trak->sample_encryption)
			writer->prevent_dispatch = GF_TRUE;

		writer->isDone = 0;
		writer->DTSprev = 0;
		writer->chunkDur = 0;
		writer->chunkSize = 0;
		writer->constant_size = writer->constant_dur = 0;
		if (writer->stbl->SampleSize->sampleSize)
			writer->constant_size = writer->stbl->SampleSize->sampleSize;
		if (writer->stbl->TimeToSample->nb_entries==1) {
			writer->constant_dur = writer->stbl->TimeToSample->entries[0].sampleDelta;
			if (writer->constant_dur>1) writer->constant_dur = 0;
		}
		if (!writer->constant_dur || !writer->constant_size || (writer->constant_size>=10))
			writer->constant_size = writer->constant_dur = 0;

		writer->stsc = (GF_SampleToChunkBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_STSC);
		if (!writer->stsc) return GF_OUT_OF_MEM;
		if (writer->stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO) {
			writer->stco = gf_isom_box_new(GF_ISOM_BOX_TYPE_STCO);
		} else {
			writer->stco = gf_isom_box_new(GF_ISOM_BOX_TYPE_CO64);
		}
		if (!writer->stco) return GF_OUT_OF_MEM;
		/*stops from chunk escape*/
		if (interleaving) writer->stbl->MaxSamplePerChunk = 0;
		/*for progress, assume only one descIndex*/
		if (Media_IsSelfContained(writer->mdia, 1))
			mw->total_samples += writer->stbl->SampleSize->sampleCount;
		/*optimization for interleaving: put audio last (this can be overridden by priorities)*/
		if (movie->storageMode != GF_ISOM_STORE_INTERLEAVED) {
			gf_list_add(writers, writer);
		} else {
			if (writer->mdia->information->InfoHeader && writer->mdia->information->InfoHeader->type == GF_ISOM_BOX_TYPE_SMHD) {
				gf_list_add(writers, writer);
			} else {
				gf_list_insert(writers, writer, 0);
			}
		}
		if (movie->sample_groups_in_traf && trak->Media->information->sampleTable) {
			gf_isom_box_array_del_parent(&trak->Media->information->sampleTable->child_boxes, trak->Media->information->sampleTable->sampleGroupsDescription);
			trak->Media->information->sampleTable->sampleGroupsDescription = NULL;
		}
	}
	return GF_OK;

exit:
	CleanWriters(writers);
	return GF_OUT_OF_MEM;
}