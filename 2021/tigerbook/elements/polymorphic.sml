fun identity(x) = x
identity(2)
identity(ord)

fun f(x) =
	if x < 10 then identity
	else rev

identity(identity)
identity(identity : int -> int)
(identity, identity)
[identity, identity]

SOME identity
let
  val x = identity(identity)
in
  x(1)
end
let
  val x = identity(identity)
in
  (x(1), x("a"))
end
val x = (1, 2)
val y =(2, 3)
x = y
x = (1,2)
val L = [1, 2, 3]
val M = [2, 3]
L <> M
L = 1::M

fun rev1(L) =
  if L = nil then nil
  else rev1(tl(L)) @ [hd(L)]

fun rev2(nil) = nil
  | rev2(x::xs) = rev2(xs) @ [x]

rev2([floor, ceil, trunc])
rev1([floor, ceil, trunc])

rev2([1.0, 2.0, 3.0])
rev1([1, 2, 3])

rev1([(rev1: int list -> int list), rev1])
rev2([(rev1: int list -> int list), rev1])
rev1([rev1, rev1])
rev2([rev2, rev2])

fun trap(a, b, n, F) =
  if n <= 0 orelse  b-a<=0.0 then 0.0
  else
    let
      val delta = (b - a) / real(n)
    in
      delta * (F(a) + F(a+delta)) / 2.0 + trap(a+delta, b, n-1, F)
    end

fun map(_, nil) = nil
  | map(F, x::xs) = F(x) :: map(F, xs)

fun add1(x) = x+1
map(add1, [1, 2, 3])

fun reduce(F, x::nil) = x
  | reduce(F, x::xs) = F(x, reduce(F, xs))

reduce(op +, [1, 2, 3])
map(fn x => x+1, [1, 2, 3])

fun square(x:real) = x*x
fun plus(x:real, y) = x + y

square(10.0)
plus(4.0, 5.0)

fun (L) = 
  let
    val n = real(length(L))
  in
    reduce(plus, map(square, L))/n - square(reduce(plus, L)/n)
  end

length([1, 2, 3])

variance([1.0, 2.0, 5.0, 8.0])

fun tabulate(a:int, delta, n, F) =
  let
    val str = Int.toString
  in
    if a = n then ""
    else str(a) ^ "\t" ^ str(F(a)) ^ "\n" ^ tabulate(a+delta, delta, n, F)
  end

print(tabulate(0, 1, 10, add1))

map

val check = fn x => if x < 0.0 then 0.0 else x
map(check, [~1.0, ~2.0, 4.0, 5.0])
map(fn x => (if x < 0.0 then 0.0 else x), [~1.0, ~2.0, 4.0, 5.0])