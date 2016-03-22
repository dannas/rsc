class StackQuestion
{
    public static boolean ok(String exp)
    {
        String alphabet = "ABCDEFGHIJ";
        int j = 0;
        Stack<Character> stack = new Stack<Character>();

        for (int i = 0; i < alphabet.length(); i++) {
            stack.push(alphabet.charAt(i));
            StdOut.println(stack);

            StdOut.printf("next=%c\n", exp.charAt(j));
            while (!stack.isEmpty() && j < exp.length() && stack.top().equals(exp.charAt(j))) {
                StdOut.printf("pop=%c\n", stack.pop());
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
