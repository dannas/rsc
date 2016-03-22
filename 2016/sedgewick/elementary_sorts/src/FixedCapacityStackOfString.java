import java.util.Arrays;
import java.util.*;

class FixedCapacityStackOfString implements Iterable<String>
{
    private String[] arr;
    private int N;

    public FixedCapacityStackOfString(int N)
    {
        arr = new String[N];
        N = 0;
    }

    public String pop()
    {
        if (N == 0)
            throw new NoSuchElementException();

        String item = arr[--N];
        arr[N] = null;
        return item;
    }

    public void push(String item)
    {
        if (N == arr.length)
            return; // TODO: throw exception

        arr[N++] = item;
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
        return Arrays.toString(arr);
    }

    public Iterator<String> iterator()
    {
        return new ReverseIterator();
    }

    public class ReverseIterator implements Iterator<String>
    {
        int i = N-1;

        public boolean hasNext()
        {
            return i > 0;
        }

        public String next()
        {
            return arr[i--];
        }

        public void remove()
        {
            throw new UnsupportedOperationException();
        }
    }

    public static void main (String [] args)
    {
        int N = Integer.parseInt(args[0]);
        FixedCapacityStackOfString stack = new FixedCapacityStackOfString(N);

        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            if (s.equals("-"))
                StdOut.print(stack.pop());
            else
                stack.push(s);
        }

        StdOut.printf("\n%s\n", stack);
    }
}
