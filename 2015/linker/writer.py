from intermediate import Intermediate
import io

def roundup(addr):
    """ Roundup to next page boundary """
    PAGE = 0x1000
    return addr + PAGE & ~(PAGE-1)

def pwrite(f, offset, data):
    # TODO: Not atomic like pwrite(2)
    f.seek(offset)
    f.write(data)

def write(ir, fout):
    self.ir = ir
    self.fout = fout

    assert '.text' in self.ir.segments
    assert '.data' in self.ir.segments
    assert '.bss' in self.ir.segments

    fout.seek(0x100)

class Writer:
    """
    Take intermediate format. 
    find .text segment
        write it at 0x1000
    find .data segment
        write it at 0x1000 + len(.text) + offset to next page
    find .bss segment
        write it at to 4 byte boundary after end(data)
    """
    def __init__(self, ir, fout):
        self.ir = ir
        self.fout = fout

        assert '.text' in self.ir.segments
        assert '.data' in self.ir.segments
        assert '.bss' in self.ir.segments

        fout.seek(0x100)


def testroundup():
    assert roundup(0x001) == 0x1000
    assert roundup(0x1000) == 0x2000
    assert roundup(0x1001) == 0x2000
    assert roundup(0x1FFF) == 0x2000
    return "testroundup passed"

def testpwrite():
    def dotest(offset, expected):
        f = io.StringIO()
        pwrite(f, offset, '\xff')
        assert f.getvalue() == expected
    dotest(0,         '\xff')
    dotest(1,     '\x00\xff')
    dotest(2, '\x00\x00\xff')

testroundup()
testpwrite()
