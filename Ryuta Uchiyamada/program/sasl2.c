#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/md5.h>

#define byte 16

/*** 2進数表示関数 ***/
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


/*** An+1 ← H(S XOR Nn+1) ***/
void An1(unsigned char pass[byte], unsigned char An1[byte]){
  int n, i, md5;
  unsigned char rand[byte], xor[byte];
  MD5_CTX c;
  
  // 乱数Nn+1を生成
  n = RAND_bytes(rand, byte);
  if(n != 1){
    printf("error: random number.\n");
  }

  // S XOR Nn+1
  for(i = 0; i < byte; i++){
    xor[i] = pass[i] ^ rand[i];
  }

  // H(S XOR Nn+1)  (ハッシュ関数：MD5)
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
  md5 = MD5_Final(An1, &c);
  if(md5 != 1){
    perror("error: output hash");
    exit(1);
  }

  /** An+1の確認 **/
  // パスワード
  //printf("pass:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(pass[i]);
  //}
  //printf("\n");
  
  // 乱数
  //printf("Nn+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(rand[i]);
  //}
  //printf("\n");
  
  // S XOR Nn+1
  //printf("S XOR Nn+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(xor[i]);
  //}
  //printf("\n");

  // An+1
  //printf("An+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(An1[i]);
  //}
  //printf("\n");
  
}


/*** α ← An+1 XOR An XOR Mn ***/
void alpha(unsigned char An1[byte], unsigned char An[byte], unsigned char Mn[byte], unsigned char alpha[byte]){
  int i;
  
  for(i = 0; i < byte; i++){
    alpha[i] = An1[i] ^ An[i] ^ Mn[i];
  }

  /** αの確認 **/
  // An+1
  //printf("An+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(An1[i]);
  //}
  //printf("\n");

  // An
  //printf("An:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(An[i]);
  //}
  //printf("\n");

  // Mn
  //printf("Mn:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(Mn[i]);
  //}
  //printf("\n"); 

  // α
  //printf("alpha:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(alpha[i]);
  //}
  //printf("\n");
  
}


/*** An+1 + An = β ? ***/
int judge(unsigned char An1[byte], unsigned char An[byte], unsigned char beta[byte]){
  int i, result;
  unsigned char carry[byte], judge[byte];
  
  // An+1 + An
  for(i = 0; i < byte; i++){
    carry[i] = 0;
  }
  for(i = 15; i >= 0; i--){
    if((An1[i] + An[i] + carry[i]) >= 256){
      judge[i] = (An1[i] + An[i] + carry[i]) - 256;
      if(i != 0){
	carry[(i - 1)] = 1;
      }
    }
    else{
      judge[i] = An1[i] + An[i] + carry[i];
    }
  }

  // An+1 + An = β ?
  result = 1;
  for(i = 0; i < byte; i++){
    if(judge[i] != beta[i]){
      result = 0;
      break;
    }
  }

  /** An+1 + An の確認 **/
  // An
  //printf("An+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(An1[i]);
  //}
  //printf("\n");

  // An
  //printf("An:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(An[i]);
  //}
  //printf("\n");

  // 繰り上げ
  //printf("carry:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(carry[i]);
  //}
  //printf("\n");

  // An+1 + An
  //printf("An+1 + An:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(judge[i]);
  //}
  //printf("\n");

  // β
  //printf("beta:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(beta[i]);
  //}
  //printf("\n");

  return result;
  
}


/*** Mn+1 ← An + Mn ***/
void Mn1(unsigned char An[byte], unsigned char Mn[byte], unsigned char Mn1[byte]){
  int i;
  unsigned char carry[byte];

  for(i = 0; i < byte; i++){
    carry[i] = 0;
  }
  for(i = 15; i >= 0; i--){
    if((An[i] + Mn[i] + carry[i]) >= 256){
      Mn1[i] = (An[i] + Mn[i] + carry[i]) - 256;
      if(i != 0){
	carry[(i - 1)] = 1;
      }
    }
    else{
      Mn1[i] = An[i] + Mn[i] + carry[i];
    }
  }

  /** Mn+1 を確認 **/
  // An
  //printf("An:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(An[i]);
  //}
  //printf("\n");

  // Mn
  //printf("Mn:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(Mn[i]);
  //}
  //printf("\n");
  
  // 繰り上げ
  //printf("carry:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(carry[i]);
  //}
  //printf("\n");  
  
  // Mn+1
  //printf("Mn+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print(Mn1[i]);
  //}
  //printf("\n");
  
}
