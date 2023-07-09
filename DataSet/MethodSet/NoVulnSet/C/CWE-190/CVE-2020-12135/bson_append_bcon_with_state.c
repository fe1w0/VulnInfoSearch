static bcon_error_t bson_append_bcon_with_state(bson *b, const bcon *bc, bcon_state_t start_state) {
    bcon_error_t ret = BCON_OK;
    bcon_state_t state = start_state;
    char *key = 0;
    char *typespec = 0;
    unsigned char doc_stack[DOC_STACK_SIZE];
    size_t doc_stack_pointer = 0;
    size_t array_index = 0;
    size_t array_index_stack[ARRAY_INDEX_STACK_SIZE];
    size_t array_index_stack_pointer = 0;
    char array_index_buffer[ARRAY_INDEX_BUFFER_SIZE]; /* max BSON size */
    int end_of_data;
    const bcon *bcp;
    for (end_of_data = 0, bcp = bc; ret == BCON_OK && !end_of_data; bcp++) {
        bcon bci = *bcp;
        char *s = bci.s;
        switch (state) {
        case State_Element:
            switch (bcon_token(s)) {
            case Token_CloseBrace:
                bson_append_finish_object( b );
                DOC_POP_STATE; /* state = ...; */
                break;
            case Token_End:
                end_of_data = 1;
                break;
            default:
                key = s;
                state = State_DocSpecValue;
                break;
            }
            break;
        case State_DocSpecValue:
            switch (bcon_token(s)) {
            case Token_Typespec:
                typespec = s;
                state = State_DocValue;
                break;
            case Token_OpenBrace:
                bson_append_start_object( b, key );
                DOC_PUSH_STATE(State_Element);
                state = State_Element;
                break;
            case Token_OpenBracket:
                bson_append_start_array( b, key );
                ARRAY_PUSH_RESET_INDEX_STATE(State_Element);
                state = State_ArraySpecValue;
                break;
            case Token_End:
                end_of_data = 1;
                break;
            default:
                ret = bson_bcon_key_value(b, key, typespec, bci);
                state = State_Element;
                break;
            }
            break;
        case State_DocValue:
            ret = bson_bcon_key_value(b, key, typespec, bci);
            state = State_Element;
            typespec = 0;
            break;
        case State_ArraySpecValue:
            switch (bcon_token(s)) {
            case Token_Typespec:
                typespec = s;
                state = State_ArrayValue;
                break;
            case Token_OpenBrace:
                key = ARRAY_KEY_STRING(array_index++);
                bson_append_start_object( b, key );
                DOC_PUSH_STATE(State_ArraySpecValue);
                state = State_Element;
                break;
            case Token_OpenBracket:
                key = ARRAY_KEY_STRING(array_index++);
                bson_append_start_array( b, key );
                ARRAY_PUSH_RESET_INDEX_STATE(State_ArraySpecValue);
                /* state = State_ArraySpecValue; */
                break;
            case Token_CloseBracket:
                bson_append_finish_array( b );
                ARRAY_POP_INDEX_STATE; /* state = ...; */
                break;
            case Token_End:
                end_of_data = 1;
                break;
            default:
                key = ARRAY_KEY_STRING(array_index++);
                ret = bson_bcon_key_value(b, key, typespec, bci);
                /* state = State_ArraySpecValue; */
                break;
            }
            break;
        case State_ArrayValue:
            key = ARRAY_KEY_STRING(array_index++);
            ret = bson_bcon_key_value(b, key, typespec, bci);
            state = State_ArraySpecValue;
            typespec = 0;
            break;
        default: assert(NOT_REACHED); break;
        }
    }
    return state == start_state ? BCON_OK : BCON_DOCUMENT_INCOMPLETE;
}