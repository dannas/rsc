import re
from itertools import permutations, combinations, count, chain, product
from collections import defaultdict
from functools import lru_cache
from math import prod

#### FILE INPUT AND PARSING ######################################
# Adapted from Peter Norvigs Advent of Code solutions, https://github.com/norvig/pytudes.
def Input(day, line_parser=str.split, line_ending='\n'):
    "For this day's input file, return a tuple of each line parsed by `line_parser`."
    if isinstance(day, int):
        filename = 'advent2020/input{}.txt'.format(day)
        buf = open(filename).read().rstrip('\n')
        return mapt(line_parser, buf.split(line_ending))
    return mapt(line_parser, day.rstrip('\n').split(line_ending))

def mapt(fn, *args):
    return tuple(map(fn, *args))

# 2-D points implemented using (x, y) tuples
def X(point): return point[0]
def Y(point): return point[1]

def neighbors8(point):
    "The eight neighbors (with diagonals)"
    x, y =  point
    return ((x+1, y), (x-1, y), (x, y+1), (x, y-1),
           (x+1, y+1), (x-1, y-1), (x+1, y-1), (x-1, y+1))

cat = ''.join
first = next
cache = lru_cache(None)
#################################################################