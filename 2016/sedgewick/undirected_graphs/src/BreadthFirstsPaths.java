public class BreadthFirstsPaths
{
    private boolean[] marked;   // Is a shortest path to this vertex known?
    private int[] edgeTo;       // last vertex on known path to this vertex
    private int[] distTo;
    private final int s;        // source

    public BreadthFirstsPaths(Graph G, int s)
    {
        marked = new boolean[G.V()];
        edgeTo = new int[G.V()];
        distTo = new int[G.V()];
        this.s = s;
        bfs(G, s);
    }

    private void bfs(Graph G, int s)
    {
        Queue<Integer> queue = new Queue<Integer>();
        marked[s] = true;
        distTo[s] = 0;
        queue.enqueue(s);
        while (!queue.isEmpty()) {
            int v = queue.dequeue();
            for (int w : G.adj(v)) {
                if (!marked[w]) {
                    edgeTo[w] = v;
                    distTo[w] = distTo[v] + 1;
                    marked[w] = true;
                    queue.enqueue(w);
                }
            }
        }
    }

    public boolean hasPathTo(int v)
    {
        return marked[v];
    }

    public Iterable<Integer> pathTo(int v)
    {
        if (!hasPathTo(v))
            return null;

        Stack<Integer> path = new Stack<Integer>();
        for (int x = v; x != s; x = edgeTo[x])
            path.push(x);
        path.push(s);

        return path;
    }

    // number of edges on the shortest path from the source to a given vertex.
    // should run in constant time
    public int distTo(int v)
    {
        return distTo[v];
    }

    public static void main (String [] args)
    {
        String filename = args[0];
        int source = Integer.parseInt(args[1]);
        Graph G = new Graph(new In(filename));

        BreadthFirstsPaths bfp = new BreadthFirstsPaths(G, source);

        for (int v = 0; v < G.V(); v++) {
            if (bfp.hasPathTo(v)) {
                StdOut.printf("[%d] ", bfp.distTo(v));
                for (int w : bfp.pathTo(v))
                    StdOut.printf("%d ", w);
                StdOut.println();
            } else {
                StdOut.printf("no path from %d\n", v);
            }
        }
    }
}
