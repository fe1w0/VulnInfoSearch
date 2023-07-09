void CLASS samsung_load_raw()
{
  int row, col, c, i, dir, op[4], len[4];

  order = 0x4949;
  for (row=0; row < raw_height; row++) {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    fseek (ifp, strip_offset+row*4, SEEK_SET);
    fseek (ifp, data_offset+get4(), SEEK_SET);
    ph1_bits(-1);
    FORC4 len[c] = row < 2 ? 7:4;
    for (col=0; col < raw_width; col+=16) {
      dir = ph1_bits(1);
      FORC4 op[c] = ph1_bits(2);
      FORC4 switch (op[c]) {
	case 3: len[c] = ph1_bits(4);	break;
	case 2: len[c]--;		break;
	case 1: len[c]++;
      }
      for (c=0; c < 16; c+=2) {
	i = len[((c & 1) << 1) | (c >> 3)];
        RAW(row,col+c) = ((signed) ph1_bits(i) << (32-i) >> (32-i)) +
	  (dir ? RAW(row+(~c | -2),col+c) : col ? RAW(row,col+(c | -2)) : 128);
	if (c == 14) c = -1;
      }
    }
  }
  for (row=0; row < raw_height-1; row+=2)
    for (col=0; col < raw_width-1; col+=2)
      SWAP (RAW(row,col+1), RAW(row+1,col));
}