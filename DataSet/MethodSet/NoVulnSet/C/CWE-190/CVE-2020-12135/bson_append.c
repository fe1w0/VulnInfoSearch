static void bson_append( bson *b, const void *data, size_t len ) {
    memcpy( b->cur , data , len );
    b->cur += len;
}