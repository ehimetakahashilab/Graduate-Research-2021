//use twelites mwx  c++ template library
#include <TWELITE>
#include <NWK_SIMPLE>
#include <BRD_APPTWELITE>

#ifdef _MSC_VER
#ifndef uint8_t
typedef unsigned __int8 uint8_t;
#endif
#ifndef uint32_t
typedef unsigned __int32 uint32_t;
#endif
#else
#include <stdint.h>
#endif

#define SHA256_BYTES 32
typedef struct {
  uint8_t  buf[64];
  uint32_t hash[8];
  uint32_t bits[2];
  uint32_t len;
} sha256_context;

#define FN_ inline static

static const uint32_t K[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
  0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
  0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
  0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
  0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
  0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#ifdef MINIMIZE_STACK_IMPACT
static uint32_t W[64];
#endif

/***Config part*/
//application ID
const uint32_t APP_ID = 0x1234abcd;

//channel
const uint8_t CHANNEL = 13;

/***funcyion prototype*/
MWX_APIRET transmit_alphan();
MWX_APIRET transmit_beta();
void receive_alphan();
void receive_beta();
void sha256_init(sha256_context *ctx);
void sha256_hash(sha256_context *ctx, const void *data, size_t len);
void sha256_done(sha256_context *ctx, uint8_t *hash);

/***application defs*/
int password[] = {20, 56, 1, 234, 23, 45, 176, 89, 56, 78, 201, 123, 88, 189, 45, 245};
uint8_t Hash[] = {72, 197, 105, 108, 182, 68, 113, 1, 128, 62, 111, 233, 219, 7, 193, 187, 92, 212, 13, 193, 156, 164, 238, 157, 15, 92, 232, 226, 217, 245, 55, 2};
uint8_t Mn[] = {244, 24, 217, 252, 111, 69, 252, 27, 16, 132, 213, 227, 17, 232, 55, 229, 106, 166, 241, 133, 55, 142, 187, 237, 47, 246, 12, 37, 146, 173, 55, 85};
uint8_t Nnext[16], Xor16[16], Xor32[32], alpha[32], Mnext[32];
uint8_t beta[32], Server_beta[32], g_recv_beta[32];

sha256_context ctx;
char data[32];
char recvdata[5];
uint8_t u8devid = 0;
uint16_t au16AI[5];
uint8_t u8DI_BM;
uint8_t HashNext[SHA256_BYTES];

int number = 0;
int n = 1;
int iLedCounter = 0;
double start = 0.0;
double end = 0.0;
double sum1 = 0.0;
double sum2 = 0.0;
double sumw = 0.0;
double sys = 0.0;
double sys1 = 0.0;
double sys2 = 0.0;
double sysa = 0.0;
double sysb = 0.0;
double sysc = 0.0;
int count = 1;
double ave1 = 0.0;
double ave2 = 0.0;
double avew = 0.0;
double wait = 0.0;

//初期登録したA1とM1//
const char APP_ALPHADATA[] = "ABCZ";
const char APP_BETADATA[] = "TWEL";

/***set up procedure (run once at cold boot)*/
void setup(){
  //init vars
  for(auto&& x : au16AI) x = 0xFFFF;
  u8DI_BM = 0xFF;

  /***SETUP section*/
  auto&& brd = the_twelite.board.use<BRD_APPTWELITE>();

  //check DIP sw settings
  u8devid = (brd.get_M1()) ? 0x00 : 0xFE;

  //setup analogue
  Analogue.setup(true, ANALOGUE::KICK_BY_TIMER0);

  //setup buttons
  Buttons.setup(5); //init button manager with 5 history table.

  //the twelite main class
  the_twelite
    << TWENET::appid(APP_ID)
	  << TWENET::channel(CHANNEL)
	  << TWENET::rx_when_idle();

  //Register Network
  auto&& nwksmpl = the_twelite.network.use<NWK_SIMPLE>();
  nwksmpl << NWK_SIMPLE::logical_id(u8devid);

  /***BEGIN section*/

  //start ADC capture
  Analogue.begin(pack_bits(
    BRD_APPTWELITE::PIN_AI1,
    BRD_APPTWELITE::PIN_AI2,
    BRD_APPTWELITE::PIN_AI3,
    BRD_APPTWELITE::PIN_AI4,
    PIN_ANALOGUE::VCC)); //_start continuous adc capture.

  //Timer setup
  Timer0.begin(32, true); //32hz Timer

  //start button check
  Buttons.begin(pack_bits(
    BRD_APPTWELITE::PIN_DI1,
    BRD_APPTWELITE::PIN_DI2,
    BRD_APPTWELITE::PIN_DI3,
    BRD_APPTWELITE::PIN_DI4),
    5,
    4
  );

  the_twelite.begin(); //start twelite!
  Serial << ("start wirelrss communication!\n");
}

/***loop procedure (called every event)*/
void loop(){
  if(Timer0.available()){
    static uint8_t u16ct;
    u16ct++;

    if((u16ct % 160) == 0){
      if(iLedCounter == 0){
        digitalWrite(BRD_APPTWELITE::PIN_DO1, HIGH);
        iLedCounter = 1;
      }
      else{
        digitalWrite(BRD_APPTWELITE::PIN_DO1, LOW);
        iLedCounter = 0;

        start = millis();
        Serial << format("random[%d] = ", n+1);
        for(int i = 0; i < 16; ++i){
          number = random(0, 255);
          Nnext[i] = number;
          Serial << format("%02d ", Nnext[i]);
        }

        Serial << format("pass xor N%d : ", n+1);
        for(int i = 0; i < 16; i++){
          Xor16[i] = 0;
          Xor16[i] = password[i] ^ Nnext[i];
          TWE_snprintf(&data[i*2], 32, "%02x", Xor16[i]);
          Serial << format("%02d ", Xor16[i]);
        }
        Serial << "\n";

        Serial << format("hash(in) = %s\n", data);

        sha256_init(&ctx);
        sha256_hash(&ctx, data, strlen(data));
        sha256_done(&ctx, HashNext);

        Serial << format("alpha(A%d xor A%d xor M%d) = \n", n+1, n, n);
        for(int i = 0; i < 32; i++){
          alpha[i] = HashNext[i] ^ Hash[i] ^ Mn[i];
          Serial << format("%02d ", alpha[i]);
        }
        Serial << "\n";

        end = millis();
        sys1 = end-start;
        start = millis();
        transmit_alphan();
        end = millis();
        sysa = end-start;
        start = millis();
      }
    }
  }

  if(the_twelite.receiver.available()){
    end = millis();
    wait = end-start;
    start = millis();
    receive_beta();
    end = millis();
    sysb = end-start;

    start = millis();
    Serial << format("A%d = ", n+1);

    for(int i = 0; i < 32; i++){
      Serial << format("%d ", HashNext[i]);
    }
    Serial << "\n";

    Serial << format("A%d = ", n);
    for(int i = 0; i < 32; i++){
      Serial << format("%d ", Hash[i]);
    }
    Serial << "\n";

    Serial << "Server Beta = ";
    for(int i = 0; i < 32; i++){
      Server_beta[i] = HashNext[i] + Hash[i];
      Serial << format("%d ", Server_beta[i]);
    }

    Serial << "\n";
    int check = 0;
    for(int i = 0; i < 32; i++){
      if( Server_beta[i] == g_recv_beta[i]){
        check++;
      }
    }

    if(check == 32){
      Serial << "---OK! SAS-L2 complete!---\n";
      for(int i = 0; i < 32; i++){
        Mn[i] =  Hash[i] + Mn[i];
        Hash[i] = HashNext[i];
      }
      n++;
    }
    else{
      Serial << "---Error! Not complete!---\n\n";
    }

    end = millis();
    sys2 = end-start;
    sys = sys1 + sys2;
    sysc = sysa + sysb;
    sum1 += sys;
    sum2 += sysc;
    sumw += wait;
    ave1 = double(sum1/count);
    ave2 = double(sum2/count);
    avew = double(sumw/count);
    count++;
    Serial << format("math average = %.2f\n\n", ave1);
    Serial << format("comm average = %.2f\n\n", ave2);
    Serial << format("wait average = %.2f\n\n", avew);
  }
}

/***transmit a packet*/
MWX_APIRET transmit_alphan() {
  if(auto&& pkt = the_twelite.network.use<NWK_SIMPLE>().prepare_tx_packet()) {
    Serial << "transmit!\n";
    //set tx packet behavior
    pkt << tx_addr(u8devid == 0 ? 0xFE : 0x00)
        << tx_retry(0x1)
        << tx_packet_delay(0,50,10);

    //prepare packet payload
    pack_bytes(pkt.get_payload()
    , make_pair(APP_ALPHADATA, 4)
    , uint8_t (alpha[0]), uint8_t (alpha[1]), uint8_t (alpha[2]), uint8_t (alpha[3])
    , uint8_t (alpha[4]), uint8_t (alpha[5]), uint8_t (alpha[6]), uint8_t (alpha[7])
    , uint8_t (alpha[8]), uint8_t (alpha[9]), uint8_t (alpha[10]), uint8_t (alpha[11])
    , uint8_t (alpha[12]), uint8_t (alpha[13]), uint8_t (alpha[14]), uint8_t (alpha[15])
    , uint8_t (alpha[16]), uint8_t (alpha[17]), uint8_t (alpha[18]), uint8_t (alpha[19])
    , uint8_t (alpha[20]), uint8_t (alpha[21]), uint8_t (alpha[22]), uint8_t (alpha[23])
    , uint8_t (alpha[24]), uint8_t (alpha[25]), uint8_t (alpha[26]), uint8_t (alpha[27])
    , uint8_t (alpha[28]), uint8_t (alpha[29]), uint8_t (alpha[30]), uint8_t (alpha[31])
    , uint8_t (n));

    // do transmit
    return pkt.transmit();
  }
  return MWX_APIRET(false, 0);
}

void receive_beta(){
  auto&& rx = the_twelite.receiver.read();
  Serial << "receive!\n";

  //expand packet payload(shall match with sent packet data structure, see pack_bytes())
  char betadata[5]{};
  auto&& np = expand_bytes(rx.get_payload().begin(), rx.get_payload().end(), make_pair((uint8_t*)betadata, 4));

  //check header
  if(strncmp(APP_BETADATA, betadata, 4)){ return; }

  uint8_t recv_beta[32];
  expand_bytes(np, rx.get_payload().end()
  , recv_beta[0], recv_beta[1], recv_beta[2], recv_beta[3]
  , recv_beta[4], recv_beta[5], recv_beta[6], recv_beta[7]
  , recv_beta[8], recv_beta[9], recv_beta[10], recv_beta[11]
  , recv_beta[12], recv_beta[13], recv_beta[14], recv_beta[15]
  , recv_beta[16], recv_beta[17], recv_beta[18], recv_beta[19]
  , recv_beta[20], recv_beta[21], recv_beta[22], recv_beta[23]
  , recv_beta[24], recv_beta[25], recv_beta[26], recv_beta[27]
  , recv_beta[28], recv_beta[29], recv_beta[30], recv_beta[31]);

  for(int i = 0; i < 32; i++){
    g_recv_beta[i] = recv_beta[i];
  }
}

FN_ uint8_t _shb(uint32_t x, uint32_t n){
  return ( (x >> (n & 31)) & 0xff );
} /*_shb*/

FN_ uint32_t _shw(uint32_t x, uint32_t n){
  return ( (x << (n & 31)) & 0xffffffff );
} /*_shw*/

FN_ uint32_t _r(uint32_t x, uint8_t n){
  return ( (x >> n) | _shw(x, 32 - n) );
} /*_r*/

FN_ uint32_t _Ch(uint32_t x, uint32_t y, uint32_t z){
  return ( (x & y) ^ ((~x) & z) );
} /*_Ch*/

FN_ uint32_t _Ma(uint32_t x, uint32_t y, uint32_t z){
  return ( (x & y) ^ (x & z) ^ (y & z) );
} /*_Ma*/

FN_ uint32_t _S0(uint32_t x){
  return ( _r(x, 2) ^ _r(x, 13) ^ _r(x, 22) );
} /*_S0*/

FN_ uint32_t _S1(uint32_t x){
  return ( _r(x, 6) ^ _r(x, 11) ^ _r(x, 25) );
} /*_S1*/

FN_ uint32_t _G0(uint32_t x){
  return ( _r(x, 7) ^ _r(x, 18) ^ (x >> 3) );
} /*_G0*/

FN_ uint32_t _G1(uint32_t x){
  return ( _r(x, 17) ^ _r(x, 19) ^ (x >> 10) );
} /*_G1*/

FN_ uint32_t _word(uint8_t *c){
  return ( _shw(c[0], 24) | _shw(c[1], 16) | _shw(c[2], 8) | (c[3]) );
} /*_word*/

FN_ void  _addbits(sha256_context *ctx, uint32_t n){
  if ( ctx->bits[0] > (0xffffffff - n) )
  ctx->bits[1] = (ctx->bits[1] + 1) & 0xFFFFFFFF;
  ctx->bits[0] = (ctx->bits[0] + n) & 0xFFFFFFFF;
} /*_addbits*/

static void _hash(sha256_context *ctx){
  register uint32_t a, b, c, d, e, f, g, h, i;
  uint32_t t[2];
  #ifndef MINIMIZE_STACK_IMPACT
  uint32_t W[64];
  #endif

  a = ctx->hash[0];
  b = ctx->hash[1];
  c = ctx->hash[2];
  d = ctx->hash[3];
  e = ctx->hash[4];
  f = ctx->hash[5];
  g = ctx->hash[6];
  h = ctx->hash[7];

  for (i = 0; i < 64; i++) {
    if ( i < 16 )
      W[i] = _word(&ctx->buf[_shw(i, 2)]);
    else
      W[i] = _G1(W[i - 2]) + W[i - 7] + _G0(W[i - 15]) + W[i - 16];

    t[0] = h + _S1(e) + _Ch(e, f, g) + K[i] + W[i];
    t[1] = _S0(a) + _Ma(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + t[0];
    d = c;
    c = b;
    b = a;
    a = t[0] + t[1];
  }

  ctx->hash[0] += a;
  ctx->hash[1] += b;
  ctx->hash[2] += c;
  ctx->hash[3] += d;
  ctx->hash[4] += e;
  ctx->hash[5] += f;
  ctx->hash[6] += g;
  ctx->hash[7] += h;
} /*_hash*/

void sha256_init(sha256_context *ctx){
  if ( ctx != NULL ) {
    ctx->bits[0] = ctx->bits[1] = 0;
    ctx->len     = 0;
    ctx->hash[0] = 0x6a09e667;
    ctx->hash[1] = 0xbb67ae85;
    ctx->hash[2] = 0x3c6ef372;
    ctx->hash[3] = 0xa54ff53a;
    ctx->hash[4] = 0x510e527f;
    ctx->hash[5] = 0x9b05688c;
    ctx->hash[6] = 0x1f83d9ab;
    ctx->hash[7] = 0x5be0cd19;
  }
} /*sha256_init*/
void sha256_hash(sha256_context *ctx, const void *data, size_t len){
  register size_t i;
  const uint8_t *bytes = (const uint8_t *)data;

  if ( (ctx != NULL) && (bytes != NULL) )
    for (i = 0; i < len; i++) {
      ctx->buf[ctx->len] = bytes[i];
      ctx->len++;
      if (ctx->len == sizeof(ctx->buf) ) {
        _hash(ctx);
        _addbits(ctx, sizeof(ctx->buf) * 8);
        ctx->len = 0;
      }
    }
} /*sha256_hash*/

void sha256_done(sha256_context *ctx, uint8_t *hash){
  register uint32_t i, j;

  if ( ctx != NULL ) {
    j = ctx->len % sizeof(ctx->buf);
    ctx->buf[j] = 0x80;
    for (i = j + 1; i < sizeof(ctx->buf); i++)
      ctx->buf[i] = 0x00;

    if ( ctx->len > 55 ) {
      _hash(ctx);
      for (j = 0; j < sizeof(ctx->buf); j++)
        ctx->buf[j] = 0x00;
    }

    _addbits(ctx, ctx->len * 8);
    ctx->buf[63] = _shb(ctx->bits[0],  0);
    ctx->buf[62] = _shb(ctx->bits[0],  8);
    ctx->buf[61] = _shb(ctx->bits[0], 16);
    ctx->buf[60] = _shb(ctx->bits[0], 24);
    ctx->buf[59] = _shb(ctx->bits[1],  0);
    ctx->buf[58] = _shb(ctx->bits[1],  8);
    ctx->buf[57] = _shb(ctx->bits[1], 16);
    ctx->buf[56] = _shb(ctx->bits[1], 24);
    _hash(ctx);

    if ( hash != NULL )
      for (i = 0, j = 24; i < 4; i++, j -= 8) {
        hash[i     ] = _shb(ctx->hash[0], j);
        hash[i +  4] = _shb(ctx->hash[1], j);
        hash[i +  8] = _shb(ctx->hash[2], j);
        hash[i + 12] = _shb(ctx->hash[3], j);
        hash[i + 16] = _shb(ctx->hash[4], j);
        hash[i + 20] = _shb(ctx->hash[5], j);
        hash[i + 24] = _shb(ctx->hash[6], j);
        hash[i + 28] = _shb(ctx->hash[7], j);
      }
  }
}
