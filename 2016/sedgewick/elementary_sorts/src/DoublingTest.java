public class DoublingTest {
    public static double time(String alg, Comparable[] a)
    {
        Stopwatch timer = new Stopwatch();
        if (alg.equals("Insertion")) Insertion.sort(a);
        if (alg.equals("Selection")) Selection.sort(a);
        if (alg.equals("Shell")) Shell.sort(a);
        return timer.elapsedTime();
    }

    public static double timeRandomInput(String alg, int N, int T)
    {
        double total = 0.0;
        Double[] a = new Double[N];
        for (int t = 0; t < T; t++) {
            for (int i = 0; i < N; i++)
                a[i] = StdRandom.uniform();
            total += time(alg, a);
        }
        return total;
    }
    public static void main (String [] args)
    {
        String alg = args[0];
        double prev, time;

        prev = timeRandomInput(alg, 1000, 3);

        for (int i = 2000; i < 1000000000; i *= 2) {
            time = timeRandomInput("Insertion", i, 3);
            StdOut.printf("%.2f\n", time/prev);
            prev = time;
        }
    }
}
