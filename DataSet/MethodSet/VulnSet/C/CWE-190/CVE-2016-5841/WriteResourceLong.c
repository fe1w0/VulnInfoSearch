}static inline void WriteResourceLong(unsigned char *p,
  const unsigned int quantum)
{
  unsigned char
    buffer[4];

  buffer[0]=(unsigned char) (quantum >> 24);
  buffer[1]=(unsigned char) (quantum >> 16);
  buffer[2]=(unsigned char) (quantum >> 8);
  buffer[3]=(unsigned char) quantum;
  (void) CopyMagickMemory(p,buffer,4);
}