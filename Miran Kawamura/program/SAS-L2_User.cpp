//use twelites mwx  c++ template library
#include <TWELITE>
#include <NWK_SIMPLE>
#include <BRD_APPTWELITE>

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

/***application defs*/
int password[] = {20, 56, 1, 234, 23, 45, 176, 89, 56, 78, 201, 123, 88, 189, 45, 245};
uint8_t Hash[] = {72, 197, 105, 108, 182, 68, 113, 1, 128, 62, 111, 233, 219, 7, 193, 187, 92, 212, 13, 193, 156, 164, 238, 157, 15, 92, 232, 226, 217, 245, 55, 2};
uint8_t Mn[] = {244, 24, 217, 252, 111, 69, 252, 27, 16, 132, 213, 227, 17, 232, 55, 229, 106, 166, 241, 133, 55, 142, 187, 237, 47, 246, 12, 37, 146, 173, 55, 85};
uint8_t alpha[32];
uint8_t HashNext[32];
uint8_t g_recv_alpha[32];

char data[32];
char recvdata[5];

uint8_t u8devid = 0;

uint16_t au16AI[5];
uint8_t u8DI_BM;
uint8_t beta[32];

int n = 0;
int check = 0;
double start = 0.0;
double end = 0.0;
double sum1 = 0.0;
double sum2 = 0.0;
double sys = 0.0;
double sys1 = 0.0;
double sys2 = 0.0;
double sysa = 0.0;
double sysb = 0.0;
double sysc = 0.0;
int count = 1;
double ave1 = 0.0;
double ave2 = 0.0;

//初期登録したA1とM1//
const char APP_FIRSTDATA[] = "A1M1";
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
  if (the_twelite.receiver.available()) {
    start = millis();
    receive_alphan();
    end = millis();
    sysa = end-start;

    Serial << format("A%d = ", n);
    for(int i = 0; i < 32; i++){
      Serial << format("%d ", Hash[i]);
    }
    Serial << "\n";

    Serial << format("M%d = ", n);
    for(int i = 0; i < 32; i++){
      Serial << format("%d ", Mn[i]);
    }
    Serial << "\n";

    Serial << "recv_alpha = ";
    for(int i = 0; i < 32; i++){
      Serial << format("%d ", g_recv_alpha[i]);
    }
    Serial << "\n";

    for(int i = 0; i < 32; i++){
      beta[i] = 0;
    }

    start = millis();
    for(int i = 0; i < 32; i++){
      HashNext[i] = g_recv_alpha[i] ^ Hash[i] ^ Mn[i];
      beta[i] = HashNext[i] + Hash[i];
    }

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

    Serial << "beta = ";
    for(int i = 0; i < 32; i++){
      Serial << format("%d ", beta[i]);
    }
    Serial << "\n";
    end = millis();
    sys1 = end-start;

    start = millis();
    transmit_beta();
    end = millis();
    sysb = end-start;

    start = millis();
    for(int i = 0; i < 32; i++){
      Mn[i] = Hash[i] + Mn[i];
      Hash[i] = HashNext[i];
    }
    end = millis();
    sys2 = end-start;

    sys = sys1 + sys2;
    sysc = sysa + sysb;
    sum1 += sys;
    sum2 += sysc;
    ave1 = double(sum1/count);
    ave2 = double(sum2/count);
    count++;
    Serial << format("math average = %.2f\n\n", ave1);
    Serial << format("comm average = %.2f\n\n", ave2);
  }
}

/***transmit a packet*/
MWX_APIRET transmit_beta() {
  if(auto&& pkt = the_twelite.network.use<NWK_SIMPLE>().prepare_tx_packet()) {
    Serial << "transmit!\n";

    //set tx packet behavior
    pkt << tx_addr(u8devid == 0 ? 0xFE : 0x00)
        << tx_retry(0x1)
        << tx_packet_delay(0,50,10);

    //prepare packet payload
    pack_bytes(pkt.get_payload()
    , make_pair(APP_BETADATA, 4)
    , uint8_t (beta[0]), uint8_t (beta[1]), uint8_t (beta[2]), uint8_t (beta[3])
    , uint8_t (beta[4]), uint8_t (beta[5]), uint8_t (beta[6]), uint8_t (beta[7])
    , uint8_t (beta[8]), uint8_t (beta[9]), uint8_t (beta[10]), uint8_t (beta[11])
    , uint8_t (beta[12]), uint8_t (beta[13]), uint8_t (beta[14]), uint8_t (beta[15])
    , uint8_t (beta[16]), uint8_t (beta[17]), uint8_t (beta[18]), uint8_t (beta[19])
    , uint8_t (beta[20]), uint8_t (beta[21]), uint8_t (beta[22]), uint8_t (beta[23])
    , uint8_t (beta[24]), uint8_t (beta[25]), uint8_t (beta[26]), uint8_t (beta[27])
    , uint8_t (beta[28]), uint8_t (beta[29]), uint8_t (beta[30]), uint8_t (beta[31]));

    // do transmit
    return pkt.transmit();
  }
  return MWX_APIRET(false, 0);
}

void receive_alphan(){
  auto&& rx = the_twelite.receiver.read();
  Serial << "receive!\n";

  //expand packet payload(shall match with sent packet data structure, see pack_bytes())
  char alphadata[5]{};
  auto&& np = expand_bytes(rx.get_payload().begin(), rx.get_payload().end(), make_pair((uint8_t*)alphadata, 4));

  //check header
  if(strncmp(APP_ALPHADATA, alphadata, 4)){ return; }

  uint8_t recv_alpha[32] = {};
  uint8_t times;
  expand_bytes(np, rx.get_payload().end()
  , recv_alpha[0], recv_alpha[1], recv_alpha[2], recv_alpha[3]
  , recv_alpha[4], recv_alpha[5], recv_alpha[6], recv_alpha[7]
  , recv_alpha[8], recv_alpha[9], recv_alpha[10], recv_alpha[11]
  , recv_alpha[12], recv_alpha[13], recv_alpha[14], recv_alpha[15]
  , recv_alpha[16], recv_alpha[17], recv_alpha[18], recv_alpha[19]
  , recv_alpha[20], recv_alpha[21], recv_alpha[22], recv_alpha[23]
  , recv_alpha[24], recv_alpha[25], recv_alpha[26], recv_alpha[27]
  , recv_alpha[28], recv_alpha[29], recv_alpha[30], recv_alpha[31]
  , times
  );

  for(int i = 0; i < 32; i++){
    g_recv_alpha[i] = recv_alpha[i];
  }
  n = times;
}
