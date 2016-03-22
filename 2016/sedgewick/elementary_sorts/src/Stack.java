class Stack<Item>
{
    private Node first;

    public Stack()
    {
        first = null;
    }

    public void push(Item item)
    {
        Node old = first;
        first = new Node();
        first.item = item;
        first.next = old;
    }

    public Item pop()
    {
        if (first == null)
            throw new RuntimeException("stack is empty");

        Item item = first.item;
        first = first.next;
        return item;
    }

    public Item top()
    {
        if (first == null)
            throw new RuntimeException("stack is empty");
        return first.item;
    }

    public boolean isEmpty()
    {
        return first == null;
    }

    private class Node
    {
        Item item;
        Node next;
    }

    @Override
    public String toString()
    {
        StringBuilder sb = new StringBuilder("[");
        for (Node p = first; p != null; p = p.next) {
            if (p == first)
                sb.append(p.item);
            else sb.append(" ,").append(p.item);
        }
        return sb.append("]").toString();
    }

    public static void main (String [] args)
    {
        Stack<String> stack = new Stack<String>();
        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            if (s.equals("-"))
                stack.pop();
            else
                stack.push(s);
            StdOut.println(stack);
        }
    }
}
