void *bson_realloc( void *ptr, size_t size ) {
    void *p;
    p = bson_realloc_func( ptr, size );
    bson_fatal_msg( !!p, "realloc() failed" );
    return p;
}