class SeparateChainingHashST<Key, Value>
{
    private int N;  // number of key-value pairs
    private int M;  // hash table size
    private SequentialSearchST<Key, Value>[] st;

    public SeparateChainingHashST()
    {
        this(997);
    }

    public SeparateChainingHashST(int M)
    {
        this.M = M;
        st = (SequentialSearchST<Key, Value>[]) new SequentialSearchST[M];

        for (int i = 0; i < M; i++)
            st[i] = new SequentialSearchST();
    }

    private int hash(Key key)
    {
        return (key.hashCode() & 0x7fffffff) % M;
    }

    public Value get(Key key)
    {
        return (Value) st[hash(key)].get(key);
    }
    
    public void put(Key key, Value val)
    {
        st[hash(key)].put(key, val);
    }

    public static void main (String [] args)
    {
        SeparateChainingHashST<Character, Integer> st = new
            SeparateChainingHashST<Character, Integer>(5);

        char[] a = "EASYQUESTION".toCharArray();

        for (char c : a)
            st.put(c, 0);
    }
}
