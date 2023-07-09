static int iwgif_read_image(struct iwgifrcontext *rctx)
{
	int retval=0;
	struct lzwdeccontext d;
	size_t subblocksize;
	int has_local_ct;
	int local_ct_size;

	unsigned int root_codesize;

	// Read image header information
	if(!iwgif_read(rctx,rctx->rbuf,9)) goto done;

	rctx->image_left = (int)iw_get_ui16le(&rctx->rbuf[0]);
	rctx->image_top = (int)iw_get_ui16le(&rctx->rbuf[2]);
	// image_left and _top may be updated in iwgif_init_screen().

	rctx->image_width = (int)iw_get_ui16le(&rctx->rbuf[4]);
	rctx->image_height = (int)iw_get_ui16le(&rctx->rbuf[6]);

	rctx->interlaced = (int)((rctx->rbuf[8]>>6)&0x01);

	has_local_ct = (int)((rctx->rbuf[8]>>7)&0x01);
	if(has_local_ct) {
		local_ct_size = (int)(rctx->rbuf[8]&0x07);
		rctx->colortable.num_entries = 1<<(1+local_ct_size);
	}

	if(has_local_ct) {
		// We only support one image, so we don't need to keep both a global and a
		// local color table. If an image has both, the local table will overwrite
		// the global one.
		if(!iwgif_read_color_table(rctx,&rctx->colortable)) goto done;
	}

	// Make the transparent color transparent.
	if(rctx->has_transparency) {
	    rctx->colortable.entry[rctx->trans_color_index].a = 0;
	}

	// Read LZW code size
	if(!iwgif_read(rctx,rctx->rbuf,1)) goto done;
	root_codesize = (unsigned int)rctx->rbuf[0];

	// The spec does not allow the "minimum code size" to be less than 2.
	// Sizes >=12 are impossible to support.
	// There's no reason for the size to be larger than 8, but the spec
	// does not seem to forbid it.
	if(root_codesize<2 || root_codesize>11) {
		iw_set_error(rctx->ctx,"Invalid LZW minimum code size");
		goto done;
	}

	// The creation of the global "screen" was deferred until now, to wait until
	// we know whether the image has transparency.
	// (And if !rctx->include_screen, to wait until we know the size of the image.)
	if(!iwgif_init_screen(rctx)) goto done;

	rctx->total_npixels = (size_t)rctx->image_width * (size_t)rctx->image_height;

	if(!iwgif_make_row_pointers(rctx)) goto done;

	lzw_init(&d,root_codesize);
	lzw_clear(&d);

	while(1) {
		// Read size of next subblock
		if(!iwgif_read(rctx,rctx->rbuf,1)) goto done;
		subblocksize = (size_t)rctx->rbuf[0];
		if(subblocksize==0) break;

		// Read next subblock
		if(!iwgif_read(rctx,rctx->rbuf,subblocksize)) goto done;
		if(!lzw_process_bytes(rctx,&d,rctx->rbuf,subblocksize)) goto done;

		if(d.eoi_flag) break;

		// Stop if we reached the end of the image. We don't care if we've read an
		// EOI code or not.
		if(rctx->pixels_set >= rctx->total_npixels) break;
	}

	retval=1;

done:
	return retval;
}