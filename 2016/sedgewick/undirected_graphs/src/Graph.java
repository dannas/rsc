public class Graph {
    private final int V;  // number of verticies
    private int E;        // number of edges
    private Bag<Integer>[] adj;

    public Graph(int V) {
        this.V = V;
        this.E = 0;
        this.adj = (Bag<Integer>[]) new Bag[V];
        for (int i = 0; i < V; i++)
            this.adj[i] = new Bag();
    }

    public Graph(In in) {
        this(in.readInt());         // read V and construct this graph
        int E = in.readInt();
        for (int i = 0; i < E; i++) {
            int v = in.readInt();
            int w = in.readInt();
            addEdge(v, w);
        }
    }

    // copy ctor. Exercise 4.1.3
    public Graph(Graph G) {
        this(G.V());
        for (int v = 0; v < G.V(); v++) {
            for (Integer w : adj[v])
                adj[v].add(w);          // ### Bag does not guarentee order
        }
        E = G.E();
    }

    public int V() { return V; }
    public int E() { return E; }

    public Iterable<Integer> adj(int v) {
        return adj[v];
    }

    public void addEdge(int v, int w) {

        // disallow self-loops and paralell edges
        // Exercise 4.1.5
        if (v == w)        return;
        if (hasEdge(v, w)) return;

        adj[v].add(w);
        adj[w].add(v);
        E++;
    }

    // Exercise 4.1.4
    public boolean hasEdge(int v, int w) {
        for (Integer s : adj[v])
            if (s == w)
                return true;
        return false;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();

        for (int v = 0; v < V; v++) {
            sb.append(v).append(" : ");
            for (int edge : adj(v))
                sb.append(edge).append(" ");
            sb.append("\n");
        }
        return sb.toString();
    }

    public static void main (String [] args)
    {
        Graph g = new Graph(new In(args[0]));

        StdOut.print(g);
    }
}
