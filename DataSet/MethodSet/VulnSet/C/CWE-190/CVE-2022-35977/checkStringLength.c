static int checkStringLength(client *c, long long size) {
    if (!mustObeyClient(c) && size > server.proto_max_bulk_len) {
        addReplyError(c,"string exceeds maximum allowed size (proto-max-bulk-len)");
        return C_ERR;
    }
    return C_OK;
}