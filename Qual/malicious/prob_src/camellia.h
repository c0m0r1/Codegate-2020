/*
 * Copyright 2006-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/* ====================================================================
 * Copyright 2006 NTT (Nippon Telegraph and Telephone Corporation) .
 * ALL RIGHTS RESERVED.
 *
 * Intellectual Property information for Camellia:
 *     http://info.isl.ntt.co.jp/crypt/eng/info/chiteki.html
 *
 * News Release for Announcement of Camellia open source:
 *     http://www.ntt.co.jp/news/news06e/0604/060413a.html
 *
 * The Camellia Code included herein is developed by
 * NTT (Nippon Telegraph and Telephone Corporation), and is contributed
 * to the OpenSSL project.
 */
 
# define CAMELLIA_BLOCK_SIZE 16
# define CAMELLIA_TABLE_BYTE_LEN 272
# define CAMELLIA_TABLE_WORD_LEN (CAMELLIA_TABLE_BYTE_LEN / 4)

# ifdef __cplusplus
extern "C" {
# endif

typedef unsigned int KEY_TABLE_TYPE[CAMELLIA_TABLE_WORD_LEN]; /* to match
                                                               * with WORD */
typedef unsigned int u32;
typedef unsigned char u8;

struct camellia_key_st {
    union {
        double d;               /* ensures 64-bit align */
        KEY_TABLE_TYPE rd_key;
    } u;
    int grand_rounds;
};
typedef struct camellia_key_st CAMELLIA_KEY;

int Camellia_set_key(const unsigned char *userKey, const int bits,
                     CAMELLIA_KEY *key);

void Camellia_encrypt(const unsigned char *in, unsigned char *out,
                      const CAMELLIA_KEY *key);
void Camellia_decrypt(const unsigned char *in, unsigned char *out,
                      const CAMELLIA_KEY *key);

int Camellia_Ekeygen(int keyBitLength, const u8 *rawKey,
                     KEY_TABLE_TYPE keyTable);
void Camellia_EncryptBlock_Rounds(int grandRounds, const u8 plaintext[],
                                  const KEY_TABLE_TYPE keyTable,
                                  u8 ciphertext[]);
void Camellia_DecryptBlock_Rounds(int grandRounds, const u8 ciphertext[],
                                  const KEY_TABLE_TYPE keyTable,
                                  u8 plaintext[]);
void Camellia_EncryptBlock(int keyBitLength, const u8 plaintext[],
                           const KEY_TABLE_TYPE keyTable, u8 ciphertext[]);
void Camellia_DecryptBlock(int keyBitLength, const u8 ciphertext[],
                          const KEY_TABLE_TYPE keyTable, u8 plaintext[]);
                          
                          
# ifdef __cplusplus
}
# endif
