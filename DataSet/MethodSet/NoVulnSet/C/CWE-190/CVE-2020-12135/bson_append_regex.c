MONGO_EXPORT int bson_append_regex( bson *b, const char *name, const char *pattern, const char *opts ) {
    const size_t plen = strlen( pattern )+1;
    const size_t olen = strlen( opts )+1;
    if ( bson_append_estart( b, BSON_REGEX, name, plen + olen ) == BSON_ERROR )
        return BSON_ERROR;
    if ( bson_check_string( b, pattern, plen - 1 ) == BSON_ERROR )
        return BSON_ERROR;
    bson_append( b , pattern , plen );
    bson_append( b , opts , olen );
    return BSON_OK;
}