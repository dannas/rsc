type signal = int list
val v = [1, 2]  : signal
val w = [1, 2]
v = w

type ('d, 'r) mapping = ('d * 'r) list
val words = [("in", 6), ("a", 1)] : (string, int) mapping

type ('a') setset = 'a' list list
type ('a, 'b, 'c) setset = ('a * 'a * 'b) list

datatype fruit = Apple | Pear | Grape

fun isApple x = x=Apple
isApple Apple
isApple Pear

datatype ('a, 'b) element = P of 'a * 'b
      		          | S of 'a
P(1, 2.0)
S(1, 1)
S(1)
val L = [P("in", 6), S("function"), P("as", 2)]
fun sumElList nil = 0
  | sumElList (S(x)::L) = sumElList L
  | sumElList (P(x, y)::L) = y + sumElList L
sumElList L

datatype 'label btree = Empty
		      | Node of 'label * 'label btree * 'label btree
Node("Ml",
	Node("as", 
		Node("a", Empty, Empty), 
		Node("in", Empty, Empty)),
	Node("types", Empty, Empty))

(* 6.2.1 *)
val t = Node(2,
		Node(1, Empty, Empty),
		Node(3, Empty, Empty))

(* 6.2.2 *)
type mapTree = (int * char) btree
val t1 : mapTree = Node((1, #"a"), Empty, Empty)

(* 6.2.3 *)
exception ExEmpty
fun f Empty = raise ExEmpty
  | f (Node(_, left, right)) = (left, right)
f t

(* 6.2.4 *)
datatype intOrEof = Eof 
		  | Integer of int

(* 6.2.7 *)
type 'node graph = ('node * 'node list) list
exception NotANode
fun succ(a, nil) = raise NotANode
  | succ(a, (b, x)::G) = if a = b then x else succ(a, G)




