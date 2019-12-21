import re
from itertools import permutations
from collections import defaultdict

def Input(day, line_parser=str.split):
    "For this day's input file, return a tuple of each line parsed by `line_parser`."
    filename = 'advent2019/input{}.txt'.format(day)
    return mapt(line_parser, open(filename))

def InputStr(day):
    filename = 'advent2019/input{}.txt'.format(day)
    return open(filename).read().rstrip('\n')

def array(string):
    return [vector(line) for line in string.splitlines()]

def vector(line):
    return [atom(word) for word in line.split()]

# TODO(dannas): Check floats as well
def atom(word):
    try:
        return int(word)
    except ValueError:
        return word

def mapt(fn, *args):
    return tuple(map(fn, *args))

def ints(line):
    matches = re.findall(r'\d+', line, re.MULTILINE)
    return mapt(int, matches)

NORTH, SOUTH, WEST, EAST = (0, 1), (0, -1), (-1, 0), (1, 0)

# 2-D points implemented using (x, y) tuples
def X(point): return point[0]
def Y(point): return point[1]

def move(pos, direction):
    return (X(pos) + X(direction), Y(pos) + Y(direction))

def neighbors4(point):
    "The four neighbors (without diagnoals)"
    x, y = point
    return ((x+1, y), (x-1, y), (x, y+1), (x, y-1))

def neighbors8(point):
    "The eight neighbors (with diagonals)"
    x, y =  point
    return ((x+1, y), (x-1, y), (x, y+1), (x, y-1),
           (x+1, y+1), (x-1, y-1), (x+1, y-1), (x-1, y+1))

def cityblock_distance(p, q=(0, 0)): 
    "City block distance between two points."
    return abs(X(p) - X(q)) + abs(Y(p) - Y(q))