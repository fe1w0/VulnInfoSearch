inline bool SegmentReader::amplifiedRead(WordCount virtualAmount) {
  return readLimiter->canRead(virtualAmount, arena);
}