MONGO_EXPORT bson_type bson_iterator_next( bson_iterator *i ) {
    size_t ds;

    if ( i->first ) {
        i->first = 0;
        return ( bson_type )( *i->cur );
    }

    switch ( bson_iterator_type( i ) ) {
    case BSON_EOO:
        return BSON_EOO; /* don't advance */
    case BSON_UNDEFINED:
    case BSON_NULL:
        ds = 0;
        break;
    case BSON_BOOL:
        ds = 1;
        break;
    case BSON_INT:
        ds = 4;
        break;
    case BSON_LONG:
    case BSON_DOUBLE:
    case BSON_TIMESTAMP:
    case BSON_DATE:
        ds = 8;
        break;
    case BSON_OID:
        ds = 12;
        break;
    case BSON_STRING:
    case BSON_SYMBOL:
    case BSON_CODE:
        ds = 4 + bson_iterator_int_raw( i );
        break;
    case BSON_BINDATA:
        ds = 5 + bson_iterator_int_raw( i );
        break;
    case BSON_OBJECT:
    case BSON_ARRAY:
    case BSON_CODEWSCOPE:
        ds = bson_iterator_int_raw( i );
        break;
    case BSON_DBREF:
        ds = 4+12 + bson_iterator_int_raw( i );
        break;
    case BSON_REGEX: {
        const char *s = bson_iterator_value( i );
        const char *p = s;
        p += strlen( p )+1;
        p += strlen( p )+1;
        ds = p-s;
        break;
    }

    default: {
        char msg[] = "unknown type: 000000000000";
        bson_numstr( msg+14, ( unsigned )( i->cur[0] ) );
        bson_fatal_msg( 0, msg );
        return 0;
    }
    }

    i->cur += 1 + strlen( i->cur + 1 ) + 1 + ds;

    return ( bson_type )( *i->cur );
}