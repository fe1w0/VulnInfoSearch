int bson_ensure_space( bson *b, const int bytesNeeded ) {
    int pos = b->cur - b->data;
    char *orig = b->data;
    int new_size;

    if ( pos + bytesNeeded <= b->dataSize )
        return BSON_OK;

    new_size = 1.5 * ( b->dataSize + bytesNeeded );

    if( new_size < b->dataSize ) {
        if( ( b->dataSize + bytesNeeded ) < INT_MAX )
            new_size = INT_MAX;
        else {
            b->err = BSON_SIZE_OVERFLOW;
            return BSON_ERROR;
        }
    }

    b->data = bson_realloc( b->data, new_size );
    if ( !b->data )
        bson_fatal_msg( !!b->data, "realloc() failed" );

    b->dataSize = new_size;
    b->cur += b->data - orig;

    return BSON_OK;
}