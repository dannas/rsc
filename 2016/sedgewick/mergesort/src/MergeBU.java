import java.util.Arrays;

public class MergeBU {
    private static Comparable[] aux;

    private static void merge(Comparable[] a, int lo, int mid, int hi)
    {
        StdOut.printf("merge(%d, %d, %d)\n", lo, mid, hi);
        int i = lo, j = mid+1;

        for (int k = lo; k <= hi; k++)
            aux[k] = a[k];

        for (int k = lo; k <= hi; k++) {
            if      (i > mid)           a[k] = aux[j++];
            else if (j > hi)            a[k] = aux[i++];
            else if (less(a[j], a[i]))  a[k] = aux[j++];
            else                        a[k] = aux[i++];
        }
    }

    private static boolean less(Comparable v, Comparable w)
    {
        return v.compareTo(w) < 0;
    }

    public static void sort(Comparable[] a)
    {
        int N = a.length;
        aux = new Comparable[N];
        for (int sz = 1; sz < N; sz = sz+sz)        // sz: subarray size
            for (int lo = 0; lo < N-sz; lo += sz+sz) // lo: subarray index
                merge(a, lo, lo+sz-1, Math.min(lo+sz+sz-1, N-1));
    }

    public static void main(String[] args)
    {
        Double[] a = new Double[20];

        for (int i = 0; i < 20; i++)
            a[i] = StdRandom.uniform();

        sort(a);
        StdOut.println(Arrays.toString(a));
    }
}
