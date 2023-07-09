int firstBitSet(int x)
{
        int position=0;
        while (x!=0)
        {
                x>>=1;
                ++position;
        }
        return position;
}