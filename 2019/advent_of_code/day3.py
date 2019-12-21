from common import *

directions = {
    'R' : EAST,
    'L' : WEST,
    'U' : NORTH,
    'D' : SOUTH,
}

def find_closest(wires):
    seen = set()
    closest = 10**30
    for w in wires:
        pos = (0, 0)
        wire_pos = set()
        for direction, steps in w:
            for _ in range(steps):
                pos = move(pos, direction)
                if pos in seen:
                    closest = min(closest, cityblock_distance(pos))
                wire_pos.add(pos)
        seen.update(wire_pos)
    return closest


def parse_step(val):
    d = directions[val[0]]
    steps = int(val[1:])
    return (d, steps)

def parse_wire(wire):
    return map(parse_step, wire)

def day3(input, fn=find_closest):
    lines = input.splitlines()
    wires = [parse_wire(L.split(',')) for L in lines]
    return fn(wires)


assert day3("R8,U5,L5,D3\nU7,R6,D4,L4") == 6   

assert day3("""R75,D30,R83,U83,L12,D49,R71,U7,L72
U62,R66,U55,R34,D71,R55,D58,R83""") == 159

assert day3("""R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51
U98,R91,D20,R16,D67,R40,U7,R15,U6,R7""") == 135

assert day3(InputStr(3)) == 375

def find_min_steps(wires):
    seen = {}
    closest = 10**30
    for w in wires:
        num_steps = 0
        pos = (0, 0)
        wire_pos = {}
        for direction, steps in w:
            for _ in range(steps):
                pos = move(pos, direction)
                num_steps += 1
                if pos in seen:
                    closest = min(closest, seen[pos] + num_steps)
                if pos not in wire_pos:
                    wire_pos[pos] = num_steps
        seen.update(wire_pos)
    return closest

assert day3("R8,U5,L5,D3\nU7,R6,D4,L4", find_min_steps) == 30

assert day3("""R75,D30,R83,U83,L12,D49,R71,U7,L72
U62,R66,U55,R34,D71,R55,D58,R83""", find_min_steps) == 610

assert day3("""R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51
U98,R91,D20,R16,D67,R40,U7,R15,U6,R7""", find_min_steps) == 410

print(day3(InputStr(3), find_min_steps))