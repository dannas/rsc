from common import *

# Report Repair
# 15 min
# A variant of the 3sum problem. Brute force is fast enough.
def expenses(day=1):
    "Find expense numbers whose sum equals 2020."
    E = Input(day, line_parser=int)
    a = first(x*y for x, y in combinations(E, 2) if x+y == 2020)
    b = first(x*y*z for x, y, z in combinations(E, 3) if x+y+z == 2020)
    return a, b

# Password Philosophy
# 15 min
# Mostly a parsing task.
def count_valid_passwords(day=2):
    def parse_policy(line):
        "A line consists of policy as begin-end, letter, and password."
        m = re.findall(r"(\d+)-(\d+) (\w): (\w+)", line)[0]
        return int(m[0]), int(m[1]), m[2] ,m[3]
    P = Input(day, line_parser=parse_policy)
    a = sum(1 for b, e, c, s in P if b <= s.count(c) <= e)
    b = sum(1 for b, e, c, s in P if (s[b-1] == c) != (s[e-1] == c))
    return a, b

# Toboggan Trajectory
# 30 min
# The map repeats in the horizontal direction. Take advantage of that by
# letting the x coordinate be modulo the width of the original area.
def count_trees_hit(day=3):
    "Given grid and slope, find numbers of trees hit during trajectory."
    from math import prod
    area = Input(day, line_parser=str.strip)
    M, N = len(area), len(area[0])
    def path(dx, dy):
        "Generate positions until y == M. Restart x positions at zero when we reach N"
        return ((x % N , y) for x, y in zip(count(0, dx), range(0, M, dy)))
    def num_trees(dx, dy):
        return sum(1 for x, y in path(dx, dy) if area[y][x] == '#')
    a = num_trees(3, 1)
    b = prod(num_trees(*slope) for slope in [(1, 1), (3, 1), (5, 1), (7, 1), (1, 2)])
    return a, b

# Passport Processing
# 60 min
# Some time wasted from mucking around with list comprehensions and the regex API. 
# It's been a while since I used Python.
def count_valid_passports(day=4):
    "Verify that passport fields are present and valid"
    def parse_fields(line):
        return dict(e.split(':') for e in line.split())
    P = Input(day, parse_fields, line_ending='\n\n')
    keys = "byr iyr eyr hgt hcl ecl pid".split()
    def present(passport):
        return all(f in passport for f in keys)
    def valid(passport):
        byr, iyr, eyr, hgt, hcl, ecl, pid = [passport[f] for f in keys]
        unit, hgt = hgt[-2:], hgt[:-2]
        return ( 
        1920 <= int(byr) <= 2002 and 
        2010 <= int(iyr)<= 2020 and
        2020 <= int(eyr) <= 2030 and
        ((unit == 'cm' and 150 <=int(hgt) <= 193) or (unit == 'in' and 59 <= int(hgt) <= 76)) and
        re.fullmatch(r'#[0-9a-f]{6}', hcl) and
        ecl in ('amb', 'blu', 'brn', 'gry', 'grn', 'hzl', 'oth') and
        re.fullmatch(r'[0-9]{9}', pid)
        )
    a = sum(1 for p in P if present(p))
    b = sum(1 for p in P if present(p) and valid(p))
    return a, b

# Binary Boarding
# 20 min
# For task b, find the number that is missing.
def find_boarding_pass(day=5):
    def decode(line):
        """Decode a string of Back, Front, Left, Right commands into a binary
        number"""
        table = dict(zip("BFRL", "1010"))
        val = cat(table[x] for x in line.strip())
        return int(val[:7], base=2), int(val[7:], base=2)
    numbers = Input(day, decode)
    seat_ids = sorted(row * 8 + col for row, col in numbers)
    a = max(seat_ids)
    b = first(x+1 for x, y in zip(seat_ids, seat_ids[1:]) if y-x > 1)
    return a, b

