static int bson_append_estart( bson *b, int type, const char *name, const size_t dataSize ) {
    const int len = strlen( name ) + 1;

    if ( b->finished ) {
        b->err |= BSON_ALREADY_FINISHED;
        return BSON_ERROR;
    }

    if ( bson_ensure_space( b, 1 + len + dataSize ) == BSON_ERROR ) {
        return BSON_ERROR;
    }

    if( bson_check_field_name( b, ( const char * )name, len - 1 ) == BSON_ERROR ) {
        bson_builder_error( b );
        return BSON_ERROR;
    }

    bson_append_byte( b, ( char )type );
    bson_append( b, name, len );
    return BSON_OK;
}