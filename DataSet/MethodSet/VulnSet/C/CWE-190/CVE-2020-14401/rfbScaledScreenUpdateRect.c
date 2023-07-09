void rfbScaledScreenUpdateRect(rfbScreenInfoPtr screen, rfbScreenInfoPtr ptr, int x0, int y0, int w0, int h0)
{
    int x,y,w,v,z;
    int x1, y1, w1, h1;
    int bitsPerPixel, bytesPerPixel, bytesPerLine, areaX, areaY, area2;
    unsigned char *srcptr, *dstptr;

    /* Nothing to do!!! */
    if (screen==ptr) return;

    x1 = x0;
    y1 = y0;
    w1 = w0;
    h1 = h0;

    rfbScaledCorrection(screen, ptr, &x1, &y1, &w1, &h1, "rfbScaledScreenUpdateRect");
    x0 = ScaleX(ptr, screen, x1);
    y0 = ScaleY(ptr, screen, y1);
    w0 = ScaleX(ptr, screen, w1);
    h0 = ScaleY(ptr, screen, h1);

    bitsPerPixel = screen->bitsPerPixel;
    bytesPerPixel = bitsPerPixel / 8;
    bytesPerLine = w1 * bytesPerPixel;
    srcptr = (unsigned char *)(screen->frameBuffer +
     (y0 * screen->paddedWidthInBytes + x0 * bytesPerPixel));
    dstptr = (unsigned char *)(ptr->frameBuffer +
     ( y1 * ptr->paddedWidthInBytes + x1 * bytesPerPixel));
    /* The area of the source framebuffer for each destination pixel */
    areaX = ScaleX(ptr,screen,1);
    areaY = ScaleY(ptr,screen,1);
    area2 = areaX*areaY;


    /* Ensure that we do not go out of bounds */
    if ((x1+w1) > (ptr->width))
    {
      if (x1==0) w1=ptr->width; else x1 = ptr->width - w1;
    }
    if ((y1+h1) > (ptr->height))
    {
      if (y1==0) h1=ptr->height; else y1 = ptr->height - h1;
    }
    /*
     * rfbLog("rfbScaledScreenUpdateRect(%dXx%dY-%dWx%dH  ->  %dXx%dY-%dWx%dH <%dx%d>) {%dWx%dH -> %dWx%dH} 0x%p\n",
     *    x0, y0, w0, h0, x1, y1, w1, h1, areaX, areaY,
     *    screen->width, screen->height, ptr->width, ptr->height, ptr->frameBuffer);
     */

    if (screen->serverFormat.trueColour) { /* Blend neighbouring pixels together */
      unsigned char *srcptr2;
      unsigned long pixel_value, red, green, blue;
      unsigned int redShift = screen->serverFormat.redShift;
      unsigned int greenShift = screen->serverFormat.greenShift;
      unsigned int blueShift = screen->serverFormat.blueShift;
      unsigned long redMax = screen->serverFormat.redMax;
      unsigned long greenMax = screen->serverFormat.greenMax;
      unsigned long blueMax = screen->serverFormat.blueMax;

     /* for each *destination* pixel... */
     for (y = 0; y < h1; y++) {
       for (x = 0; x < w1; x++) {
         red = green = blue = 0;
         /* Get the totals for rgb from the source grid... */
         for (w = 0; w < areaX; w++) {
           for (v = 0; v < areaY; v++) {
             srcptr2 = &srcptr[(((x * areaX) + w) * bytesPerPixel) +
                               (v * screen->paddedWidthInBytes)];
             pixel_value = 0;


             switch (bytesPerPixel) {
             case 4: pixel_value = *((unsigned int *)srcptr2);   break;
             case 2: pixel_value = *((unsigned short *)srcptr2); break;
             case 1: pixel_value = *((unsigned char *)srcptr2);  break;
             default:
               /* fixme: endianness problem? */
               for (z = 0; z < bytesPerPixel; z++)
                 pixel_value += (srcptr2[z] << (8 * z));
                break;
              }
              /*
              srcptr2 += bytesPerPixel;
              */

            red += ((pixel_value >> redShift) & redMax);
            green += ((pixel_value >> greenShift) & greenMax);
            blue += ((pixel_value >> blueShift) & blueMax);

           }
         }
         /* We now have a total for all of the colors, find the average! */
         red /= area2;
         green /= area2;
         blue /= area2;
          /* Stuff the new value back into memory */
         pixel_value = ((red & redMax) << redShift) | ((green & greenMax) << greenShift) | ((blue & blueMax) << blueShift);

         switch (bytesPerPixel) {
         case 4: *((unsigned int *)dstptr)   = (unsigned int)   pixel_value; break;
         case 2: *((unsigned short *)dstptr) = (unsigned short) pixel_value; break;
         case 1: *((unsigned char *)dstptr)  = (unsigned char)  pixel_value; break;
         default:
           /* fixme: endianness problem? */
           for (z = 0; z < bytesPerPixel; z++)
             dstptr[z]=(pixel_value >> (8 * z)) & 0xff;
            break;
          }
          dstptr += bytesPerPixel;
       }
       srcptr += (screen->paddedWidthInBytes * areaY);
       dstptr += (ptr->paddedWidthInBytes - bytesPerLine);
     }
   } else
   { /* Not truecolour, so we can't blend. Just use the top-left pixel instead */
     for (y = y1; y < (y1+h1); y++) {
       for (x = x1; x < (x1+w1); x++)
         memcpy (&ptr->frameBuffer[(y *ptr->paddedWidthInBytes) + (x * bytesPerPixel)],
                 &screen->frameBuffer[(y * areaY * screen->paddedWidthInBytes) + (x *areaX * bytesPerPixel)], bytesPerPixel);
     }
  }
}