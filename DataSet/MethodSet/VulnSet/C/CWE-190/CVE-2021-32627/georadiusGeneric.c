void georadiusGeneric(client *c, int flags) {
    robj *key = c->argv[1];
    robj *storekey = NULL;
    int storedist = 0; /* 0 for STORE, 1 for STOREDIST. */

    /* Look up the requested zset */
    robj *zobj = NULL;
    if ((zobj = lookupKeyReadOrReply(c, key, shared.emptyarray)) == NULL ||
        checkType(c, zobj, OBJ_ZSET)) {
        return;
    }

    /* Find long/lat to use for radius search based on inquiry type */
    int base_args;
    double xy[2] = { 0 };
    if (flags & RADIUS_COORDS) {
        base_args = 6;
        if (extractLongLatOrReply(c, c->argv + 2, xy) == C_ERR)
            return;
    } else if (flags & RADIUS_MEMBER) {
        base_args = 5;
        robj *member = c->argv[2];
        if (longLatFromMember(zobj, member, xy) == C_ERR) {
            addReplyError(c, "could not decode requested zset member");
            return;
        }
    } else {
        addReplyError(c, "Unknown georadius search type");
        return;
    }

    /* Extract radius and units from arguments */
    double radius_meters = 0, conversion = 1;
    if ((radius_meters = extractDistanceOrReply(c, c->argv + base_args - 2,
                                                &conversion)) < 0) {
        return;
    }

    /* Discover and populate all optional parameters. */
    int withdist = 0, withhash = 0, withcoords = 0;
    int sort = SORT_NONE;
    long long count = 0;
    if (c->argc > base_args) {
        int remaining = c->argc - base_args;
        for (int i = 0; i < remaining; i++) {
            char *arg = c->argv[base_args + i]->ptr;
            if (!strcasecmp(arg, "withdist")) {
                withdist = 1;
            } else if (!strcasecmp(arg, "withhash")) {
                withhash = 1;
            } else if (!strcasecmp(arg, "withcoord")) {
                withcoords = 1;
            } else if (!strcasecmp(arg, "asc")) {
                sort = SORT_ASC;
            } else if (!strcasecmp(arg, "desc")) {
                sort = SORT_DESC;
            } else if (!strcasecmp(arg, "count") && (i+1) < remaining) {
                if (getLongLongFromObjectOrReply(c, c->argv[base_args+i+1],
                    &count, NULL) != C_OK) return;
                if (count <= 0) {
                    addReplyError(c,"COUNT must be > 0");
                    return;
                }
                i++;
            } else if (!strcasecmp(arg, "store") &&
                       (i+1) < remaining &&
                       !(flags & RADIUS_NOSTORE))
            {
                storekey = c->argv[base_args+i+1];
                storedist = 0;
                i++;
            } else if (!strcasecmp(arg, "storedist") &&
                       (i+1) < remaining &&
                       !(flags & RADIUS_NOSTORE))
            {
                storekey = c->argv[base_args+i+1];
                storedist = 1;
                i++;
            } else {
                addReply(c, shared.syntaxerr);
                return;
            }
        }
    }

    /* Trap options not compatible with STORE and STOREDIST. */
    if (storekey && (withdist || withhash || withcoords)) {
        addReplyError(c,
            "STORE option in GEORADIUS is not compatible with "
            "WITHDIST, WITHHASH and WITHCOORDS options");
        return;
    }

    /* COUNT without ordering does not make much sense, force ASC
     * ordering if COUNT was specified but no sorting was requested. */
    if (count != 0 && sort == SORT_NONE) sort = SORT_ASC;

    /* Get all neighbor geohash boxes for our radius search */
    GeoHashRadius georadius =
        geohashGetAreasByRadiusWGS84(xy[0], xy[1], radius_meters);

    /* Search the zset for all matching points */
    geoArray *ga = geoArrayCreate();
    membersOfAllNeighbors(zobj, georadius, xy[0], xy[1], radius_meters, ga);

    /* If no matching results, the user gets an empty reply. */
    if (ga->used == 0 && storekey == NULL) {
        addReply(c,shared.emptyarray);
        geoArrayFree(ga);
        return;
    }

    long result_length = ga->used;
    long returned_items = (count == 0 || result_length < count) ?
                          result_length : count;
    long option_length = 0;

    /* Process [optional] requested sorting */
    if (sort == SORT_ASC) {
        qsort(ga->array, result_length, sizeof(geoPoint), sort_gp_asc);
    } else if (sort == SORT_DESC) {
        qsort(ga->array, result_length, sizeof(geoPoint), sort_gp_desc);
    }

    if (storekey == NULL) {
        /* No target key, return results to user. */

        /* Our options are self-contained nested multibulk replies, so we
         * only need to track how many of those nested replies we return. */
        if (withdist)
            option_length++;

        if (withcoords)
            option_length++;

        if (withhash)
            option_length++;

        /* The array len we send is exactly result_length. The result is
         * either all strings of just zset members  *or* a nested multi-bulk
         * reply containing the zset member string _and_ all the additional
         * options the user enabled for this request. */
        addReplyArrayLen(c, returned_items);

        /* Finally send results back to the caller */
        int i;
        for (i = 0; i < returned_items; i++) {
            geoPoint *gp = ga->array+i;
            gp->dist /= conversion; /* Fix according to unit. */

            /* If we have options in option_length, return each sub-result
             * as a nested multi-bulk.  Add 1 to account for result value
             * itself. */
            if (option_length)
                addReplyArrayLen(c, option_length + 1);

            addReplyBulkSds(c,gp->member);
            gp->member = NULL;

            if (withdist)
                addReplyDoubleDistance(c, gp->dist);

            if (withhash)
                addReplyLongLong(c, gp->score);

            if (withcoords) {
                addReplyArrayLen(c, 2);
                addReplyHumanLongDouble(c, gp->longitude);
                addReplyHumanLongDouble(c, gp->latitude);
            }
        }
    } else {
        /* Target key, create a sorted set with the results. */
        robj *zobj;
        zset *zs;
        int i;
        size_t maxelelen = 0;

        if (returned_items) {
            zobj = createZsetObject();
            zs = zobj->ptr;
        }

        for (i = 0; i < returned_items; i++) {
            zskiplistNode *znode;
            geoPoint *gp = ga->array+i;
            gp->dist /= conversion; /* Fix according to unit. */
            double score = storedist ? gp->dist : gp->score;
            size_t elelen = sdslen(gp->member);

            if (maxelelen < elelen) maxelelen = elelen;
            znode = zslInsert(zs->zsl,score,gp->member);
            serverAssert(dictAdd(zs->dict,gp->member,&znode->score) == DICT_OK);
            gp->member = NULL;
        }

        if (returned_items) {
            zsetConvertToZiplistIfNeeded(zobj,maxelelen);
            setKey(c,c->db,storekey,zobj);
            decrRefCount(zobj);
            notifyKeyspaceEvent(NOTIFY_ZSET,"georadiusstore",storekey,
                                c->db->id);
            server.dirty += returned_items;
        } else if (dbDelete(c->db,storekey)) {
            signalModifiedKey(c,c->db,storekey);
            notifyKeyspaceEvent(NOTIFY_GENERIC,"del",storekey,c->db->id);
            server.dirty++;
        }
        addReplyLongLong(c, returned_items);
    }
    geoArrayFree(ga);
}