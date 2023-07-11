MONGO_EXPORT void *bson_malloc( size_t size ) {
    void *p;
    p = bson_malloc_func( size );
    bson_fatal_msg( !!p, "malloc() failed" );
    return p;
}