MONGO_EXPORT void *bson_malloc( int size ) {
    void *p;
    p = bson_malloc_func( size );
    bson_fatal_msg( !!p, "malloc() failed" );
    return p;
}