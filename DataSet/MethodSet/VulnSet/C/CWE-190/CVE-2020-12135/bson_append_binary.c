MONGO_EXPORT int bson_append_binary( bson *b, const char *name, char type, const char *str, int len ) {
    if ( type == BSON_BIN_BINARY_OLD ) {
        int subtwolen = len + 4;
        if ( bson_append_estart( b, BSON_BINDATA, name, 4+1+4+len ) == BSON_ERROR )
            return BSON_ERROR;
        bson_append32( b, &subtwolen );
        bson_append_byte( b, type );
        bson_append32( b, &len );
        bson_append( b, str, len );
    }
    else {
        if ( bson_append_estart( b, BSON_BINDATA, name, 4+1+len ) == BSON_ERROR )
            return BSON_ERROR;
        bson_append32( b, &len );
        bson_append_byte( b, type );
        bson_append( b, str, len );
    }
    return BSON_OK;
}