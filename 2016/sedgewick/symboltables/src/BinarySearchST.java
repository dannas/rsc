import java.awt.Font;

public class BinarySearchST<Key extends Comparable<Key>, Value>
{
    private Key[] keys;
    private Value[] vals;
    private int N;
    private int ops;
    private int total;

    public BinarySearchST(int size)
    {
        keys = (Key[]) new Comparable[size];
        vals = (Value[]) new Object[size];
        N = 0;
        ops = 0;
        total = 0;
        axis(size);
    }

    public void put(Key key, Value val)
    {
        int cost = 0;
        int i = rank(key);

        if (i != -1) {
            vals[i] = val;
            cost++;
        } else {
            int j = N;
            while (j > 0 && less(key, keys[j-1])) {
                keys[j] = keys[j-1];
                vals[j] = vals[j-1];
                j--;
                cost += 2;
            }
            keys[j] = key;
            vals[j] = val;
            N++;
        }
        ops++;
        total += cost;
        draw(ops, cost, total/ops);
    }

    public Value get(Key key)
    {
        int i = rank(key);
        if (i != -1) {
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

    private void axis(int sz)
    {
        StdDraw.setCanvasSize(1024, 512);
        StdDraw.setXscale(-2, sz);
        StdDraw.setYscale(-2, sz/2);

        StdDraw.setFont(new Font("SansSerif", Font.PLAIN, 13));
        StdDraw.line(-1, 0, sz, 0);
        StdDraw.line(0, -1, 0, sz/2);
        StdDraw.line(sz, -1, sz, 0);
        StdDraw.line(-1, sz/2, 0, sz/2);
        StdDraw.setPenRadius(0.003);
    }

    private void draw(int ops, int cost, int amortized)
    {
        StdDraw.setPenColor(StdDraw.DARK_GRAY);
        StdDraw.point(ops, cost);
        StdDraw.setPenColor(StdDraw.RED);
        StdDraw.point(ops, amortized);
    }

    public static void main(String [] args)
    {
        int N = Integer.parseInt(args[0]);
        BinarySearchST<String, Integer> st = new BinarySearchST<String, Integer>(N);

        int i = 0;
        while (!StdIn.isEmpty()) {
            String s = StdIn.readString();
            st.put(s, i);
            i++;
        }
        StdDraw.show(0);
    }
}
