from common import *

def day1(masses):
    return sum(int(m / 3) - 2 for m in masses)

def day1b(masses):
    total_fuel = 0
    for m in masses:
        fuel = int(m / 3) - 2
        t = int(fuel / 3) - 2
        while t > 0:
            fuel += t
            t = int(t / 3) - 2
        total_fuel += fuel
    return total_fuel

assert day1(ints(InputStr(1))) == 3425624

assert day1b([14])  == 2
assert day1b([1969])  == 966
assert day1b(ints(InputStr(1))) == 5135558
