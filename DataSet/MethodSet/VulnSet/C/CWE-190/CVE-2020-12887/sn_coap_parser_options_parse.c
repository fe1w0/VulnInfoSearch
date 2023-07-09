static int8_t sn_coap_parser_options_parse(struct coap_s *handle, uint8_t **packet_data_pptr, sn_coap_hdr_s *dst_coap_msg_ptr, uint8_t *packet_data_start_ptr, uint16_t packet_len)
{
    uint8_t previous_option_number = 0;
    int8_t  ret_status             = 0;
    uint16_t message_left          = sn_coap_parser_move_packet_ptr(packet_data_pptr,
                                                                    packet_data_start_ptr,
                                                                    packet_len,
                                                                    0);

    /*  Parse token, if exists  */
    dst_coap_msg_ptr->token_len = *packet_data_start_ptr & COAP_HEADER_TOKEN_LENGTH_MASK;

    if (dst_coap_msg_ptr->token_len) {
        int8_t ptr_check_result;
        if ((dst_coap_msg_ptr->token_len > 8) || dst_coap_msg_ptr->token_ptr) {
            tr_error("sn_coap_parser_options_parse - token not valid!");
            return -1;
        }

        ptr_check_result = sn_coap_parser_check_packet_ptr(*packet_data_pptr, packet_data_start_ptr, packet_len, dst_coap_msg_ptr->token_len);
        if (0 != ptr_check_result) {
            tr_error("sn_coap_parser_options_parse - **packet_data_pptr overflow !");
            return -1;
        }

        dst_coap_msg_ptr->token_ptr = sn_coap_protocol_malloc_copy(handle, *packet_data_pptr, dst_coap_msg_ptr->token_len);

        if (dst_coap_msg_ptr->token_ptr == NULL) {
            tr_error("sn_coap_parser_options_parse - failed to allocate token!");
            return -1;
        }

        message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr,
                                                      packet_data_start_ptr,
                                                      packet_len,
                                                      dst_coap_msg_ptr->token_len);
    }

    /* Loop all Options */
    while (message_left && (**packet_data_pptr != 0xff)) {
        /* Get option length WITHOUT extensions */
        uint16_t option_len = (**packet_data_pptr & 0x0F);
        /* Get option number WITHOUT extensions */
        uint16_t  option_number = (**packet_data_pptr >> COAP_OPTIONS_OPTION_NUMBER_SHIFT);

        message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr, packet_data_start_ptr, packet_len, 1);

        int8_t    option_parse_result;
        /* Add possible option delta extension */
        option_parse_result = parse_ext_option(&option_number,
                                                packet_data_pptr,
                                                packet_data_start_ptr,
                                                packet_len,
                                                &message_left);
        if (option_parse_result != 0) {
            return -1;
        }
        /* Add previous option to option delta and get option number */
        option_number += previous_option_number;

        /* Add possible option length extension to resolve full length of the option */
        option_parse_result = parse_ext_option(&option_len,
                                                packet_data_pptr,
                                                packet_data_start_ptr,
                                                packet_len,
                                                &message_left);
        if (option_parse_result != 0) {
            return -1;
        }

        /* * * Parse option itself * * */
        /* Some options are handled independently in own functions */
        previous_option_number = option_number;
        /* Allocate options_list_ptr if needed */
        switch (option_number) {
            case COAP_OPTION_MAX_AGE:
            case COAP_OPTION_PROXY_URI:
            case COAP_OPTION_ETAG:
            case COAP_OPTION_URI_HOST:
            case COAP_OPTION_LOCATION_PATH:
            case COAP_OPTION_URI_PORT:
            case COAP_OPTION_LOCATION_QUERY:
            case COAP_OPTION_OBSERVE:
            case COAP_OPTION_URI_QUERY:
            case COAP_OPTION_BLOCK2:
            case COAP_OPTION_BLOCK1:
            case COAP_OPTION_ACCEPT:
            case COAP_OPTION_SIZE1:
            case COAP_OPTION_SIZE2:
                if (sn_coap_parser_alloc_options(handle, dst_coap_msg_ptr) == NULL) {
                    tr_error("sn_coap_parser_options_parse - failed to allocate options!");
                    return -1;
                }
                break;
        }

        if (message_left < option_len){
            /* packet_data_pptr would overflow! */
            tr_error("sn_coap_parser_options_parse - **packet_data_pptr would overflow when parsing options!");
            return -1;
        }

        /* Parse option */
        switch (option_number) {
            case COAP_OPTION_CONTENT_FORMAT:
                if ((option_len > 2) || (dst_coap_msg_ptr->content_format != COAP_CT_NONE)) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_CONTENT_FORMAT not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->content_format = (sn_coap_content_format_e) sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            case COAP_OPTION_MAX_AGE:
                if (option_len > 4) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_MAX_AGE not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->max_age = sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            case COAP_OPTION_PROXY_URI:
                if ((option_len > 1034) || (option_len < 1) || dst_coap_msg_ptr->options_list_ptr->proxy_uri_ptr) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_PROXY_URI not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->proxy_uri_len = option_len;
                dst_coap_msg_ptr->options_list_ptr->proxy_uri_ptr = sn_coap_protocol_malloc_copy(handle, *packet_data_pptr, option_len);

                if (dst_coap_msg_ptr->options_list_ptr->proxy_uri_ptr == NULL) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_PROXY_URI allocation failed!");
                    return -1;
                }
                message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr, packet_data_start_ptr, packet_len, option_len);
                break;

            case COAP_OPTION_ETAG:
                /* This is managed independently because User gives this option in one character table */
                ret_status = sn_coap_parser_options_parse_multiple_options(handle, packet_data_pptr,
                             message_left,
                             &dst_coap_msg_ptr->options_list_ptr->etag_ptr,
                             (uint16_t *)&dst_coap_msg_ptr->options_list_ptr->etag_len,
                             COAP_OPTION_ETAG, option_len);
                if (ret_status < 0) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_ETAG not valid!");
                    return -1;
                }
                break;

            case COAP_OPTION_URI_HOST:
                if ((option_len > 255) || (option_len < 1) || dst_coap_msg_ptr->options_list_ptr->uri_host_ptr) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_URI_HOST not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->uri_host_len = option_len;
                dst_coap_msg_ptr->options_list_ptr->uri_host_ptr = sn_coap_protocol_malloc_copy(handle, *packet_data_pptr, option_len);

                if (dst_coap_msg_ptr->options_list_ptr->uri_host_ptr == NULL) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_URI_HOST allocation failed!");
                    return -1;
                }
                message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr, packet_data_start_ptr, packet_len, option_len);
                break;

            case COAP_OPTION_LOCATION_PATH:
                if (dst_coap_msg_ptr->options_list_ptr->location_path_ptr) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_LOCATION_PATH exists!");
                    return -1;
                }
                /* This is managed independently because User gives this option in one character table */
                ret_status = sn_coap_parser_options_parse_multiple_options(handle, packet_data_pptr, message_left,
                             &dst_coap_msg_ptr->options_list_ptr->location_path_ptr, &dst_coap_msg_ptr->options_list_ptr->location_path_len,
                             COAP_OPTION_LOCATION_PATH, option_len);
                if (ret_status <0) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_LOCATION_PATH not valid!");
                    return -1;
                }
                break;

            case COAP_OPTION_URI_PORT:
                if ((option_len > 2) || dst_coap_msg_ptr->options_list_ptr->uri_port != COAP_OPTION_URI_PORT_NONE) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_URI_PORT not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->uri_port = sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            case COAP_OPTION_LOCATION_QUERY:
                ret_status = sn_coap_parser_options_parse_multiple_options(handle, packet_data_pptr, message_left,
                             &dst_coap_msg_ptr->options_list_ptr->location_query_ptr, &dst_coap_msg_ptr->options_list_ptr->location_query_len,
                             COAP_OPTION_LOCATION_QUERY, option_len);
                if (ret_status < 0) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_LOCATION_QUERY not valid!");
                    return -1;
                }

                break;

            case COAP_OPTION_URI_PATH:
                ret_status = sn_coap_parser_options_parse_multiple_options(handle, packet_data_pptr, message_left,
                             &dst_coap_msg_ptr->uri_path_ptr, &dst_coap_msg_ptr->uri_path_len,
                             COAP_OPTION_URI_PATH, option_len);
                if (ret_status < 0) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_URI_PATH not valid!");
                    return -1;
                }
                break;

            case COAP_OPTION_OBSERVE:
                if ((option_len > 2) || dst_coap_msg_ptr->options_list_ptr->observe != COAP_OBSERVE_NONE) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_OBSERVE not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->observe = sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            case COAP_OPTION_URI_QUERY:
                ret_status = sn_coap_parser_options_parse_multiple_options(handle, packet_data_pptr, message_left,
                             &dst_coap_msg_ptr->options_list_ptr->uri_query_ptr, &dst_coap_msg_ptr->options_list_ptr->uri_query_len,
                             COAP_OPTION_URI_QUERY, option_len);
                if (ret_status < 0) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_URI_QUERY not valid!");
                    return -1;
                }
                break;

            case COAP_OPTION_BLOCK2:
                if ((option_len > 3) || dst_coap_msg_ptr->options_list_ptr->block2 != COAP_OPTION_BLOCK_NONE) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_BLOCK2 not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->block2 = sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            case COAP_OPTION_BLOCK1:
                if ((option_len > 3) || dst_coap_msg_ptr->options_list_ptr->block1 != COAP_OPTION_BLOCK_NONE) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_BLOCK1 not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->block1 = sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            case COAP_OPTION_ACCEPT:
                if ((option_len > 2) || (dst_coap_msg_ptr->options_list_ptr->accept != COAP_CT_NONE)) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_ACCEPT not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->accept = (sn_coap_content_format_e) sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            case COAP_OPTION_SIZE1:
                if ((option_len > 4) || dst_coap_msg_ptr->options_list_ptr->use_size1) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_SIZE1 not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->use_size1 = true;
                dst_coap_msg_ptr->options_list_ptr->size1 = sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            case COAP_OPTION_SIZE2:
                if ((option_len > 4) || dst_coap_msg_ptr->options_list_ptr->use_size2) {
                    tr_error("sn_coap_parser_options_parse - COAP_OPTION_SIZE2 not valid!");
                    return -1;
                }
                dst_coap_msg_ptr->options_list_ptr->use_size2 = true;
                dst_coap_msg_ptr->options_list_ptr->size2 = sn_coap_parser_options_parse_uint(packet_data_pptr, option_len);
                break;

            default:
                tr_error("sn_coap_parser_options_parse - unknown option!");
                return -1;
        }

        /* Check for overflow */
        if ((*packet_data_pptr - packet_data_start_ptr) > packet_len) {
            return -1;
        }
        message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr,
                                                      packet_data_start_ptr,
                                                      packet_len,
                                                      0);
    }
    return 0;
}