static st64 buf_format(RBuffer *dst, RBuffer *src, const char *fmt, int n) {
	st64 res = 0;
	int i;
	for (i = 0; i < n; i++) {
		int j;
		int m = 1;
		int tsize = 2;
		bool bigendian = true;

		for (j = 0; fmt[j]; j++) {
			switch (fmt[j]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (m == 1) {
					m = r_num_get (NULL, &fmt[j]);
				}
				continue;
			case 's': tsize = 2; bigendian = false; break;
			case 'S': tsize = 2; bigendian = true; break;
			case 'i': tsize = 4; bigendian = false; break;
			case 'I': tsize = 4; bigendian = true; break;
			case 'l': tsize = 8; bigendian = false; break;
			case 'L': tsize = 8; bigendian = true; break;
			case 'c': tsize = 1; bigendian = false; break;
			default: return -1;
			}

			int k;
			for (k = 0; k < m; k++) {
				ut8 tmp[sizeof (ut64)];
				ut8 d1;
				ut16 d2;
				ut32 d3;
				ut64 d4;
				st64 r = r_buf_read (src, tmp, tsize);
				if (r < tsize) {
					return -1;
				}

				switch (tsize) {
				case 1:
					d1 = r_read_ble8 (tmp);
					r = r_buf_write (dst, (ut8 *)&d1, 1);
					break;
				case 2:
					d2 = r_read_ble16 (tmp, bigendian);
					r = r_buf_write (dst, (ut8 *)&d2, 2);
					break;
				case 4:
					d3 = r_read_ble32 (tmp, bigendian);
					r = r_buf_write (dst, (ut8 *)&d3, 4);
					break;
				case 8:
					d4 = r_read_ble64 (tmp, bigendian);
					r = r_buf_write (dst, (ut8 *)&d4, 8);
					break;
				}
				if (r < 0) {
					return -1;
				}
				res += r;
			}
			m = 1;
		}
	}
	return res;
}