# Custom Customs
# 15 min
def count_answers(day=6):
    def make_set(line):
        return mapt(set, line.split())
    answers = Input(day, make_set, line_ending='\n\n')
    a = sum(len(set.union(*group)) for group in answers)
    b = sum(len(set.intersection(*group)) for group in answers)
    return a, b

# Handy Haversacks
# 30 min
# I first had two functions for part 1 and part 2, but then factored them
# into a generator.
def count_bags(day=7):
    def parse_rule(line):
        "Remove superflous words, extract container and list of inner bags."
        k, v = re.sub(r'bags?\.?', '', line).split(' contain ')
        colors = re.findall(r'(\d+) (\w+ \w+)', v)
        return k.strip(), [(name, int(n)) for n, name in colors] 
    def iterate_bags(key, N=None):
        "Breadth first scan of inner bags."
        q = [key]
        while q:
            for name, n in rules[q.pop()]:
                q.extend([name for _ in range(1 if N else n)])
                yield name, n
    rules = dict(Input(day, parse_rule))
    a = sum(1 for k in rules if any(bag == 'shiny gold' for bag, _ in iterate_bags(k, 1)))
    b = sum(n for _, n in iterate_bags('shiny gold'))
    return a, b

# Handheld Halting
# 30 min
# Dunno what I was smoking. I stumbled on part 1: write a simple 
# straightforward interpreter. Misread the instructions.
def find_inifinite_loop(day=8):
    def parse_instr(line):
        op, arg = line.split()
        return op, int(arg)
    def run(program):
        pc = acc = 0
        visited = set()
        while True:
            op, arg = program[pc]
            if pc in visited: return 0, acc
            visited.add(pc)
            pc += 1
            if pc >= len(program): return 1, acc
            if   op == 'nop':  pass
            elif op == 'acc':  acc += arg
            elif op == 'jmp':  pc += arg - 1
    def modifications(P):
        table = {'nop' : 'jmp', 'jmp' : 'nop'}
        for i, (op, arg) in enumerate(P):
            if op in table:
                val = (table[op], arg)
                yield P[:i] + [val] + P[i+1:]
    program = list(Input(day, parse_instr))
    a = run(program)[1]
    b = first(acc for exit, acc in map(run, modifications(program)) if exit == 1)
    return a, b

# Encoding Error
# 30 min
# Used a brute force solution first that took more than 1s to run.
# I tried experimenting with prefix sums, before I realized that a sliding
# window can be used.
def find_encryption_weakness(day=9):
    def not_sum_numbers():
        "Return numbers who is not the sum of two of the previous 25 numbers"
        for i in range(25, len(numbers)):
            val = numbers[i]
            if not any((x, y) for x, y in combinations(numbers[i-25:i], 2) if x+y==val):
                yield val
    def matching_range(L, val):
        "Return range whose sum matches val."
        i, j = 0, 1
        while sum(L[i:j]) != val:
            if i != j and sum(L[i:j]) > val:
                i += 1
            else:
                j += 1
        return L[i:j]
    numbers = Input(day, int)
    a = first(val for val in not_sum_numbers())
    x = matching_range(numbers, a)
    b = min(x) + max(x)
    return a, b

# Adapter Array
# I had to give up on part 2 and cheat. 
# Spent an hour writing three different
# functions that all calculated the right value for the test input, but took
# too long for the real input. 
# I need to do more Dynamic Programming.
def find_adapter_chain(day=10):
    def count_joltage_diff():
        ones = sum(1 for x, y in zip(L, L[1:]) if y-x == 1)
        threes = sum(1 for x, y in zip(L, L[1:]) if y-x == 3)
        return ones, threes
    @cache
    def find_arrangements(pos=0):
        if pos == len(L)-1: return 1
        return sum(find_arrangements(i) for i in range(pos+1, len(L)) if L[i] - L[pos] <= 3)
    L = sorted(Input(day, int))
    L = [0] + L + [L[-1]+3]
    a = prod(count_joltage_diff())
    b = find_arrangements()
    return a, b

