void CLASS quicktake_100_load_raw()
{
  uchar pixel[484][644];
  static const short gstep[16] =
  { -89,-60,-44,-32,-22,-15,-8,-2,2,8,15,22,32,44,60,89 };
  static const short rstep[6][4] =
  { {  -3,-1,1,3  }, {  -5,-1,1,5  }, {  -8,-2,2,8  },
    { -13,-3,3,13 }, { -19,-4,4,19 }, { -28,-6,6,28 } };
  static const short t_curve[256] =
  { 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
    28,29,30,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,53,
    54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,74,75,76,77,78,
    79,80,81,82,83,84,86,88,90,92,94,97,99,101,103,105,107,110,112,114,116,
    118,120,123,125,127,129,131,134,136,138,140,142,144,147,149,151,153,155,
    158,160,162,164,166,168,171,173,175,177,179,181,184,186,188,190,192,195,
    197,199,201,203,205,208,210,212,214,216,218,221,223,226,230,235,239,244,
    248,252,257,261,265,270,274,278,283,287,291,296,300,305,309,313,318,322,
    326,331,335,339,344,348,352,357,361,365,370,374,379,383,387,392,396,400,
    405,409,413,418,422,426,431,435,440,444,448,453,457,461,466,470,474,479,
    483,487,492,496,500,508,519,531,542,553,564,575,587,598,609,620,631,643,
    654,665,676,687,698,710,721,732,743,754,766,777,788,799,810,822,833,844,
    855,866,878,889,900,911,922,933,945,956,967,978,989,1001,1012,1023 };
  int rb, row, col, sharp, val=0;

  getbits(-1);
  memset (pixel, 0x80, sizeof pixel);
  for (row=2; row < height+2; row++) {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    for (col=2+(row & 1); col < width+2; col+=2) {
      val = ((pixel[row-1][col-1] + 2*pixel[row-1][col+1] +
		pixel[row][col-2]) >> 2) + gstep[getbits(4)];
      pixel[row][col] = val = LIM(val,0,255);
      if (col < 4)
	pixel[row][col-2] = pixel[row+1][~row & 1] = val;
      if (row == 2)
	pixel[row-1][col+1] = pixel[row-1][col+3] = val;
    }
    pixel[row][col] = val;
  }
  for (rb=0; rb < 2; rb++)
    for (row=2+rb; row < height+2; row+=2)
    {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
      for (col=3-(row & 1); col < width+2; col+=2) {
	if (row < 4 || col < 4) sharp = 2;
	else {
	  val = ABS(pixel[row-2][col] - pixel[row][col-2])
	      + ABS(pixel[row-2][col] - pixel[row-2][col-2])
	      + ABS(pixel[row][col-2] - pixel[row-2][col-2]);
	  sharp = val <  4 ? 0 : val <  8 ? 1 : val < 16 ? 2 :
		  val < 32 ? 3 : val < 48 ? 4 : 5;
	}
	val = ((pixel[row-2][col] + pixel[row][col-2]) >> 1)
	      + rstep[sharp][getbits(2)];
	pixel[row][col] = val = LIM(val,0,255);
	if (row < 4) pixel[row-2][col+2] = val;
	if (col < 4) pixel[row+2][col-2] = val;
      }
    }
  for (row=2; row < height+2; row++)
  {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    for (col=3-(row & 1); col < width+2; col+=2) {
      val = ((pixel[row][col-1] + (pixel[row][col] << 2) +
	      pixel[row][col+1]) >> 1) - 0x100;
      pixel[row][col] = LIM(val,0,255);
    }
  }
  for (row=0; row < height; row++)
  {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    for (col=0; col < width; col++)
      RAW(row,col) = t_curve[pixel[row+2][col+2]];
  }
  maximum = 0x3ff;
}