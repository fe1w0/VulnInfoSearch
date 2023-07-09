static void determine_macros(struct all_attrs_item *all_attrs,
			     const struct attr_stack *stack)
{
	for (; stack; stack = stack->prev) {
		int i;
		for (i = stack->num_matches - 1; i >= 0; i--) {
			const struct match_attr *ma = stack->attrs[i];
			if (ma->is_macro) {
				int n = ma->u.attr->attr_nr;
				if (!all_attrs[n].macro) {
					all_attrs[n].macro = ma;
				}
			}
		}
	}
}