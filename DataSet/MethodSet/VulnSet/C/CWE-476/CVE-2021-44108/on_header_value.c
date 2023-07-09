static int on_header_value(
        multipart_parser *parser, const char *at, size_t length)
{
    multipart_parser_data_t *data = NULL;

    ogs_assert(parser);
    data = multipart_parser_get_data(parser);
    ogs_assert(data);

    if (at && length) {
        SWITCH(data->header_field)
        CASE(OGS_SBI_CONTENT_TYPE)
            if (data->part[data->num_of_part].content_type)
                ogs_free(data->part[data->num_of_part].content_type);
            data->part[data->num_of_part].content_type =
                ogs_strndup(at, length);
            ogs_assert(data->part[data->num_of_part].content_type);
            break;
        CASE(OGS_SBI_CONTENT_ID)
            if (data->part[data->num_of_part].content_id)
                ogs_free(data->part[data->num_of_part].content_id);
            data->part[data->num_of_part].content_id =
                ogs_strndup(at, length);
            ogs_assert(data->part[data->num_of_part].content_id);
            break;

        DEFAULT
            ogs_error("Unknown header field [%s]", data->header_field);
        END
    }

    return 0;
}