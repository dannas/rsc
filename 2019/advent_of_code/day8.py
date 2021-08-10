from common import *

def Layers(image, width=25, height=6):
    image = mapt(int, image)
    N = len(image)
    M = width*height
    return [[ image[f+r:f+r+width] for r in range(0, M, width)]  for f in range(0, N, M)]

def count_digits(layer, digit=0):
    return sum(row.count(digit) for row in layer)

def fewest_zeroes(layers):
    return min(layers, key=count_digits)

def checksum(layers):
    layer = fewest_zeroes(layers)
    return count_digits(layer, 1) * count_digits(layer, 2)

input = Layers(InputStr(8))
assert checksum(input) == 1064

def decode(image, width=25, height=6):
    chars = {0 : ' ', 1 : '█', 2 : ' '}
    final = [[2 for col in range(width)] for row in range(height)]
    for layer in reversed(image):
        for row in range(height):
            for col in range(width):
                if layer[row][col] != 2:
                    final[row][col] = chars[layer[row][col]]
    for row in final:
        print(''.join(row))

example = Layers("0222112222120000", width=2, height=2)
decode(example, width=2, height=2)

input = Layers(InputStr(8))
decode(input)