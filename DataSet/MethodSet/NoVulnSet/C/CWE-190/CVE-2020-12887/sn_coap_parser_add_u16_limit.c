static int8_t sn_coap_parser_add_u16_limit(uint16_t a, uint16_t b, uint16_t *result)
{
    uint16_t c;

    c = a + b;
    if (c < a || c < b)
    {
        return -1;
    }

    *result = c;

    return 0;
}