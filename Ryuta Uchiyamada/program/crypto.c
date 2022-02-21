#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/md5.h>

#define byte 16


/*** 共用体 ***/
union IntAndFloat{
  int ival;
  float fval;
};


/*** unsigned char 2進数表示関数 ***/
void binary_print_uchar(unsigned char a) {
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


/*** float 2進数表示関数 ***/
void binary_print_float(float a) {
  int i;
  union IntAndFloat target;
  target.fval = a;

  for (i = 0; i < 32; i++) {
    if((target.ival & 0x80000000) != 0) {
      printf("1");
    }
    else {
      printf("0");
    }
    target.ival = target.ival << 1;
  }
  printf(" ");

}


/*** int 2進数表示関数 ***/
void binary_print_int(int a) {
  int i;

  for (i = 0; i < 32; i++) {
    if((a & 0x80000000) != 0) {
      printf("1");
    }
    else {
      printf("0");
    }
    a = a << 1;
  }
  printf(" ");
  
}


/*** SD,date ← γ XOR An+1 XOR An, SD → float, date → int ***/
void sd_date(unsigned char gamma[byte], unsigned char An1[byte], unsigned char An[byte], float sd[1], int date[6]){
  int i, j, sift, t;
  float sd_f;
  int data[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, _sd[4] = {0, 0, 0, 0}, _year[4] = {0, 0}, _month[1] = {0}, _day[1] = {0}, _hour[1] = {0}, _min[1] = {0}, _sec[1] = {0};
  union IntAndFloat target;

  /** SD, date ← γ XOR An+1 XOR An **/
  for(i = 0; i < byte; i++){
    data[i] = gamma[i] ^ An1[i] ^ An[i];
  }

  /** 複号の確認 **/
  // γ
  //printf("gamma:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(gamma[i]);
  //}
  //printf("\n");

  // An+1
  //printf("An+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(An1[i]);
  //}
  //printf("\n");

  // An
  //printf("An:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(An[i]);
  //}
  //printf("\n");

  // SD,date
  //printf("data:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(data[i]);
  //}
  //printf("\n");
  
  /** センシングデータと日付を振分け **/
  for(i = 0; i < 4; i++){
    _sd[i] = data[i];
  }
  for(i = 0; i < 2; i++){
    _year[i] = data[(i + 4)];
  }
  _month[0] = data[6];
  _day[0] = data[7];
  _hour[0] = data[8];
  _min[0] = data[9];
  _sec[0] = data[10];

  // 確認
  //printf("_sd:\n");
  //for(i = 0; i < 4; i++){
  //binary_print_int(_sd[i]);
  //}
  //printf("\n");
  //printf("_year:\n");
  //for(i = 0; i < 2; i++){
  //binary_print_int(_year[i]);
  //}
  //printf("\n");
  //printf("_month:\n");
  //binary_print_int(_month[0]);
  //printf("\n");
  //printf("_day:\n");
  //binary_print_int(_day[0]);
  //printf("\n");
  //printf("_hour:\n");
  //binary_print_int(_hour[0]);
  //printf("\n");
  //printf("_min:\n");
  //binary_print_int(_min[0]);
  //printf("\n");
  //printf("_sec:\n");
  //binary_print_int(_sec[0]);
  //printf("\n");

  /** SD → float **/
  target.fval = sd[0];
  sift = 0;
  for (i = 3; i >= 0; i--) {
    _sd[i] = _sd[i] << sift;
    target.ival = _sd[i] | target.ival;
    sift += 8; 
  }
  sd[0] = target.fval;

  /** date → int **/
  sift = 0;
  for (i = 1; i >= 0; i--) {
    _year[i] = _year[i] << sift;
    date[0] = _year[i] | date[0];
    sift += 8; 
  }
  date[1] = _month[0] | date[1];
  date[2] = _day[0] | date[2];
  date[3] = _hour[0] | date[3];
  date[4] = _min[0] | date[4];
  date[5] = _sec[0] | date[5];
  
  // SDと日付の確認
  //printf("SD:\n");
  //binary_print_float(sd[0]);
  //printf("\n");
  //printf("DATE:\n");
  //for(i = 0; i < 6; i++){
  //binary_print_int(date[i]);
  //printf("\n");
  //}
  
}


/*** An+2 ← H(S XOR Nn+2) ***/
void An2(unsigned char pass[byte], unsigned char An2[byte]){
  int n, i, md5;
  unsigned char rand[byte], xor[byte];
  MD5_CTX c;
  
  // 乱数Nn+2を生成
  n = RAND_bytes(rand, byte);
  if(n != 1){
    printf("error: random number.\n");
  }

  // S XOR Nn+2
  for(i = 0; i < byte; i++){
    xor[i] = pass[i] ^ rand[i];
  }
  
  // H(S XOR Nn+2)  (ハッシュ関数：MD5)
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
  md5 = MD5_Final(An2, &c);
  if(md5 != 1){
    perror("error: output hash");
    exit(1);
  }

  /** An+2の確認 **/
  // パスワード
  //printf("pass:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(pass[i]);
  //}
  //printf("\n");
  
  // 乱数
  //printf("Nn+2:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(rand[i]);
  //}
  //printf("\n");
  
  // S XOR Nn+2
  //printf("S XOR Nn+2:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(xor[i]);
  //}
  //printf("\n");

  // An+2
  //printf("An+2:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(An2[i]);
  //}
  //printf("\n");

}


/*** α ← An+2 XOR An+1 XOR Mn+1 ***/
void alpha(unsigned char An2[byte], unsigned char An1[byte], unsigned char Mn1[byte], unsigned char alpha[byte]){
  int i;
  
  for(i = 0; i < byte; i++){
    alpha[i] = An2[i] ^ An1[i] ^ Mn1[i];
  }

  /** α の確認 **/ 
  // An+2
  //printf("An+2:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(An2[i]);
  //}
  //printf("\n");

  // An+1
  //printf("An+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(An1[i]);
  //}
  //printf("\n");

  // Mn+1
  //printf("Mn+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(Mn1[i]);
  //}
  //printf("\n");
  
  //α
  //printf("alpha:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(alpha[i]);
  //}
  //printf("\n");

}


/*** Mn+2 ← An+1 + Mn+1 ***/
void Mn2(unsigned char An1[byte], unsigned char Mn1[byte], unsigned char Mn2[byte]){
  int i;
  unsigned char carry[byte];
  
  for(i = 0; i < sizeof(carry); i++){
    carry[i] = 0;
  }
  for(i = 15; i >= 0; i--){
    if((An1[i] + Mn1[i] + carry[i]) >= 256){
      Mn2[i] = (An1[i] + Mn1[i] + carry[i]) - 256;
      if(i != 0){
	carry[(i - 1)] = 1;
      }
    }
    else{
      Mn2[i] = An1[i] + Mn1[i] + carry[i];
    }
  }

  /** Mn+2 を確認 **/
  // An+1
  //printf("An+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(An1[i]);
  //}
  //printf("\n");

  // Mn+1
  //printf("Mn+1:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(Mn1[i]);
  //}
  //printf("\n");
  
  // 繰り上げ
  //printf("carry:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(carry[i]);
  //}
  //printf("\n");  

  // Mn+2
  //printf("Mn+2:\n");
  //for(i = 0; i < byte; i++){
  //binary_print_uchar(Mn2[i]);
  //}
  //printf("\n");

}



