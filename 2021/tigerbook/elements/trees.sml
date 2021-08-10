datatype ('label) tree = Node of 'label * 'label tree list

val t = Node(3, 
	[Node(1, nil),
	Node(2, nil),
	Node(1, nil)])

fun sum(Node(a, nil)) = a
  | sum(Node(a, x::xs)) = sum(x) + sum(Node(a, xs))

sum t

(* 6.4.1 *)
fun exists x (Node(a, nil)) = x = a
  | exists x (Node(a, t::ts)) = exists x t orelse exists x (Node(a, ts))

exists 3 t
exists 7 t

fun exists x (Node(a, ts)) = x = a orelse  foldr (fn (x, y) => x orelse y) false  (map (exists x) ts)

exists 3 t
exists 7 t

(* 6.4.2 *)
fun count (Node(a, nil)) x = if x = a then 1 else 0
  | count (Node(a, t::ts)) x = count t x + count (Node(a, ts)) x

count t 1
fun count x (Node(a, ts)) = if x = a then 1 else 0 + foldr (op +) 0 (map (count x) ts)

count 1 t

(* 6.4.3 *)

fun max (x, y) = if x > y then x else y
fun depth (Node(a, nil)):int = 1
  | depth (Node(a, t::ts)) = max(1 + depth t, 1 + depth (Node(a, ts)))