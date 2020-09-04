#include "SignVerifier.h"
#include "aes.h"
#include <stdio.h>
#include <err.h>

static int base64_decode_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1, 0,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

/*
 * Class:     SignVerifier
 * Method:    decode
 * Signature: (Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_SignVerifier_decode (JNIEnv * env, jobject obj, jstring input, jbyteArray buf){
  jboolean copied;

  //get char array from encoded string 	
  const jchar * src = (*env)->GetStringUTFChars(env, input, 0);
  
  //get Primitive Array
  char * dst = (*env)->GetPrimitiveArrayCritical(env, buf, &copied);
  if(copied) errx(1, "Critical Array Copied Error");


  unsigned char * cursor, c;
  int d, space_idx = 0, state = 0, prev_d = 0;

  //do decode
  for(cursor = src; *cursor != '\0'; ++cursor) {
    if(*cursor != '='){
      d = base64_decode_table[*cursor];

      //invalid char
      if(d == -1) errx(1, "Invalid Character Error");

      switch(state) {
        case 0:
          ++state;
          break;
        case 1:
          c = ((prev_d << 2) | ((d & 0x30) >> 4));
          dst[space_idx++] = c;
          ++state;
          break;
        case 2:
          c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
          dst[space_idx++] = c;
          ++state;
          break;
        case 3:
          c = (((prev_d & 0x03) << 6) | d);
          dst[space_idx++] = c;
          state = 0;
          break;
      }
      prev_d = d;
    }
    else { //handle pad
        switch(state) {
          case 0:
          case 1:
            errx(1, "Invalid Padding Error"); // invalid pad
          case 2:
            if(cursor[1] != '=' || cursor[2]) errx(1, "Invalid Padding Error"); //invalid pad
            dst[space_idx++] = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
            break;
          case 3:
            if(cursor[1]) errx(1, "Invalid Padding Error"); //invalid pad
            dst[space_idx++] = (((prev_d & 0x03) << 6) | d);
            break;
      }
      return ;
    }
  }
  if(state != 0) errx(1, "Invalid format Error"); //invalid format
  return ;
}

/*
 * Class:     SignVerifier
 * Method:    verify
 * Signature: ([B[B[B)V
 */
JNIEXPORT void JNICALL Java_SignVerifier_verify (JNIEnv * env, jobject obj, jbyteArray sign, jbyteArray verify, jbyteArray key){
  
  struct AES_ctx ctx;
  jboolean copied;
	
  //get Primitive Arrays
  char * ct = (*env)->GetPrimitiveArrayCritical(env, sign, &copied);
  if(copied) errx(1, "Critical Array Copied Error");
  char * pt = (*env)->GetPrimitiveArrayCritical(env, verify, &copied);
  if(copied) errx(1, "Critical Array Copied Error");
  char * k = (*env)->GetPrimitiveArrayCritical(env, key, &copied);
  if(copied) errx(1, "Critical Array Copied Error");

  

  //init AES struct with key
  AES_init_ctx(&ctx, k);

  //copy ciphertext into plaintext buffer
  for(int i = 0; i < 16; i++)
    pt[i] = ct[i];

  //do ECB decrypt
  AES_ECB_decrypt(&ctx, pt);
  
  //release Primitive Arrays
  (*env)->ReleasePrimitiveArrayCritical(env, sign, ct, 0);
  (*env)->ReleasePrimitiveArrayCritical(env, verify, pt, 0);
  (*env)->ReleasePrimitiveArrayCritical(env, key, k, 0);
}


