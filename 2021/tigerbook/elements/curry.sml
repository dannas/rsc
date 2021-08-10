fun exponent1(x, 0) = 1.0
  | exponent1(x, y) = x * exponent1(x, y - 1)

fun exponent2 x 0 = 1.0
  | exponent2 x y = x * exponent2 x (y - 1)

exponent1(3.0, 4)
exponent2 3.0 4

val g = exponent2 3.0

fun map(F, nil) = nil
  | map(F, x::xs) = F(x) :: map(F, xs)

fun applyList nil x = nil
  | applyList (F::Fs) x = F x :: applyList Fs x

fun square x = x * x
applyList [op ~, square] 2

fun identity(x) = x

fun applyList nil x = []
  | applyList (F::Fs) x = F x :: applyList Fs x

fun add1 x = x + 1 
fun add2 x = x + 2
fun add3 x = x + 3 
applyList [fn x => x+1, fn x => x+2, fn x => x+3] 10

fun makeFnList F nil = []
  | makeFnList F (x::xs) = F(x) :: makeFnList F xs

fun ss1 _ nil = true
  | ss1 nil _ = true
  | ss1 (x::xs) (y::ys) = x=y andalso ss1 xs ys

fun ss2 nil _ = true
  | ss2 _ nil = false
  | ss2 (L as x::xs) (y::ys) = x=y andalso ss1 xs ys orelse ss2 L ys

ss1 [1, 2, 3] [4, 1, 2]
ss2 [1, 2, 3] [4, 1, 2,  5]