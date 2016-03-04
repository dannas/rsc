// TODO: Not finished

public class DetailedTraceDfs {
    private static int VERTICAL = 400;
    private static int NROWS =  5;

    private boolean[] marked;
    private double[] xpos;
    private double[] ypos;

    private double row;
    private double col;

   DetailedTraceDfs(Graph G, int s) {
       marked = new boolean[G.V()];

       xpos = new double[] {.4, .5, .5, .6, .6, .4, .6, .4, .5, .5, .5, .4};
       ypos = new double[] {.9, .2, .7, .6, .3, .5, .9, .4, .4, .1, .5, .2};

       row = 0.0;
       col = 0.0;

       dfs(G, s);
   }

   public void dfs(Graph G, int v) {
        draw(G);
        marked[v] = true;
        for (int w : G.adj(v))
            if (!marked[w])
                dfs(G, w);
   }

   public void draw(Graph G) {

       StdDraw.text(col, row, "dfs(0)");


       for (int v = 0; v < G.V(); v++) {
           vertex(xpos[v], ypos[v] + row, v, marked[v]);
           for (int w : G.adj(v))
               edge(xpos[v], ypos[v] + row, xpos[w], ypos[w] + row);
       }
       row++;
       col += 0.02;
   }

   public static void vertex(double x, double y, int name, boolean marked) {
       String text = Integer.toString(name);

       if (marked)
            StdDraw.circle(x, y, .04);
       else {
            StdDraw.setPenColor(StdDraw.LIGHT_GRAY);
            StdDraw.filledCircle(x, y, .04);
            StdDraw.setPenColor(StdDraw.BLACK);
       }
       StdDraw.text(x, y, text);
   }

   public static void edge(double x0, double y0, double x1, double y1) {
       // How draw two paralell lines?
       StdDraw.line(x0, y0, x1, y1);
   }

   public static void main (String [] args) {

       StdDraw.setCanvasSize(800, NROWS * VERTICAL);
       StdDraw.setXscale(0, 1);
       StdDraw.setYscale(NROWS, 0);

       String fname = args[0];
       Graph G = new Graph(new In(fname));

       DetailedTraceDfs dfs = new DetailedTraceDfs(G, 0);

   }
}
