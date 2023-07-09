static int on_part_data_end(multipart_parser *parser)
{
    multipart_parser_data_t *data = NULL;

    ogs_assert(parser);
    data = multipart_parser_get_data(parser);
    ogs_assert(data);

    if (data->num_of_part < OGS_SBI_MAX_NUM_OF_PART) {
        data->num_of_part++;
    }

    return 0;
}