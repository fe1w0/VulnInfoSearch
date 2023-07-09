static void ExportIndexAlphaQuantum(const Image *image,
  QuantumInfo *quantum_info,const MagickSizeType number_pixels,
  const PixelPacket *magick_restrict p,
  const IndexPacket *magick_restrict indexes,unsigned char *magick_restrict q,
  ExceptionInfo *exception)
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

      for (x=((ssize_t) number_pixels-3); x > 0; x-=4)
      {
        pixel=(unsigned char) ((ssize_t) *indexes++);
        *q=((pixel & 0x01) << 7);
        pixel=(unsigned char) (GetPixelOpacity(p) == (Quantum)
          TransparentOpacity ? 1 : 0);
        *q|=((pixel & 0x01) << 6);
        p++;
        pixel=(unsigned char) ((ssize_t) *indexes++);
        *q|=((pixel & 0x01) << 5);
        pixel=(unsigned char) (GetPixelOpacity(p) == (Quantum)
          TransparentOpacity ? 1 : 0);
        *q|=((pixel & 0x01) << 4);
        p++;
        pixel=(unsigned char) ((ssize_t) *indexes++);
        *q|=((pixel & 0x01) << 3);
        pixel=(unsigned char) (GetPixelOpacity(p) == (Quantum)
          TransparentOpacity ? 1 : 0);
        *q|=((pixel & 0x01) << 2);
        p++;
        pixel=(unsigned char) ((ssize_t) *indexes++);
        *q|=((pixel & 0x01) << 1);
        pixel=(unsigned char) (GetPixelOpacity(p) == (Quantum)
          TransparentOpacity ? 1 : 0);
        *q|=((pixel & 0x01) << 0);
        p++;
        q++;
      }
      if ((number_pixels % 4) != 0)
        {
          *q='\0';
          for (bit=3; bit >= (ssize_t) (4-(number_pixels % 4)); bit-=2)
          {
            pixel=(unsigned char) ((ssize_t) *indexes++);
            *q|=((pixel & 0x01) << (unsigned char) (bit+4));
            pixel=(unsigned char) (GetPixelOpacity(p) == (Quantum)
              TransparentOpacity ? 1 : 0);
            *q|=((pixel & 0x01) << (unsigned char) (bit+4-1));
            p++;
          }
          q++;
        }
      break;
    }
    case 4:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels ; x++)
      {
        pixel=(unsigned char) ((ssize_t) *indexes++);
        *q=((pixel & 0xf) << 4);
        pixel=(unsigned char) ((ssize_t) (16*QuantumScale*((Quantum)
          (QuantumRange-GetPixelOpacity(p)))+0.5));
        *q|=((pixel & 0xf) << 0);
        p++;
        q++;
      }
      break;
    }
    case 8:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopCharPixel((unsigned char) GetPixelIndex(indexes+x),q);
        pixel=ScaleQuantumToChar((Quantum) (QuantumRange-GetPixelOpacity(p)));
        q=PopCharPixel(pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 16:
    {
      unsigned short
        pixel;

      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopShortPixel(quantum_info->endian,(unsigned short)
              ((ssize_t) GetPixelIndex(indexes+x)),q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelAlpha(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopShortPixel(quantum_info->endian,(unsigned short)
          ((ssize_t) GetPixelIndex(indexes+x)),q);
        pixel=ScaleQuantumToShort((Quantum) (QuantumRange-GetPixelOpacity(p)));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 32:
    {
      unsigned int
        pixel;

      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            float
              pixel;

            q=PopFloatPixel(quantum_info,(float) GetPixelIndex(indexes+x),q);
            pixel=(float)  (GetPixelAlpha(p));
            q=PopFloatPixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopLongPixel(quantum_info->endian,(unsigned int)
          GetPixelIndex(indexes+x),q);
        pixel=ScaleQuantumToLong((Quantum) (QuantumRange-GetPixelOpacity(p)));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        p++;
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
            double
              pixel;

            q=PopDoublePixel(quantum_info,(double) GetPixelIndex(indexes+x),
              q);
            pixel=(double) (GetPixelAlpha(p));
            q=PopDoublePixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
    }
    default:
    {
      QuantumAny
        range;

      range=GetQuantumRange(quantum_info->depth);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,GetPixelIndex(indexes+x),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny((Quantum)
          (GetPixelAlpha(p)),range),q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
  }
}