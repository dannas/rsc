fun reverse (nil) = nil
| reverse (x::xs) = reverse(xs) @ [x]

reverse([1, 2, 3, 4])

merge([1, 3, 5], [2, 4, 6])

fun comb(_, 0) = 1
| comb(n, m) =
    if m=n then 1
    else comb(n-1, m) + comb(n-1, m-1)

comb(4, 2)

fun sum_pairs(nil) = 0
| sum_pairs((x, y)::xs) = x + y + sum_pairs(xs)

sum_pairs([(1, 2), (1, 2), (1, 2)])

fun sum_lists(nil) = 0
| sum_lists(nil::YS) = sum_lists(YS)
| sum_lists((x::xs)::YS) = x + sum_lists(xs::YS)

fun length(nil) = 0
| length(xs @ [x]) = 1 + length(xs)

fun factorial (0) = 0
| factorial (1) = 1
| factorial (2) = 2
| factorial (n) = factorial(n-1) + factorial(n-2)

factorial(5)

fun cycle1(nil) = nil
| cycle1(x::xs) = xs @ [x]

cycle1([1])
cycle1([1, 2, 3])

fun cycle(nil, _) = nil
| cycle(xs, 0) = xs
| cycle(x::xs, i) = cycle(xs @ [x], i-1)

cycle([1,2,3,4,5], 2)

fun duplicate(nil) = nil
| duplicate(x::xs) = x::x::duplicate(xs)

duplicate([1, 2, 3, 4])

fun power(_, 0) = 1
| power(x, i) = x * power(x, i-1)

power(3, 5)

fun max(x, y) : real = if x > y then x else y

fun max_list(nil) = 0.0
| max_list(x::nil) = x : real
| max_list(x::xs) = max(x, max_list(xs))

max_list([1.0, 5.0, 78.0, 43.0, 2.0])

fun flip_alternate(nil) = nil
| flip_alternate(x::nil) = [x]
| flip_alternate(x::xs) = hd xs :: x :: flip_alternate(tl xs)

flip_alternate([1, 2, 3, 4, 5, 6, 7])

fun del_nth(x::xs, 0) = xs
| del_nth(nil, _) = nil
| del_nth(x::xs, i) = x :: del_nth(xs, i-1)

del_nth([1, 2, 3, 4, 5], 4)

fun order(x, y) = if x > y then (y, x) else (x, y)

fun order_pairs(nil) = nil
| order_pairs(L as (x,y)::xs) = order(x, y) :: order_pairs(xs)

order_pairs([(1, 2), (4, 3), (4, 2)])

fun is_empty(nil) = true
| is_empty(_) = false

is_empty([1, 2])

fun member(_, nil) = false
| member(x, s::S) = if x = s then true else member(x, S)

member(5, [1, 2, 3, 4])

fun delete(_, nil) = nil
| delete(x, s::S) = if x = s then S else s :: delete(x, S)

delete(2, [1, 2, 5, 3, 4])

fun insert(x, nil) = [x]
| insert(x, S) = if member(x, S) then S else x :: S

insert(6, [1, 2, 3, 4, 5])

fun hundrethPower(x:real) =
let
  val four = x * x *x * x
  val twenty = four * four * four * four * four
in
  twenty * twenty * twenty * twenty * twenty
end

hundrethPower 2.0
hundrethPower(1.01)

fun split nil = (nil, nil)
| split (a::nil) = ([a], nil)
| split (a::b::cs) = 
    let
      val (M, N) = split(cs)
    in
      (a::M, b::N)
    end

fun merge (nil, M) = M 
| merge (L, nil) = L 
| merge (L as x::xs, M as y::ys) = 
    if x < y then x :: merge(xs, M)
    else y :: merge(L, ys)

fun mergeSort (nil) = nil
| mergeSort ([a]) = [a]
| mergeSort (L) =
    let
      val (M, N) = split(L)
      val M = mergeSort(M)
      val N = mergeSort(N)
    in
      merge(M, N)
    end

fun compute1000 (x:real) =
let
  val x = x * x * x * x * x
  val x = x * x * x * x * x
  val x = x * x * x * x * x
in
  x * x * x * x * x * x * x * x
end

compute1000(1.001)

fun split2 nil = (nil, nil)
| split2 (a::nil) = ([a], nil)
| split2 (a::b::cs) = 
    let
      val L = split(cs)
      val M = #1 L
      val N = #2 L
    in
      (a::M, b::N)
    end

split2([1, 2, 3, 4, 5])


f (1, [[2, 2, 2], [3, 3, 3]])

fun prepend (a, nil) = nil
| prepend (a, x::xs) = (a :: x) :: prepend (a, xs)

fun powerSet (nil) = [nil]
| powerSet (x::xs) = 
    let
      val M = powerSet(xs)
    in
      prepend(x, M) @ M
    end

powerSet(nil)
powerSet([1])
powerSet([1, 2])
powerSet([1, 2, 3])

fun max(x::nil) = x
| max (x::xs) = 
    let
      val m = max(xs)
    in
      if x > m then x else m
    end

max([1, 2, 3])

fun f (x, 0) = 1
| f (x, i) = x*x * f(x, i-1)

f(2, 3)

fun sumPairs (nil) = (0, 0)
| sumPairs ((a, b)::cs) = 
    let
      val (x, y) =  sumPairs(cs)
    in
      (a+x, b+y)
    end

sumPairs([(1, 2), (2,3), (4, 5)])

fun sumEvenOdd (nil) = (0, 0)
| sumEvenOdd (a::nil) = (a, 0)
| sumEvenOdd (a::b::cs) = 
    let
      val (x, y) = sumEvenOdd(cs)
    in
      (a+x, b+y)
    end

sumEvenOdd([1, 2, 0, 2, 1, 2, 1])

fun cat1 (nil, M, nil) = M
    | cat1(x::xs, Y, N) = cat1(xs, Y, x::N)
    | cat1(nil, M, y::ys) = cat1(nil, y::M, ys)

fun cat (L, M) = cat1(L, M, nil)

cat([1, 2, 3], [5, 6, 7])


fun padd(P, nil) =  P
| padd(nil, Q) = Q
| padd((p : real)::ps, q::qs) = (p + q) :: padd(ps, qs)

padd([1.0, 2.0, 3.0], [1.0, 2.0, 3.0])
padd([1.0, 2.0, 3.0], [1.0, 2.0 ])
padd([], [])


fun smult(nil, q) = nil
| smult((p : real)::ps, q) = (p * q) :: smult(ps, q)

smult([1.0, 2.0, 3.0], 2.0)