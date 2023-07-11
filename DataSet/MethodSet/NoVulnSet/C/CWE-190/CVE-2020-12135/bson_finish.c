MONGO_EXPORT int bson_finish( bson *b ) {
    int i;

    if( b->err & BSON_NOT_UTF8 )
        return BSON_ERROR;

    if ( ! b->finished ) {
        if ( bson_ensure_space( b, 1 ) == BSON_ERROR ) return BSON_ERROR;
        bson_append_byte( b, 0 );
        i = ( int )( b->cur - b->data );
        bson_little_endian32( b->data, &i );
        b->finished = 1;
    }

    return BSON_OK;
}