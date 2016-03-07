public class Improvements {
    private static Comparable[] aux;
    private static double N;

    private static void merge(Comparable[] a, int lo, int mid, int hi) {
        int i = lo, j = mid+1;

        for (int k = lo; k <= hi; k++)
            aux[k] = a[k];

        for (int k = lo; k <= hi; k++) {
            if      (i > mid)               a[k] = aux[j++];
            else if (j > hi)                a[k] = aux[i++];
            else if (less(aux[j], aux[i]))  a[k] = aux[j++];
            else                            a[k] = aux[i++];
        }
    }

    public static void sort(Comparable[] a) {
        aux = new Comparable[a.length]; // Allocate space just once.
        sort (a, 0, a.length-1);
    }

    public static void sort(Comparable[] a, int lo, int hi) {
        if (hi <= lo)
            return;

        int mid = lo + (hi - lo) / 2;

        sort(a, lo, mid);
        sort(a, mid+1, hi);

        merge(a, lo, mid, hi);
    }

    public static void isort(Comparable[] a, int lo, int hi) {
        for (int i = lo; i <= hi; i++)
            for (int j = i; j > lo && less(a[j], a[j-1]); j--)
                exch(a, j, j-1);
    }

    public static void sortWithCutoff(Comparable[] a) {
        aux = new Comparable[a.length]; // Allocate space just once.
        sort (a, 0, a.length-1);
    }

    public static void sortWithCutoff(Comparable[] a, int lo, int hi) {
        if (hi - lo <= 15) {
            isort(a, lo, hi);
            return;
        }

        int mid = lo + (hi - lo) / 2;

        sort(a, lo, mid);
        sort(a, mid+1, hi);

        merge(a, lo, mid, hi);
    }

    public static void sortWithOrderCheck(Comparable[] a) {
        aux = new Comparable[a.length]; // Allocate space just once.
        sort (a, 0, a.length-1);
    }

    public static void sortWithOrderCheck(Comparable[] a, int lo, int hi) {
        if (hi <= lo)
            return;

        int mid = lo + (hi - lo) / 2;

        sort(a, lo, mid);
        sort(a, mid+1, hi);

        if (less(a[mid+1], a[mid+1]))
            merge(a, lo, mid, hi);
    }

    public static void sortCutoffOrder(Comparable[] a) {
        aux = new Comparable[a.length]; // Allocate space just once.
        sort (a, 0, a.length-1);
    }

    public static void sortCutoffOrder(Comparable[] a, int lo, int hi) {
        if (hi - lo <= 15) {
            isort(a, lo, hi);
            return;
        }

        int mid = lo + (hi - lo) / 2;

        sort(a, lo, mid);
        sort(a, mid+1, hi);

        if (less(a[mid+1], a[mid+1]))
            merge(a, lo, mid, hi);
    }

    public static void sortThatAvoidsCopies(Comparable[] a) {
        aux = new Comparable[a.length]; // Allocate space just once.
        sort (a, 0, a.length-1);
    }

    public static void sortThatAvoidsCopies(Comparable[] a, int lo, int hi) {
        if (hi <= lo)
            return;

        int mid = lo + (hi - lo) / 2;

        sort(a, lo, mid);
        sort(a, mid+1, hi);

        merge(a, lo, mid, hi);
    }

    private static boolean less(Comparable v, Comparable w) {
        return v.compareTo(w) < 0;
    }

    private static void exch(Comparable[] a, int i, int j) {
        Comparable t = a[i];
        a[i] = a[j];
        a[j] = t;
    }

    private static boolean isSorted(Comparable[] a, int lo, int hi) {
        for (int i = lo+1; i <= hi; i++)
            if (less(a[i], a[i-1]))
                return false;
        return true;
    }

    public static double time(String alg, Comparable[] a) {
        Stopwatch timer = new Stopwatch();
        if (alg.equals("regular"))  sort(a);
        if (alg.equals("cutoff"))   sortWithCutoff(a);
        if (alg.equals("order"))    sortWithOrderCheck(a);
        if (alg.equals("ordercutoff")) sortCutoffOrder(a);
        if (alg.equals("copies"))   sortThatAvoidsCopies(a);
        return timer.elapsedTime();
    }

    public static double timeRandomInput(String alg, int N, int T) {
        double total = 0.0;
        Double[] a = new Double[N];
        for (int t = 0; t < T; t++) {
            for (int i = 0; i < N; i++)
                a[i] = StdRandom.uniform();
            total += time(alg, a);
        }
        return total;
    }

    public static void main (String [] args) {
        String alg = args[0];
        int N = Integer.parseInt(args[1]);
        int T = Integer.parseInt(args[2]);

        double t1 = timeRandomInput("regular", N, T);
        double t2 = timeRandomInput("cutoff", N, T);
        double t3 = timeRandomInput("order", N, T);
        double t4 = timeRandomInput("ordercutoff", N, T);
        StdOut.printf("%.2f %.2f %.2f %.2f\n", t1, t2, t3, t4);

    }
}
