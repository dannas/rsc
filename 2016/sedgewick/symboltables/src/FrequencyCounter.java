public class FrequencyCounter 
{
    public static void main (String [] args)
    {
        int minlen = Integer.parseInt(args[0]);

        ST<String, Integer> st = new ST<String, Integer>();

        while (!StdIn.isEmpty()) {
            String word = StdIn.readString();
            if (word.length() > minlen) {
                if (!st.contains(word))
                    st.put(word, 1);
                else
                    st.put(word, st.get(word) + 1);
            }
        }
        String max = "";
        for (String word : st.keys())
            if (st.get(word) > st.get(max))
                max = word;
        StdOut.print(max + " " + st.get(max));
    }
}
