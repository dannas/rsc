public class DepthFirstSearch {
    private int count;
    private boolean[] marked;

    DepthFirstSearch(Graph G, int s) {
        dfs(G, s);
    }

    private void dfs(Graph G, int v) {
        marked[v] = true;
        count++;

        for (int w : G.adj(v)) {
            if (!marked[w])
                dfs(G, w);
        }
    }

    private int count() {
        return count;
    }

    public static void main (String [] args)
    {
        /* code */
    }
}
