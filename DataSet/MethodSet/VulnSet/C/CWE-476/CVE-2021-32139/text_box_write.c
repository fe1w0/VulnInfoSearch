GF_Err text_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u16 pSize;
	GF_TextSampleEntryBox *ptr = (GF_TextSampleEntryBox*)s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, ptr->reserved, 6);
	gf_bs_write_u16(bs, ptr->dataReferenceIndex);
	gf_bs_write_u32(bs, ptr->displayFlags);			/*Display flags*/
	gf_bs_write_u32(bs, ptr->textJustification);	/*Text justification*/
	gf_bs_write_data(bs, ptr->background_color, 6);	/*Background color*/
	gpp_write_box(bs, &ptr->default_box);			/*Default text box*/
	gf_bs_write_data(bs, ptr->reserved1, 8);		/*Reserved*/
	gf_bs_write_u16(bs, ptr->fontNumber);			/*Font number*/
	gf_bs_write_u16(bs, ptr->fontFace);				/*Font face*/
	gf_bs_write_u8(bs, ptr->reserved2);				/*Reserved*/
	gf_bs_write_u16(bs, ptr->reserved3);			/*Reserved*/
	gf_bs_write_data(bs, ptr->foreground_color, 6);	/*Foreground color*/
	//pSize assignment below is not a mistake
	if (ptr->textName && (pSize = (u16) strlen(ptr->textName))) {
		gf_bs_write_u8(bs, pSize);					/*a Pascal string begins with its size*/
		gf_bs_write_data(bs, ptr->textName, pSize);	/*Font name*/
	} else {
		gf_bs_write_u8(bs, 0);
	}
	return GF_OK;
}