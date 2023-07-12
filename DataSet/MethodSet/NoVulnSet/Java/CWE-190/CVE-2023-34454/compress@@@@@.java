public class Snappy
{
    public static byte[] compress(float[] input)
            throws IOException
    {
        int byteSize = input.length * 4;
        if (byteSize < input.length) {
            throw new SnappyError(SnappyErrorCode.TOO_LARGE_INPUT, "input array size is too large: " + input.length);
        }
        return rawCompress(input, byteSize); // float uses 4 bytes
    }
}