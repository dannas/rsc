"""
Text format consisting of four sections; sizes given by values in header.

IDENTIFIER := 'LINK'
HEADER :=  DIGIT DIGIT DIGIT
SEGMENTS := SEGMENT*
SYMBOLS := SYMBOL*
RELOCATIONS := RELOCATION*
DATA := HEX*
SEGMENT := NAME ADDRESS LEN ATTRIBUTES
SYMBOL := NAME VALUE SEGMENT-NUMBER TYPE
RELOCATION := LOCATION SEGMENT SYMBOL-TO-BE-RELOCATED TYPE

Example:

    LINK
    3 2 3
    .text 1000 2500 RP
    .data 4000  C00 RWP
    .bss  5000 1900 RW
    foo      2    2  D
    bar      4    3  D
    1f       1    1 R4
"""

import sys
import re
import io

class ParserException(Exception):
    def __init__(self, msg):
        super().__init__(msg)

class Parser:
    def __init__(self, f):
        self.fin = f
        self.tokens = []
        self.lineno = 0
        self.segments = {}
        self.symbols = {}
        self.relocations = []
        self.data = []

        self.advance()
        nsegs, nsyms, nrels = self.readheader()

        self.readsegments(nsegs)
        self.readsymbols(nsyms)
        self.readrelocations(nrels)
        self.readdata(nsegs)

    def readheader(self):
        self.eat("LINK")
        nvals = map(int, self.tokens)
        self.advance()
        return nvals

    def readsegments(self, N):
        for _ in range(N):
            name, start, len, attr = self.tokens
            self.segments[name] = [int(start), int(len), attr]
            self.advance()

    def readsymbols(self, N):
        for _ in range(N):
            name, val, segment, type = self.tokens
            self.symbols[name] = [val, int(segment), type]
            self.advance()

    def readrelocations(self, N):
        for _ in range(N):
            loc, segment, ref, type = self.tokens
            self.relocations.append([int(loc), int(segment), int(ref), type])
            self.advance()

    def readdata(self, N):
        for _ in range(N):
            bytes = ''.join(self.tokens)
            self.data.append(bytes)
            self.advance()

    def advance(self):
        line = self.fin.readline()
        if line == '':
            raise ParserException('ParserError line %d: unexeptected EOF' % self.lineno)
        self.tokens = line.split()
        self.lineno += 1

    def eat(self, pat):
        if len(self.tokens) == 0 or not re.match(pat, self.tokens[0]):
            raise ParserException("ParserError line %d: expected '%s', got '%s'" % 
                    (self.lineno, pat, self.tokens))
        self.advance()


def printobject(p):
    print(p.segments)
    print(p.symbols)
    print(p.relocations)
    print(p.data)

s = """LINK
0 0 0

"""
p = Parser(io.StringIO(s))
assert p.segments == {}
assert p.symbols == {}
assert p.relocations == []
assert p.data == []

s = """LINK
1 1 1
.text 1000 3 RP
foo ff 1 D
01 1 1 A4
11223344

"""

p = Parser(io.StringIO(s))
assert p.segments == {'.text' : [1000, 3, 'RP']}
assert p.symbols == {'foo' : ['ff', 1, 'D']}
assert p.relocations == [[1, 1, 1, 'A4']]
assert p.data == ['11223344']
