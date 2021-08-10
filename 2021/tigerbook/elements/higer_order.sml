fun simpleMap(_, nil) = nil
  | simpleMap(F, x::xs) = F(x) ::simpleMap(F, xs)

exception Empty

fun reduce(_, nil) = raise Empty
  | reduce(F, x::nil) = x
  | reduce(F, x::xs) = F(x, reduce(F, xs))


simpleMap(fn x => x+1, [1, 2, 3]) = [2, 3, 4]
reduce(op +, [1, 2, 3]) = 6

fun filter(P, nil) = nil
  | filter(P, x::xs) = if P(x) then x::filter(P, xs) else filter(P, xs)

fun isEven(x) = x mod 2 = 0
filter(isEven, [1, 2, 3, 4, 5])
filter(fn x => x mod 2 = 0, [1, 2, 3, 4, 5])

simpleMap(fn x => if x < 0.0 then 0.0 else x, [1.0, 2.0, 3.0, 4.0, ~5.0, 6.0, 7.0, ~8.0])
simpleMap(fn x => x + 1, [1, 2, 3, 5, 5])
simpleMap(fn x => if Char.isUpper x then x else Char.toLower(x), [#"a", #"B", #"c", #"D"])

filter(fn x => x >= 1 andalso x <=5, [1, 2, 3, 5, 6, 7, 8, 9])

String.size("abc")