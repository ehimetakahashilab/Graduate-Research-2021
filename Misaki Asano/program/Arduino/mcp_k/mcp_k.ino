#include <TimeLib.h>
#include <EEPROM.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include "ESP8266.h"

///------------------------
////定義
///------------------------
#define SSID "Elecom2g-Takahashi"
#define PASSWORD "tkhs59957"
#define R_HOST_NAME   "192.168.2.110"
#define R_HOST_PORT   (49152)
#define N_HOST_NAME "ntp.nict.jp"
#define N_HOST_PORT   (123)
#define DATALEN 16  //128bit=16byte
#define NTP_PACKET_SIZE   (48)

uint8_t buffer[128] = {0};
uint8_t An[DATALEN]={0};  //An:認証情報
uint8_t Am[DATALEN]={0};  //An+1:認証情報
uint8_t Al[DATALEN]={0};  //An+2:認証情報
uint8_t Mn[DATALEN]={0};  //Mn:秘匿情報
uint8_t Mm[DATALEN]={0};  //Mn+1:秘匿情報
uint8_t Ml[DATALEN]={0};  //Mn+2:秘匿情報
uint8_t beta[DATALEN]={0}; //ベータ
uint8_t carry[DATALEN]={0}; //桁上がり
uint8_t request[DATALEN] = {0}; //認証要請用配列
uint8_t format[DATALEN]={0}; //データフォーマット
const int timeZone = 9; //JST = GMT+9
uint8_t packetBuffer[NTP_PACKET_SIZE] = {0}; //ntprequestpackage

/*共用体*/
union IntAndFloat{
  int32_t sd_int;
  float sd_float; 
  };
  
//HardwareSerial
ESP8266 wifi(Serial1);
// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
time_t getNtpTime();

void setup() {
  // put your setup code here, to run once:
   /*シリアル通信速度設定:9600*/
  Serial.begin(9600);
  while (!Serial) {
 // wait for serial port to connect. Needed for native USB port only
  }
  
 /* Initialise I2C communication as MASTER*/
  Wire.begin();
  
  Serial.print("setup begin\r\n");
  /*エラーライト*/
  pinMode(5,OUTPUT);

  /*esp8266との接続確認*/
   if(wifi.kick()){
     Serial.println("ESP8266 OK");
     }else{
       Serial.print("ESP8266 ERROR");
     error_reboot();
     }
///------------------------
////センサ初期設定
///------------------------
 if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    while (1);
  }
  Serial.println("Found MCP9808!");

  tempsensor.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 
///------------------------
//wifi接続
///------------------------ 
  /*wifiのモード設定:station,softAP,station+softAPから選択*/
  if (wifi.setOprToStation()) {
    } else {
    error_reboot();
  }
  
  /*アクセスポイントに接続*/
  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.println("connect success");
  } else {
    Serial.println("connect error");
    error_reboot();
  }
//------------------------------------------------------------------
///時刻同期 setSyncInterval:同期の間隔
//-------------------------------------------------------------------
   setSyncProvider(getNtpTime);
   setSyncInterval(100000000);
}

