void xaddCommand(client *c) {
    streamID id;
    int id_given = 0; /* Was an ID different than "*" specified? */
    long long maxlen = -1;  /* If left to -1 no trimming is performed. */
    int approx_maxlen = 0;  /* If 1 only delete whole radix tree nodes, so
                               the maximum length is not applied verbatim. */
    int maxlen_arg_idx = 0; /* Index of the count in MAXLEN, for rewriting. */

    /* Parse options. */
    int i = 2; /* This is the first argument position where we could
                  find an option, or the ID. */
    for (; i < c->argc; i++) {
        int moreargs = (c->argc-1) - i; /* Number of additional arguments. */
        char *opt = c->argv[i]->ptr;
        if (opt[0] == '*' && opt[1] == '\0') {
            /* This is just a fast path for the common case of auto-ID
             * creation. */
            break;
        } else if (!strcasecmp(opt,"maxlen") && moreargs) {
            approx_maxlen = 0;
            char *next = c->argv[i+1]->ptr;
            /* Check for the form MAXLEN ~ <count>. */
            if (moreargs >= 2 && next[0] == '~' && next[1] == '\0') {
                approx_maxlen = 1;
                i++;
            } else if (moreargs >= 2 && next[0] == '=' && next[1] == '\0') {
                i++;
            }
            if (getLongLongFromObjectOrReply(c,c->argv[i+1],&maxlen,NULL)
                != C_OK) return;

            if (maxlen < 0) {
                addReplyError(c,"The MAXLEN argument must be >= 0.");
                return;
            }
            i++;
            maxlen_arg_idx = i;
        } else {
            /* If we are here is a syntax error or a valid ID. */
            if (streamParseStrictIDOrReply(c,c->argv[i],&id,0) != C_OK) return;
            id_given = 1;
            break;
        }
    }
    int field_pos = i+1;

    /* Check arity. */
    if ((c->argc - field_pos) < 2 || ((c->argc-field_pos) % 2) == 1) {
        addReplyError(c,"wrong number of arguments for XADD");
        return;
    }

    /* Return ASAP if minimal ID (0-0) was given so we avoid possibly creating
     * a new stream and have streamAppendItem fail, leaving an empty key in the
     * database. */
    if (id_given && id.ms == 0 && id.seq == 0) {
        addReplyError(c,"The ID specified in XADD must be greater than 0-0");
        return;
    }

    /* Lookup the stream at key. */
    robj *o;
    stream *s;
    if ((o = streamTypeLookupWriteOrCreate(c,c->argv[1])) == NULL) return;
    s = o->ptr;

    /* Return ASAP if the stream has reached the last possible ID */
    if (s->last_id.ms == UINT64_MAX && s->last_id.seq == UINT64_MAX) {
        addReplyError(c,"The stream has exhausted the last possible ID, "
                        "unable to add more items");
        return;
    }

    /* Append using the low level function and return the ID. */
    if (streamAppendItem(s,c->argv+field_pos,(c->argc-field_pos)/2,
        &id, id_given ? &id : NULL)
        == C_ERR)
    {
        addReplyError(c,"The ID specified in XADD is equal or smaller than the "
                        "target stream top item");
        return;
    }
    addReplyStreamID(c,&id);

    signalModifiedKey(c,c->db,c->argv[1]);
    notifyKeyspaceEvent(NOTIFY_STREAM,"xadd",c->argv[1],c->db->id);
    server.dirty++;

    if (maxlen >= 0) {
        /* Notify xtrim event if needed. */
        if (streamTrimByLength(s,maxlen,approx_maxlen)) {
            notifyKeyspaceEvent(NOTIFY_STREAM,"xtrim",c->argv[1],c->db->id);
        }
        if (approx_maxlen) streamRewriteApproxMaxlen(c,s,maxlen_arg_idx);
    }

    /* Let's rewrite the ID argument with the one actually generated for
     * AOF/replication propagation. */
    robj *idarg = createObjectFromStreamID(&id);
    rewriteClientCommandArgument(c,i,idarg);
    decrRefCount(idarg);

    /* We need to signal to blocked clients that there is new data on this
     * stream. */
    if (server.blocked_clients_by_type[BLOCKED_STREAM])
        signalKeyAsReady(c->db, c->argv[1]);
}