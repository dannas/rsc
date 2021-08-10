val rec  reverse = fn 
	nil => nil |
	x::xs => reverse(xs) @ [x]

val addOne = fn x => x+1

(fn x => x +1)(3)

if x<y then #"a" else "b"

val rec padd = fn 
	(P, nil) => P |
	(nil, Q) => Q |
	(p::ps, q::qs) => (p + q) :: padd(ps, qs)

padd([1, 1, 1], [2, 2, 3])

val rec smult = fn 
	(nil, q) => nil |
	(p::ps, q) => (p * q) :: smult(ps, q)

smult([1, 2, 3], 2)

val rec sumPairs = fn 
	(nil) => nil |
	((a, b)::cs) => (a + b) :: sumPairs(cs)

sumPairs([(1, 2), (3, 4), (5, 6)])

5 div 0

hd (nil : int list)

tl (nil : real list)
chr(500)
exception Foo

raise Foo

exception BadN and BadM
fun comb(n, m) =
	if n<0 then raise BadN
	else if m<0 orelse m>n then raise BadM
	else if m=0 orelse m=n then 1
	else comb(n-1, m) + comb(n-1,m-1)

comb(5,2)
comb(~1,0)
comb(5, 6)

exception Foo of string;
raise Foo("bar")

raise Foo

exception OutOfRange of int*int

fun comb1(n, m) =
	if n<0 then raise OutOfRange(n, m)
	else if m<0 orelse m>n then 
		raise OutOfRange(n, m)
	else if m=0 orelse m=n then 1
	else comb1(n-1, m) + comb1(n-1,m-1)

fun comb(n,m) = comb1(n,m) handle
	OutOfRange(0, 0) => 1 |
	OutOfRange(n, m) => (
		print("out of range: n=");
		print(Int.toString(n));
		print(" m=");
		print(Int.toString(m));
		print("\n");
		0
	)

comb(3, 4)

exception OutOfRange of int
fun third(nil) = raise OutOfRange(0)
  | third(x::nil) = raise OutOfRange(1)
  | third(x::y::nil) = raise OutOfRange(2)
  | third(x::y::z::xs) = z:int

third([])
third([1])
third([1, 2])
third([1, 2, 3])

exception Negative of int

fun fac1(n) = 
	if n < 0 then raise Negative(n)
	else if n = 0 then 1
	else n * fac1(n-1)

fun fac(n) = fac1(n) handle
	Negative(n) => (print("Negative"); 0)

fac(0)
fac(2)
fac(3)
fac(~3)

fun f("joe") = Div
  | f("papa") = Match

f("...")