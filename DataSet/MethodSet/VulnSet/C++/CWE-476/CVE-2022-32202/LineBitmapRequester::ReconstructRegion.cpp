void LineBitmapRequester::ReconstructRegion(const RectAngle<LONG> &orgregion,const struct RectangleRequest *rr)
{
  class ColorTrafo *ctrafo = ColorTrafoOf(false,!rr->rr_bColorTrafo);
  UBYTE i;

  if (m_bSubsampling && rr->rr_bUpsampling) { 
    for(i = rr->rr_usFirstComponent;i <= rr->rr_usLastComponent;i++) {
      class Component *comp = m_pFrame->ComponentOf(i);
      UBYTE subx            = comp->SubXOf();
      UBYTE suby            = comp->SubYOf();
      class UpsamplerBase *up;  // upsampler
      LONG bx,by;
      RectAngle<LONG> blocks;
      //
      // Compute the region of blocks
      assert(subx > 0 && suby > 0);
      if ((up = m_ppUpsampler[i])) {
        LONG bwidth           = ((m_ulPixelWidth  + subx - 1) / subx + 7) >> 3;
        LONG bheight          = ((m_ulPixelHeight + suby - 1) / suby + 7) >> 3;
        LONG rx               = (subx > 1)?(1):(0);
        LONG ry               = (suby > 1)?(1):(0);
        // The +/-1 include additional lines required for subsampling expansion
        blocks.ra_MinX        = ((orgregion.ra_MinX / subx - rx) >> 3);
        blocks.ra_MaxX        = ((orgregion.ra_MaxX / subx + rx) >> 3);
        blocks.ra_MinY        = ((orgregion.ra_MinY / suby - ry) >> 3);
        blocks.ra_MaxY        = ((orgregion.ra_MaxY / suby + ry) >> 3);
        // Clip.
        if (blocks.ra_MinX < 0)        blocks.ra_MinX = 0;
        if (blocks.ra_MaxX >= bwidth)  blocks.ra_MaxX = bwidth - 1;
        if (blocks.ra_MinY < 0)        blocks.ra_MinY = 0;
        if (blocks.ra_MaxY >= bheight) blocks.ra_MaxY = bheight - 1;
        up->SetBufferedRegion(blocks); // also removes the rectangle of blocks already buffered.
        //
        for(by = blocks.ra_MinY;by <= blocks.ra_MaxY;by++) {
          for(bx = blocks.ra_MinX;bx <= blocks.ra_MaxX;bx++) {
            LONG dst[64];
            if (*m_pppImage[i]) {
              FetchRegion(bx,*m_pppImage[i],dst);
            } else {
              memset(dst,0,sizeof(dst));
            }
            up->DefineRegion(bx,by,dst);
          }
          Next8Lines(i);
        }
      }
    }
    // Now push blocks into the color transformer from the upsampler.
    {
      RectAngle<LONG> r;
      ULONG minx   = orgregion.ra_MinX >> 3;
      ULONG maxx   = orgregion.ra_MaxX >> 3;
      ULONG miny   = orgregion.ra_MinY >> 3;
      ULONG maxy   = orgregion.ra_MaxY >> 3;
      ULONG x,y;
      
      if (maxy > m_ulMaxMCU)
        maxy = m_ulMaxMCU;

      for(y = miny,r.ra_MinY = orgregion.ra_MinY;y <= maxy;y++,r.ra_MinY = r.ra_MaxY + 1) {
        r.ra_MaxY = (r.ra_MinY & -8) + 7;
        if (r.ra_MaxY > orgregion.ra_MaxY)
          r.ra_MaxY = orgregion.ra_MaxY;
        
        for(x = minx,r.ra_MinX = orgregion.ra_MinX;x <= maxx;x++,r.ra_MinX = r.ra_MaxX + 1) {
          r.ra_MaxX = (r.ra_MinX & -8) + 7;
          if (r.ra_MaxX > orgregion.ra_MaxX)
            r.ra_MaxX = orgregion.ra_MaxX;
          
          for(i = 0;i < m_ucCount;i++) {
            if (i >= rr->rr_usFirstComponent && i <= rr->rr_usLastComponent) {
              ExtractBitmap(m_ppTempIBM[i],r,i);
              if (m_ppUpsampler[i]) {
                // Upsampled case, take from the upsampler, transform
                // into the color buffer.
                m_ppUpsampler[i]->UpsampleRegion(r,m_ppCTemp[i]);
              } else if (*m_pppImage[i]) {
                FetchRegion(x,*m_pppImage[i],m_ppCTemp[i]);
              } else {
                memset(m_ppCTemp[0],0,sizeof(LONG) * 64);
              }
            } else {
              // Not requested, zero the buffer.
              memset(m_ppCTemp[i],0,sizeof(LONG) * 64);
            }
          }
          ctrafo->YCbCr2RGB(r,m_ppTempIBM,m_ppCTemp,NULL);
        }
        //
        // Advance the quantized rows for the non-subsampled components,
        // upsampled components have been advanced above.
        for(i = rr->rr_usFirstComponent;i <= rr->rr_usLastComponent;i++) {
          if (m_ppUpsampler[i] == NULL)
            Next8Lines(i);
        }
      }
    }
  } else { // direct case, no upsampling required, residual coding possible.
    RectAngle<LONG> r;
    RectAngle<LONG> region = orgregion;
    SubsampledRegion(region,rr);
    ULONG minx   = region.ra_MinX >> 3;
    ULONG maxx   = region.ra_MaxX >> 3;
    ULONG miny   = region.ra_MinY >> 3;
    ULONG maxy   = region.ra_MaxY >> 3;
    ULONG x,y;
      
    if (maxy > m_ulMaxMCU)
      maxy = m_ulMaxMCU;

    for(y = miny,r.ra_MinY = region.ra_MinY;y <= maxy;y++,r.ra_MinY = r.ra_MaxY + 1) {
      r.ra_MaxY = (r.ra_MinY & -8) + 7;
      if (r.ra_MaxY > region.ra_MaxY)
        r.ra_MaxY = region.ra_MaxY;
        
      for(x = minx,r.ra_MinX = region.ra_MinX;x <= maxx;x++,r.ra_MinX = r.ra_MaxX + 1) {
        r.ra_MaxX = (r.ra_MinX & -8) + 7;
        if (r.ra_MaxX > region.ra_MaxX)
          r.ra_MaxX = region.ra_MaxX;

        for(i = 0;i < m_ucCount;i++) {      
          LONG *dst = m_ppCTemp[i];
          if (i >= rr->rr_usFirstComponent && i <= rr->rr_usLastComponent) {
            ExtractBitmap(m_ppTempIBM[i],r,i);
            FetchRegion(x,*m_pppImage[i],dst);
          } else {
            memset(dst,0,sizeof(LONG) * 64);
          }
        }
        //
        // Perform the color transformation now.
        ctrafo->YCbCr2RGB(r,m_ppTempIBM,m_ppCTemp,NULL);
      } // of loop over x
      //
      // Advance the rows.
      for(i = rr->rr_usFirstComponent;i <= rr->rr_usLastComponent;i++) {
        Next8Lines(i);
      }
    }
  }
}