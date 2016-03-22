import java.awt.Font;
import java.util.Iterator;

public class OrderedArrayST<Key extends Comparable<Key>, Value>
{
    private Key[] keys;
    private Value[] vals;
    private Key cachedKey;
    private Value cachedVal;
    private int N;
    private Tracer tracer;

    public OrderedArrayST(int size)
    {
        keys = (Key[]) new Comparable[size];
        vals = (Value[]) new Object[size];
        N = 0;
        tracer = new Tracer(size, size);
    }

    public void put(Key key, Value val)
    {
        int i = rank(key);

        if (i != -1) {
            vals[i] = val;
            tracer.draw(key, val, i, true);
            return;
        }

        int j = N;
        while (j > 0 && less(key, keys[j-1])) {
            keys[j] = keys[j-1];
            vals[j] = vals[j-1];
            j--;
        }
        keys[j] = key;
        vals[j] = val;
        N++;
        tracer.draw(key, val, j, false);
    }

    public Value get(Key key)
    {
        if (key.equals(cachedKey))
            return cachedVal;

        int i = rank(key);
        if (i != -1) {
            cachedKey = key;
            cachedVal = vals[i];
            return vals[i];
        }
        return null;
    }

    public boolean contains(Key key)
    {
        return rank(key) != -1;
    }

    private boolean less(Comparable v, Comparable w)
    {
        return v.compareTo(w) < 0;
    }

    private int rank(Key key)
    {
        int lo = 0, hi = N-1;

        while (lo <= hi) {
            int mid = lo + (hi - lo) / 2;
            int cmp = key.compareTo(keys[mid]);
            if (cmp < 0)
                hi = mid - 1;
            else if (cmp > 0)
                lo = mid + 1;
            else
                return mid;
        }
        return -1;
    }

    private class Tracer
    {
        private int sz;
        private int off;   // values offset
        private double row; // current row

        public Tracer(int size, int nops)
        {
            row = 0;
            sz = size;
            int width = 3 + sz + 2 + sz;
            off = sz + 2;

            StdDraw.setCanvasSize(30 *(width), 30 * (nops+3));
            StdDraw.setXscale(-3, width);
            StdDraw.setYscale(nops, -3);

            StdDraw.setFont(new Font("SansSerif", Font.PLAIN, 13));
            header();
        }

        public void header()
        {
            StdDraw.setPenColor(StdDraw.BLACK);
            StdDraw.text(sz/2, -2, "keys[]");
            StdDraw.text(sz + 2 + sz/2, -2, "values[]");
            StdDraw.setPenColor(StdDraw.BOOK_RED);

            StdDraw.line(-.5, -1.65, sz-.5, -1.65);
            StdDraw.line(off-.5, -1.65, off+sz-.5, -1.65);
            StdDraw.setFont(new Font(null, 0, 10));
            StdDraw.text(-3.00, -1, "key");
            StdDraw.text(-1.75, -1, "value");

            StdDraw.setFont(new Font("SansSerif", Font.PLAIN, 13));
            StdDraw.setPenColor(StdDraw.BLACK);
            for (int i = 0; i < sz; i++)
                StdDraw.text(i, -1, "" + i);

            StdDraw.text(sz + .5, -1, "N");

            for (int i = 0; i < sz; i++)
                StdDraw.text(off + i, -1, "" + i);
        }

        public void draw(Key key, Value val, int pos, boolean replace)
        {
            StdDraw.setPenColor(StdDraw.BLACK);
            StdDraw.text(-3.00, row, "" + key);
            StdDraw.text(-1.75, row, "" + val);
            StdDraw.text(sz + .5, row, "" + N);

            for (int i = 0; i < N; i++) {
                if (i == pos)
                    StdDraw.setPenColor(StdDraw.RED);
                else if (i > pos && !replace)
                    StdDraw.setPenColor(StdDraw.BLACK);
                else
                    StdDraw.setPenColor(StdDraw.GRAY);
                StdDraw.text(i, row, "" + keys[i]);
                StdDraw.text(off + i, row, "" + vals[i]);
            }
            row++;
        }
    }

    public static void main(String [] args)
    {
        int N = Integer.parseInt(args[0]);
        OrderedArrayST<String, Integer> st = new OrderedArrayST<String, Integer>(N);

        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            Integer i = StdIn.readInt();
            st.put(s, i);
        }
        StdDraw.show(0);
    }
}
