#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <time.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080
#define BUF_SIZE 32

void xor(unsigned char result[],unsigned char x[],unsigned char y[],int len);
void add(unsigned char result[],unsigned char x[],unsigned char y[],int len);
void show(unsigned char s[],int len);
int transfer_first(int sock,unsigned char S[],unsigned char N1[],unsigned char M1[]);
int transfer_n(int sock,unsigned char S[],unsigned char Nn[],unsigned char Mn[],unsigned char *F,clock_t *rand_clock,clock_t *sock_clock);

int main(void) {
  int w_addr, c_sock;
  struct sockaddr_in a_addr;
  unsigned char S[BUF_SIZE] = {0};//password
  unsigned char N1[BUF_SIZE] = {0};//rondom
  unsigned char M1[BUF_SIZE] = {0};//
  unsigned char Nn[BUF_SIZE] = {0};//rondom
  unsigned char Mn[BUF_SIZE] = {0};
  unsigned char F='1';
  FILE *file;
  int i,cnt = 0;
  clock_t s_clock,e_clock,sum_clock = 0,rand_clock = 0,sock_clock = 0;

  /* ソケットを作成 */
  w_addr = socket(AF_INET, SOCK_STREAM, 0);
  if (w_addr == -1) {
    printf("socket error\n");
    return -1;
  }

  /* 構造体を全て0にセット */
  memset(&a_addr, 0, sizeof(struct sockaddr_in));

  /* サーバーのIPアドレスとポートの情報を設定 */
  a_addr.sin_family = AF_INET;
  a_addr.sin_port = htons((unsigned short)SERVER_PORT);
  a_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

  /* ソケットに情報を設定 */
  if (bind(w_addr, (const struct sockaddr *)&a_addr, sizeof(a_addr)) == -1) {
    printf("bind error\n");
    close(w_addr);
    return -1;
  }

  /* ソケットを接続待ちに設定 */
  if (listen(w_addr, 3) == -1) {
    printf("listen error\n");
    close(w_addr);
    return -1;
  }
      
  //接続要求の受け付け（接続要求くるまで待ち)
  printf("Waiting connect...\n");
  c_sock = accept(w_addr, NULL, NULL);
  if (c_sock == -1) {
    printf("accept error\n");
    close(w_addr);
    return -1;
  }
  printf("Connected!!\n");

  printf("---初回登録---\n");
  //接続済のソケットでデータのやり取り
  transfer_first(c_sock,S,N1,M1);

    
  for(i = 0;i < BUF_SIZE;i++){
    Nn[i] = N1[i];
    Mn[i] = M1[i];
  }
    
  //------------------------------------------------
  while(cnt !=1000){
    //接続要求の受け付け（接続要求くるまで待ち)
    printf("Waiting connect...\n");
    c_sock = accept(w_addr, NULL, NULL);
    if (c_sock == -1) {
      printf("accept error\n");
      close(w_addr);
      return -1;
    }
    printf("Connected!!\n");
	    	   

 
    printf("---N回目認証---\n");
            
    printf("cnt:%d\n",cnt);

    s_clock  = clock();    
    //接続済のソケットでデータのやり取り
    transfer_n(c_sock,S,Nn,Mn,&F,&rand_clock,&sock_clock);

    e_clock  = clock();
    sum_clock = sum_clock + (e_clock - s_clock);

    cnt++;
  }
	      
  //ソケット通信をクローズ
  close(c_sock);
  printf("cnt:%d, sum_clock  :%f\n",cnt,(double)(sum_clock)/CLOCKS_PER_SEC);
  printf("cnt:%d, rand_clock :%f\n",cnt,(double)(rand_clock)/CLOCKS_PER_SEC);
  printf("cnt:%d, sock_clock :%f\n",cnt,(double)(sock_clock)/CLOCKS_PER_SEC);
  printf("cnt:%d, sum-ran-soc:%f\n",cnt,(double)(sum_clock - rand_clock - sock_clock)/CLOCKS_PER_SEC);

  /* 接続待ちソケットをクローズ */
  close(w_addr);

  return 0;
}

