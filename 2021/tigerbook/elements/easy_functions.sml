fun upper c = chr(ord(c) - 32)
upper #"A"

fun square(x : real) = x * x
fun square2 x = x : real * x

val x = 3.0
val y = 4.0
square x + y

fun max3(a: real, b, c) =
if a > b then
    if a > c then a 
    else c
else
    if b > c then b
    else c

max3(1.0, 2.0, 5.0)

fun cube x : real = x * x * x
cube(3.0)

fun smallest3 (a : real, b, c) =
if a < b then
    if a < c then a
    else c
else
    if b < c then b
    else a

smallest3(1, 2, 3)

fun third (xs) = hd (tl (tl xs))

third([1, 2, 3, 4, 5])

fun rev_tuple (x, y) = (y, x)

rev_tuple(3, 4)

fun str_third str = third (explode(str))

str_third "abcdef"

fun cycle xs = tl xs @ [hd xs]
cycle([1, 2, 3, 4])

fun maxmin3 x  = (max3(x), smallest3(x))

maxmin3 (1.0, 2.0, 3.0)

