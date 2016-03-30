class SeparateChainingHashLinkedListST<Key, Value>
{
    class Node<Key, Value>
    {
        Key key;
        Value val;
        Node next;

        Node(Key key, Value val, Node next)
        {
            this.key = key;
            this.val = val;
            this.next = next;
        }
    }

    private int N;  // number of key-value pairs
    private int M;  // size of table
    private Node<Key, Value>[] st;

    public SeparateChainingHashLinkedListST()
    {
        this(997);
    }

    public SeparateChainingHashLinkedListST(int M)
    {
        this.M = M;
        st = (Node<Key, Value>[]) new Node[M];

        for (int i = 0; i < M; i++)
            st[i] = null;
    }

    public int hash(Key key)
    {
        return (key.hashCode() & 0x7fffffff) % M;
    }

    public Value get(Key key)
    {
        Node first = st[hash(key)];

        for (Node x = first; x != null; x = x.next)
            if (key.equals(x.key))
                return (Value) x.val;
        return null;
    }

    private void delete(Key key)
    {
        Node first = st[hash(key)];
        Node prev = first;

        if (first == null)
            return;

        if (key.equals(first.key))
            st[hash(key)] = null;

        for (Node x = first; x != null; x = x.next) {
            if (key.equals(x.key)) {
                prev.next = x.next;
                break;
            }
            prev = x;
        }
        N--;
    }

    public void put(Key key, Value val)
    {
        Node first = st[hash(key)];

        if (first == null) {
            st[hash(key)] = new Node(key, val, null);
        } else {
            for (Node x = first; x != null; x = x.next)
                if (key.equals(x.key)) { x.val = val; return; }
            first = new Node(key, val, first);
        }
        N++;
    }

    public static void main (String [] args)
    {
        SeparateChainingHashLinkedListST<String, Integer> st =
            new SeparateChainingHashLinkedListST<String, Integer>();
    }
}
