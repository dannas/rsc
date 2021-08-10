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
                M[dst] = next(input_device)
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
    return output

M = list(map(int, InputStr(7).split(',')))
#print(M[:20])

def amplifier(M, phase, input):
    Mcopy = M[:]
    return interpret(Mcopy, [phase, input])[0]

def amplifier_controller(M, phases):
    r = amplifier(M, phases[0], 0)
    print(r)
    for phase in phases[1:]:
        r = amplifier(M, phase, r)
        print(r)
    return r

#print("      ", amplifier_controller([3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0], [4, 3, 2, 1, 0]))
#print(amplifier_controller([3,23,3,24,1002,24,10,24,1002,23,-1,23,101,5,23,23,1,24,23,23,4,23,99,0,0], [0, 1, 2, 3, 4]))
#print("      ", amplifier([3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0], 4, 0))

def find_max_output_signal(M):
    m = -1
    for phases in permutations(range(5), 5):
        m = max(m, amplifier_controller(M, phases))
    return m
    

#print(find_max_output_signal([3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0]))
#print(find_max_output_signal(M))
        

# IN,15,
# IN,16,
# 1002,16,10,16,
# 1,16,15,15,   #8,9,10
# OUT,15,
# HALT,
# 0,   #15
# 0    #16

def interpret2(M, input, pc=0):
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

def feedback_loop(M, phases):
    M1 = M[:]
    M2 = M[:]
    M3 = M[:]
    M4 = M[:]
    M5 = M[:]
    op = 1
    r1, pc1, _ = interpret2(M1, [phases[0], 0])
    r2, pc2, _ = interpret2(M2, [phases[1]] + r1)
    r3, pc3, _ = interpret2(M3, [phases[2]] + r2)
    r4, pc4, _ = interpret2(M4, [phases[3]] + r3)
    r5, pc5, op = interpret2(M5,[phases[4]] + r4)
    last_result = 0
    while op != 99:
        r1, pc1, _ = interpret2(M1, r5, pc1)
        r2, pc2, _ = interpret2(M2, r1, pc2)
        r3, pc3, _ = interpret2(M3, r2, pc3)
        r4, pc4, _ = interpret2(M4, r3, pc4)
        r5, pc5, op = interpret2(M5, r4, pc5)
        if op != 99:
            last_result = r5[0]
        print(r5, op)
    return last_result

#feedback_loop(M, [])
def find_max_feedback(M):
    m = -1
    for phases in permutations(range(5, 10), 5):
        m = max(m, feedback_loop(M, phases))
    return m

r = find_max_feedback([3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26, 27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5])
#r = feedback_loop([3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26, 27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5], [9,8,7,6,5])
r = find_max_feedback(M)
print(r)