void xor(unsigned char result[],unsigned char x[],unsigned char y[],int len){
  int i;
  for(i=0;i< len;i++){
    result[i] = x[i] ^ y[i];
  }  
}

void add(unsigned char result[],unsigned char x[],unsigned char y[],int len){
  int i;
  for(i=0;i< len;i++){
    result[i] = x[i] + y[i];
  }  
}

void show(unsigned char s[],int len){
  int i;
  for(i=0;i< len;i++){
    printf("%02X",s[i]);
  }
  printf("\n");
}

int transfer_first(int sock,unsigned char S[],unsigned char N1[],unsigned char M1[]) {
  unsigned char send_buf[BUF_SIZE*2]={0};
  char recv_buf          ;
  int send_size, recv_size;  
  int i;

  SHA256_CTX sha_ctx;//コンテキストを生成

  unsigned char xor_result[BUF_SIZE] = {0};//result_xor
  unsigned char A1[BUF_SIZE] ={0};//hash
  FILE *fp;

  
  //printf("パスワードを入力してください please password\n");
  //scanf("%s",S);

  //password  data -> S
  RAND_bytes(S,BUF_SIZE);
  //ランダムデータ random data -> N1
  RAND_bytes(N1,BUF_SIZE);
  //make M1
  RAND_bytes(M1,BUF_SIZE);

  //xor
  xor(xor_result,S,N1,sizeof(xor_result));

  //hash
  SHA256_Init(&sha_ctx);
  SHA256_Update(&sha_ctx,xor_result,sizeof(xor_result));
  SHA256_Final(A1,&sha_ctx);

  //print
  //printf("S     = ");
  //show(S,sizeof(S));
  //printf("N1    = ");
  //show(N1,sizeof(N1));
  //printf("A1    = ");
  //show(A1,sizeof(A1));
  //printf("M1    = ");
  //show(M1,sizeof(M1));

  //---------connect A1 M1----------
  for(i=0;i< BUF_SIZE;i++){
    send_buf[i] = A1[i];
  }
  int k = 0;
  for(i=BUF_SIZE;i< BUF_SIZE*2;i++){
    send_buf[i] = M1[k];
    k++;
  }
  //---------------------------------------
 
  /* 文字列を送信 */
  send_size = send(sock, send_buf, sizeof(send_buf) + 1, 0);
  if (send_size == -1) {
    printf("send error\n");
    return 0;
  }
  /* サーバーからの応答を受信 */
  recv_size = recv(sock, &recv_buf, 1, 0);
  if (recv_size == -1) {
    printf("recv error\n");
    return 0;
  }
  if (recv_size == 0) {
    /* 受信サイズが0の場合は相手が接続閉じていると判断 */
    printf("connection ended\n");
    return 0;
  }
  /* 応答が0の場合はデータ送信終了 */
  if (recv_buf == 0) {
    printf("接続を終了しました\n");
    return 0;
  }
  
  return 0;
}


