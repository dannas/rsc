class PerfectHash 
{
    public static void main (String [] args)
    {
        char[] letters = "SEARCHXMPL".toCharArray();

        for (int a = 20; a >= 0; a--) {
            for (int m = 20; m >= 0; m--) {
                ST<Character, Integer> st = new ST<Character, Integer>();
                boolean perfect = true;
                for (Character c : letters)
                    if (st.get(c) != null)
                        perfect = false;
                if (perfect)
                    StdOut.printf("a=%d M=%d\n", a, m);
            }
        }
    }
}