void loop() {
  // put your main code here, to run repeatedly:
///------------------------
//定義
///------------------------
uint8_t ganma[DATALEN]={}; //センシングデータ排他的論理和
uint8_t sd[16] = {}; //センシングデータ
uint8_t request[DATALEN] = {0};
int32_t n = 0; //暗号化回数
uint32_t len = 0;
union IntAndFloat target;

/*毎分0秒からスタート*/
while(! second() == 0){
  }
///------------------------
//TCP接続
///------------------------
  
/*//コネクションモードの設定(シングルモード)*/
 if (wifi.disableMUX()) {
  } else {
    error_reboot();
 }

 /*TCPコネクション開始*/
  if (wifi.createTCP(R_HOST_NAME, R_HOST_PORT)) {
       Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
        error_reboot();
    }  
///------------------------
//認証
///------------------------ 
  request[0] = 2;
  if(wifi.send(request, DATALEN)){
    /*認証要請一回目*/
    Serial.print("request message ok\r\n");
      /*α受信待機200秒*/
      uint8_t len = wifi.recv(buffer, sizeof(buffer), 200000);
      if (len > 0){
        Serial.println("Received α ok");
      }else{
        //Serial.println("Received α error");
        /*α受信出来なければ再起動*/
        error_reboot();
    }
   }else{
      /*認証要請送信失敗なら、再度認証要請送信*/
      Serial.print("request message err\r\n");
      release_TCP();
      create_TCP();
       if(wifi.send(request, DATALEN)){
         Serial.print("request message ok\r\n");
         len = wifi.recv(buffer, sizeof(buffer), 200000);
         if (len > 0){
           Serial.println("Received α ok");
         }else{
           Serial.println("Received α error");
           error_reboot();
         }
       }else{
       Serial.print("request message err\r\n");
          error_reboot();
      }
    }

   Serial.println("alpha:");
   for(uint32_t i = 0; i < DATALEN; i++){
    Serial.print(buffer[i]);
    Serial.print("\t");
   }
   Serial.print("\r\n"); 
 
 /*α xor An xor Mn*/
 /*AnとMnをROMからget*/
  EEPROM.get(16,Am);  //0-15:An 16-31:Am 32-47:Mn 48-63:Mm
  EEPROM.get(32,Mm);
  
    for(uint32_t i = 0; i < DATALEN; i++){
    Al[i] = buffer[i]^Am[i]^Mm[i];
   }

  for(uint32_t i = 0; i < DATALEN; i++){
   carry[i]= 0;
  }
   /*βの計算*/
  for(int32_t i = 15; i >= 0; i--){
    if((Al[i] + Am[i] + carry[i])>=256){
    beta[i] = (Al[i] + Am[i] + carry[i]) - 256;
    if(i!=0){
    carry[(i-1)]=1;
    }
   }else{
     beta[i] = Al[i] + Am[i] + carry[i];
   }
  }
  
  Serial.println("beta:");
  for(uint32_t i = 0; i < DATALEN; i++){
   Serial.print(beta[i]);
   Serial.print("\t");
  }
   Serial.print("\r\n"); 
  
    /*bata送信*/
    if(wifi.send(beta, DATALEN)){
      Serial.print("beta send ok\r\n");
     }else{
      Serial.print("bera send err\r\n");
      error_reboot();
      }

  buffer[0] = 0;   
  len = wifi.recv(buffer, sizeof(buffer), 10000);
       if (len > 0){
        Serial.println("Received:Authentication ok");
       }else{
        Serial.println("Received:Authentication error");
        error_reboot(); 
          }


/*もし受信メッセージが0だったらTCP切断後、再記号するように警告(赤ライト点滅)"*/
if(buffer[0] == 1){
  Serial.print("Authentication  is successful.\r\n");
 /*認証情報と秘匿情報を更新*/
  /*Mn+1を作成Mn+1=An+Mn*/
  for(uint32_t i = 0; i < DATALEN; i++){
    carry[i]= 0;
  }

   for(int32_t i = 15; i >= 0; i--){
    if((Am[i] + Mm[i] + carry[i])>=256){
    Ml[i] = (Am[i] + Mm[i] + carry[i]) - 256;
    if(i!=0){
    carry[(i-1)]=1;
    }
    }else{
    Ml[i] = Am[i] + Mm[i] + carry[i];
    }
  }
  
  EEPROM.put(0,Am);  //0-15:An 16-31:Am 32-47:Mn
  EEPROM.put(16,Al);
  EEPROM.put(32,Ml);
  
  }else if(buffer[0] == 0){
    release_TCP();
    Serial.print("Authentication is unsuccessful.");
    error_reboot();
  }else{
    Serial.print("system error.");
    error_reboot();
   }

  /*An-1とAnとMnをget　暗号化で使用*/
  EEPROM.get(0,An);  //0-15:An 16-31:Am 32-47:Mn 48-63:Mm
  EEPROM.get(16,Am);
  EEPROM.get(32,Mm);

  Serial.println("An+1:");
  for(uint32_t i = 0; i < DATALEN; i++){
   Serial.print(Am[i]);
   Serial.print("\t");
  }
  Serial.print("\r\n"); 
  
  Serial.println("Mn+1:");
    for(uint32_t i = 0; i < DATALEN; i++){
  Serial.print(Mm[i]);
  Serial.print("\t");
  }
  Serial.print("\r\n"); 
  
  while(n < 10){
///------------------------
//センサ値取得
///------------------------ 
int int_year = 0;
int8_t int_month = 0;
int8_t int_day = 0;
int8_t int_hour = 0;
int8_t int_minute = 0;
int8_t int_second = 0;
 
 // put your main code here, to run repeatedly:
  //Serial.println("wake up MCP9808.... "); // wake up MCP9808 - power consumption ~200 mikro Ampere
  tempsensor.wake();   // wake up, ready to read!

  // Read and print out the temperature, also shows the resolution mode used for reading.
  //Serial.print("Resolution in mode: ");
  //Serial.println (tempsensor.getResolution());
  float c = tempsensor.readTempC();
   int_year = year();
   int_month = (int8_t)month();
   int_day = (int8_t)day();
   int_hour = (int8_t)hour();
   int_minute = (int8_t)minute();
   int_second = (int8_t)second();
   
  Serial.println("===================");
  Serial.print("Temp: "); 
  Serial.print(c, 4); Serial.println("*C"); 
  
  Serial.print(int_year);
  Serial.print("/");
  Serial.print(int_month);
  Serial.print("/");
  Serial.print(int_day);
  Serial.print("/");
  Serial.print(int_hour);
  Serial.print("/");
  Serial.print(int_minute);
  Serial.print("/");
  Serial.println(int_second);
  
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling

 ///------------------------
//通信フォーマット
///------------------------ 
    for(uint32_t i = 0; i < DATALEN; i++){
    format[i] = 0;
   }
 target.sd_float = c;
/*uint8_tのデータ型に格納*/
/*センシングデータ*/
 sd_to_uint(target.sd_int);
/*日時*/
 year_to_uint(int_year,4);
 date_time_to_uint(int_month,6);
 date_time_to_uint(int_day,7);
 date_time_to_uint(int_hour,8);
 date_time_to_uint(int_minute,9);
 date_time_to_uint(int_second,10);
 format[11] = 0;
 format[12] = 1;
 format[13] = 0;
 format[14] = 1;
 format[15] = 0;

///------------------------
//暗号化
///------------------------
  for(uint32_t i = 0; i < DATALEN; i++){
    ganma[i] = Am[i]^An[i]^format[i];
   }

  Serial.println("ganma:");
   for(uint32_t i = 0; i < DATALEN; i++){
    Serial.print(ganma[i]);
    Serial.print("\t");
   }
  Serial.print("\r\n");
  
  /*C送信*/
    if(wifi.send(ganma, DATALEN)){
      Serial.print("send(ganma) ok\r\n");
     }else{
      Serial.print("send(ganma) err\r\n");
      error_reboot();
     }

   /*α受信*/
    uint8_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0){
      Serial.println("Received α ok");
    }else{
      Serial.println("Received α error");
      error_reboot();
      }  

   Serial.println("alpha:");
   for(uint32_t i = 0; i < DATALEN; i++){
    Serial.print(buffer[i]);
    Serial.print("\t");
   }
   Serial.print("\r\n"); 
       
    for(uint32_t i = 0; i < DATALEN; i++){
    Al[i] = buffer[i]^Am[i]^Mm[i];
   }
 
 /*Mn+1を作成Mn+1=An+Mn*/
 for(uint32_t i = 0; i < DATALEN; i++){
    carry[i]= 0;
  }
  
  for(int32_t i = 15; i >= 0; i--){
    if((Am[i] + Mm[i] + carry[i])>=256){
    Ml[i] = (Am[i] + Mm[i] + carry[i]) - 256;
   if(i!=0){
    carry[(i-1)]=1;
    }
    }else{
     Ml[i] = Am[i] + Mm[i] + carry[i];
    }
  }  
  
/*認証情報・秘匿情報の更新*/
/*暗号化1-9なら配列の更新。暗号化10ならメモリの更新*/
if(n < 9){
  for(uint32_t i = 0; i < DATALEN; i++){
    An[i] = Am[i];
    Am[i] = Al[i];
    Mm[i] = Ml[i];  
   }
  }else if( n == 9){ 
  buffer[0] = 0;
 /*暗号化10回まで正常終了したら1がサーバーから送られてくる.何も送られて来なければ異常ありで中断*/
     len = wifi.recv(buffer, sizeof(buffer), 10000);
       if (len > 0){
        Serial.println("Received:ango ok");
       }else{
        Serial.println("Received:ango error");
        error_reboot(); 
          } 

   Serial.println(buffer[0]);
    /*送られてきた配列が1ならば*/
    if(buffer[0] == 1){    
      EEPROM.put(0,Am);  //0-15:An 16-31:Am 32-47:Mn 48-63:Mm
      EEPROM.put(16,Al);
      EEPROM.put(32,Ml);
    }else {
      error_reboot(); 
    }
  }

 //finish
n = n+1;
}
  EEPROM.get(0,An);
  EEPROM.get(32,Mn);
  Serial.println("An:");
  for(uint32_t i = 0; i < DATALEN; i++){
   Serial.print(Am[i]);
   Serial.print("\t");
  }
  Serial.print("\r\n"); 

  Serial.println("Mn:");
  for(uint32_t i = 0; i < DATALEN; i++){
   Serial.print(Ml[i]);
   Serial.print("\t");
  }
  Serial.print("\r\n"); 

