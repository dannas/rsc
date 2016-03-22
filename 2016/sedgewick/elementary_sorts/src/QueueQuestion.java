class QueueQuestion 
{
    public static boolean ok(String exp)
    {
        String alphabet = "ABCDEFGHIJ";
        int j = 0;
        Queue<Character> q = new Queue<Character>();

        for (int i = 0; i < alphabet.length(); i++) {
            q.enqueue(alphabet.charAt(i));
            StdOut.println(q);

            StdOut.printf("next=%c\n", exp.charAt(j), q.peek());
            while (!q.isEmpty() && j < exp.length() && q.peek().equals(exp.charAt(j))) {
                StdOut.printf("dequeue=%c\n", q.dequeue());
                j++;
            }
        }
        return j == exp.length();
    }

    public static void main (String [] args)
    {
        String s = "";
        while (!StdIn.isEmpty())
            s += StdIn.readString();
        StdOut.println(ok(s));
    }
}
