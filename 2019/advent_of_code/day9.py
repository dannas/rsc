from common import *

relative_base = 0

def op_mode(val):
    op = val % 100
    m1 = (val // 100) % 10
    m2 = (val // 1000) % 10
    m3 = (val // 10000) % 10
    return op, m1, m2, m3

def mem(M, param, mode):
    global relative_base
    if mode == 0:
        return M[param]
    elif mode == 1:
        return param
    elif mode == 2:
        return M[relative_base + param]

def interpret(M, input, pc=0):
    global relative_base
    relative_base = 0
    output = []
    input_device = iter(input)
    while True:
        op, m1, m2, m3 = op_mode(M[pc])
        #print('pc=',pc,M[pc], M[pc+1:pc+4], relative_base)
        if op == 1: 
            x, y, dst = M[pc+1:pc+4]
            if m3 == 2: dst = dst + relative_base
            M[dst] = mem(M, x, m1) + mem(M, y, m2)
            pc += 4
        elif op == 2:
            x, y, dst = M[pc+1:pc+4]
            if m3 == 2: dst = dst + relative_base
            M[dst] = mem(M, x, m1) * mem(M, y, m2)
            pc += 4
        elif op == 3:
            dst = M[pc+1]
            if m1 == 2: 
                dst = dst + relative_base
            M[dst] = next(input_device)
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
            if m3 == 2: dst = dst + relative_base
            M[dst] = 1 if mem(M, x, m1) < mem(M, y, m2) else 0
            pc += 4
        elif op == 8:
            x, y, dst = M[pc+1:pc+4]
            if m3 == 2: dst = dst + relative_base
            M[dst] = 1 if mem(M, x, m1) == mem(M, y, m2) else 0
            pc += 4
        elif op == 9:
            x = M[pc+1]
            relative_base += mem(M, x, m1)
            pc += 2
        elif op == 99:
            break
    return output

def Memory(codes, mem_size=10**6):
    return codes + [0 for _ in range(mem_size)]

# Output quine
assert interpret(Memory([109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99]), []) == [109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99]
# Output 16 digit number
assert len(str(interpret([1102,34915192,34915192,7,4,7,99,0], [])[0])) == 16
# Output the large number in the middle
assert interpret([104,1125899906842624,99], []) == [1125899906842624]

print('hello world')
M = Memory(list(map(int, InputStr(9).split(','))))
assert interpret(M, [1]) == [2682107844]

M = Memory(list(map(int, InputStr(9).split(','))))
assert interpret(M, [2]) == [34738]
