static void listdir(unsigned int depth, int f, void * const tls_fd,
                    const char *name)
{
    PureFileInfo *dir;
    char *names;
    PureFileInfo *s;
    PureFileInfo *r;
    int d;

    if (depth >= max_ls_depth || matches >= max_ls_files) {
        return;
    }
    if ((dir = sreaddir(&names)) == NULL) {
        addreply(226, MSG_CANT_READ_FILE, name);
        return;
    }
    s = dir;
    while (s->name_offset != (size_t) -1) {
        d = 0;
        if (FI_NAME(s)[0] != '.') {
            d = listfile(s, NULL);
        } else if (opt_a) {
            if (FI_NAME(s)[1] == 0 ||
                (FI_NAME(s)[1] == '.' && FI_NAME(s)[2] == 0)) {
                listfile(s, NULL);
            } else {
                d = listfile(s, NULL);
            }
        }
        if (!d) {
            s->name_offset = (size_t) -1;
        }
        s++;
    }
    outputfiles(f, tls_fd);
    r = dir;
    while (opt_R && r != s) {
        if (r->name_offset != (size_t) -1 && !chdir(FI_NAME(r))) {
            char *alloca_subdir;
            const size_t sizeof_subdir = PATH_MAX + 1U;

            if ((alloca_subdir = ALLOCA(sizeof_subdir)) == NULL) {
                goto toomany;
            }
            if (SNCHECK(snprintf(alloca_subdir, sizeof_subdir, "%s/%s",
                                 name, FI_NAME(r)), sizeof_subdir)) {
                goto nolist;
            }
            wrstr(f, tls_fd, "\r\n\r\n");
            wrstr(f, tls_fd, alloca_subdir);
            wrstr(f, tls_fd, ":\r\n\r\n");
            listdir(depth + 1U, f, tls_fd, alloca_subdir);
            nolist:
            ALLOCA_FREE(alloca_subdir);
            if (matches >= max_ls_files) {
                goto toomany;
            }
            if (chdir("..")) {    /* defensive in the extreme... */
                if (chdir(wd) || chdir(name)) {    /* someone rmdir()'d it? */
                    die(421, LOG_ERR, "chdir: %s", strerror(errno));
                }
            }
        }
        r++;
    }
    toomany:
    free(names);
    free(dir);
    names = NULL;
}