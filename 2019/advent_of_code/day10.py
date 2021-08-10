from common import *
import math

EMPTY = '.'
ASTEROID = '#'

def positions_between(p, q):
    """ Return a list of  positions following the hypotenusa. """
    ymin = min(Y(p), Y(q))
    ymax = max(Y(p), Y(q))
    ydelta = Y(p) - Y(q)
    xdelta = X(p) - X(q)
    ratio = 1 if (xdelta == 0 or ydelta == 0) else ydelta / xdelta

    #positions = [(y / ratio, float(y)) for y in range(ymin+1, ymax)]
    positions = [(y / ratio, float(y)) for y in range(ymin, ymax)]
    #print('          ', positions)
    return [p for p in positions if X(p).is_integer()], positions

#print(list(positions_between((0, 0), (-2, -2))))
#print(list(positions_between((0, 0), (2, -2))))
#print(list(positions_between((0, 0), (2, 2))))
#print(list(positions_between((0, 0), (-2, 2))))
#print(list(positions_between((0, 0), (-10, 10))))

def asteroids(space_map):
    return [(j, i) for i, row in enumerate(space_map) for j, col in enumerate(row) if col == '#']

def optimal_station(space_map):
    best_pos = (0,0)
    best = 0
    asteroid_positions = asteroids(space_map)
    for p in asteroids(space_map):
        n = 0
        for q in asteroid_positions:
            #print('    ', q)
            L, T = positions_between(p, q)
            print('     ', p, q, L, T)
            if p != q and len([1 for P in L if P in asteroid_positions]) == 0:
                n += 1
        print('pos={} n={}'.format(p, n))
        if n > best:
            best = n
            best_pos = p 
    return best, best_pos
        
                

    # for each asteroid
    # keep track of how many have been seen
    # scan neighbours from closest to further away, or always start from 0,0
    # calculate the positions of 

def parse_line(line):
    return list(line.rstrip())
input = Input(10, parse_line)


example=""".#..#
.....
#####
....#
...##""".split()
print(optimal_station([ parse_line(L) for L in example]))