void gitn_box_del(GF_Box *s)
{
	u32 i;
	GroupIdToNameBox *ptr = (GroupIdToNameBox *)s;
	if (ptr == NULL) return;
	if (ptr->entries) {
		for (i=0; i<ptr->nb_entries; i++) {
			if (ptr->entries[i].name) gf_free(ptr->entries[i].name);
		}
		gf_free(ptr->entries);
	}
	gf_free(ptr);
}