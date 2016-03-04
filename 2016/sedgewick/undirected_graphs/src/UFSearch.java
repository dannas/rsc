class UF {
    private int[] id;   // parent link (site indexed)
    private int[] sz;   // size of component for roots (site indexed)
    private int count;  // number of components

    UF(int N){
        count = N;
        id = new int[N];
        for (int i = 0; i < N; i++)
            id[i] = i;
        sz = new int[N];
        for (int i = 0; i < N; i++)
            sz[i] = 1;
    }

    void union(int p, int q) {
        int i = find(p);
        int j = find(q);

        if (i == j)
            return;

        // Make smaller root point to larger one.
        if (sz[i] < sz[j]) { id[i] = j; sz[j] += sz[i]; }
        else               { id[j] = i; sz[i] += sz[j]; }
        count--;
    }

    int find(int p) {
        // Follow links to find a root.
        while (p != id[p])
            p = id[p];
        return p;
    }

    boolean connected(int p, int q) {
        return find(p) == find(q);
    }

    int count(int s) {
        return sz[find(s)];
    }
}

// Exercise 4.1.8
public class UFSearch {
    private UF uf;
    int s;

    public UFSearch(Graph G, int s) {
        uf = new UF(G.V());
        this.s = s;

        // ### build UF object
        // ### do union() op for each graph edges

        for (int v = 0; v < G.V(); v++) {
            for (Integer w : G.adj(v)) {
                uf.union(v, w);
            }
        }
    }

    public boolean marked(int v) {
        return uf.connected(s, v);
    }

    public int count() {
        return uf.count(s);
    }

    public static void main (String [] args) {
        String fname = args[0];
        int needle = Integer.parseInt(args[1]);
        Graph G = new Graph(new In(fname));
        UFSearch ufs = new UFSearch(G, needle);

        StdOut.printf("%d vertices are connected to %d\n", ufs.count(), needle);
        for (int v = 0; v < G.V(); v++) {
            StdOut.printf("%d %s\n", v, ufs.marked(v) ? "connected" : "not connected");
        }
    }
}
