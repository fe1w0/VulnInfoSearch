int bson_check_field_name( bson *b, const char *string,
                           const size_t length ) {

    return bson_validate_string( b, ( const unsigned char * )string, length, 1, 1, 1 );
}