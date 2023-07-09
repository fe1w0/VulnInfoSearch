static void test_reply_reader(void) {
    redisReader *reader;
    void *reply, *root;
    int ret;
    int i;

    test("Error handling in reply parser: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader,(char*)"@foo\r\n",6);
    ret = redisReaderGetReply(reader,NULL);
    test_cond(ret == REDIS_ERR &&
              strcasecmp(reader->errstr,"Protocol error, got \"@\" as reply type byte") == 0);
    redisReaderFree(reader);

    /* when the reply already contains multiple items, they must be free'd
     * on an error. valgrind will bark when this doesn't happen. */
    test("Memory cleanup in reply parser: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader,(char*)"*2\r\n",4);
    redisReaderFeed(reader,(char*)"$5\r\nhello\r\n",11);
    redisReaderFeed(reader,(char*)"@foo\r\n",6);
    ret = redisReaderGetReply(reader,NULL);
    test_cond(ret == REDIS_ERR &&
              strcasecmp(reader->errstr,"Protocol error, got \"@\" as reply type byte") == 0);
    redisReaderFree(reader);

    reader = redisReaderCreate();
    test("Can handle arbitrarily nested multi-bulks: ");
    for (i = 0; i < 128; i++) {
        redisReaderFeed(reader,(char*)"*1\r\n", 4);
    }
    redisReaderFeed(reader,(char*)"$6\r\nLOLWUT\r\n",12);
    ret = redisReaderGetReply(reader,&reply);
    root = reply; /* Keep track of the root reply */
    test_cond(ret == REDIS_OK &&
        ((redisReply*)reply)->type == REDIS_REPLY_ARRAY &&
        ((redisReply*)reply)->elements == 1);

    test("Can parse arbitrarily nested multi-bulks correctly: ");
    while(i--) {
        assert(reply != NULL && ((redisReply*)reply)->type == REDIS_REPLY_ARRAY);
        reply = ((redisReply*)reply)->element[0];
    }
    test_cond(((redisReply*)reply)->type == REDIS_REPLY_STRING &&
        !memcmp(((redisReply*)reply)->str, "LOLWUT", 6));
    freeReplyObject(root);
    redisReaderFree(reader);

    test("Correctly parses LLONG_MAX: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader, ":9223372036854775807\r\n",22);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_OK &&
            ((redisReply*)reply)->type == REDIS_REPLY_INTEGER &&
            ((redisReply*)reply)->integer == LLONG_MAX);
    freeReplyObject(reply);
    redisReaderFree(reader);

    test("Set error when > LLONG_MAX: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader, ":9223372036854775808\r\n",22);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_ERR &&
              strcasecmp(reader->errstr,"Bad integer value") == 0);
    freeReplyObject(reply);
    redisReaderFree(reader);

    test("Correctly parses LLONG_MIN: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader, ":-9223372036854775808\r\n",23);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_OK &&
            ((redisReply*)reply)->type == REDIS_REPLY_INTEGER &&
            ((redisReply*)reply)->integer == LLONG_MIN);
    freeReplyObject(reply);
    redisReaderFree(reader);

    test("Set error when < LLONG_MIN: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader, ":-9223372036854775809\r\n",23);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_ERR &&
              strcasecmp(reader->errstr,"Bad integer value") == 0);
    freeReplyObject(reply);
    redisReaderFree(reader);

    test("Set error when array < -1: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader, "*-2\r\n+asdf\r\n",12);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_ERR &&
              strcasecmp(reader->errstr,"Multi-bulk length out of range") == 0);
    freeReplyObject(reply);
    redisReaderFree(reader);

    test("Set error when bulk < -1: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader, "$-2\r\nasdf\r\n",11);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_ERR &&
              strcasecmp(reader->errstr,"Bulk string length out of range") == 0);
    freeReplyObject(reply);
    redisReaderFree(reader);

    test("Can configure maximum multi-bulk elements: ");
    reader = redisReaderCreate();
    reader->maxelements = 1024;
    redisReaderFeed(reader, "*1025\r\n", 7);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_ERR &&
              strcasecmp(reader->errstr, "Multi-bulk length out of range") == 0);
    freeReplyObject(reply);
    redisReaderFree(reader);

#if LLONG_MAX > SIZE_MAX
    test("Set error when array > SIZE_MAX: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader, "*9223372036854775807\r\n+asdf\r\n",29);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_ERR &&
            strcasecmp(reader->errstr,"Multi-bulk length out of range") == 0);
    freeReplyObject(reply);
    redisReaderFree(reader);

    test("Set error when bulk > SIZE_MAX: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader, "$9223372036854775807\r\nasdf\r\n",28);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_ERR &&
            strcasecmp(reader->errstr,"Bulk string length out of range") == 0);
    freeReplyObject(reply);
    redisReaderFree(reader);
#endif

    test("Works with NULL functions for reply: ");
    reader = redisReaderCreate();
    reader->fn = NULL;
    redisReaderFeed(reader,(char*)"+OK\r\n",5);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_OK && reply == (void*)REDIS_REPLY_STATUS);
    redisReaderFree(reader);

    test("Works when a single newline (\\r\\n) covers two calls to feed: ");
    reader = redisReaderCreate();
    reader->fn = NULL;
    redisReaderFeed(reader,(char*)"+OK\r",4);
    ret = redisReaderGetReply(reader,&reply);
    assert(ret == REDIS_OK && reply == NULL);
    redisReaderFeed(reader,(char*)"\n",1);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_OK && reply == (void*)REDIS_REPLY_STATUS);
    redisReaderFree(reader);

    test("Don't reset state after protocol error: ");
    reader = redisReaderCreate();
    reader->fn = NULL;
    redisReaderFeed(reader,(char*)"x",1);
    ret = redisReaderGetReply(reader,&reply);
    assert(ret == REDIS_ERR);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_ERR && reply == NULL);
    redisReaderFree(reader);

    /* Regression test for issue #45 on GitHub. */
    test("Don't do empty allocation for empty multi bulk: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader,(char*)"*0\r\n",4);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_OK &&
        ((redisReply*)reply)->type == REDIS_REPLY_ARRAY &&
        ((redisReply*)reply)->elements == 0);
    freeReplyObject(reply);
    redisReaderFree(reader);

    /* RESP3 verbatim strings (GitHub issue #802) */
    test("Can parse RESP3 verbatim strings: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader,(char*)"=10\r\ntxt:LOLWUT\r\n",17);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_OK &&
        ((redisReply*)reply)->type == REDIS_REPLY_VERB &&
         !memcmp(((redisReply*)reply)->str,"LOLWUT", 6));
    freeReplyObject(reply);
    redisReaderFree(reader);

    /* RESP3 push messages (Github issue #815) */
    test("Can parse RESP3 push messages: ");
    reader = redisReaderCreate();
    redisReaderFeed(reader,(char*)">2\r\n$6\r\nLOLWUT\r\n:42\r\n",21);
    ret = redisReaderGetReply(reader,&reply);
    test_cond(ret == REDIS_OK &&
        ((redisReply*)reply)->type == REDIS_REPLY_PUSH &&
        ((redisReply*)reply)->elements == 2 &&
        ((redisReply*)reply)->element[0]->type == REDIS_REPLY_STRING &&
        !memcmp(((redisReply*)reply)->element[0]->str,"LOLWUT",6) &&
        ((redisReply*)reply)->element[1]->type == REDIS_REPLY_INTEGER &&
        ((redisReply*)reply)->element[1]->integer == 42);
    freeReplyObject(reply);
    redisReaderFree(reader);
}