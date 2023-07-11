MONGO_EXPORT int bson_append_code_w_scope_n( bson *b, const char *name,
        const char *code, int len, const bson *scope ) {

    int sl, size;
    if ( !scope ) return BSON_ERROR;
    sl = len + 1;
    size = 4 + 4 + sl + bson_size( scope );
    if ( bson_append_estart( b, BSON_CODEWSCOPE, name, size ) == BSON_ERROR )
        return BSON_ERROR;
    bson_append32( b, &size );
    bson_append32( b, &sl );
    bson_append( b, code, sl );
    bson_append( b, scope->data, bson_size( scope ) );
    return BSON_OK;
}