int transfer_n(int sock,unsigned char S[],unsigned char Nn[],unsigned char Mn[],unsigned char *F,clock_t *rand_clock,clock_t *sock_clock) {
  unsigned char send_buf[BUF_SIZE+1]={0};
  unsigned char recv_buf[BUF_SIZE]={0};
  int send_size, recv_size;  
  int i;

  SHA256_CTX sha_ctx;//コンテキストを生成
  unsigned char Nn_next[BUF_SIZE] = {0};//N n+1
  unsigned char xor_result[BUF_SIZE] = {0};//result_xor
  unsigned char An[BUF_SIZE] ={0};
  unsigned char An_next[BUF_SIZE] ={0};
  unsigned char alpha[BUF_SIZE] ={0};
  unsigned char beta[BUF_SIZE] ={0};
  unsigned char Mn_next[BUF_SIZE] = {0};
  clock_t s_time,e_time;
  FILE *fp,*fp2;
  
  //printf("パスワードを入力してください please password\n");
  //scanf("%s",S);

  //printf("S=");
  //show(S,BUF_SIZE);
  //printf("Nn=");
  //show(Nn,BUF_SIZE);
  //printf("Mn=");
  //show(Mn,BUF_SIZE);
  
  
  /*----------make An----------*/
  //xor
  xor(xor_result,S,Nn,sizeof(xor_result));

  //hash
  SHA256_Init(&sha_ctx);
  SHA256_Update(&sha_ctx,xor_result,sizeof(xor_result));
  SHA256_Final(An,&sha_ctx);
 
  //----------generate Nn next----------
  s_time = clock();
  
  RAND_bytes(Nn_next,sizeof(Nn_next));

  e_time = clock();
  *rand_clock += (e_time - s_time);
  
  //----------make An_next----------
  //xor
  xor(xor_result,S,Nn_next,sizeof(xor_result));

  //hash
  SHA256_Init(&sha_ctx);
  SHA256_Update(&sha_ctx,xor_result,sizeof(xor_result));
  SHA256_Final(An_next,&sha_ctx);
  
  //----------make alpha----------
  xor(alpha,An_next,An,BUF_SIZE);
  xor(alpha,alpha,Mn,BUF_SIZE);
  
  //----------send_buf <- alpha----------
  for(i=0;i< BUF_SIZE;i++){
    send_buf[i] = alpha[i];
  }
  //printf("送る前F=%c\n",*F);
  send_buf[BUF_SIZE] = *F;
  *F = '0';
  
  /*
  printf("An      = ");
  show(An,BUF_SIZE);
  printf("Mn      = ");
  show(Mn,BUF_SIZE);
  printf("Nn_next = ");
  show(Nn_next,sizeof(Nn_next));  
  printf("An_next = ");
  show(An_next,BUF_SIZE);
  printf("alpha   = ");
  show(alpha,BUF_SIZE);
  printf("send    = ");
  show(send_buf,BUF_SIZE+1);
  //----------make beta----------
  */
  add(beta,An_next,An,BUF_SIZE);
  /*
  printf("An+1 +An= ");
  show(beta,BUF_SIZE);
  */

    //----------send alpha  ----------
  s_time = clock();
  
  send_size = send(sock, send_buf , sizeof(send_buf) + 1, 0);
  if (send_size == -1) {
    printf("send error\n");
    return 0;
  }
  // clientからの応答を受信
  recv_size = recv(sock, recv_buf, BUF_SIZE, 0);
  if (recv_size == -1) {
    printf("recv error\n");
    return 0;
  }
  
  if (recv_size == 0) {
    //受信サイズが0の場合は相手が接続閉じていると判断 
    printf("connection ended\n");
    return 0;
  }
  //応答が0の場合はデータ送信終了
  if (recv_buf == 0) {
    printf("接続を終了しました\n");
    return 0;
  }

  e_time = clock();
  *sock_clock += (e_time - s_time);
  
  //------------------------------------------------

  int flag = 0;
  //xor
  for(i=0;i< BUF_SIZE;i++){
    if((recv_buf[i] ^ beta[i])!=0){
      flag= 1;
      break;
    }
    else{
      flag = 0;
    }
  }
  
  if(flag == 0){
    printf("認証成功\n");
    
    //----------make Mn_next----------
    //add
    add(Mn_next,An,Mn,sizeof(Mn_next));
    
    for(i = 0;i < BUF_SIZE;i++){
      Nn[i] = Nn_next[i];
      Mn[i] = Mn_next[i];
    }
    *F = '1';
    
  }
  else{
    printf("認証失敗\n");
  }
  
  return 0;
}
