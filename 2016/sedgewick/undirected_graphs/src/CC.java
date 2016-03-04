public class CC 
{
    private boolean[] marked;
    private int[] id;
    private int count;

    public CC(Graph G) {
        marked = new boolean[G.V()];
        id = new int[G.V()];
        count = 0;

        for (int s = 0; s < G.V(); s++) {
            if (!marked[s]) {
                dfs(G, s);
                count++;
            }
        }
    }

    private void dfs(Graph G, int v) {
        marked[v] = true;
        id[v] = count;

        for (int w : G.adj(v)) {
            if (!marked[w]) {
                dfs(G, w);
            }
        }
    }

    public boolean connected(int v, int w) {
        return id[v] == id[w];
    }

    public int id(int v) {
        return id[v];
    }

    public int count() {
        return count;
    }

    public static void main (String [] args)
    {
        Graph G = new Graph(new In(args[0]));

        CC cc = new CC(G);

        StdOut.printf("%d components\n", cc.count());

        for (int id = 0; id < cc.count(); id++) {
            for (int v = 0; v < G.V(); v++) {
                if (cc.id[v] == id)
                    StdOut.printf("%d ", v);
            }
            StdOut.printf("\n");
        }
    }
}
