public class SymbolGraph 
{
    private ST<String, Integer> st;
    private String[] keys;
    private Graph G;

    public SymbolGraph(String fname, String delim) {
        st = new ST<String, Integer>();

        In in = new In(fname);

        while (in.hasNextLine()) {
            String[] a = in.readLine().split(delim);

            for (int i = 0; i < a.length; i++)
                if (!st.contains(a[i]))
                    st.put(a[i], st.size());
        }

        keys = new String[st.size()];

        for (String name : st.keys())
            keys[st.get(name)] = name;

        G = new Graph(st.size());
        in = new In(fname);

        while (in.hasNextLine()) {
            String[] a = in.readLine().split(delim);
            int v = st.get(a[0]);

            for (int i = 1; i < a.length; i++)
                G.addEdge(v, st.get(a[i]));
        }
    }

    public boolean contains(String key) {
        return st.contains(key);
    }

    public int index(String key) {
        return st.get(key);
    }

    public String name(int v) {
        return keys[v];
    }

    public Graph G() {
        return G;
    }

    public static void main (String [] args)
    {
        String fname = args[0];
        String delim = args[1];

        SymbolGraph sg = new SymbolGraph(fname, delim);

        Graph G = sg.G();

        for (int v = 0; v < G.V(); v++) {
            StdOut.println(sg.name(v));
            for (Integer w : G.adj(v))
                StdOut.printf("\t%s\n", sg.name(w));
        }
    }
}
