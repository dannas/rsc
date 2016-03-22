import java.awt.Font;

public class ArrayST<Key extends Comparable<Key>, Value>
{
    private Key[] keys;
    private Value[] vals;
    private int N;
    Tracer tracer;

    public ArrayST(int size)
    {
        keys = (Key[]) new Comparable[size];
        vals = (Value[]) new Object[size];
        N = 0;
        tracer = new Tracer(size, size);
    }

    public void put(Key key, Value val)
    {
        if (val == null) {
            delete(key);
            return;
        }

        int i = rank(key);
        if (i == -1) {
            keys[N] = key;
            vals[N] = val;
            N++;
            tracer.draw(key, val, N-1, false);
        } else {
            Key k = keys[i];
            Value v = vals[i];
            for (int j = i; j > 0; j--) {
                keys[j] = keys[j-1];
                vals[j] = vals[j-1];
            }
            keys[0] = k;
            vals[0] = v;
            tracer.draw(key, val, 0, true);
        }
    }

    public Value get(Key key)
    {
        int i = rank(key);
        if (i == -1)
            return null;
        else
            return vals[i];
    }

    public int size()
    {
        return N;
    }

    private int rank(Key key)
    {
        for (int i = 0; i < N; i++)
            if (keys[i].equals(key))
                return i;
        return -1;
    }

    private void delete(Key key)
    {
        int pos = rank(key);
        for (int i = pos+1; i < N; i++) {
            keys[i-1] = keys[i];
            vals[i-1] = vals[i];
        }
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

    public static void main (String [] args)
    {
        int N = Integer.parseInt(args[0]);
        ArrayST<String, Integer> st = new ArrayST<String, Integer>(N);

        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            Integer i = StdIn.readInt();
            st.put(s, i);
        }
        StdDraw.show(0);
    }
}
