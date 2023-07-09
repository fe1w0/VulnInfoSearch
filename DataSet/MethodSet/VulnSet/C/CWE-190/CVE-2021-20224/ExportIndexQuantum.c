static void ExportIndexQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const Quantum *magick_restrict p,
  unsigned char *magick_restrict q,ExceptionInfo *exception)
{
  ssize_t
    x;

  ssize_t
    bit;

  if (image->storage_class != PseudoClass)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
        "ColormappedImageRequired","`%s'",image->filename);
      return;
    }
  switch (quantum_info->depth)
  {
    case 1:
    {
      unsigned char
        pixel;

      for (x=((ssize_t) number_pixels-7); x > 0; x-=8)
      {
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q=((pixel & 0x01) << 7);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 6);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 5);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 4);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 3);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 2);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 1);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 0);
        p+=GetPixelChannels(image);
        q++;
      }
      if ((number_pixels % 8) != 0)
        {
          *q='\0';
          for (bit=7; bit >= (ssize_t) (8-(number_pixels % 8)); bit--)
          {
            pixel=(unsigned char) GetPixelIndex(image,p);
            *q|=((pixel & 0x01) << (unsigned char) bit);
            p+=GetPixelChannels(image);
          }
          q++;
        }
      break;
    }
    case 4:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) (number_pixels-1) ; x+=2)
      {
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q=((pixel & 0xf) << 4);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0xf) << 0);
        p+=GetPixelChannels(image);
        q++;
      }
      if ((number_pixels % 2) != 0)
        {
          pixel=(unsigned char) GetPixelIndex(image,p);
          *q=((pixel & 0xf) << 4);
          p+=GetPixelChannels(image);
          q++;
        }
      break;
    }
    case 8:
    {
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopCharPixel((unsigned char) GetPixelIndex(image,p),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 16:
    {
      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopShortPixel(quantum_info->endian,SinglePrecisionToHalf(
              QuantumScale*GetPixelIndex(image,p)),q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopShortPixel(quantum_info->endian,(unsigned short)
          GetPixelIndex(image,p),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 32:
    {
      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopFloatPixel(quantum_info,(float) GetPixelIndex(image,p),q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopLongPixel(quantum_info->endian,(unsigned int)
          GetPixelIndex(image,p),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 64:
    {
      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopDoublePixel(quantum_info,(double) GetPixelIndex(image,p),q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
    }
    default:
    {
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,GetPixelIndex(image,p),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
  }
}