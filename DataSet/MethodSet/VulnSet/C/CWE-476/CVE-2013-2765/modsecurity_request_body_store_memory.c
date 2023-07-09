static apr_status_t modsecurity_request_body_store_memory(modsec_rec *msr,
    const char *data, apr_size_t length, char **error_msg)
{
    *error_msg = NULL;

    /* Would storing this chunk mean going over the limit? */
    if ((msr->msc_reqbody_spilltodisk)
        && (msr->msc_reqbody_length + length > (apr_size_t)msr->txcfg->reqbody_inmemory_limit))
    {
        msc_data_chunk **chunks;
        unsigned int disklen = 0;
        int i;

        msr_log(msr, 4, "Input filter: Request too large to store in memory, switching to disk.");

        /* NOTE Must use modsecurity_request_body_store_disk() here
         *      to prevent data to be sent to the streaming
         *      processors again.
         */

        /* Initialise disk storage */
        msr->msc_reqbody_storage = MSC_REQBODY_DISK;
        if (modsecurity_request_body_start_init(msr, error_msg) < 0) return -1;

        /* Write the data we keep in memory */
        chunks = (msc_data_chunk **)msr->msc_reqbody_chunks->elts;
        for(i = 0; i < msr->msc_reqbody_chunks->nelts; i++) {
            disklen += chunks[i]->length;

            if (modsecurity_request_body_store_disk(msr, chunks[i]->data, chunks[i]->length, error_msg) < 0) {
                return -1;
            }

            free(chunks[i]->data);
            chunks[i]->data = NULL;
        }

        /* Clear the memory pool as we no longer need the bits. */

        /* IMP1 But since we only used apr_pool_clear memory might
         * not be released back to the OS straight away?
         */
        msr->msc_reqbody_chunks = NULL;
        apr_pool_clear(msr->msc_reqbody_mp);

        msr_log(msr, 4, "Input filter: Wrote %u bytes from memory to disk.", disklen);

        /* Continue with disk storage from now on */
        return modsecurity_request_body_store_disk(msr, data, length, error_msg);
    }

    /* If we're here that means we are not over the
     * request body in-memory limit yet.
     */
    {
        unsigned long int bucket_offset, bucket_left;

        bucket_offset = 0;
        bucket_left = length;

        /* Although we store the request body in chunks we don't
         * want to use the same chunk sizes as the incoming memory
         * buffers. They are often of very small sizes and that
         * would make us waste a lot of memory. That's why we
         * use our own chunks of CHUNK_CAPACITY sizes.
         */

        /* Loop until we empty this bucket into our chunks. */
        while(bucket_left > 0) {
            /* Allocate a new chunk if we have to. */
            if (msr->msc_reqbody_chunk_current == NULL) {
                msr->msc_reqbody_chunk_current = (msc_data_chunk *)
                    apr_pcalloc(msr->msc_reqbody_mp, sizeof(msc_data_chunk));
                if (msr->msc_reqbody_chunk_current == NULL) {
                    *error_msg = apr_psprintf(msr->mp, "Input filter: Failed to allocate %lu bytes "
                        "for request body chunk.", (unsigned long)sizeof(msc_data_chunk));
                    return -1;
                }

                msr->msc_reqbody_chunk_current->data = malloc(CHUNK_CAPACITY);
                if (msr->msc_reqbody_chunk_current->data == NULL) {
                    *error_msg = apr_psprintf(msr->mp, "Input filter: Failed to allocate %d bytes "
                        "for request body chunk data.", CHUNK_CAPACITY);
                    return -1;
                }

                msr->msc_reqbody_chunk_current->length = 0;
                msr->msc_reqbody_chunk_current->is_permanent = 1;

                *(const msc_data_chunk **)apr_array_push(msr->msc_reqbody_chunks)
                    = msr->msc_reqbody_chunk_current;
            }

            if (bucket_left < (CHUNK_CAPACITY - msr->msc_reqbody_chunk_current->length)) {
                /* There's enough space in the current chunk. */
                memcpy(msr->msc_reqbody_chunk_current->data +
                    msr->msc_reqbody_chunk_current->length, data + bucket_offset, bucket_left);
                msr->msc_reqbody_chunk_current->length += bucket_left;
                bucket_left = 0;
            } else {
                /* Fill the existing chunk. */
                unsigned long int copy_length = CHUNK_CAPACITY -
                    msr->msc_reqbody_chunk_current->length;

                memcpy(msr->msc_reqbody_chunk_current->data +
                    msr->msc_reqbody_chunk_current->length, data + bucket_offset, copy_length);
                bucket_offset += copy_length;
                bucket_left -= copy_length;
                msr->msc_reqbody_chunk_current->length += copy_length;

                /* We're done with this chunk. Setting the pointer
                 * to NULL is going to force a new chunk to be allocated
                 * on the next go.
                 */
                msr->msc_reqbody_chunk_current = NULL;
            }
        }

        msr->msc_reqbody_length += length;
    }

    return 1;
}