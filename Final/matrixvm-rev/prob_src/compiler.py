#!/usr/bin/python

from pwn import *
import sys

def new_matrix(out, row, col, data):
    code = b""
    code += p8(0)
    code += p64(out)
    code += p64(row)
    code += p64(col)
    assert(len(data) == row * col)
    for x in data:
        code += p64(x)
    return code

def mul_matrix(out, op1, op2):
    code = b""
    code += p8(1)
    code += p64(out)
    code += p64(op1)
    code += p64(op2)
    return code

def index_read(op, row, col):
    code = b""
    code += p8(2)
    code += p64(op)
    code += p64(row)
    code += p64(col)
    return code

def index_write(op, row, col):
    code = b""
    code += p8(3)
    code += p64(op)
    code += p64(row)
    code += p64(col)
    return code

def index_move(dest, dest_row, dest_col, src, src_row, src_col):
    code = b""
    code += p8(4)
    code += p64(dest)
    code += p64(dest_row)
    code += p64(dest_col)
    code += p64(src)
    code += p64(src_row)
    code += p64(src_col)
    return code

def branch(op, label):
    code = b""
    code += p8(5)
    code += p64(op)
    if label < 0:
        label = label + 2**64
    code += p64(label)
    return code

if __name__ == "__main__":

    code = b""
    code += new_matrix(1337, 2, 2, [ord('a') for _ in range(4)])
    code += index_write(1337, 0, 0)
    code += branch(1337, 1)
    code += index_move(1337, 0, 0, 1337, 1, 1)
    
    with open(sys.argv[1], "wb") as f:
        f.write(code)
    