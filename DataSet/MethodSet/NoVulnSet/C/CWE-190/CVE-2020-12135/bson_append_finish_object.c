MONGO_EXPORT int bson_append_finish_object( bson *b ) {
    char *start;
    int i;
    if ( bson_ensure_space( b, 1 ) == BSON_ERROR ) return BSON_ERROR;
    bson_append_byte( b , 0 );

    start = b->data + b->stack[ --b->stackPos ];
    i = ( int )( b->cur - start );
    bson_little_endian32( start, &i );

    return BSON_OK;
}