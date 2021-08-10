type id = string

datatype binop = Plus | Minus | Times | Div

datatype stm = CompoundStm of stm * stm
	     | AssignStm of id * exp
	     | PrintStm of exp list

     and exp = IdExp of id
	     | NumExp of int
       | OpExp of exp * binop * exp
       | EseqExp of stm * exp
val prog = 
 CompoundStm(AssignStm("a",OpExp(NumExp 5, Plus, NumExp 3)),
  CompoundStm(AssignStm("b",
      EseqExp(PrintStm[IdExp"a",OpExp(IdExp"a", Minus,NumExp 1)],
           OpExp(NumExp 10, Times, IdExp"a"))),
   PrintStm[IdExp "b"]))

fun max (a, b) = if a  > b then a else b

fun maxargs (CompoundStm(left, right)) = max(maxargs(left), maxargs(right))
  | maxargs (AssignStm(_, expr)) = expargs expr
  | maxargs (PrintStm(exprs)) = 1 + length exprs
and
    expargs(EseqExp(left, right)) = maxargs left + expargs right 
  | expargs _ = 0

maxargs(prog)

exception Empty
fun update(key, v, table) = (key, v)::table

fun lookup(key, (k, v)::table) = if key = k then v else lookup(key, table)
  | lookup(key, nil) = raise Empty

val L = [(1, "a"), (2, "b"), (1, "c")]
lookup(1, L)
update(1, "d", L)

fun printList xs = print(String.concatWith ", " (map Int.toString xs) ^ "\n")

fun toOp(operation) = case operation of
    Plus => op +
  | Minus => op -
  | Div => op div
  | Times => op *

fun interpStm (stm, env) = case stm of
    CompoundStm(left, right) => interpStm(right, interpStm(left, env))
  | AssignStm(id, expr) => update(id, interpExp(expr, env), env)
  | PrintStm(exprs) => (printList(map (fn e => interpExp(e, env)) exprs); env)
and
    interpExp (expr, env) = case expr of
    IdExp(id) => lookup(id, env)
  | NumExp(x) => x
  | OpExp(left, operation, right) => 
      toOp operation (interpExp(left, env), interpExp(right, env))
  | EseqExp(stm, exp) => (interpStm(stm, env); interpExp(exp, env))

interpExp(OpExp(NumExp 3, Minus, NumExp 2), nil) = 1
interpExp(OpExp(NumExp 3, Minus, NumExp 2), nil) = 1

 interpStm(CompoundStm(
   AssignStm("a",
      OpExp(NumExp 5, Plus, NumExp 3)),
    AssignStm("a", 
      OpExp(IdExp "a", Times, NumExp 3))), nil)

interpStm(prog, nil)
