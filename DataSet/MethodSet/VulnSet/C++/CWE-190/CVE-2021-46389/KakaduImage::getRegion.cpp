RawTile KakaduImage::getRegion( int seq, int ang, unsigned int res, int layers, int x, int y, unsigned int w, unsigned int h )
{
  // Scale up our output bit depth to the nearest factor of 8
  unsigned int obpc = bpc;
  if( bpc <= 16 && bpc > 8 ) obpc = 16;
  else if( bpc <= 8 ) obpc = 8;

#ifdef DEBUG
  Timer timer;
  timer.start();
#endif

  RawTile rawtile( 0, res, seq, ang, w, h, channels, obpc );

  if( obpc == 16 ) rawtile.data = new unsigned short[w*h*channels];
  else if( obpc == 8 ) rawtile.data = new unsigned char[w*h*channels];
  else throw file_error( "Kakadu :: Unsupported number of bits" );

  rawtile.dataLength = w*h*channels*(obpc/8);
  rawtile.filename = getImagePath();
  rawtile.timestamp = timestamp;

  process( res, layers, x, y, w, h, rawtile.data );

#ifdef DEBUG
  logfile << "Kakadu :: getRegion() :: " << timer.getTime() << " microseconds" << endl;
#endif

  return rawtile;

}