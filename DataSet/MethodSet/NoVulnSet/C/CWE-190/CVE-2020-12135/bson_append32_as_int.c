static void bson_append32_as_int( bson *b, int data ) {
    bson_little_endian32( b->cur, &data );
    b->cur += 4;
}