# Rain Risk
def find_ferry_position(day=12):
    headings = NORTH, EAST, SOUTH, WEST = (0, -1), (1, 0), (0, 1), (-1, 0)
    def parse(line):
        table = {'N' : NORTH, 'E' : EAST, 'S' : SOUTH, 'W' : WEST}
        if line[0] in table:
            return table[line[0]], int(line[1:])
        return line[0], int(line[1:])
    def move(pos, direction, steps):
        num_steps = tuple(x * steps for x in direction)
        return mapt(sum, zip(pos, num_steps))
    def turn(degrees, direction = EAST):
        val = int(degrees / 90)
        i = (headings.index(direction) + val) % len(headings)
        return headings[i]
    def manhattan_distance(pos):
        x, y = pos
        return abs(x) + abs(y)
    def distance(p, q):
        return (p[0]-q[0], p[1]-q[1])
    def run(instructions, pos = (0, 0), direction = EAST):
        for op, arg in instructions:
            if op in headings:
                pos = move(pos, op, arg)
            elif op == 'L':
                direction = turn(-arg, direction)
            elif op == 'R':
                direction = turn(arg, direction)
            elif op == 'F':
                pos = move(pos, direction, arg)
        return pos
    def rotate(pos, degrees):
        x, y = pos
        if degrees in (90, -270):
            x, y = -y, x
        elif degrees in (180, -180):
            x, y = (-x, -y)
        elif degrees in (270, -90):
            x, y = y, -x
        return x, y
    def run2(instructions, pos = (0, 0), waypoint = (10, -1), direction = EAST):
        for op, arg in instructions:
            if op in headings:
                waypoint = move(waypoint, op, arg)
            elif op == 'L':
                diff = distance(waypoint, pos)
                diff = rotate(diff, -arg)
                waypoint = move(pos, diff, 1)
            elif op == 'R':
                diff = distance(waypoint, pos)
                diff = rotate(diff, arg)
                waypoint = move(pos, diff, 1)
            elif op == 'F':
                diff = distance(waypoint, pos)
                pos = move(pos, diff, arg)
                waypoint = move(pos, diff, 1)
        return pos
    instructions = Input(day, parse)
    a = manhattan_distance(run(instructions))
    b = manhattan_distance(run2(instructions))
    return a, b

# Shuttle Search
def find_earliest_shuttle(day=13):
    def parse_timestamps(line):
        return mapt(int, re.findall(r'(\d+)', line))
    def parse_buses(line):
        return [(int(x), i) for i, x in enumerate(line.split(',')) if x != 'x']
    def find_matching_timestamp(buses):
        step, t = buses[0]
        for b, i in buses[1:]:
            while (t + i) % b != 0:
                t += step
            step *= b
        return t
    earliest, timestamps = Input(day, parse_timestamps)
    earliest = earliest[0]
    ts, bus = min((earliest - (earliest % x) + x, x) for x in timestamps)
    a = (ts - earliest) * bus
    _, buses = Input(day, parse_buses)
    b = find_matching_timestamp(buses)
    return a, b

