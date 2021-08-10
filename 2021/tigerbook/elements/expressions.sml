print "Hello world!";

(* Booleans, floating point and integer numbers are builtin *)
val logical : bool = true
val a_float : real = 1.0
val an_integer : int = 5
val a_hex_integer : int = 0x42

(* Don't need type annotation *)
val default_float = 3.0
val default_integer = 5

val var_declared = 1
(* Variables can't be reassigned
var_declared = 2
*)

(* Value bindings can be shadowed *)
val var_declared = 2

(* Plus/minus/times are same for int and real. Division differs. *)
1 + 2
1 - 2
2.0 / 1.0
2 div 1

(* Unary minus differ from binary minus. *)
1 - ~1

(* Strings are concatenated with ^ *)
"abc" ^ "def"

(* Logical or and else have strange names *)
true andalso true
true orelse false
not true

(* Tuples can have members of different type *)
val long_tuple = (1, 3.14, #"a", "foo")
(* Tuples can be accessed by tuple indexing (1-based) *)
#3 long_tuple
#1 long_tuple

val tuple_of_tuples = ((1,2,3), (4, 5, 6))
(* You can't have one element tuples
val one_element_tuple = (1)
*)

(* Tuples can be destructured *)
val tuple = (1, 2, 3)
val (a, b, c) = tuple

(* SML must accept only one argument, but that can be a tuple *)
fun reverse (x : int, y : int) = (y, x)
reverse (1, 2)

(* Fixed size list (type signature is superflous) *)
val xs : int list = [1, 2, 3]

(* bracket notation is syntactic sugar. Underneath it's this *)
val xs = 1 :: 2 :: 3 :: nil

(* Lists can be concatenated with @ *)
val xs = [1, 2] @ [3]

(* You cannot concatenate a lhs list with a rhs element. 
val xs = [1, 2] :: 3
*)

(* There is a length function provided by the Basis. *)
length xs

(* It's defined something like this. *)
fun len nil = 0
	| len (_::t) = 1 + len t

len xs

(* The Basis also contains rev for reversing lists. *)
rev xs

(* That function can be defined like this. local creates local scope. *)
local
  fun rev_helper (nil, ret) = ret
  | rev_helper (x::xs, ret) = rev_helper (xs, x::ret)
in
  fun rev xs = rev_helper(xs, nil)
end

rev xs

"I write on  \
\multiple lines\
\even more lines"

1+2*3
11 div 2 mod 3
"foo"^"bar"
3<4 orelse 5<6 andalso not (7 <>8)
0xAB + 123
if 6<10 then 6.0 else 10.0
if 2<3 then 4 else 0
4.0 + 3.5
1.0<2.0 or 3>4