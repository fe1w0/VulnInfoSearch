MONGO_EXPORT size_t bson_buffer_size( const bson *b ) {
    return (b->cur - b->data + 1);
}