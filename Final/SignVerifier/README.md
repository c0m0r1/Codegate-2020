# SignVerifier - stage 1

## Description

Super Fast SignVerifier with JNI & CDS  
leak the signature verify key  
flag format is `CODEGATE2020{[leaked signature verify key]}`  

# SignVerifier - stage 2

## Description

Super Fast SignVerifier with JNI & CDS  
read the flag file  
flag is located in `./flag`  

## Comments for maintainer

You should distribute files in `dist` folder.
Two challenge share same daemon.

The helper script for exploit is `exploit_writeup/CDS_parser.py`

The exploit of SignVerifier - stage 1 is `exploit_writeup/stage1.py`.
FLAG of stage 1: `CODEGATE2020{c0rrupt_jvm_h34p}`

The exploit of SignVerifier - stage 2 is `exploit_writeup/stage2.py`.
FLAG of stage 2: `CODEGATE2020{cl4ss_c0nfusi0n_by_0v3rwr1ting_k1ass_ptr}`

Brute force need to be strongly prohibited!!  
It makes those challenge into a piece of cake... :(  
