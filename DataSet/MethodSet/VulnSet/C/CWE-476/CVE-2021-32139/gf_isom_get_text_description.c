GF_Err gf_isom_get_text_description(GF_ISOFile *movie, u32 trackNumber, u32 descriptionIndex, GF_TextSampleDescriptor **out_desc)
{
	GF_TrackBox *trak;
	u32 i;
	Bool is_qt_text = GF_FALSE;
	GF_Tx3gSampleEntryBox *txt;

	if (!descriptionIndex || !out_desc) return GF_BAD_PARAM;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media) return GF_BAD_PARAM;

	switch (trak->Media->handler->handlerType) {
	case GF_ISOM_MEDIA_TEXT:
	case GF_ISOM_MEDIA_SUBT:
		break;
	default:
		return GF_BAD_PARAM;
	}

	txt = (GF_Tx3gSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, descriptionIndex - 1);
	if (!txt) return GF_BAD_PARAM;
	switch (txt->type) {
	case GF_ISOM_BOX_TYPE_TX3G:
		break;
	case GF_ISOM_BOX_TYPE_TEXT:
		is_qt_text = GF_TRUE;
		break;
	default:
		return GF_BAD_PARAM;
	}

	(*out_desc) = (GF_TextSampleDescriptor *) gf_odf_desc_new(GF_ODF_TX3G_TAG);
	if (! (*out_desc) ) return GF_OUT_OF_MEM;
	(*out_desc)->back_color = txt->back_color;
	(*out_desc)->default_pos = txt->default_box;
	(*out_desc)->default_style = txt->default_style;
	(*out_desc)->displayFlags = txt->displayFlags;
	(*out_desc)->vert_justif = txt->vertical_justification;
	(*out_desc)->horiz_justif = txt->horizontal_justification;
	if (is_qt_text) {
		GF_TextSampleEntryBox *qt_txt = (GF_TextSampleEntryBox *) txt;
		if (qt_txt->textName) {
			(*out_desc)->font_count = 1;
			(*out_desc)->fonts = (GF_FontRecord *) gf_malloc(sizeof(GF_FontRecord));
			(*out_desc)->fonts[0].fontName = gf_strdup(qt_txt->textName);
		}
	} else {
		(*out_desc)->font_count = txt->font_table->entry_count;
		(*out_desc)->fonts = (GF_FontRecord *) gf_malloc(sizeof(GF_FontRecord) * txt->font_table->entry_count);
		for (i=0; i<txt->font_table->entry_count; i++) {
			(*out_desc)->fonts[i].fontID = txt->font_table->fonts[i].fontID;
			if (txt->font_table->fonts[i].fontName)
				(*out_desc)->fonts[i].fontName = gf_strdup(txt->font_table->fonts[i].fontName);
		}
	}
	return GF_OK;
}