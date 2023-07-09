void CLASS leaf_hdr_load_raw()
{
  ushort *pixel=0;
  unsigned tile=0, r, c, row, col;

  if (!filters) {
    pixel = (ushort *) calloc (raw_width, sizeof *pixel);
    merror (pixel, "leaf_hdr_load_raw()");
  }
#ifdef LIBRAW_LIBRARY_BUILD
  try {
#endif
  FORC(tiff_samples)
    for (r=0; r < raw_height; r++) {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
      if (r % tile_length == 0) {
	fseek (ifp, data_offset + 4*tile++, SEEK_SET);
	fseek (ifp, get4(), SEEK_SET);
      }
      if (filters && c != shot_select) continue;
      if (filters) pixel = raw_image + r*raw_width;
      read_shorts (pixel, raw_width);
      if (!filters && (row = r - top_margin) < height)
	for (col=0; col < width; col++)
	  image[row*width+col][c] = pixel[col+left_margin];
    }
#ifdef LIBRAW_LIBRARY_BUILD
  } catch (...) {
    if(!filters) free(pixel);
    throw;
  }
#endif
  if (!filters) {
    maximum = 0xffff;
    raw_color = 1;
    free (pixel);
  }
}