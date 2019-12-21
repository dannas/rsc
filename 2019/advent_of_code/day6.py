from common import *

def count_parents(planet, orbits):
    parents = []
    if planet == 'COM':
        return 0, parents
    n = 0
    while planet != 'COM':
        planet = orbits[planet]
        parents.append(planet)
        n += 1
    return n, parents

print(count_parents('A', {'A': 'COM'}))
print(count_parents('B', {'A': 'COM', 'B' : 'A'}))

def count_orbits(orbit_map):
    orbits = {}
    # Fill in map with planet -> parent
    for x, y in orbit_map:
        orbits[y] = x
    num_orbits = 0
    orbits['COM'] = 'COM'

    # Traverse chain of parents and count
    # Stop when we reach 'COM'
    for planet in orbits:
        retval = count_parents(planet, orbits)
        n, _ = retval
        num_orbits += n
    return num_orbits

def parse(line): 
    return re.findall('[A-Z1-9]+', line)

input = Input(6, parse)
print(count_orbits(input))

example = [parse(L) for L in """COM)B B)C C)D """.split()]

example = [parse(L) for L in """ COM)B
B)C 
C)D
D)E
E)F
B)G
G)H
D)I
E)J
J)K
K)L
B)C
C)D
D)E
E)F
B)G
G)H
D)I
E)J
J)K
K)L""".split()]

print(example)
print(count_orbits(example))

def count_transfer(you, santa, orbit_map):
    orbits = {}
    # Fill in map with planet -> parent
    for x, y in orbit_map:
        orbits[y] = x
    num_orbits = 0
    orbits['COM'] = 'COM'

    _, you_path = count_parents(you, orbits)
    _, santa_path = count_parents(santa, orbits)
    for i, x in enumerate(you_path):
        for j, y in enumerate(santa_path):
            if x == y:
                return i+j
    return 0
example = [parse(L) for L in """COM)B
B)C
C)D
D)E
E)F
B)G
G)H
D)I
E)J
J)K
K)L
K)YOU
I)SAN""".split()]

print(count_transfer('YOU', 'SAN', input))