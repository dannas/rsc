public class FrequencyCountFromDictionary 
{
    public static void main (String [] args)
    {
        In in = new In(args[0]);

        ST<String, Integer> dict = new ST<String, Integer>();
        while (!in.isEmpty()) {
            String word = in.readString();
            dict.put(word, 1);
        }

        ST<String, Integer> st = new ST<String, Integer>();
        while (!StdIn.isEmpty()) {
            String word = StdIn.readString();
            if (dict.contains(word)) {
                if (st.contains(word))
                    st.put(word, st.get(word) + 1);
                else
                    st.put(word, 1);
            }
        }

        ST<Integer, String> freq = new ST<Integer, String>();
        for (String word : st.keys())
            freq.put(st.get(word), word);

        Stack<String> stack = new Stack<String>();
        for (Integer count : freq.keys())
            stack.push(freq.get(count));

        for (String word : stack)
            StdOut.println(word + " " + st.get(word));

    }
}
