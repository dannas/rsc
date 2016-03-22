public class Distribution {
    public static double time(String alg, Comparable[] a)
    {
        Stopwatch timer = new Stopwatch();
        if (alg.equals("Insertion"))    Insertion.sort(a);
        if (alg.equals("Selection"))    Selection.sort(a);
        if (alg.equals("Shell"))        Shell.sort(a);
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

    public static double jitter()
    {
        return StdRandom.uniform();
    }

    public static void main (String [] args)
    {
        String alg = args[0];
        int N = Integer.parseInt(args[1]);
        int M = Integer.parseInt(args[2]);

        StdDraw.setXscale(-10.0, 10.0);
        StdDraw.setYscale(0, 1.0);
        StdDraw.setPenRadius(.005);

        while (true) {
            double time = timeRandomInput(alg, N, M);
            StdDraw.point(jitter(), time);
            StdOut.printf("%.2f\n", time);
            StdDraw.show();
        }

    }
}
