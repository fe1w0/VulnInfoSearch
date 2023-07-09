static int on_part_data(
        multipart_parser *parser, const char *at, size_t length)
{
    multipart_parser_data_t *data = NULL;

    ogs_assert(parser);
    data = multipart_parser_get_data(parser);
    ogs_assert(data);

    if (at && length) {
        SWITCH(data->part[data->num_of_part].content_type)
        CASE(OGS_SBI_CONTENT_JSON_TYPE)
        CASE(OGS_SBI_CONTENT_5GNAS_TYPE)
        CASE(OGS_SBI_CONTENT_NGAP_TYPE)
            size_t offset = 0;

            if (data->part[data->num_of_part].content == NULL) {
                data->part[data->num_of_part].content_length = length;
                data->part[data->num_of_part].content =
                    (char *)ogs_malloc(length + 1);
                ogs_assert(data->part[data->num_of_part].content);
            } else {
                offset = data->part[data->num_of_part].content_length;
                if ((data->part[data->num_of_part].content_length + length) >
                        OGS_HUGE_LEN) {
                    ogs_error("Overflow length [%d:%d]",
                        (int)data->part[data->num_of_part].content_length,
                        (int)length);
                    ogs_assert_if_reached();
                    return 0;
                }
                data->part[data->num_of_part].content_length += length;
                data->part[data->num_of_part].content = (char *)ogs_realloc(
                    data->part[data->num_of_part].content,
                    data->part[data->num_of_part].content_length + 1);
                ogs_assert(data->part[data->num_of_part].content);
            }
            memcpy(data->part[data->num_of_part].content + offset, at, length);
            data->part[data->num_of_part].content[
                data->part[data->num_of_part].content_length] = 0;
            break;

        DEFAULT
            ogs_log_hexdump(OGS_LOG_FATAL, (unsigned char *)at, length);
            ogs_error("Unknown content_type [%s]",
                    data->part[data->num_of_part].content_type);
        END
    }
    return 0;
}