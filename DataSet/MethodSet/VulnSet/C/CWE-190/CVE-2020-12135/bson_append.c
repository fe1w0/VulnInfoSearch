static void bson_append( bson *b, const void *data, int len ) {
    memcpy( b->cur , data , len );
    b->cur += len;
}