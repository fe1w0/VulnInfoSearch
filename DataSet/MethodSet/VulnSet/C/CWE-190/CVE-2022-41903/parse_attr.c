static const char *parse_attr(const char *src, int lineno, const char *cp,
			      struct attr_state *e)
{
	const char *ep, *equals;
	int len;

	ep = cp + strcspn(cp, blank);
	equals = strchr(cp, '=');
	if (equals && ep < equals)
		equals = NULL;
	if (equals)
		len = equals - cp;
	else
		len = ep - cp;
	if (!e) {
		if (*cp == '-' || *cp == '!') {
			cp++;
			len--;
		}
		if (!attr_name_valid(cp, len)) {
			report_invalid_attr(cp, len, src, lineno);
			return NULL;
		}
	} else {
		/*
		 * As this function is always called twice, once with
		 * e == NULL in the first pass and then e != NULL in
		 * the second pass, no need for attr_name_valid()
		 * check here.
		 */
		if (*cp == '-' || *cp == '!') {
			e->setto = (*cp == '-') ? ATTR__FALSE : ATTR__UNSET;
			cp++;
			len--;
		}
		else if (!equals)
			e->setto = ATTR__TRUE;
		else {
			e->setto = xmemdupz(equals + 1, ep - equals - 1);
		}
		e->attr = git_attr_internal(cp, len);
	}
	return ep + strspn(ep, blank);
}