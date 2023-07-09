static void ExportRGBQuantum(QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const PixelPacket *magick_restrict p,
  unsigned char *magick_restrict q)
{
  QuantumAny
    range;

  ssize_t
    x;

  ssize_t
    bit;

  switch (quantum_info->depth)
  {
    case 8:
    {
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopCharPixel(ScaleQuantumToChar(GetPixelRed(p)),q);
        q=PopCharPixel(ScaleQuantumToChar(GetPixelGreen(p)),q);
        q=PopCharPixel(ScaleQuantumToChar(GetPixelBlue(p)),q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 10:
    {
      unsigned int
        pixel;

      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=(unsigned int) (
              ScaleQuantumToAny(GetPixelRed(p),range) << 22 |
              ScaleQuantumToAny(GetPixelGreen(p),range) << 12 |
              ScaleQuantumToAny(GetPixelBlue(p),range) << 2);
            q=PopLongPixel(quantum_info->endian,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      if (quantum_info->quantum == 32UL)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 12:
    {
      unsigned int
        pixel;

      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          for (x=0; x < (ssize_t) (3*number_pixels-1); x+=2)
          {
            switch (x % 3)
            {
              default:
              case 0:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
                break;
              }
              case 1:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
                break;
              }
              case 2:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
                p++;
                break;
              }
            }
            q=PopShortPixel(quantum_info->endian,(unsigned short) (pixel << 4),
              q);
            switch ((x+1) % 3)
            {
              default:
              case 0:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
                break;
              }
              case 1:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
                break;
              }
              case 2:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
                p++;
                break;
              }
            }
            q=PopShortPixel(quantum_info->endian,(unsigned short) (pixel << 4),
              q);
            q+=quantum_info->pad;
          }
          for (bit=0; bit < (ssize_t) (3*number_pixels % 2); bit++)
          {
            switch ((x+bit) % 3)
            {
              default:
              case 0:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
                break;
              }
              case 1:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
                break;
              }
              case 2:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
                p++;
                break;
              }
            }
            q=PopShortPixel(quantum_info->endian,(unsigned short) (pixel << 4),
              q);
            q+=quantum_info->pad;
          }
          if (bit != 0)
            p++;
          break;
        }
      if (quantum_info->quantum == 32UL)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
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
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelRed(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelGreen(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelBlue(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToShort(GetPixelRed(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelGreen(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelBlue(p));
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
            q=PopFloatPixel(quantum_info,(float) GetPixelRed(p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelGreen(p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelBlue(p),q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(GetPixelRed(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelGreen(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelBlue(p));
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
            q=PopDoublePixel(quantum_info,(double) GetPixelRed(p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelGreen(p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelBlue(p),q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
    }
    default:
    {
      range=GetQuantumRange(quantum_info->depth);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(GetPixelRed(p),range),q);
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(GetPixelGreen(p),range),q);
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(GetPixelBlue(p),range),q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
  }
}