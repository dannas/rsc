class Selection {
    public static void sort(Comparable[] a) {
        int N = a.length;

        for (int i = 0; i < N; i++) {
            int min = i;
            for (int j = i+1; j < N; j++)
                if (less(a[j], a[min]))
                    min = j;
            exch(a, min, i);
            assert isSorted(a, i);
        }
        assert isSorted(a, a.length);
    }
    
    private static boolean less(Comparable v, Comparable w) {
        return v.compareTo(w) < 0;
    }

    private static void exch(Comparable[]a, int i, int j) {
        Comparable swap = a[i];
        a[i] = a[j];
        a[j] = swap;
    }

    private static boolean isSorted(Comparable[] a, int N) {
        for (int i = 1; i < N; i++)
            if (less(a[i-1], a[i]))
                return false;
        return true;
    }
}
