public class MergingSortedQueues
{
    public static Queue<Integer> Merge(Queue<Integer> q1, Queue<Integer> q2)
    {
        int N = q1.size() + q2.size();
        Queue<Integer> q = new Queue<Integer>();
        Integer v = null, prev = null;

        for (int i = 0; i < N; i++) {
            if (i % 2 == 0)
                v = q1.isEmpty() ? null : q1.dequeue();
            else
                v = q2.isEmpty() ? null : q2.dequeue();

            if (less(v, prev)) {
                q.enqueue(v);
            } else {
                q.enqueue(prev);
                prev = v;
            }
        }
        return q;
    }

    public static boolean less(Integer v, Integer w)
    {
        if (v == null)
            return false;
        else if (w == null)
            return true;
        else
            return v.compareTo(w) < 0;
    }

    public static void main (String [] args)
    {
        // TODO: Not working - doesn't sort!!!

        Queue<Integer> q1 = new Queue<Integer>();
        Queue<Integer> q2 = new Queue<Integer>();

        for (int i = 0; i < 10; i++) {
            q1.enqueue(i);
            q2.enqueue(i*2);
        }
        Queue<Integer> q = Merge(q1, q2);
        StdOut.println(q);
    }
}
