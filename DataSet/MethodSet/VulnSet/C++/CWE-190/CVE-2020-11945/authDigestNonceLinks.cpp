static int
authDigestNonceLinks(digest_nonce_h * nonce)
{
    if (!nonce)
        return -1;

    return nonce->references;
}