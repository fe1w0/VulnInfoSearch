MONGO_EXPORT int bson_append_string_n( bson *b, const char *name, const char *value, size_t len ) {
    return bson_append_string_base( b, name, value, len, BSON_STRING );
}