# Docking Data
def decode_initialization(day=14):
    def parse(line):
        if line.startswith('mask'):
            _, mask = line.split(' = ')
            one_mask = sum((1<<i) for i, val in enumerate(reversed(mask)) if val == '1')
            zero_mask = sum((1<<i) for i, val in enumerate(reversed(mask)) if val in 'X1')
            return 'mask', one_mask, zero_mask
        else:
            addr, val = re.findall('mem\[(\d+)\] = (\d+)', line)[0]
            return 'mem', int(addr), int(val)
    def run(instructions):
        M = defaultdict(int)
        one_mask, zero_mask = 0, 0
        for op, *args in instructions:
            if op == 'mask':
                one_mask, zero_mask = args
            elif op == 'mem':
                addr, val = args
                M[addr] = (val | one_mask) & zero_mask
        return sum(v for k, v in M.items())
    def parse2(line):
        if line.startswith('mask'):
            return tuple(line.split(' = '))
        else:
            addr, val = re.findall('mem\[(\d+)\] = (\d+)', line)[0]
            return 'mem', '{:036b}'.format(int(addr)), int(val)
    def gen_bin_numbers(mask):
        num_positions = mask.count('X')
        max_num = (1<<num_positions)
        for i in range(max_num):
            yield '{num:0{width}b}'.format(num=i, width=num_positions)
    def generate_addresses(addr, mask):
        addr = cat(x if x in 'X1' else y for x, y in zip(mask, addr))
        for y in gen_bin_numbers(addr):
            it = iter(y)
            yield int(cat([x if x != 'X' else next(it) for x in addr]), base=2)
    def run2(instructions):
        mask = ''
        M = defaultdict(int)
        for op, *args in instructions:
            if op == 'mask':
                mask = args[0]
            elif op == 'mem':
                addr, val = args
                for a in generate_addresses(addr, mask):
                    M[a] = val
        return sum(v for k, v in M.items())
    instructions = Input(day, parse)
    a = run(instructions)
    instructions = Input(day, parse2)
    b = run2(instructions)
    return a, b

# Rambunctious Recitation
def find_last_spoken():
    def run(starting_numbers, limit=2020):
        spoken_numbers = {}
        steps = 1
        for val in test_data:
            spoken_numbers[val] = steps
            spoken = 0
            steps += 1
        while steps < limit:
            if spoken in spoken_numbers:
                diff = steps - spoken_numbers[spoken]
                spoken_numbers[spoken] = steps
                spoken = diff
            else:
                spoken_numbers[spoken] = steps
                spoken = 0
            steps += 1
        return spoken
    input = [13,0,10,12,1,5,8]
    a = run(input, limit=2020)
    b = run(input, limit=30000000)
    return a, b

# Ticket Translation
def translate_tickets(day=16):
    def parse_interval(word):
        a, b = re.findall('(\d+)-(\d+)', word)[0]
        return int(a), int(b)
    def parse_rule(line):
        k, a, b, c, d = re.findall('([^:]+): (\d+)-(\d+) or (\d+)-(\d+)', line)[0]
        return k, int(a), int(b), int(c), int(d)
    def parse_lines(lines):
        RULES, YOUR, NEARBY = range(3)
        state = RULES
        rules = {}
        your = []
        nearby = []
        for line in lines:
            if len(line) == 0:
                state += 1
            elif state == RULES:
                key, *r = parse_rule(line)
                rules[key] = r
            elif state == YOUR and not line.startswith('your'):
                your = mapt(int, line.split(','))
            elif state == NEARBY and not line.startswith('nearby'):
                nearby.append(mapt(int, line.split(',')))
        return rules, your, nearby
    def count_scanning_error_rate(rules, nearby):
        allowed = set(val for a,b, c, d in rules.values() for val in chain(range(a, b+1), range(c, d+1)))
        return sum(f for val in nearby for f in val if f not in allowed)
    def column_valid(key, rules, column):
            a, b, c, d = rules[key]
            return all((a <= x <= b+1) or (c <= x <= d+1) for x in column)
    def minmax(fields): return min(fields), max(fields)

    rules, your, nearby = parse_lines(Input(day, str))

    allowed = set(val for a,b, c, d in rules.values() for val in chain(range(a, b+1), range(c, d+1)))
    tickets = [val for val in nearby if all(f in allowed for f in val)]

    columns = mapt(list, zip(*tickets))

    L = [(sum([(column_valid(k, rules, c)) for c in columns]), k) for k in rules]
    sorted(L)

    a = count_scanning_error_rate(rules, nearby)
    return a



test_data = """.#.
..#
###"""

day = test_data

# 3d grid x, y, z
# active (#) or inactive (.)

