MONGO_EXPORT int bson_iterator_int( const bson_iterator *i ) {
    switch ( bson_iterator_type( i ) ) {
    case BSON_INT:
        return bson_iterator_int_raw( i );
    case BSON_LONG:
        return ( int )bson_iterator_long_raw( i );
    case BSON_DOUBLE:
        return bson_iterator_double_raw( i );
    default:
        return 0;
    }
}