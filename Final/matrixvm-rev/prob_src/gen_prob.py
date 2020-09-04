#!/usr/bin/python

from compiler import *


def neg(x):
    return 0x10000000000000000 - x

GLOBAL_START = 1000
LOCAL_START = 2000

GLOBAL_MATRIX = [GLOBAL_START + i for i in range(1000)]
LOCAL_MATRIX = [LOCAL_START + i for i in range(1000)]
LOCAL_CURSOR = 0

# special variable definition
CONST_TABLE = 13100
ADD_TRANS = CONST_TABLE + 1
SUB_TRANS = ADD_TRANS + 1
TRANS_VAR = SUB_TRANS + 1
RESULT_VAR = TRANS_VAR + 1

def add(op1, op1r, op1c, op2, op2r, op2c, res, resr, resc):
    global LOCAL_CURSOR

    code = b''
    # copy value into var
    code += index_move(TRANS_VAR, 0, 0, op1, op1r, op1c)
    code += index_move(TRANS_VAR, 0, 1, op2, op2r, op2c)
    # multiply two matrix
    code += mul_matrix(LOCAL_MATRIX[LOCAL_CURSOR], TRANS_VAR, ADD_TRANS)
    # copy to res
    code += index_move(res, resr, resc, LOCAL_MATRIX[LOCAL_CURSOR], 0, 0)
    LOCAL_CURSOR += 1
    return code

def sub(op1, op1r, op1c, op2, op2r, op2c, res, resr, resc):
    global LOCAL_CURSOR

    code = b''
    # copy value into var 
    code += index_move(TRANS_VAR, 0, 0, op1, op1r, op1c)
    code += index_move(TRANS_VAR, 0, 1, op2, op2r, op2c)
    # multiply two matrix
    code += mul_matrix(LOCAL_MATRIX[LOCAL_CURSOR], TRANS_VAR, SUB_TRANS)
    # copy to res
    code += index_move(res, resr, resc, LOCAL_MATRIX[LOCAL_CURSOR], 0, 0)
    LOCAL_CURSOR += 1
    return code

def add_matrix(op1, op2, r, c, res):
    code = b''
    for i in range(r):
        for j in range(c):
            code += add(op1, i, j, op2, i, j, res, i, j)
    return code


def print_string(st):
    global LOCAL_CURSOR

    code = b''
    # copy value into var 
    code += new_matrix(LOCAL_MATRIX[LOCAL_CURSOR], 1, len(st), [ord(c) for c in st])
    for i in range(len(st)):
        code += index_write(LOCAL_MATRIX[LOCAL_CURSOR], 0, i)
    LOCAL_CURSOR += 1
    return code

def add_opnum(): return 4
def sub_opnum(): return 4
def print_string_opnum(st): return len(st) + 1

flag = "welcome_to_MatrixVM_world"
key = "we_will_miss_you_procfs.."

# global variable definition
input_mat = GLOBAL_MATRIX[0]
key_mat = GLOBAL_MATRIX[1]
const_mat = GLOBAL_MATRIX[2]
result_mat = GLOBAL_MATRIX[3]
compare_mat = GLOBAL_MATRIX[4]

code = b''
# init const table
code += new_matrix(CONST_TABLE, 3, 100, [x for x in range(300)])
# init trans
code += new_matrix(ADD_TRANS, 2, 1, [1, 1])
code += new_matrix(SUB_TRANS, 2, 1, [1, neg(1)])
# init var
code += new_matrix(TRANS_VAR, 1, 2, [0, 0])
code += new_matrix(RESULT_VAR, 1, 1, [0])

# read 25 charactor flag matrix
code += new_matrix(input_mat, 5, 5, [0 for i in range(25)])
for i in range(5):
    for j in range(5):
        code += index_read(input_mat, i, j)

# define key matrix
code += new_matrix(key_mat, 5, 5, [ord(x) for x in key])

# multiply with key
code += mul_matrix(result_mat, input_mat, key_mat)

# define const matrix
random.seed(13100)
code += new_matrix(const_mat, 5, 5, [random.randint(0, 0xffff) for i in range(25)])

# add const matrix
code += add_matrix(result_mat, const_mat, 5, 5, result_mat)

# now compare!
# using plain? zig-zag? snail?
# random!!

# result array
result_li = [101607,  64693, 108504,  66997,  70993, 115942, 115270,  82702,  74378,  59728, 59188, 102446,  96720,  80703,  95089, 98601,  58826,  59426, 102259,  53330,  89909, 103118, 102655,  66767,  64083]
# make map
result_li = [(result_li[i], i) for i in range(len(result_li))]
# shuffle the map
random.shuffle(result_li)

# gen result matrix
code += new_matrix(compare_mat, 1, len(result_li), [x[0] for x in result_li])

for i in range(len(result_li)):
    code += sub(result_mat, result_li[i][1] // 5, result_li[i][1] % 5, compare_mat, 0, i, result_mat, result_li[i][1] // 5, result_li[i][1] % 5)

# check if all entry is zero

for i in range(5):
    for j in range(5):
        code += index_move(RESULT_VAR, 0, 0, result_mat, i, j)
        code += branch(RESULT_VAR, (25 - 5 * i - j) * 2 + print_string_opnum("correct!"))

code += print_string("correct!")
code += branch(ADD_TRANS, print_string_opnum("wrong!") + 1)
code += print_string("wrong!")
code += index_write(CONST_TABLE, 0, 10)

with open("prob.mv", "wb") as f:
    f.write(code)




# sample test
"""
code = b''
# init const table
code += new_matrix(CONST_TABLE, 3, 100, [x for x in range(300)])
# init trans
code += new_matrix(ADD_TRANS, 2, 1, [1, 1])
code += new_matrix(SUB_TRANS, 2, 1, [1, neg(1)])
# init var
code += new_matrix(TRANS_VAR, 1, 2, [0, 0])
code += new_matrix(RESULT_VAR, 1, 1, [0])

# read 25 charactor flag
code += new_matrix(GLOBAL_MATRIX[0], 1, 1, [ord('a')])
code += new_matrix(GLOBAL_MATRIX[1], 1, 1, [5])
code += new_matrix(GLOBAL_MATRIX[2], 1, 1, [ord('0')])
code += index_read(GLOBAL_MATRIX[2], 0, 0)
code += add(GLOBAL_MATRIX[0], 0, 0, GLOBAL_MATRIX[1], 0, 0, RESULT_VAR, 0, 0)
code += index_write(GLOBAL_MATRIX[2], 0, 0)
code += index_write(CONST_TABLE, 0, 10)
code += sub(RESULT_VAR, 0, 0, GLOBAL_MATRIX[2], 0, 0, RESULT_VAR, 0, 0)

code += branch(RESULT_VAR, print_string_opnum("corrent!") + 2)
code += print_string("corrent!")
code += branch(ADD_TRANS, print_string_opnum("wrong!") + 1)
code += print_string("wrong!")
code += index_write(CONST_TABLE, 0, 10)
"""