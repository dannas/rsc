datatype 'a btree = Empty
		  | Node of 'a * 'a btree * 'a btree

fun lower nil = nil
  | lower (c::cs) = Char.toLower c::lower(cs)

fun strLt(x, y) = implode (lower (explode(x))) < implode (lower (explode(y)))

fun lookup lt Empty x = false
  | lookup lt (Node(y, left, right)) x =
	if lt(x, y) then lookup lt left x
	else if lt(y, x) then lookup lt right x
	else true

fun insert lt Empty x = Node(x, Empty, Empty)
  | insert lt (T as Node(y, left, right)) x = 
	if lt(x, y) then Node(y, insert lt left x, right)
	else if lt (y, x) then Node(y, left, insert lt right x)
	else T

(*
Delete does nothing if the node is not found or the tree is empty (same case)
If we delete a node that has siblings, we need to reattach those to the tree
If it doesn't have siblings we can just replace the node from it's parent with Empty
But we need to have a reference to the parent.

 *)

exception EmptyTree

 fun deleteMin Empty = raise EmptyTree
   | deleteMin (Node(y, Empty, right)) = (y, right)
   | deleteMin (Node(y, left, right)) = let
     val (y', left') = deleteMin left
   in
     (y, Node(y', left', right))
   end
fun delete lt Empty x = Empty
  | delete lt (Node(y, left, right)) x =
	if lt(x, y) then Node(y, delete lt left x, right)
	else if lt(y, x) then Node(y, left, delete lt right x)
	else
	    case (left, right) of 
	        (Empty, r) => r
	      | (l, Empty) => l
	      | (l, r) =>  let
		val (z, r1) = deleteMin(r)
	      in
		Node(z,l,r1)
	      end

fun sum Empty = 0
  | sum (Node(x, left, right)) = x + sum left + sum right

datatype 'a btree = Empty
		  | Node of 'a * 'a btree * 'a btree
fun preOrder Empty = nil
  | preOrder (Node(x, left, right)) = [x] @ preOrder left @ preOrder right

(* 6.3.1 *)
fun postOrder Empty = nil
  | postOrder (Node(x, left, right)) = preOrder left @ preOrder right @ [x]
fun inOrder Empty = nil
  | inOrder (Node(x, left, right)) = preOrder left @ [x] @ preOrder right

fun preOrder tree = let
    fun pre (Empty, L)  = L
      | pre (Node(x, left, right), L) = let
	val L = pre(right, L)
	val L = pre(left, L)
      in
	x::L
      end
  in
    pre (tree, nil)
  end

fun preOrder tree = let
    fun pre (Empty, L)  = L
      | pre (Node(x, left, right), L) = x::pre(left, pre(right, L))
  in
    pre (tree, nil)
  end

fun preOrder tree = let
    fun pre (Empty, L)  = L
      | pre (Node(x, left, right), L) = x::pre(left, pre(right, L))
  in
    pre (tree, nil)
  end

val t = Node(5, Node(3, Node(1, Empty, Empty), Node(4, Empty, Empty)), Node(9, Empty, Empty))

preOrder t

foldr
foldl