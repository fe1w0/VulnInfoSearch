static int bson_validate_string( bson *b, const unsigned char *string,
                                 const size_t length, const char check_utf8, const char check_dot,
                                 const char check_dollar ) {

    size_t position = 0;
    int sequence_length = 1;

    if( check_dollar && string[0] == '$' ) {
        if( !bson_string_is_db_ref( string, length ) )
            b->err |= BSON_FIELD_INIT_DOLLAR;
    }

    while ( position < length ) {
        if ( check_dot && *( string + position ) == '.' ) {
            b->err |= BSON_FIELD_HAS_DOT;
        }

        if ( check_utf8 ) {
            sequence_length = trailingBytesForUTF8[*( string + position )] + 1;
            if ( ( position + sequence_length ) > length ) {
                b->err |= BSON_NOT_UTF8;
                return BSON_ERROR;
            }
            if ( !isLegalUTF8( string + position, sequence_length ) ) {
                b->err |= BSON_NOT_UTF8;
                return BSON_ERROR;
            }
        }
        position += sequence_length;
    }

    return BSON_OK;
}