static int8_t parse_ext_option(uint16_t *dst, uint8_t **packet_data_pptr, uint8_t *packet_data_start_ptr, uint16_t packet_len, uint16_t *message_left)
{
    uint16_t option_number = *dst;

    if (option_number == 13) {
        uint8_t option_ext;
        int8_t read_result = sn_coap_parser_read_packet_u8(&option_ext,
                                                            *packet_data_pptr,
                                                            packet_data_start_ptr,
                                                            packet_len);
        if (read_result != 0) {
            /* packet_data_pptr would overflow! */
            tr_error("sn_coap_parser_options_parse - **packet_data_pptr overflow !");
            return -1;
        }
        else {
            if(sn_coap_parser_add_u16_limit(option_number, option_ext, &option_number) != 0)
            {
                return -1;
            }

            *message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr,
                                                            packet_data_start_ptr,
                                                            packet_len,
                                                            1);
        }
    } else if (option_number == 14) {
            int8_t read_result = sn_coap_parser_read_packet_u16(&option_number,
                                                                *packet_data_pptr,
                                                                packet_data_start_ptr,
                                                                packet_len);
            if (read_result != 0) {
                /* packet_data_pptr would overflow! */
                tr_error("sn_coap_parser_options_parse - **packet_data_pptr overflow !");
                return -1;
            }
            else {
                if(sn_coap_parser_add_u16_limit(option_number, 269, &option_number) != 0)
                {
                    return -1;
                }

                *message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr,
                                                            packet_data_start_ptr,
                                                            packet_len,
                                                            2);
            }
    }
    /* Option number 15 reserved for payload marker. This is handled as a error! */
    else if (option_number == 15) {
        tr_error("sn_coap_parser_options_parse - invalid option number(15)!");
        return -1;
    }

    *dst = option_number;
    return 0;
}