from common import *

test_data = '''0: 1 2
1: "a"
2: 1 3 | 3 1
3: "b"'''

day = test_data

def Atom(token):
    try:
        return int(token)
    except ValueError:
        if token.startswith('"'):
            return token[1]
        return token
def parse(line):
    key, val = line.split(':')
    val = [x.split() for x in val.split('|')]
    return int(key), [[Atom(y) for y in x] for x in val]
rules = dict(Input(day, parse))

def expand(rules, key, prefix=''):
    """ Return list of strings"""
    alt = rules[key]
    if len(alt) == 1 and isinstance(val[0], str):
        return val[0]
    return [prefix+r for seq in alt for val in seq for r in expand(rules, val)]

#1 min/km => 60 min / 60 km => 60 km/h
# 2 min/km => 60 / 2 * 

    
