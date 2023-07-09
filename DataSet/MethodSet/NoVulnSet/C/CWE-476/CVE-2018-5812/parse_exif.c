void CLASS parse_exif (int base)
{
  unsigned kodak, entries, tag, type, len, save, c;
  double expo,ape;

  kodak = !strncmp(make,"EASTMAN",7) && tiff_nifds < 3;
  entries = get2();
  if(!strncmp(make,"Hasselblad",10) && (tiff_nifds > 3) && (entries > 512)) return;
#ifdef LIBRAW_LIBRARY_BUILD
  INT64 fsize = ifp->size();
#endif
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);

#ifdef LIBRAW_LIBRARY_BUILD
    INT64 savepos = ftell(ifp);
    if(len > 8 && savepos + len > fsize*2) continue;
    if(callbacks.exif_cb)
      {
        callbacks.exif_cb(callbacks.exifparser_data,tag,type,len,order,ifp);
        fseek(ifp,savepos,SEEK_SET);
      }
#endif
    switch (tag) {
#ifdef LIBRAW_LIBRARY_BUILD
    case 0xa405:		// FocalLengthIn35mmFormat
      imgdata.lens.FocalLengthIn35mmFormat = get2();
      break;
    case 0xa431:		// BodySerialNumber
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
      break;
    case 0xa432:		// LensInfo, 42034dec, Lens Specification per EXIF standard
      imgdata.lens.MinFocal = getreal(type);
      imgdata.lens.MaxFocal = getreal(type);
      imgdata.lens.MaxAp4MinFocal = getreal(type);
      imgdata.lens.MaxAp4MaxFocal = getreal(type);
      break;
    case 0xa435:		// LensSerialNumber
      stmread(imgdata.lens.LensSerial, len, ifp);
      break;
    case 0xc630:		// DNG LensInfo, Lens Specification per EXIF standard
      imgdata.lens.dng.MinFocal = getreal(type);
      imgdata.lens.dng.MaxFocal = getreal(type);
      imgdata.lens.dng.MaxAp4MinFocal = getreal(type);
      imgdata.lens.dng.MaxAp4MaxFocal = getreal(type);
      break;
    case 0xa433:		// LensMake
      stmread(imgdata.lens.LensMake, len, ifp);
      break;
    case 0xa434:		// LensModel
      stmread(imgdata.lens.Lens, len, ifp);
      if (!strncmp(imgdata.lens.Lens, "----", 4))
        imgdata.lens.Lens[0] = 0;
      break;
    case 0x9205:
      imgdata.lens.EXIF_MaxAp = libraw_powf64(2.0f, (getreal(type) / 2.0f));
      break;
#endif
      case 33434:  tiff_ifd[tiff_nifds-1].t_shutter =
		   shutter = getreal(type);		break;
      case 33437:  aperture = getreal(type);		break;  // 0x829d FNumber
      case 34855:  iso_speed = get2();			break;
      case 34866:
        if (iso_speed == 0xffff && (!strncasecmp(make, "SONY",4) || !strncasecmp(make, "CANON",5)))
          iso_speed = getreal(type);
        break;
      case 36867:
      case 36868:  get_timestamp(0);			break;
      case 37377:  if ((expo = -getreal(type)) < 128 && shutter == 0.)
		     tiff_ifd[tiff_nifds-1].t_shutter =
			shutter = libraw_powf64(2.0, expo);
		break;
      case 37378:                                       // 0x9202 ApertureValue
        if ((fabs(ape = getreal(type))<256.0) && (!aperture))
          aperture = libraw_powf64(2.0, ape/2);
        break;
      case 37385:  flash_used = getreal(type);          break;
      case 37386:  focal_len = getreal(type);		break;
      case 37500:  	                         // tag 0x927c
#ifdef LIBRAW_LIBRARY_BUILD
       if (((make[0] == '\0') && (!strncmp(model, "ov5647",6))) ||
           ((!strncmp(make, "RaspberryPi",11)) && (!strncmp(model, "RP_OV5647",9))) ||
           ((!strncmp(make, "RaspberryPi",11)) && (!strncmp(model, "RP_imx219",9)))) {
        char mn_text[512];
        char *pos;
        char ccms[512];
        ushort l;
        float num;

	fgets(mn_text, MIN(len,511), ifp);
        mn_text[511] = 0;

        pos = strstr(mn_text, "gain_r=");
        if (pos)
          cam_mul[0] = atof(pos + 7);
        pos = strstr(mn_text, "gain_b=");
        if (pos)
          cam_mul[2] = atof(pos + 7);
        if ((cam_mul[0] > 0.001f) && (cam_mul[2] > 0.001f))
          cam_mul[1] = cam_mul[3] = 1.0f;
        else
          cam_mul[0] = cam_mul[2] = 0.0f;

        pos = strstr(mn_text, "ccm=");
        if(pos)
        {
         pos +=4;
         char *pos2 = strstr(pos, " ");
         if(pos2)
         {
           l = pos2 - pos;
           memcpy(ccms, pos, l);
           ccms[l] = '\0';
#if defined WIN32 || defined(__MINGW32__)
           // Win32 strtok is already thread-safe
          pos = strtok(ccms, ",");
#else
          char *last=0;
          pos = strtok_r(ccms, ",",&last);
#endif
          if(pos)
          {
            for (l = 0; l < 4; l++)
            {
              num = 0.0;
              for (c = 0; c < 3; c++)
              {
                imgdata.color.ccm[l][c] = (float)atoi(pos);
                num += imgdata.color.ccm[l][c];
#if defined WIN32 || defined(__MINGW32__)
                pos = strtok(NULL, ",");
#else
                pos = strtok_r(NULL, ",",&last);
#endif
                if(!pos) goto end; // broken
              }
              if (num > 0.01)
                FORC3 imgdata.color.ccm[l][c] = imgdata.color.ccm[l][c] / num;
            }
          }
        }
       }
      end:;
      }
      else
#endif
        parse_makernote (base, 0);
       break;
      case 40962:  if (kodak) raw_width  = get4();	break;
      case 40963:  if (kodak) raw_height = get4();	break;
      case 41730:
	if (get4() == 0x20002)
	  for (exif_cfa=c=0; c < 8; c+=2)
	    exif_cfa |= fgetc(ifp) * 0x01010101 << c;
    }
    fseek (ifp, save, SEEK_SET);
  }
}