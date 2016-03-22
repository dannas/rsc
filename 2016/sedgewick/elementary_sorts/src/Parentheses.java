class Parentheses 
{
    public static boolean isBalanced(String s)
    {
        Stack<Character> stack = new Stack<Character>();
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) ==          '[')
                stack.push('[');
            else if (s.charAt(i) ==     '{')
                stack.push('{');
            else if (s.charAt(i) ==     '(')
                stack.push('(');
            else if (s.charAt(i) ==     ']') {
                if (stack.isEmpty())    return false;
                if (stack.pop() != '[') return false;
            } else if (s.charAt(i) ==   '}') {
                if (stack.isEmpty())    return false;
                if (stack.pop() != '{') return false;
            } else if (s.charAt(i) ==   ')') {
                if (stack.isEmpty())    return false;
                if (stack.pop() != '(') return false;
            }
        }
        return stack.isEmpty();
    }

    public static void main (String [] args)
    {
        In in = new In();
        String s = in.readAll().trim();
        StdOut.print(isBalanced(s));
    }
}
