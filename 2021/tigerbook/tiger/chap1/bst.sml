type key = string
datatype tree = LEAF | TREE of tree * key * tree

val empty = LEAF

fun insert(key, LEAF) = TREE(LEAF, key, LEAF)
  | insert(key, TREE(lhs, k, rhs)) = 
  	if key < k then TREE(insert(key, lhs), k, rhs)
	else if key > k then TREE(lhs, k, insert(key, rhs))
	else TREE(lhs, key, rhs)


exception Empty
fun member(key, LEAF) = raise Empty
  | member(key, TREE(lhs, k, rhs)) = 
  	if key < k then member(key, lhs)
	else if key > k then member(key, rhs)
	else k

val t = TREE(
	    TREE(
	        TREE(LEAF, "2", LEAF), 
		"3", 
		TREE(LEAF, "4", LEAF)),
	    "5",
	    TREE(LEAF, "8", LEAF))

member("5", t)

(* 
TODO: Write dot graph generator
digraph BST {
    15 -> 6;
    null0 [shape=point];
    6 -> null0;
    null1 [shape=point];
    6 -> null1;
    15 -> 18;
    18 -> 17;
    null2 [shape=point];
    17 -> null2;
    null3 [shape=point];
    17 -> null3;
    null4 [shape=point];
    18 -> null4;
}
*)
