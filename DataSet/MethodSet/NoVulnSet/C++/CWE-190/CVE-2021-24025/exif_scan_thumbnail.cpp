static int exif_scan_thumbnail(image_info_type *ImageInfo) {
  unsigned char c, *data = (unsigned char*)ImageInfo->Thumbnail.data;
  int n, marker;
  size_t length=2, pos=0;
  jpeg_sof_info sof_info;

  if (!data || ImageInfo->Thumbnail.size < 4) {
    return 0; /* nothing to do here */
  }
  if (memcmp(data, "\xFF\xD8\xFF", 3)) {
    if (!ImageInfo->Thumbnail.width && !ImageInfo->Thumbnail.height) {
      raise_warning("Thumbnail is not a JPEG image");
    }
    return 0;
  }
  for (;;) {
    pos += length;
    if (pos>=ImageInfo->Thumbnail.size)
      return 0;
    c = data[pos++];
    if (pos>=ImageInfo->Thumbnail.size)
      return 0;
    if (c != 0xFF) {
      return 0;
    }
    n = 8;
    while ((c = data[pos++]) == 0xFF && n--) {
      if (pos+3>=ImageInfo->Thumbnail.size)
        return 0;
      /* +3 = pos++ of next check when reaching marker + 2 bytes for length */
    }
    if (c == 0xFF)
      return 0;
    marker = c;
    if (ImageInfo->Thumbnail.size - 2 < pos) return 0;
    length = php_jpg_get16(data+pos);
    if (length > ImageInfo->Thumbnail.size || pos >= ImageInfo->Thumbnail.size - length) {
      return 0;
    }
    switch (marker) {
      case M_SOF0:
      case M_SOF1:
      case M_SOF2:
      case M_SOF3:
      case M_SOF5:
      case M_SOF6:
      case M_SOF7:
      case M_SOF9:
      case M_SOF10:
      case M_SOF11:
      case M_SOF13:
      case M_SOF14:
      case M_SOF15:
        /* handle SOFn block */
        if (length < 8 || ImageInfo->Thumbnail.size - 8 < pos) {
          /* exif_process_SOFn needs 8 bytes */
          return 0;
        }
        exif_process_SOFn(data+pos, marker, &sof_info);
        ImageInfo->Thumbnail.height   = sof_info.height;
        ImageInfo->Thumbnail.width    = sof_info.width;
        return 1;

      case M_SOS:
      case M_EOI:
        raise_warning("Could not compute size of thumbnail");
        return 0;
        break;

      default:
        /* just skip */
        break;
    }
  }

  raise_warning("Could not compute size of thumbnail");
  return 0;
}