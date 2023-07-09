static void get_sem_elements(struct sem_data *p)
{
	size_t i;

	if (!p || !p->sem_nsems || p->sem_perm.id < 0)
		return;

	p->elements = xcalloc(p->sem_nsems, sizeof(struct sem_elem));

	for (i = 0; i < p->sem_nsems; i++) {
		struct sem_elem *e = &p->elements[i];
		union semun arg = { .val = 0 };

		e->semval = semctl(p->sem_perm.id, i, GETVAL, arg);
		if (e->semval < 0)
			err(EXIT_FAILURE, _("%s failed"), "semctl(GETVAL)");

		e->ncount = semctl(p->sem_perm.id, i, GETNCNT, arg);
		if (e->ncount < 0)
			err(EXIT_FAILURE, _("%s failed"), "semctl(GETNCNT)");

		e->zcount = semctl(p->sem_perm.id, i, GETZCNT, arg);
		if (e->zcount < 0)
			err(EXIT_FAILURE, _("%s failed"), "semctl(GETZCNT)");

		e->pid = semctl(p->sem_perm.id, i, GETPID, arg);
		if (e->pid < 0)
			err(EXIT_FAILURE, _("%s failed"), "semctl(GETPID)");
	}
}