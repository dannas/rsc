from common import *
import sys

# 50min part 1
# 15min part 2
# Mixed up def of parameter modes
# Function ints did not handle negative numbers
# Did not increment pc correctly
# Could not call sys.stdin.read from debugger
# next(str) did not work 
# str[0] was a str, should have casted it to int

# Lessons
# Be sure to test all utilities
# Check small sample in debugger

ADD, MUL, IN, OUT, BT, BF, BLT, BEQ, HALT = 1, 2, 3, 4, 5, 6, 7, 8, 99

def op_mode(val):
    op = val % 100
    m1 = (val // 100) % 10
    m2 = (val // 1000) % 10
    m3 = (val // 10000) % 10
    return op, m1, m2, m3

def mem(M, param, mode):
    if mode == 0:
        return M[param]
    elif mode == 1:
        return param

def interpret(M, input, pc=0):
    output = []
    while True:
        op, m1, m2, m3 = op_mode(M[pc])
        if op == 1: 
            x, y, dst = M[pc+1:pc+4]
            M[dst] = mem(M, x, m1) + mem(M, y, m2)
            pc += 4
        elif op == 2:
            x, y, dst = M[pc+1:pc+4]
            M[dst] = mem(M, x, m1) * mem(M, y, m2)
            pc += 4
        elif op == 3:
            dst = M[pc+1]
            M[dst] = input[0]
            pc += 2
        elif op == 4:
            x = M[pc+1]
            output.append(mem(M, x, m1))
            pc += 2
        elif op == 5:
            x, new_pc = M[pc+1:pc+3]
            pc = mem(M, new_pc, m2) if mem(M, x, m1) != 0 else pc+3
        elif op == 6:
            x, new_pc = M[pc+1:pc+3]
            pc = mem(M, new_pc, m2) if mem(M, x, m1) == 0 else pc+3
        elif op == 7:
            x, y, dst = M[pc+1:pc+4]
            M[dst] = 1 if mem(M, x, m1) < mem(M, y, m2) else 0
            pc += 4
        elif op == 8:
            x, y, dst = M[pc+1:pc+4]
            M[dst] = 1 if mem(M, x, m1) == mem(M, y, m2) else 0
            pc += 4
        elif op == 99:
            break
    return output

M = list(map(int, InputStr(5).split(',')))
assert interpret(M, [1]) == [0, 0, 0, 0, 0, 0, 0, 0, 0, 12896948]

assert interpret([3,9,8,9,10,9,4,9,99,-1,8], [7]) == [0]
assert interpret([3,3,1108,-1,8,3,4,3,99], [8]) == [1]
M = list(map(int, InputStr(5).split(',')))
assert interpret(M, [5]) == [7704130]

assert op_mode(1002) == (2, 0, 1, 0)
assert op_mode(1102) == (2, 1, 1, 0)
assert op_mode(11102) == (2, 1, 1, 1)
