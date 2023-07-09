static int parse_multipart(
        ogs_sbi_message_t *message, ogs_sbi_http_message_t *http)
{
    char *boundary = NULL;
    int i;

    multipart_parser_settings settings;
    multipart_parser_data_t data;

    multipart_parser *parser = NULL;

    ogs_assert(message);
    ogs_assert(http);

    memset(&settings, 0, sizeof(settings));
    settings.on_header_field = &on_header_field;
    settings.on_header_value = &on_header_value;
    settings.on_part_data = &on_part_data;
    settings.on_part_data_end = &on_part_data_end;

    for (i = 0; i < http->content_length; i++) {
        if (http->content[i] == '\r' && http->content[i+1] == '\n')
            break;
    }

    if (i >= http->content_length) {
        ogs_error("Invalid HTTP content [%d]", i);
        ogs_log_hexdump(OGS_LOG_ERROR,
                (unsigned char *)http->content, http->content_length);
        return OGS_ERROR;
    }

    boundary = ogs_strndup(http->content, i);
    ogs_assert(boundary);

    parser = multipart_parser_init(boundary, &settings);
    ogs_assert(parser);

    memset(&data, 0, sizeof(data));
    multipart_parser_set_data(parser, &data);
    multipart_parser_execute(parser, http->content, http->content_length);

    multipart_parser_free(parser);
    ogs_free(boundary);

    for (i = 0; i < data.num_of_part; i++) {
        SWITCH(data.part[i].content_type)
        CASE(OGS_SBI_CONTENT_JSON_TYPE)
            parse_json(message,
                    data.part[i].content_type, data.part[i].content);

            if (data.part[i].content_id)
                ogs_free(data.part[i].content_id);
            if (data.part[i].content_type)
                ogs_free(data.part[i].content_type);
            if (data.part[i].content)
                ogs_free(data.part[i].content);

            break;

        CASE(OGS_SBI_CONTENT_5GNAS_TYPE)
        CASE(OGS_SBI_CONTENT_NGAP_TYPE)
            http->part[http->num_of_part].content_id =
                data.part[i].content_id;
            http->part[http->num_of_part].content_type =
                data.part[i].content_type;
            http->part[http->num_of_part].pkbuf =
                ogs_pkbuf_alloc(NULL, data.part[i].content_length);
            ogs_expect_or_return_val(
                http->part[http->num_of_part].pkbuf, OGS_ERROR);
            ogs_pkbuf_put_data(http->part[http->num_of_part].pkbuf,
                data.part[i].content, data.part[i].content_length);

            message->part[message->num_of_part].content_id =
                http->part[http->num_of_part].content_id;
            message->part[message->num_of_part].content_type =
                http->part[http->num_of_part].content_type;
            message->part[message->num_of_part].pkbuf =
                ogs_pkbuf_copy(http->part[http->num_of_part].pkbuf);
            ogs_expect_or_return_val(
                message->part[message->num_of_part].pkbuf, OGS_ERROR);

            http->num_of_part++;
            message->num_of_part++;

            if (data.part[i].content)
                ogs_free(data.part[i].content);
            break;

        DEFAULT
            ogs_error("Unknown content-type[%s]", data.part[i].content_type);
        END
    }

    if (data.part[i].content_id)
        ogs_free(data.part[i].content_id);
    if (data.part[i].content_type)
        ogs_free(data.part[i].content_type);

    if (data.header_field)
        ogs_free(data.header_field);

    return OGS_OK;
}