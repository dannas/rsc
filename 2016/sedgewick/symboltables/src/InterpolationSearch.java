class InterpolationSearch<Key extends Comparable<Key>, Value>
{
    private Key[] keys;
    private Value[] vals;
    private int N;

    public InterpolationSearch(int size)
    {
        keys = (Key[]) new Comparable[size];
        vals = (Value[]) new Object[size];
        N = 0;
    }

    public void put(Key key, Value val)
    {
        // if exists => replace
        // else insert and rearrange
    }

    public Value get(Key key)
    {
        // if found => return val
        // else return null
    }

    private int  rank(Key key, int pos)
    {
    }

    private int rank(Key key)
    {
    }
}
