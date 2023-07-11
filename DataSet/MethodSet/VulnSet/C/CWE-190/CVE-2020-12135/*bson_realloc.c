void *bson_realloc( void *ptr, int size ) {
    void *p;
    p = bson_realloc_func( ptr, size );
    bson_fatal_msg( !!p, "realloc() failed" );
    return p;
}