Serial.print("finish\r\n");
release_TCP();
delay(1000); //一秒待機
}

/*エラー赤LED点滅*/
void func_led(){
      digitalWrite(5,HIGH);
      delay(1000);
      digitalWrite(5,LOW);
      delay(1000);
  }

/*再起動要求*/
void error_reboot(){
  Serial.println("Please reboot.");
  while(1){
    func_led();
    }
  }

/*TCP切断*/
void release_TCP(){
      if (wifi.releaseTCP()) {
         Serial.print("release tcp ok\r\n");
      } else {
        Serial.print("release tcp err\r\n");
     }
  }

/*TCP接続*/
void create_TCP(){
    if (wifi.createTCP(R_HOST_NAME, R_HOST_PORT)) {
       Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }
  }

/*wifi接続成功*/
void wificonnect_success(){
    Serial.println("connect success");
    Serial.print("IP:");
    Serial.println(wifi.getLocalIP().c_str());
  }
  
/*認証要求*/
void request_server(){
  uint8_t request[DATALEN] = {0};
      request[0] = 2;
  if(wifi.send(request, DATALEN)){
    Serial.print("request message ok\r\n");
  }else{
    Serial.print("request message err\r\n");
  }
}


/*時刻同期時の時刻*/
time_t getNtpTime(){
/*受信パケットの初期化*/
   if(sizeof(buffer) > 0){
      for(uint32_t i = 0; i < 128; i++){
   buffer[i] = 0;
   }
  } 
  /*サーバーに送るパッケージの作成*/
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  
  /*UDP接続*/
  if (! wifi.registerUDP(N_HOST_NAME, N_HOST_PORT)) {
      error_reboot();
    }
 /*request送信*/
  if(! wifi.send(packetBuffer, 48)){
   error_reboot();
    }
   uint32_t beginWait = millis();
   while (millis() - beginWait < 1500) {
   /* 受信bufferの長さ確認*/
 uint32_t len = wifi.recv(buffer, sizeof(buffer),10000);
   if (len >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)buffer[40] << 24;
      secsSince1900 |= (unsigned long)buffer[41] << 16;
      secsSince1900 |= (unsigned long)buffer[42] << 8;
      secsSince1900 |= (unsigned long)buffer[43];
   /*UDP切断*/
    if (! wifi.unregisterUDP()) {
        Serial.print("unregister udp err\r\n");
        error_reboot();
    }
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
  }
 }
 Serial.println("No NTP Response :-(");
     /*UDP切断*/
    if (! wifi.unregisterUDP()) {
        Serial.print("unregister udp err\r\n");
        error_reboot();
    }
  return 0; // return 0 if unable to get the tim
}

/*センシングデータをuint8_t配列に格納*/  
void sd_to_uint(int32_t x){  
  for(int8_t i = 0;i < 4;i++){
   uint8_t sum = 0;
   uint8_t add = 128;
    for(int8_t j = 0;j < 8;j++){
      if((x & 0x80000000) != 0){
      sum += add;
      }
      x = x << 1;
      add /= 2;
    }
    format[i] = sum;
  }
 }
/*年をuint8_t配列に格納*/  
void year_to_uint(int x,int8_t n){
 for(int8_t i = n;i < n+2;i++){
   uint8_t sum = 0;
   uint8_t add = 128;
   for(int8_t j = 0;j < 8;j++){
     if((x & 0x80000000) != 0){
      sum += add;
     }
     x = x << 1;
     add /= 2;
   }
   format[i] = sum;
  } 
 }

/*時間・日付(年以外)をuint8_t配列に格納*/  
void date_time_to_uint(int8_t x,int8_t n){
    uint8_t sum = 0;
   uint8_t add = 128;
   for(int8_t j = 0;j < 8;j++){
     if((x & 0x80000000) != 0){
      sum += add;
     }
     x = x << 1;
     add /= 2;
   }
   format[n] = sum;
 }
