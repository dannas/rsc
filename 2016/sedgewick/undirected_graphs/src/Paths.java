public class Paths {
    private boolean[] marked;   // Has dfs() been called for this vertex?
    private int[] edgeTo;       // last vertex on known path to this vertex
    private final int s;        // source

    public Paths(Graph G, int s) {
        marked = new boolean[G.V()];
        edgeTo = new int[G.V()];
        this.s = s;

        dfs(G, s);
    }
    private void dfs(Graph G, int v) {
        marked[v] = true;

        for (int w : G.adj(v)) {
            if (!marked[w]) {
                edgeTo[w] = v;
                dfs(G, v);
            }
        }
    }
    public boolean hasPathTo(int v) {
        return marked[v];
    }

    public Iterable<Integer> pathTo(int v) {

        if (!hasPathTo(v)) return null;

        Stack<Integer> path = new Stack<Integer>();

        for (int x = v; x != s; x = edgeTo[x])
            path.push(x);
        path.push(s);

        return path;
    }

    private static void test(Paths paths, int v) {
        StdOut.printf("hasPathTo(%d) ? %b\n", v, paths.hasPathTo(v));

        if (paths.hasPathTo(v)) {
            for (int w : paths.pathTo(v))
                StdOut.printf("%d ", w);
            StdOut.printf("\n");
        }
    }

    public static void main (String [] args)
    {
        Graph G = new Graph(5);
        G.addEdge(0, 1);
        G.addEdge(0, 2);
        G.addEdge(3, 4);

        Paths paths = new Paths(G, 0);

        for (int i = 0; i < G.V(); i++)
            test(paths, i);
    }
}
