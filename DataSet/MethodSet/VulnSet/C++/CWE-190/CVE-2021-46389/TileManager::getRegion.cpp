RawTile TileManager::getRegion( unsigned int res, int seq, int ang, int layers, unsigned int x, unsigned int y, unsigned int width, unsigned int height ){

  // If our image type can directly handle region compositing, simply return that
  if( image->regionDecoding() ){
    if( loglevel >= 3 ){
      *logfile << "TileManager getRegion :: requesting region directly from image" << endl;
    }
    return image->getRegion( seq, ang, res, layers, x, y, width, height );
  }

  // Otherwise do the compositing ourselves

  // The tile size of the source tile
  unsigned int src_tile_width = image->getTileWidth();
  unsigned int src_tile_height = image->getTileHeight();

  // The tile size of the destination tile
  unsigned int dst_tile_width = src_tile_width;
  unsigned int dst_tile_height = src_tile_height;

  // The basic tile size ie. not the current tile
  unsigned int basic_tile_width = src_tile_width;
  unsigned int basic_tile_height = src_tile_height;

  int num_res = image->getNumResolutions();
  unsigned int im_width = image->image_widths[num_res-res-1];
  unsigned int im_height = image->image_heights[num_res-res-1];

  unsigned int rem_x = im_width % src_tile_width;
  unsigned int rem_y = im_height % src_tile_height;

  // The number of tiles in each direction
  unsigned int ntlx = (im_width / src_tile_width) + (rem_x == 0 ? 0 : 1);
  unsigned int ntly = (im_height / src_tile_height) + (rem_y == 0 ? 0 : 1);

  // Start and end tiles and pixel offsets
  unsigned int startx, endx, starty, endy, xoffset, yoffset;


  if( ! ( x==0 && y==0 && width==im_width && height==im_height ) ){
    // Calculate the start tiles
    startx = (unsigned int) ( x / src_tile_width );
    starty = (unsigned int) ( y / src_tile_height );
    xoffset = x % src_tile_width;
    yoffset = y % src_tile_height;

    endx = (unsigned int) ceil( (float)(width + x) / (float)src_tile_width );
    endy = (unsigned int) ceil( (float)(height + y) / (float)src_tile_height );

    if( loglevel >= 3 ){
      *logfile << "TileManager getRegion :: Total tiles in image: " << ntlx << "x" << ntly << " tiles" << endl
	       << "TileManager getRegion :: Tile start: " << startx << "," << starty << " with offset: "
	       << xoffset << "," << yoffset << endl
	       << "TileManager getRegion :: Tile end: " << endx-1 << "," << endy-1 << endl;
    }
  }
  else{
    startx = starty = xoffset = yoffset = 0;
    endx = ntlx;
    endy = ntly;
  }


  unsigned int channels = image->getNumChannels();
  unsigned int bpc = image->getNumBitsPerPixel();
  SampleType sampleType = image->getSampleType();

  // Assume 1 bit data has been unpacked to 8 bits per channel
  if( bpc == 1 ) bpc = 8;

  // Create an empty tile with the correct dimensions
  RawTile region( 0, res, seq, ang, width, height, channels, bpc );
  region.dataLength = width * height * channels * (bpc/8);
  region.sampleType = sampleType;

  // Allocate memory for the region
  if( bpc == 8 ) region.data = new unsigned char[width*height*channels];
  else if( bpc == 16 ) region.data = new unsigned short[width*height*channels];
  else if( bpc == 32 && sampleType == FIXEDPOINT ) region.data = new int[width*height*channels];
  else if( bpc == 32 && sampleType == FLOATINGPOINT ) region.data = new float[width*height*channels];

  unsigned int current_height = 0;

  // Decode the image strip by strip
  for( unsigned int i=starty; i<endy; i++ ){

    unsigned int buffer_index = 0;

    // Keep track of the current pixel boundary horizontally. ie. only up
    //  to the beginning of the current tile boundary.
    unsigned int current_width = 0;

    for( unsigned int j=startx; j<endx; j++ ){

      // Time the tile retrieval
      if( loglevel >= 3 ) tile_timer.start();

      // Get an uncompressed tile
      RawTile rawtile = this->getTile( res, (i*ntlx) + j, seq, ang, layers, UNCOMPRESSED );

      if( loglevel >= 5 ){
	*logfile << "TileManager getRegion :: Tile access time " << tile_timer.getTime() << " microseconds for tile "
		 << (i*ntlx) + j << " at resolution " << res << endl;
      }


      // Only print this out once per image
      if( (loglevel >= 5) && (i==starty) && (j==starty) ){
	*logfile << "TileManager getRegion :: Tile data is " << rawtile.channels << " channels, "
		 << rawtile.bpc << " bits per channel" << endl;
      }

      // Set the tile width and height to be that of the source tile - Use the rawtile data
      // because if we take a tile from cache the image pointer will not necessarily be pointing
      // to the the current tile
      src_tile_width = rawtile.width;
      src_tile_height = rawtile.height;
      dst_tile_width = src_tile_width;
      dst_tile_height = src_tile_height;

      // Variables for the pixel offset within the current tile
      unsigned int xf = 0;
      unsigned int yf = 0;

      // If our viewport has been set, we need to modify our start
      // and end points on the source image
      if( !( x==0 && y==0 && width==im_width && height==im_height ) ){

	unsigned int remainder;  // Remaining pixels in the final row or column

	if( j == startx ){
	  // Calculate the width used in the current tile
	  // If there is only 1 tile, the width is just the view width
	  if( j < endx - 1 ) dst_tile_width = src_tile_width - xoffset;
	  else dst_tile_width = width;
	  xf = xoffset;
	}
	else if( j == endx-1 ){
	  // If this is the final row, calculate the remaining number of pixels
	  remainder = (width+x) % basic_tile_width;
	  if( remainder != 0 ) dst_tile_width = remainder;
	}

	if( i == starty ){
	  // Calculate the height used in the current row of tiles
	  // If there is only 1 row the height is just the view height
	  if( i < endy - 1 ) dst_tile_height = src_tile_height - yoffset;
	  else dst_tile_height = height;
	  yf = yoffset;
	}
	else if( i == endy-1 ){
	  // If this is the final row, calculate the remaining number of pixels
	  remainder = (height+y) % basic_tile_height;
	  if( remainder != 0 ) dst_tile_height = remainder;
	}

	if( loglevel >= 5 ){
	  *logfile << "TileManager getRegion :: destination tile width: " << dst_tile_width
		   << ", tile height: " << dst_tile_height << endl;
	}
      }


      // Copy our tile data into the appropriate part of the strip memory
      // one whole tile width at a time
      for( unsigned int k=0; k<dst_tile_height; k++ ){

	buffer_index = (current_width*channels) + (k*width*channels) + (current_height*width*channels);
	unsigned int inx = ((k+yf)*rawtile.width*channels) + (xf*channels);

	// Simply copy the line of data across
	if( bpc == 8 ){
	  unsigned char* ptr = (unsigned char*) rawtile.data;
	  unsigned char* buf = (unsigned char*) region.data;
	  memcpy( &buf[buffer_index], &ptr[inx], dst_tile_width*channels );
	}
	else if( bpc ==  16 ){
	  unsigned short* ptr = (unsigned short*) rawtile.data;
	  unsigned short* buf = (unsigned short*) region.data;
	  memcpy( &buf[buffer_index], &ptr[inx], dst_tile_width*channels*2 );
	}
	else if( bpc == 32 && sampleType == FIXEDPOINT ){
	  unsigned int* ptr = (unsigned int*) rawtile.data;
	  unsigned int* buf = (unsigned int*) region.data;
	  memcpy( &buf[buffer_index], &ptr[inx], dst_tile_width*channels*4 );
	}
	else if( bpc == 32 && sampleType == FLOATINGPOINT ){
	  float* ptr = (float*) rawtile.data;
	  float* buf = (float*) region.data;
	  memcpy( &buf[buffer_index], &ptr[inx], dst_tile_width*channels*4 );
	}
      }

      current_width += dst_tile_width;
    }

    current_height += dst_tile_height;

  }

  return region;

}