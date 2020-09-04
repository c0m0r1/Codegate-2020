#!/usr/bin/python
import numpy as np
import random

flag = "welcome_to_MatrixVM_world"
assert(len(flag) == 25)

key = "we_will_miss_you_procfs.."

flag_li = []
key_li = []
for i in range(5):
    flag_li.append([])
    key_li.append([])
    for j in range(5):
        flag_li[i].append(ord(flag[i * 5 + j]))
        key_li[i].append(ord(key[i * 5 + j]))

flag_mat = np.array(flag_li) 
key_mat = np.array(key_li)

enc_mat = np.matmul(flag_mat, key_mat)
print("key * flag")
print(enc_mat)

random.seed(13100)

const_li = []
for i in range(5):
    const_li.append([])
    for j in range(5):
        const_li[i].append(random.randint(0, 0xffff))

const_mat = np.array(const_li)
enc_mat = np.add(enc_mat, const_mat)
print("flag matrix")
print(flag_mat)
print("key matrix")
print(key_mat)
print("constant matrix")
print(const_mat)
print("encrypted flag matrix")
print(enc_mat)
