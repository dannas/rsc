#!/usr/bin/env python

"""
NAME
    testdriver.py

SYNOPSIS
    python testdriver.py SRCDIR BUILDDIR ASSEMBLER-FILE

DESCRIPTION
    Calls the assembler that creates an intermediate file. Feed that file to
    the interpreter. Verifies each step.
"""

import sys
import os.path
import subprocess

def basename(path):
    """ 
    Mimics basename(3). 

    >>> basename('path/to/filename.suffix')
    'filename'
    """
    tail = path.split('/')[-1]
    b = tail.split('.')[0]
    return b

def run_test(execpath, inpath, outpath):
    cmdline = ' '.join([execpath, inpath, outpath])
    assert subprocess.call(cmdline, shell=True) == 0

def main():
    srcdir, bindir, fname = sys.argv[1:]
    bname = basename(fname)

    inpath = os.path.join(srcdir, fname)
    # TODO(dannas): Create the outpath in the /tmp directory
    outpath = os.path.join(srcdir, 'testdata', bname + '.out')
    execpath = os.path.join(bindir, 'assembler')
    print execpath, inpath, outpath

    run_test(execpath, inpath, outpath)

    # TODO(dannas): Run interpreter
    # TODO(dannas): Verify that the output is the expected

main()
