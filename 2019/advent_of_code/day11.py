from common import *

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
    input_device = iter(input)
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
            try:
                M[dst] = next(input_device)
            except StopIteration:
                break
            pc += 2
        elif op == 4:
            x = M[pc+1]
            output.append(mem(M, x, m1))
            pc += 2
            break
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
    #print(output, pc, op)
    return output, pc, op

M = list(map(int, InputStr(11).split(',')))

print(M)
