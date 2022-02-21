#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/md5.h>

#define byte 16

/** 2進数表示関数 **/
void binary_print(unsigned char a) {
  int i;

  for (i = 0; i < 8; i++) {
    if((a & 0b10000000) != 0) {
      printf("1");
    }
    else {
      printf("0");
    }
    a = a << 1;
  }
  printf(" ");

}


int main(){
  int i, j, cut, n, md5;
  unsigned char pass[16] = {201, 140, 96, 184, 12, 154, 6, 166, 234, 126, 65, 230, 28, 54, 106, 26},
    A1[16], M1[16], rand[16], xor[16];
  MD5_CTX c;
  
  /*** A1 ← H(S XOR N1) ***/ 
  // 乱数N1を生成
  n = RAND_bytes(rand, byte);
  if(n != 1){
    printf("error: random number.\n");
  }

  // S XOR N1
  for(j = 0; j < byte; j++){
    xor[j] = pass[j] ^ rand[j];
  }

  // H(S XOR N1)  (ハッシュ関数：MD5)
  md5 = MD5_Init(&c);
  if(md5 != 1){
    perror("error: md5 context");
    exit(1);
  }   
  md5 = MD5_Update(&c, xor, byte);
  if(md5 != 1){
    perror("error: hash");
    exit(1);
  } 
  md5 = MD5_Final(A1, &c);
  if(md5 != 1){
    perror("error: output hash");
    exit(1);
  }

  /** A1の確認 **/
  // パスワード
  //printf("pass:\n");
  //for(j = 0; j < byte; j++){
  //binary_print(pass[j]);
  //}
  //printf("\n");
  
  // 乱数
  //printf("Nn+1:\n");
  //for(j = 0; j < byte; j++){
  //binary_print(rand[j]);
  //}
  //printf("\n");
  
  // S XOR N1
  //printf("S XOR Nn+1:\n");
  //for(j = 0; j < byte; j++){
  //binary_print(xor[j]);
  //}
  //printf("\n");

  // A1
  //printf("A1:\n");
  //for(j = 0; j < byte; j++){
  //binary_print(A1[j]);
  //}
  //printf("\n");

    
  /***M1(乱数) ***/
  // M1を生成(乱数)
  n = RAND_bytes(M1, byte);
  if(n != 1){
    printf("error: random number(M1).\n");
  }
  
  /** M1の確認 **/
  // password
  //printf("pass:\n");
  //for(j = 0; j < byte; j++){
  //printf("%03u ", pass[j]);
  //}
  //printf("\n");
    
  // A1
  //printf("A1:\n");
  //for(j = 0; j < byte; j++){
  //printf("%03u ", A1[j]);
  //}
  //printf("\n");
    
  // M1
  //printf("M1:\n");
  //for(j = 0; j < byte; j++){
  //printf("%03u ", M1[j]);
  //}
  //printf("\n");
    

  return 0;

}
