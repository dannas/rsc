from common import *

# 21 minutes
#
# BUGS
# Uncertain about how to convert int to str
# First tried to iterate using bitwise
# Uncertain about how to convert char back to int for comparison
# Missed to assign prev
# 2b: Had num_streak set to 0 but should have been 1
#
# Possible improvements
# Use division and modulo by ten for looping over the numbers
# 

def valid_password(val):
    val = str(val)
    prev = val[0]
    has_pair = False
    for x in val[1:]:
        if x == prev: has_pair = True
        if ord(x) < ord(prev):
            return False
        prev = x
    return has_pair


def day4(start=271973, end=785961):
    return len([x for x in range(start, end+1) if valid_password(x)])

assert valid_password(111111) == True
assert valid_password(223450) == False
assert valid_password(123789) == False

assert day4() == 925

def only_one_pair(val):
    val = str(val)
    prev = val[0]
    has_pair = False
    num_streak = 1
    for x in val[1:]:
        if x == prev:
            num_streak += 1
        else:
            if num_streak == 2:
                has_pair = True
            num_streak = 1
        if ord(x) < ord(prev):
            return False
        prev = x
    return has_pair or num_streak == 2

def day4b(start=271973, end=785961):
    return len([x for x in range(start, end+1) if only_one_pair(x)])

assert day4b() == 607