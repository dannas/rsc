import java.util.*;
import java.util.Arrays;

class ResizingArrayStack<Item>
{
    private Item[] a;
    private int N;

    public ResizingArrayStack()
    {
        a = (Item[]) new Object[2];
        N = 0;
    }

    public Item pop()
    {
        if (N == 0)
            throw new NoSuchElementException();

        if (N < a.length / 4)
            resize(a.length / 2);

        Item item = a[--N];
        a[N] = null;
        return item;
    }

    public void push(Item item)
    {
        if (N == a.length)
            resize(2 * a.length);
        a[N++] = item;
    }

    private void resize(int capacity)
    {
        Item[] copy = (Item[]) new Object[capacity];
        for (int i = 0; i < N; i++)
            copy[i] = a[i];
        a = copy;
    }

    @Override
    public String toString()
    {
        return Arrays.toString(a);
    }

    public static void main (String [] args)
    {
        ResizingArrayStack<Integer> stack = new ResizingArrayStack<Integer>();

        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            if (s.equals("-"))
                stack.pop();
            else
                stack.push(Integer.parseInt(s));
            StdOut.println(stack);
        }
    }
}
