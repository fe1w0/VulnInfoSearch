static void attr_stack_free(struct attr_stack *e)
{
	int i;
	free(e->origin);
	for (i = 0; i < e->num_matches; i++) {
		struct match_attr *a = e->attrs[i];
		int j;
		for (j = 0; j < a->num_attr; j++) {
			const char *setto = a->state[j].setto;
			if (setto == ATTR__TRUE ||
			    setto == ATTR__FALSE ||
			    setto == ATTR__UNSET ||
			    setto == ATTR__UNKNOWN)
				;
			else
				free((char *) setto);
		}
		free(a);
	}
	free(e->attrs);
	free(e);
}