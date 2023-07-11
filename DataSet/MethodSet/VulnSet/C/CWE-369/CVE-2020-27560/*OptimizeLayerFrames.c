static Image *OptimizeLayerFrames(const Image *image,const LayerMethod method,
  ExceptionInfo *exception)
{
  ExceptionInfo
    *sans_exception;

  Image
    *prev_image,
    *dup_image,
    *bgnd_image,
    *optimized_image;

  RectangleInfo
    try_bounds,
    bgnd_bounds,
    dup_bounds,
    *bounds;

  MagickBooleanType
    add_frames,
    try_cleared,
    cleared;

  DisposeType
    *disposals;

  register const Image
    *curr;

  register ssize_t
    i;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  assert(method == OptimizeLayer ||
         method == OptimizeImageLayer ||
         method == OptimizePlusLayer);
  /*
    Are we allowed to add/remove frames from animation?
  */
  add_frames=method == OptimizePlusLayer ? MagickTrue : MagickFalse;
  /*
    Ensure  all the images are the same size.
  */
  curr=GetFirstImageInList(image);
  for (; curr != (Image *) NULL; curr=GetNextImageInList(curr))
  {
    if ((curr->columns != image->columns) || (curr->rows != image->rows))
      ThrowImageException(OptionError,"ImagesAreNotTheSameSize");

    if ((curr->page.x != 0) || (curr->page.y != 0) ||
        (curr->page.width != image->page.width) ||
        (curr->page.height != image->page.height))
      ThrowImageException(OptionError,"ImagePagesAreNotCoalesced");
  }
  /*
    Allocate memory (times 2 if we allow the use of frame duplications)
  */
  curr=GetFirstImageInList(image);
  bounds=(RectangleInfo *) AcquireQuantumMemory((size_t)
    GetImageListLength(curr),(add_frames != MagickFalse ? 2UL : 1UL)*
    sizeof(*bounds));
  if (bounds == (RectangleInfo *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
  disposals=(DisposeType *) AcquireQuantumMemory((size_t)
    GetImageListLength(image),(add_frames != MagickFalse ? 2UL : 1UL)*
    sizeof(*disposals));
  if (disposals == (DisposeType *) NULL)
    {
      bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
    }
  /*
    Initialise Previous Image as fully transparent
  */
  prev_image=CloneImage(curr,curr->columns,curr->rows,MagickTrue,exception);
  if (prev_image == (Image *) NULL)
    {
      bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
      disposals=(DisposeType *) RelinquishMagickMemory(disposals);
      return((Image *) NULL);
    }
  prev_image->page=curr->page;  /* ERROR: <-- should not be need, but is! */
  prev_image->page.x=0;
  prev_image->page.y=0;
  prev_image->dispose=NoneDispose;
  prev_image->background_color.alpha_trait=BlendPixelTrait;
  prev_image->background_color.alpha=(MagickRealType) TransparentAlpha;
  (void) SetImageBackgroundColor(prev_image,exception);
  /*
    Figure out the area of overlay of the first frame
    No pixel could be cleared as all pixels are already cleared.
  */
#if DEBUG_OPT_FRAME
  i=0;
  (void) FormatLocaleFile(stderr,"frame %.20g :-\n",(double) i);
#endif
  disposals[0]=NoneDispose;
  bounds[0]=CompareImagesBounds(prev_image,curr,CompareAnyLayer,exception);
#if DEBUG_OPT_FRAME
  (void) FormatLocaleFile(stderr, "overlay: %.20gx%.20g%+.20g%+.20g\n\n",
    (double) bounds[i].width,(double) bounds[i].height,
    (double) bounds[i].x,(double) bounds[i].y );
#endif
  /*
    Compute the bounding box of changes for each pair of images.
  */
  i=1;
  bgnd_image=(Image *) NULL;
  dup_image=(Image *) NULL;
  dup_bounds.width=0;
  dup_bounds.height=0;
  dup_bounds.x=0;
  dup_bounds.y=0;
  curr=GetNextImageInList(curr);
  for ( ; curr != (const Image *) NULL; curr=GetNextImageInList(curr))
  {
#if DEBUG_OPT_FRAME
    (void) FormatLocaleFile(stderr,"frame %.20g :-\n",(double) i);
#endif
    /*
      Assume none disposal is the best
    */
    bounds[i]=CompareImagesBounds(curr->previous,curr,CompareAnyLayer,exception);
    cleared=IsBoundsCleared(curr->previous,curr,&bounds[i],exception);
    disposals[i-1]=NoneDispose;
#if DEBUG_OPT_FRAME
    (void) FormatLocaleFile(stderr, "overlay: %.20gx%.20g%+.20g%+.20g%s%s\n",
         (double) bounds[i].width,(double) bounds[i].height,
         (double) bounds[i].x,(double) bounds[i].y,
         bounds[i].x < 0?"  (unchanged)":"",
         cleared?"  (pixels cleared)":"");
#endif
    if ( bounds[i].x < 0 ) {
      /*
        Image frame is exactly the same as the previous frame!
        If not adding frames leave it to be cropped down to a null image.
        Otherwise mark previous image for deleted, transfering its crop bounds
        to the current image.
      */
      if ( add_frames && i>=2 ) {
        disposals[i-1]=DelDispose;
        disposals[i]=NoneDispose;
        bounds[i]=bounds[i-1];
        i++;
        continue;
      }
    }
    else
      {
        /*
          Compare a none disposal against a previous disposal
        */
        try_bounds=CompareImagesBounds(prev_image,curr,CompareAnyLayer,exception);
        try_cleared=IsBoundsCleared(prev_image,curr,&try_bounds,exception);
#if DEBUG_OPT_FRAME
    (void) FormatLocaleFile(stderr, "test_prev: %.20gx%.20g%+.20g%+.20g%s\n",
         (double) try_bounds.width,(double) try_bounds.height,
         (double) try_bounds.x,(double) try_bounds.y,
         try_cleared?"  (pixels were cleared)":"");
#endif
        if ( (!try_cleared && cleared ) ||
                try_bounds.width * try_bounds.height
                    <  bounds[i].width * bounds[i].height )
          {
            cleared=try_cleared;
            bounds[i]=try_bounds;
            disposals[i-1]=PreviousDispose;
#if DEBUG_OPT_FRAME
            (void) FormatLocaleFile(stderr,"previous: accepted\n");
          } else {
            (void) FormatLocaleFile(stderr,"previous: rejected\n");
#endif
          }

        /*
          If we are allowed lets try a complex frame duplication.
          It is useless if the previous image already clears pixels correctly.
          This method will always clear all the pixels that need to be cleared.
        */
        dup_bounds.width=dup_bounds.height=0; /* no dup, no pixel added */
        if ( add_frames )
          {
            dup_image=CloneImage(curr->previous,0,0,MagickTrue,exception);
            if (dup_image == (Image *) NULL)
              {
                bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
                disposals=(DisposeType *) RelinquishMagickMemory(disposals);
                prev_image=DestroyImage(prev_image);
                return((Image *) NULL);
              }
            dup_image->background_color.alpha_trait=BlendPixelTrait;
            dup_bounds=CompareImagesBounds(dup_image,curr,CompareClearLayer,exception);
            ClearBounds(dup_image,&dup_bounds,exception);
            try_bounds=CompareImagesBounds(dup_image,curr,CompareAnyLayer,exception);
            if ( cleared ||
                   dup_bounds.width*dup_bounds.height
                      +try_bounds.width*try_bounds.height
                   < bounds[i].width * bounds[i].height )
              {
                cleared=MagickFalse;
                bounds[i]=try_bounds;
                disposals[i-1]=DupDispose;
                /* to be finalised later, if found to be optimial */
              }
            else
              dup_bounds.width=dup_bounds.height=0;
          }
        /*
          Now compare against a simple background disposal
        */
        bgnd_image=CloneImage(curr->previous,0,0,MagickTrue,exception);
        if (bgnd_image == (Image *) NULL)
          {
            bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
            disposals=(DisposeType *) RelinquishMagickMemory(disposals);
            prev_image=DestroyImage(prev_image);
            if ( dup_image != (Image *) NULL)
              dup_image=DestroyImage(dup_image);
            return((Image *) NULL);
          }
        bgnd_image->background_color.alpha_trait=BlendPixelTrait;
        bgnd_bounds=bounds[i-1]; /* interum bounds of the previous image */
        ClearBounds(bgnd_image,&bgnd_bounds,exception);
        try_bounds=CompareImagesBounds(bgnd_image,curr,CompareAnyLayer,exception);
        try_cleared=IsBoundsCleared(bgnd_image,curr,&try_bounds,exception);
#if DEBUG_OPT_FRAME
    (void) FormatLocaleFile(stderr, "background: %s\n",
         try_cleared?"(pixels cleared)":"");
#endif
        if ( try_cleared )
          {
            /*
              Straight background disposal failed to clear pixels needed!
              Lets try expanding the disposal area of the previous frame, to
              include the pixels that are cleared.  This guaranteed
              to work, though may not be the most optimized solution.
            */
            try_bounds=CompareImagesBounds(curr->previous,curr,CompareClearLayer,exception);
#if DEBUG_OPT_FRAME
            (void) FormatLocaleFile(stderr, "expand_clear: %.20gx%.20g%+.20g%+.20g%s\n",
                (double) try_bounds.width,(double) try_bounds.height,
                (double) try_bounds.x,(double) try_bounds.y,
                try_bounds.x<0?"  (no expand nessary)":"");
#endif
            if ( bgnd_bounds.x < 0 )
              bgnd_bounds = try_bounds;
            else
              {
#if DEBUG_OPT_FRAME
                (void) FormatLocaleFile(stderr, "expand_bgnd: %.20gx%.20g%+.20g%+.20g\n",
                    (double) bgnd_bounds.width,(double) bgnd_bounds.height,
                    (double) bgnd_bounds.x,(double) bgnd_bounds.y );
#endif
                if ( try_bounds.x < bgnd_bounds.x )
                  {
                     bgnd_bounds.width+= bgnd_bounds.x-try_bounds.x;
                     if ( bgnd_bounds.width < try_bounds.width )
                       bgnd_bounds.width = try_bounds.width;
                     bgnd_bounds.x = try_bounds.x;
                  }
                else
                  {
                     try_bounds.width += try_bounds.x - bgnd_bounds.x;
                     if ( bgnd_bounds.width < try_bounds.width )
                       bgnd_bounds.width = try_bounds.width;
                  }
                if ( try_bounds.y < bgnd_bounds.y )
                  {
                     bgnd_bounds.height += bgnd_bounds.y - try_bounds.y;
                     if ( bgnd_bounds.height < try_bounds.height )
                       bgnd_bounds.height = try_bounds.height;
                     bgnd_bounds.y = try_bounds.y;
                  }
                else
                  {
                    try_bounds.height += try_bounds.y - bgnd_bounds.y;
                     if ( bgnd_bounds.height < try_bounds.height )
                       bgnd_bounds.height = try_bounds.height;
                  }
#if DEBUG_OPT_FRAME
                (void) FormatLocaleFile(stderr, "        to : %.20gx%.20g%+.20g%+.20g\n",
                    (double) bgnd_bounds.width,(double) bgnd_bounds.height,
                    (double) bgnd_bounds.x,(double) bgnd_bounds.y );
#endif
              }
            ClearBounds(bgnd_image,&bgnd_bounds,exception);
#if DEBUG_OPT_FRAME
/* Something strange is happening with a specific animation
 * CompareAnyLayers (normal method) and CompareClearLayers returns the whole
 * image, which is not posibly correct!  As verified by previous tests.
 * Something changed beyond the bgnd_bounds clearing.  But without being able
 * to see, or writet he image at this point it is hard to tell what is wrong!
 * Only CompareOverlay seemed to return something sensible.
 */
            try_bounds=CompareImagesBounds(bgnd_image,curr,CompareClearLayer,exception);
            (void) FormatLocaleFile(stderr, "expand_ctst: %.20gx%.20g%+.20g%+.20g\n",
                (double) try_bounds.width,(double) try_bounds.height,
                (double) try_bounds.x,(double) try_bounds.y );
            try_bounds=CompareImagesBounds(bgnd_image,curr,CompareAnyLayer,exception);
            try_cleared=IsBoundsCleared(bgnd_image,curr,&try_bounds,exception);
            (void) FormatLocaleFile(stderr, "expand_any : %.20gx%.20g%+.20g%+.20g%s\n",
                (double) try_bounds.width,(double) try_bounds.height,
                (double) try_bounds.x,(double) try_bounds.y,
                try_cleared?"   (pixels cleared)":"");
#endif
            try_bounds=CompareImagesBounds(bgnd_image,curr,CompareOverlayLayer,exception);
#if DEBUG_OPT_FRAME
            try_cleared=IsBoundsCleared(bgnd_image,curr,&try_bounds,exception);
            (void) FormatLocaleFile(stderr, "expand_test: %.20gx%.20g%+.20g%+.20g%s\n",
                (double) try_bounds.width,(double) try_bounds.height,
                (double) try_bounds.x,(double) try_bounds.y,
                try_cleared?"   (pixels cleared)":"");
#endif
          }
        /*
          Test if this background dispose is smaller than any of the
          other methods we tryed before this (including duplicated frame)
        */
        if ( cleared ||
              bgnd_bounds.width*bgnd_bounds.height
                +try_bounds.width*try_bounds.height
              < bounds[i-1].width*bounds[i-1].height
                  +dup_bounds.width*dup_bounds.height
                  +bounds[i].width*bounds[i].height )
          {
            cleared=MagickFalse;
            bounds[i-1]=bgnd_bounds;
            bounds[i]=try_bounds;
            if ( disposals[i-1] == DupDispose )
              dup_image=DestroyImage(dup_image);
            disposals[i-1]=BackgroundDispose;
#if DEBUG_OPT_FRAME
    (void) FormatLocaleFile(stderr,"expand_bgnd: accepted\n");
          } else {
    (void) FormatLocaleFile(stderr,"expand_bgnd: reject\n");
#endif
          }
      }
    /*
       Finalise choice of dispose, set new prev_image,
       and junk any extra images as appropriate,
    */
    if ( disposals[i-1] == DupDispose )
      {
         if (bgnd_image != (Image *) NULL)
           bgnd_image=DestroyImage(bgnd_image);
         prev_image=DestroyImage(prev_image);
         prev_image=dup_image, dup_image=(Image *) NULL;
         bounds[i+1]=bounds[i];
         bounds[i]=dup_bounds;
         disposals[i-1]=DupDispose;
         disposals[i]=BackgroundDispose;
         i++;
      }
    else
      {
        if ( dup_image != (Image *) NULL)
          dup_image=DestroyImage(dup_image);
        if ( disposals[i-1] != PreviousDispose )
          prev_image=DestroyImage(prev_image);
        if ( disposals[i-1] == BackgroundDispose )
          prev_image=bgnd_image, bgnd_image=(Image *) NULL;
        if (bgnd_image != (Image *) NULL)
          bgnd_image=DestroyImage(bgnd_image);
        if ( disposals[i-1] == NoneDispose )
          {
            prev_image=ReferenceImage(curr->previous);
            if (prev_image == (Image *) NULL)
              {
                bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
                disposals=(DisposeType *) RelinquishMagickMemory(disposals);
                return((Image *) NULL);
              }
          }

      }
    assert(prev_image != (Image *) NULL);
    disposals[i]=disposals[i-1];
#if DEBUG_OPT_FRAME
    (void) FormatLocaleFile(stderr, "final   %.20g : %s  %.20gx%.20g%+.20g%+.20g\n",
         (double) i-1,
         CommandOptionToMnemonic(MagickDisposeOptions,disposals[i-1]),
         (double) bounds[i-1].width,(double) bounds[i-1].height,
         (double) bounds[i-1].x,(double) bounds[i-1].y );
#endif
#if DEBUG_OPT_FRAME
    (void) FormatLocaleFile(stderr, "interum %.20g : %s  %.20gx%.20g%+.20g%+.20g\n",
         (double) i,
         CommandOptionToMnemonic(MagickDisposeOptions,disposals[i]),
         (double) bounds[i].width,(double) bounds[i].height,
         (double) bounds[i].x,(double) bounds[i].y );
    (void) FormatLocaleFile(stderr,"\n");
#endif
    i++;
  }
  prev_image=DestroyImage(prev_image);
  /*
    Optimize all images in sequence.
  */
  sans_exception=AcquireExceptionInfo();
  i=0;
  curr=GetFirstImageInList(image);
  optimized_image=NewImageList();
  while ( curr != (const Image *) NULL )
  {
    prev_image=CloneImage(curr,0,0,MagickTrue,exception);
    if (prev_image == (Image *) NULL)
      break;
    prev_image->background_color.alpha_trait=BlendPixelTrait;
    if ( disposals[i] == DelDispose ) {
      size_t time = 0;
      while ( disposals[i] == DelDispose ) {
        time += curr->delay*1000/curr->ticks_per_second;
        curr=GetNextImageInList(curr);
        i++;
      }
      time += curr->delay*1000/curr->ticks_per_second;
      prev_image->ticks_per_second = 100L;
      prev_image->delay = time*prev_image->ticks_per_second/1000;
    }
    bgnd_image=CropImage(prev_image,&bounds[i],sans_exception);
    prev_image=DestroyImage(prev_image);
    if (bgnd_image == (Image *) NULL)
      break;
    bgnd_image->dispose=disposals[i];
    if ( disposals[i] == DupDispose ) {
      bgnd_image->delay=0;
      bgnd_image->dispose=NoneDispose;
    }
    else
      curr=GetNextImageInList(curr);
    AppendImageToList(&optimized_image,bgnd_image);
    i++;
  }
  sans_exception=DestroyExceptionInfo(sans_exception);
  bounds=(RectangleInfo *) RelinquishMagickMemory(bounds);
  disposals=(DisposeType *) RelinquishMagickMemory(disposals);
  if (curr != (Image *) NULL)
    {
      optimized_image=DestroyImageList(optimized_image);
      return((Image *) NULL);
    }
  return(GetFirstImageInList(optimized_image));
}