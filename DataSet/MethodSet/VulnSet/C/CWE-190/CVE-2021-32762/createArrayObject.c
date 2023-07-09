static void *createArrayObject(const redisReadTask *task, size_t elements) {
    redisReply *r, *parent;

    r = createReplyObject(task->type);
    if (r == NULL)
        return NULL;

    if (elements > 0) {
        r->element = hi_calloc(elements,sizeof(redisReply*));
        if (r->element == NULL) {
            freeReplyObject(r);
            return NULL;
        }
    }

    r->elements = elements;

    if (task->parent) {
        parent = task->parent->obj;
        assert(parent->type == REDIS_REPLY_ARRAY ||
               parent->type == REDIS_REPLY_MAP ||
               parent->type == REDIS_REPLY_SET ||
               parent->type == REDIS_REPLY_PUSH);
        parent->element[task->idx] = r;
    }
    return r;
}