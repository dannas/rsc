from common import *

def interpret(M, pc=0):
    while True:
        op = M[pc]
        if op == 1:
            x, y, dst = M[pc+1:pc+4]
            M[dst] = M[x] + M[y]
        elif op == 2:
            x, y, dst = M[pc+1:pc+4]
            M[dst] = M[x] * M[y]
        elif op == 99:
            break
        pc += 4
    return M

def find_noun_verb(M, expected=19690720):
    for noun, verb in permutations(range(100), 2):
        Mcopy = M[:]
        Mcopy[1] = noun
        Mcopy[2] = verb
        if interpret(Mcopy)[0] == expected:
            return 100 * noun + verb

assert interpret([1, 0, 0, 0, 99]) == [2, 0, 0, 0, 99]
assert interpret([2, 3, 0, 3, 99]) == [2, 3, 0, 6, 99]
M = list(ints(InputStr(2)))
M[1] = 12
M[2] = 2
assert interpret(M)[0] == 5305097

assert find_noun_verb(list(ints(InputStr(2)))) == 4925