// TODO: not finished
// 4.1.11
public class DrawTreeForBFS 
{
    private static int WIDTH = 300;
    private static int HEIGHT = 300;
    private static double px = 1.0 / 300;
    private static double fontpx = 16 * px;

    private boolean[] marked;
    private int[] edgeTo;
    private int N;

    public DrawTreeForBFS(Graph G, int s)
    {
        marked = new boolean[G.V()];
        edgeTo = new int[G.V()];
        N = G.V();
        bfs(G, s);

        drawTable();
        drawGraph();
    }

    private void bfs(Graph G, int s)
    {
        Queue<Integer> queue = new Queue<Integer>();

        queue.enqueue(s);

        while (!queue.isEmpty()) {
            int v = queue.dequeue();
            for (int w : G.adj(v)) {
                if (!marked[w]) {
                    edgeTo[w] = v;
                    marked[w] = true;
                    queue.enqueue(w);
                }
            }
        }
    }

    void drawTable()
    {
        double mid = 45 * px;
        StdDraw.text(mid, 1.0, "edgeTo[]");
        StdDraw.line(mid, 0.0, mid, 1.0 - 1 * fontpx);
        double y = 1.0 - 1.25 * fontpx;

        for (int i = 0; i < edgeTo.length; i++) {
            if (edgeTo[i] != 0) {
                StdDraw.text(mid - 1.0 * fontpx, y, Integer.toString(i));
                StdDraw.text(mid + 1.0 * fontpx, y, Integer.toString(edgeTo[i]));
                y -= 1.25 * fontpx;
            }
        }
    }

    void drawGraph()
    {
        double mid = 0.3;

        vertex(mid + 0.25, 0.25, 3);
        vertex(mid + 0.5, 0.25, 4);

        edge(mid+0.25, 0.25, mid+0.50, 0.25);
    }

    void vertex(double x, double y, int name)
    {
        String text = Integer.toString(name);
        StdDraw.circle(x, y, 16 * px);
        StdDraw.text(x, y, text);
    }

    void edge(double x0, double y0, double x1, double y1)
    {
        // ### subtract radius dist
        StdDraw.line(x0, y0, x1, y1);
    }

    public static void main (String [] args)
    {
        StdDraw.setCanvasSize(WIDTH, HEIGHT);

        String fname = args[0];
        int source = Integer.parseInt(args[1]);

        Graph G = new Graph(new In(fname));
        DrawTreeForBFS dtfbfs = new DrawTreeForBFS(G, source);

    }
}
