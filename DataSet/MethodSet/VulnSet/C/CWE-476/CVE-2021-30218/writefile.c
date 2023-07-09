int
writefile(const char *name, struct string *s)
{
	FILE *f;
	int ret;

	f = fopen(name, "w");
	if (!f) {
		warn("open %s:", name);
		return -1;
	}
	ret = 0;
	if (fwrite(s->s, 1, s->n, f) != s->n || fflush(f) != 0) {
		warn("write %s:", name);
		ret = -1;
	}
	fclose(f);

	return ret;
}