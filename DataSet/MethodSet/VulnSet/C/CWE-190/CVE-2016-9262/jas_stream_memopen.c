jas_stream_t *jas_stream_memopen(char *buf, int bufsize)
{
	jas_stream_t *stream;
	jas_stream_memobj_t *obj;

	JAS_DBGLOG(100, ("jas_stream_memopen(%p, %d)\n", buf, bufsize));

	if (!(stream = jas_stream_create())) {
		return 0;
	}

	/* A stream associated with a memory buffer is always opened
	for both reading and writing in binary mode. */
	stream->openmode_ = JAS_STREAM_READ | JAS_STREAM_WRITE | JAS_STREAM_BINARY;

	/* Since the stream data is already resident in memory, buffering
	is not necessary. */
	/* But... It still may be faster to use buffering anyways. */
	jas_stream_initbuf(stream, JAS_STREAM_FULLBUF, 0, 0);

	/* Select the operations for a memory stream. */
	stream->ops_ = &jas_stream_memops;

	/* Allocate memory for the underlying memory stream object. */
	if (!(obj = jas_malloc(sizeof(jas_stream_memobj_t)))) {
		jas_stream_destroy(stream);
		return 0;
	}
	stream->obj_ = (void *) obj;

	/* Initialize a few important members of the memory stream object. */
	obj->myalloc_ = 0;
	obj->buf_ = 0;

	/* If the buffer size specified is nonpositive, then the buffer
	is allocated internally and automatically grown as needed. */
	if (bufsize <= 0) {
		obj->bufsize_ = 1024;
		obj->growable_ = 1;
	} else {
		obj->bufsize_ = bufsize;
		obj->growable_ = 0;
	}
	if (buf) {
		obj->buf_ = (unsigned char *) buf;
	} else {
		obj->buf_ = jas_malloc(obj->bufsize_);
		obj->myalloc_ = 1;
	}
	if (!obj->buf_) {
		jas_stream_close(stream);
		return 0;
	}
	JAS_DBGLOG(100, ("jas_stream_memopen buffer buf=%p myalloc=%d\n",
	  obj->buf_, obj->myalloc_));

	if (bufsize > 0 && buf) {
		/* If a buffer was supplied by the caller and its length is positive,
		  make the associated buffer data appear in the stream initially. */
		obj->len_ = bufsize;
	} else {
		/* The stream is initially empty. */
		obj->len_ = 0;
	}
	obj->pos_ = 0;
	
	return stream;
}