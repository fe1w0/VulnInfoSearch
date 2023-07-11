void Transform::interpolate_bilinear( RawTile& in, unsigned int resampled_width, unsigned int resampled_height ){

  // Pointer to input buffer
  unsigned char *input = (unsigned char*) in.data;

  int channels = in.channels;
  unsigned int width = in.width;
  unsigned int height = in.height;

  // Define a max index position on the input buffer
  unsigned long max = ( (width*height) - 1 ) * channels;

  // Create new buffer and pointer for our output - make sure we have enough digits via unsigned long long
  unsigned char *output = new unsigned char[(unsigned long long)resampled_width*resampled_height*channels];

  // Calculate our scale
  float xscale = (float)(width) / (float)resampled_width;
  float yscale = (float)(height) / (float)resampled_height;


  // Do not parallelize for small images (256x256 pixels) as this can be slower that single threaded
#if defined(__ICC) || defined(__INTEL_COMPILER)
#pragma ivdep
#elif defined(_OPENMP)
#pragma omp parallel for if( resampled_width*resampled_height > PARALLEL_THRESHOLD )
#endif
  for( unsigned int j=0; j<resampled_height; j++ ){

    // Index to the current pyramid resolution's top left pixel
    int jj = (int) floor( j*yscale );

    // Calculate some weights - do this in the highest loop possible
    float jscale = j*yscale;
    float c = (float)(jj+1) - jscale;
    float d = jscale - (float)jj;

    for( unsigned int i=0; i<resampled_width; i++ ){

      // Index to the current pyramid resolution's top left pixel
      int ii = (int) floor( i*xscale );

      // Calculate the indices of the 4 surrounding pixels
      unsigned long p11, p12, p21, p22;
      unsigned long jj_w = jj*width;
      p11 = (unsigned long) ( channels * ( ii + jj_w ) );
      p12 = (unsigned long) ( channels * ( ii + (jj_w+width) ) );
      p21 = (unsigned long) ( channels * ( (ii+1) + jj_w ) );
      p22 = (unsigned long) ( channels * ( (ii+1) + (jj_w+width) ) );

      // Make sure we don't stray outside our input buffer boundary
      // - replicate at the edge
      p12 = (p12<=max)? p12 : max;
      p21 = (p21<=max)? p21 : max;
      p22 = (p22<=max)? p22 : max;

      // Calculate the rest of our weights
      float iscale = i*xscale;
      float a = (float)(ii+1) - iscale;
      float b = iscale - (float)ii;

      // Output buffer index
      unsigned long long resampled_index = (unsigned long long)( (j*resampled_width + i) * channels );

      for( int k=0; k<channels; k++ ){
	float tx = input[p11+k]*a + input[p21+k]*b;
	float ty = input[p12+k]*a + input[p22+k]*b;
	unsigned char r = (unsigned char)( c*tx + d*ty );
	output[resampled_index+k] = r;
      }
    }
  }

  // Delete original buffer
  delete[] (unsigned char*) input;

  // Correctly set our Rawtile info
  in.width = resampled_width;
  in.height = resampled_height;
  in.dataLength = resampled_width * resampled_height * channels * (in.bpc/8);
  in.data = output;
}