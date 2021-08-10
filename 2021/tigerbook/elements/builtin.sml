fun comp(F, G, x) = G(F(x))
comp(fn x => x+3, fn y => y*y+2*y, 10)

fun F x = x+3
fun G y = y*y + 2*y
val H = G o F
H 10

fun map F nil = nil
  | map F (x::xs) = F x :: map F xs

fun foldr F y nil = y
  | foldr F y (x::xs) = F(x, foldr F y xs)

fun foldl F y nil = y
  | foldl F y (x::xs) = foldl F (F(x, y)) xs

fun add1(x, y) = x+y

val L = [2, 3, 4]
foldr add1 0 L
foldl add1 0 L

val intToChar = map chr
intToChar [65, 66, 67]

val intToReal = map real
intToReal [1, 2, 3]

val implode = foldr (op ^) "" o map str
implode [#"a", #"b", #"c", #"d"]

val concat = foldr op ^ ""
concat ["a", "b", "c"]

val altsum = foldr (op -) 0

altsum [1, 2] 