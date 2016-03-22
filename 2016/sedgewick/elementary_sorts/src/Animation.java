import java.util.Arrays;

public class Animation {
    public static void sort(Integer[] a) {
        int N = a.length;

        for (int i = 0; i < N; i++) {
            for (int j = i; j > 0 && less(a[j], a[j-1]); j--) {
                exch(a, j, j-1);
                show(a, j-1, i);
            }
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

        StdDraw.clear();

        for (int i = 0; i < N; i++) {
            double x = i;
            double y = a[i]/2.0;
            double rw = 0.5;
            double rh = a[i]/2.0;
            if (i == lo)
                StdDraw.setPenColor(StdDraw.RED);
            else if (i > lo && i <= hi)
                StdDraw.setPenColor(StdDraw.BLACK);
            else
                StdDraw.setPenColor(StdDraw.GRAY);
            StdDraw.filledRectangle(x, y, rw, rh);
        }
        StdDraw.show(1);
    }

    public static void main(String [] args) {
        int N = Integer.parseInt(args[0]);
        Integer[] a = new Integer[N];

        for (int i = 0; i < N; i++)
            a[i] = (int)(StdRandom.random() * N);

        StdDraw.setXscale(0, N);
        StdDraw.setYscale(0, N);

        sort(a);
    }
}
