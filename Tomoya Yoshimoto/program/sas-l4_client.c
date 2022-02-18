#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
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
int transfer_first(int sock,unsigned char A1[],unsigned char M1[]);
int transfer_n(int sock,unsigned char An[],unsigned char Mn[],clock_t *sock_clock);


int main(void) {
  int sock;
  struct sockaddr_in addr;
  FILE *file;
  int i,cnt=0;
  unsigned char A1[BUF_SIZE] = {0};
  unsigned char M1[BUF_SIZE] = {0};
  unsigned char An[BUF_SIZE] = {0};
  unsigned char Mn[BUF_SIZE] = {0};
  clock_t s_clock,e_clock,sum_clock = 0,sock_clock = 0;
  
  //---------------------------------------------
  /* ソケットを作成*/
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    printf("socket error\n");
    return -1;
  }
  
  /* 構造体を全て0にセット */
  memset(&addr, 0, sizeof(struct sockaddr_in));
  
  /* サーバーのIPアドレスとポートの情報を設定*/
  addr.sin_family = AF_INET;
  addr.sin_port = htons((unsigned short)SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
  
  /* サーバーに接続要求送信 */
  printf("接続を開始します...\n");
  if (connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
    printf("connect error\n");
    close(sock);
    return -1;
  }
  printf("接続が完了しました!\n");
  //---------------------------------------------
  printf("---初回登録---\n");
  /* 初回登録時の接続済のソケットでデータのやり取り */

  transfer_first(sock,A1,M1);

  for(i = 0;i < BUF_SIZE;i++){
    An[i] = A1[i];
    Mn[i] = M1[i];
  }

  /* ソケット通信をクローズ */
  close(sock);
  
  printf("---N回目認証---\n");

  while(cnt !=1000){
    
    //ソケットを作成
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
      printf("socket error\n");
      return -1;
    }
    
    //構造体を全て0にセット 
    memset(&addr, 0, sizeof(struct sockaddr_in));
    
    //サーバーのIPアドレスとポートの情報を設定
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    
    /* サーバーに接続要求送信 */
    printf("接続を開始します...\n");
    if (connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
      printf("connect error\n");
      close(sock);
      return -1;
    }
    printf("接続が完了しました!\n");

    printf("cnt:%d\n",cnt);

    //---------------------------------------------
    s_clock = clock();
    
    /* N回目認証時の接続済のソケットでデータのやり取り */
    transfer_n(sock,An,Mn,&sock_clock);
    
    e_clock = clock();

    sum_clock = sum_clock + (e_clock - s_clock);

    
    /* ソケット通信をクローズ */
    close(sock);
    cnt++;
  }
   
  printf("cnt:%d, sum_clock :%f\n",cnt,(double)(sum_clock)/CLOCKS_PER_SEC);
  printf("cnt:%d, sock_clock:%f\n",cnt,(double)(sock_clock)/CLOCKS_PER_SEC);
  printf("cnt:%d, sock-sum  :%f\n",cnt,(double)(sum_clock - sock_clock)/CLOCKS_PER_SEC);
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

int transfer_first(int sock,unsigned char A1[],unsigned char M1[]) {
  int recv_size, send_size,i;
  unsigned char recv_buf[BUF_SIZE*2];
  char send_buf;
 
  FILE *fp;
 
  /* クライアントから文字列を受信 */
  recv_size = recv(sock, recv_buf, BUF_SIZE*2, 0);
  if (recv_size == -1) {
    printf("recv error\n");
    return 0;
  }
  if (recv_size == 0) {
    /* 受信サイズが0の場合は相手が接続閉じていると判断 */
    printf("connection ended\n");
    return 0;
  }
    
  //A1
  for(i = 0;i < BUF_SIZE;i++){
    A1[i] = recv_buf[i];
  }
  
  //M1
  int k = 0;
  for(i = BUF_SIZE;i < BUF_SIZE*2;i++){
    M1[k] = recv_buf[i];
    k++;
  }

  //-------------------------------------------
  /* 接続終了を表す0を送信 */
  send_buf = 0;
  send_size = send(sock, &send_buf, 1, 0);
  if (send_size == -1) {
    printf("send error\n");
    return 0;
  }

  printf("接続を終了しました\n");
  return 0;
    
}

int transfer_n(int sock,unsigned char An[],unsigned char Mn[],clock_t *sock_clock) {
  int recv_size, send_size,i;
  unsigned char recv_buf[BUF_SIZE]={0};
  unsigned char send_buf[BUF_SIZE]={0};
  unsigned char An_next[BUF_SIZE] ={0};
  unsigned char Mn_next[BUF_SIZE] ={0};
  unsigned char alpha[BUF_SIZE] ={0};
  unsigned char beta[BUF_SIZE] ={0};
  clock_t s_time,e_time;
  FILE *fp;
  
  //----------receve alpha----------
  s_time = clock();
  
  // serverから文字列を受信
  recv_size = recv(sock, recv_buf, BUF_SIZE, 0);
  if (recv_size == -1) {
    printf("recv error\n");
    return 0;
  }
  if (recv_size == 0) {
    // 受信サイズが0の場合は相手が接続閉じていると判断
    printf("connection ended\n");
    return 0;
  }

  e_time = clock();
  *sock_clock += (e_time - s_time);
  
  //alpha
  for(i = 0;i < BUF_SIZE;i++){
    alpha[i] = recv_buf[i];
  }
  
  //----------make beta---------- 
  xor(beta,alpha,An,BUF_SIZE);
  xor(An_next,beta,Mn,BUF_SIZE);//make An_next
  add(beta,An_next,An,BUF_SIZE);

  /*
  //print
  printf("An      = ");
  show(An,BUF_SIZE);
  printf("An+1    = ");
  show(An_next,BUF_SIZE);
  printf("alpha   = ");
  show(alpha,BUF_SIZE);
  printf("beta?   = ");
  show(beta,BUF_SIZE);
  */


  //----------send beta---------
  s_time = clock();
  
  for(i = 0;i < BUF_SIZE;i++){
    send_buf[i] = beta[i];
  }
  send_size = send(sock, send_buf, BUF_SIZE, 0);
  if (send_size == -1) {
    printf("send error\n");
    return -1;
  }

  e_time = clock();
  *sock_clock += (e_time - s_time);
  
  //----------make Mn_next----------
  add(Mn_next,An,Mn,sizeof(Mn_next));

  for(i = 0;i < BUF_SIZE;i++){
    An[i] = An_next[i];
    Mn[i] = Mn_next[i];
  }

  printf("接続を終了しました\n");
  return 0;
 
}
