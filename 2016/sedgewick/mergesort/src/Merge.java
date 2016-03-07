public class Merge 
{
    private static Comparable[] aux;
    private static double N;
    private static double row;

    private static void merge(Comparable[] a, int lo, int mid, int hi)
    {
        int i = lo, j = mid+1;

        for (int k = lo; k <= hi; k++)
            aux[k] = a[k];

        for (int k = lo; k <= hi; k++) {
            if      (i > mid)               a[k] = aux[j++];
            else if (j > hi)                a[k] = aux[i++];
            else if (less(aux[j], aux[i]))  a[k] = aux[j++];
            else                            a[k] = aux[i++];
            //draw(a, k, i, j, aux);
        }
    }

    public static void sort(Comparable[] a)
    {
        aux = new Comparable[a.length]; // Allocate space just once.
        sort (a, 0, a.length-1);
    }

    public static void sort(Comparable[] a, int lo, int hi)
    {
        if (hi <= lo)
            return;

        int mid = lo + (hi - lo) / 2;

        sort(a, lo, mid);
        sort(a, mid+1, hi);

        merge(a, lo, mid, hi);
    }

    private static boolean less(Comparable v, Comparable w)
    {
        return v.compareTo(w) < 0;
    }

    private static boolean isSorted(Comparable[] a, int lo, int hi)
    {
        for (int i = lo+1; i <= hi; i++)
            if (less(a[i], a[i-1]))
                return false;
        return true;
    }

    private static void draw(Comparable[] a, int k, int i, int j, Comparable[] aux)
    {
        String[] s = (String[]) a;
        for (int t = 0; t < k; t++) {
            double x = t / N;
            double y = row;
            StdOut.printf("x=%f y=%f\n", x, y);
            StdDraw.text(x, y, s[t]);
        }
        row -= 1.0/N;
    }

    public static void main (String [] args)
    {
        String[] a = StdIn.readStrings();
        N = a.length;
        row = 1.0;

        sort(a);
        StdDraw.show();
    }

}
