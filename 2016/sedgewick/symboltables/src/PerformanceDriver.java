public class PerformanceDriver 
{
    public static void main (String [] args)
    {
        int N = Integer.parseInt(args[0]);

        char[] a = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz".toCharArray();
        BinarySearchST<String, Integer> st = new BinarySearchST<String, Integer>(N);

        for (int i = 0; i < N; i++) {
            int x = StdRandom.uniform(2, 50);
            st.put(new String(a, 0, x), i);
        }

        for (int i = 0; i < N; i++) {
        }
    }
}
