import java.util.Arrays;

class Insertion 
{
    public static void sort(Comparable[] a) {
        for (int i = 0; i < a.length; i++) {
            int j;
            for (j = i; j > 0 && less(a[j], a[j-1]); j--)
                exch(a, j, j-1);
            assert isSorted(a, i);
            //show(a, j, j, i);
        }
        assert isSorted(a, a.length);
    }
    private static boolean less(Comparable v, Comparable w) {
        return v.compareTo(w) < 0;
    }

    private static void exch(Comparable[] a, int i, int j) {
        Comparable swap = a[i];
        a[i] = a[j];
        a[j] = swap;
    }

    private static boolean isSorted(Comparable[] a, int N) {
        for (int i = 1; i < N; i++)
            if (less(a[i], a[i-1]))
                return false;
        return true;
    }

    private static void show(Comparable[] a, int min, int fc, int lc)
    {
        Double[] v = (Double[]) a;
        int N = v.length;
        StdDraw.show(0);
        StdDraw.clear();
        for (int i = 0; i < N; i++) {
            StdDraw.setPenColor(StdDraw.GRAY);
            if (i >= fc && i <= lc)
                StdDraw.setPenColor(StdDraw.BLACK);
            if (i == min)
                StdDraw.setPenColor(StdDraw.RED);
            double x = 1.0*i/N;
            double y = v[i]/(2.0*10);
            double rw =  1/(2.0*N);
            double rh = v[i]/(2.0*10);
            StdDraw.filledRectangle(x, y, rw, rh);
        }
        StdDraw.show(600);
    }

    public static void main (String [] args) {
        int N = 50;
        Double[] a = new Double[N];

        for (int i = 0; i < N; i++)
            a[i] = StdRandom.random() * 10.0;
        sort(a);
    }
}
