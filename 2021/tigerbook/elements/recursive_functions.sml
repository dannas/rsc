fun reverse L : int list =
    if L = nil then nil
    else reverse(tl L) @ [hd L]

reverse([1, 2, 3, 4])

fun comb(n, m) =
	if m = 0 orelse m = n then 1
	else comb(n-1, m) + comb(n-1, m-1)

comb(4, 2)

fun
	take L =
	if L = nil then nil
	else hd L :: skip (tl L)
and
	skip L = 
	if L = nil then nil
	else take(tl L)

take([1, 2, 3, 4, 5, 6])

fun cycle (L, i) =
	if i = 0 then L 
	else cycle(tl L @ [hd L], i-1)

cycle([1, 2, 3, 4], 0)
cycle([1, 2, 3, 4], 1)
cycle([1, 2, 3, 4], 2)

fun duplicate(xs) =
	if xs = nil then nil
	else hd xs :: hd xs :: duplicate(tl xs)

duplicate([1])
duplicate([1, 2])
duplicate([1])

fun list_len xs =
	if xs = nil then 0
	else 1 + list_len(tl xs)
list_len([1])
list_len([1, 2, 3])

fun power (x, i) =
	if i = 0 then 1
	else x * power(x, i-1)


power (5, 2)

fun max(x, y) = if x > y then x else y

fun max_list xs =
	if tl xs = nil then hd xs
	else max(hd xs, max_list(tl xs))

max_list [1, 2, 3, 45, 3, 100, 6]