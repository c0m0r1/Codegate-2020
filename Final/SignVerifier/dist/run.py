#!/usr/bin/python3

import hashlib
import random
import signal
import sys
import os

HASH_DIFFICULTY = 22
NONCE_LEN = 16
CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

def _print(content):
    print(content)
    sys.stdout.flush()

if __name__ == "__main__":
    signal.alarm(60)
    try:
        nonce = "".join(random.sample(CHARSET, NONCE_LEN))
        _print("Give me an alphanumeric string S such that md5( S || %s ) has %d leading zero bits."%(nonce, HASH_DIFFICULTY))
        s = sys.stdin.readline().strip()
        if s.isalnum() == False:
            exit(1)

        h = hashlib.md5()
        h.update((s + nonce).encode())
        md = h.digest()

        for i in range(HASH_DIFFICULTY):
            byte_idx = i // 8
            bit_idx = 7 - i % 8

            if md[byte_idx] & (1 << bit_idx):
                exit(1)
        _print("Good Work! Now I'll run SignVerifier for you...")
        os.system("./run.sh")
    except Exception as inst:
        _print(inst)
        _print("error!")
