import java.util.*;

class FixedCapacityStack<Item>
{
    private Item[] a;
    private int N;

    public FixedCapacityStack(int N)
    {
        this.N = 0;
        a = (Item[]) new Object[N];
    }

    public Item pop()
    {
        if (N == 0)
            throw new NoSuchElementException();
        Item item = a[--N];
        a[N] = null;
        return item;
    }

    public void push(Item item)
    {
        if (N == a.length)
            return; // TODO: throw exception

        a[N++] = item;
    }

    public static void main (String [] args)
    {
        int N = Integer.parseInt(args[0]);
        FixedCapacityStack<Integer> stack = new FixedCapacityStack<Integer>(N);

        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            if (s.equals("-"))
                StdOut.print(stack.pop());
            else
                stack.push(Integer.parseInt(s));
        }
    }
}
