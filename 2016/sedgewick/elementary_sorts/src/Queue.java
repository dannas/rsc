class Queue<Item>
{
    private class Node
    {
        Item item;
        Node next;
    }

    private Node first;
    private Node last;
    private int N;

    public Queue()
    {
        first = last = null;
        N = 0;
        assert check();
    }

    public void enqueue(Item item)
    {
        Node oldLast = last;
        last = new Node();
        last.item = item;
        last.next = null;
        if (isEmpty())
            first = last;
        else
            oldLast.next = last;
        N++;
        assert check();
    }

    public Item dequeue()
    {
        if (isEmpty())
            throw new RuntimeException("empty queue");

        Item item = first.item;
        first = first.next;
        N--;
        if (isEmpty())
            last = null;    // to avoid loitering
        assert check();
        return item;
    }

    public Item peek()
    {
        if (isEmpty())
            throw new RuntimeException("empty queue");

        return first.item;
    }

    private boolean check()
    {
        if (N == 0)
            if (first != null || last != null) return false;
        else if (N == 1) {
            if (first != last) return false;
            if (first == null) return false;
            if (first.next != null) return false;
            if (last.next != null) return false;
        } else {
            int numberOfNodes = 0;
            for (Node p = first; p != null; p = p.next)
                numberOfNodes++;
            if (N != numberOfNodes) return false;
        }
        return true;
    }

    public boolean isEmpty()
    {
        return N == 0;
    }

    @Override
    public String toString()
    {

        StringBuilder sb = new StringBuilder();
        for (Node p = first; p != null; p = p.next) {
            if (p == first)
                sb.append(p.item);
            else
                sb.append(", ").append(p.item);
        }
        return sb.toString();
    }

    public static void main (String [] args)
    {
        Queue<String> queue = new Queue<String>();

        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            if (s.equals("-"))
                queue.dequeue();
            else
                queue.enqueue(s);
            StdOut.println(queue);
        }
    }
}