# Execute 6 cycles

# active and 2-3 neighbours acgtive => active
# inactive 3 neighbors active => active
# else inactive

# simulate

def neighbors(coordinate):
    N = len(coordinate)
    for val in product((-1, 0, 1), repeat=N):
        if not all(i == 0 for i in val):
            yield tuple(i+j for i, j in zip(coordinate, val))

class Grid(defaultdict):
    def __init__(self, *args):
        defaultdict.__init__(self, lambda : '.', *args)

def active_neighbors(grid, pos):
    return sum(1 for x in neighbors(pos) if x in grid and grid[x] == '#')

def apply_rule(grid, pos):
    N = active_neighbors(grid, pos)
    val = '.' if pos not in grid else grid[pos]
    if val == '#' and N not in (3, 4):
        return pos, '#'
    if val == '.' and N == 3:
        return pos, '#'
    return pos, val
def visit(grid, pos):
    yield apply_rule(grid, pos)
    for p in neighbors(pos):
        yield apply_rule(grid, p)

grid = Grid(((i, j, 0), col) 
    for i, row in enumerate(Input(day, str)) 
    for j, col in enumerate(row))

for _ in range(5):
    grid = Grid((p, val) for pos in grid for p, val in visit(grid, pos))
sum(1 for pos in grid if grid[pos] == '#')

def operation_order(day=18):
    def to_rpn(expression, verbose=False):
        ops, Q = [], []
        for token in expression:
            if token in '+*':
                while ops and ops[-1] != '(':
                    Q.append(ops.pop())
                ops.append(token)
            elif token in '(':
                ops.append(token)
            elif token in ')':
                while ops and ops[-1] != '(':
                    Q.append(ops.pop())
                ops.pop()
            else:
                Q.append(int(token))
            if verbose: print(Q, ops)
        while ops:
            Q.append(ops.pop())
        return Q
    def calculate(rpn):
        stack = []
        for token in rpn:
            if isinstance(token, int):
                stack.append(token)
            else:
                a = stack.pop()
                b = stack.pop()
                r = a * b if token == '*' else a + b
                stack.append(r)
        return stack.pop()
    def to_rpn2(expression, verbose=False):
        ops, Q = [], []
        for token in expression:
            if token == '*':
                while ops and ops[-1] != '(':
                    Q.append(ops.pop())
                ops.append(token)
            elif token == '+':
                ops.append(token)
            elif token in '(':
                ops.append(token)
            elif token in ')':
                while ops and ops[-1] != '(':
                    Q.append(ops.pop())
                ops.pop()
            else:
                Q.append(int(token))
            if verbose: print(Q, ops)
        while ops:
            Q.append(ops.pop())
        return Q
    tokenize = re.compile(r'\d+|\+|\*|\(|\)').findall
    expressions = Input(day, tokenize)
    a = sum(calculate(to_rpn(expr)) for expr in expressions)
    b = sum(calculate(to_rpn2(expr)) for expr in expressions)
    return a, b
def run_tests():
    assert expenses() == (357504, 12747392)
    assert count_valid_passwords() == (519, 708)
    assert count_trees_hit() == (200, 3737923200)
    assert count_valid_passports() == (237, 172)
    assert find_boarding_pass() == (953, 615)
    assert count_answers() == (6259, 3178)
    assert count_bags() == (289, 30055)
    assert find_inifinite_loop() == (1810, 969)
    assert find_encryption_weakness() == (105950735, 13826915)
    assert find_adapter_chain() == (2775, 518344341716992)
    assert find_ferry_position() == (1496, 63843)
    assert find_earliest_shuttle() == (115, 756261495958122)
    assert decode_initialization() == (5875750429995, 5272149590143)
    #assert find_last_spoken() == (260, 950)
    assert translate_tickets() == 19240
    assert operation_order() == (29839238838303, 201376568795521)
    print("all tests passed")
run_tests()
