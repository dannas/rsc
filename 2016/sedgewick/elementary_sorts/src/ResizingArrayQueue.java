import java.util.*;
import java.util.Arrays;

class ResizingArrayQueue<Item>
{
    private Item[] a;
    private int first;
    private int last;
    private int N;

    public ResizingArrayQueue()
    {
        a = (Item[]) new Object[2];
        first = last = N = 0;
    }

    public void enqueue(Item item)
    {
        if (N == a.length)
            resize(2 * a.length);

        a[last++] = item;

        if (last == a.length)
            last = 0;               // wrap around

        N++;
    }

    public Item dequeue()
    {
        if (first == last)
            throw new NoSuchElementException();

        Item item = a[first];
        a[first] = null;        // to avoid loitering
        N--;
        first++;
        if (first == a.length)
            first = 0;          // wrap around

        if (N < a.length / 4)
            resize(a.length / 2);

        return item;
    }

    public boolean isEmpty()
    {
        return N == 0;
    }

    public int size()
    {
        return N;
    }

    @Override
    public String toString()
    {
        if (a.length == 0)
            return "";

        StringBuilder sb = new StringBuilder("[");

        for (int i = 0; i < a.length-1; i++) {
            if (i == first)
                sb.append("f, ");
            else if (i == last)
                sb.append("l, ");
            else if (a[i] == null)
                sb.append("null, ");
            else
                sb.append(" , ");
        }
        if (a.length-1 == first)
            sb.append("f]");
        else if (a.length-1 == last)
            sb.append("l]");
        else if (a[a.length-1] == null)
            sb.append("null]");
        else
            sb.append(" ]");
        return sb.toString();
    }

    private void resize(int capacity)
    {
        assert capacity >= N;
        Item[] copy = (Item[]) new Object[capacity];
        for (int i = 0; i < N; i++)
            copy[i] = a[first+i % a.length];
        a = copy;
        first = 0;
        last = N;
    }
    public static void main (String [] args)
    {
        ResizingArrayQueue<Integer> queue = new ResizingArrayQueue<Integer>();

        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            if (s.equals("-"))
                queue.dequeue();
            else
                queue.enqueue(Integer.parseInt(s));
            StdOut.println(queue);
        }
    }
}
