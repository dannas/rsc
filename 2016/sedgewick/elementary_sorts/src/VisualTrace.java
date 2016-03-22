
public class VisualTrace {
    static int off = 0;
    public static void sort(Integer[] a) {
        int N = a.length;

        for (int i = 0; i < N; i++) {
            int j;
            for (j = i; j > 0 && less(a[j], a[j-1]); j--) {
                exch(a, j, j-1);
            }
            show(a, j, i);
        }
    }

    public static boolean less(Comparable v, Comparable w) {
        return v.compareTo(w) < 0;
    }

    public static void exch(Comparable[] a, int i, int j) {
        Comparable t = a[i];
        a[i] = a[j];
        a[j] = t;
    }

    public static void show(Integer[] a, int lo, int hi) {
        int N = a.length;

        for (int i = 0; i < N; i++) {
            double x = i;
            double y = a[i]/2.0;
            double rw = 0.37;
            double rh = a[i]/2.0;

            if (i == lo)
                StdDraw.setPenColor(StdDraw.RED);
            else if (i > lo && i <= hi)
                StdDraw.setPenColor(StdDraw.BLACK);
            else
                StdDraw.setPenColor(StdDraw.GRAY);
            StdDraw.filledRectangle(x, y, rw, rh);
        }

        off = off + 2 * N;
        StdDraw.setYscale(-2*N*N + off, N+off);
    }

    public static void main(String [] args) {
        int N = Integer.parseInt(args[0]);
        Integer[] a = new Integer[N];

        for (int i = 0; i < N; i++)
            a[i] = (int)(StdRandom.random() * N);

        StdDraw.setXscale(0, 3*N);
        StdDraw.setYscale(-2*N*N, N);

        sort(a);
        StdDraw.show();
    }
}
