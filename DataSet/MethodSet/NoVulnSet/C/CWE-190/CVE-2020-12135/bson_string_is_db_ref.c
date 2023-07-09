static int bson_string_is_db_ref( const unsigned char *string, const size_t length ) {
    int result = 0;

    if( length >= 4 ) {
        if( string[1] == 'r' && string[2] == 'e' && string[3] == 'f' )
            result = 1;
    }
    else if( length >= 3 ) {
        if( string[1] == 'i' && string[2] == 'd' )
            result = 1;
        else if( string[1] == 'd' && string[2] == 'b' )
            result = 1;
    }

    return result;
}