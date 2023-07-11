MONGO_EXPORT int bson_append_element( bson *b, const char *name_or_null, const bson_iterator *elem ) {
    bson_iterator next = *elem;
    size_t size;

    bson_iterator_next( &next );
    size = next.cur - elem->cur;

    if ( name_or_null == NULL ) {
        if( bson_ensure_space( b, size ) == BSON_ERROR )
            return BSON_ERROR;
        bson_append( b, elem->cur, size );
    }
    else {
        size_t data_size = size - 2 - strlen( bson_iterator_key( elem ) );
        bson_append_estart( b, elem->cur[0], name_or_null, data_size );
        bson_append( b, bson_iterator_value( elem ), data_size );
    }

    return BSON_OK;
}