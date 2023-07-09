GF_Err text_box_size(GF_Box *s)
{
	GF_TextSampleEntryBox *ptr = (GF_TextSampleEntryBox*)s;

	s->size += 8;
	/*base + this + string length*/
	s->size += 43 + 1;
	if (ptr->textName)
		s->size += strlen(ptr->textName);
	return GF_OK;
}