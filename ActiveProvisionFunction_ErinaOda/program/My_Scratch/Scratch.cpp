// use twelite mwx c++ template library
#include <TWELITE>
#include <NWK_SIMPLE>

/*** Config part */
// application ID
const uint32_t APP_ID = 0x1234abcd;

// channel
const uint8_t CHANNEL = 13;

// 論理ID
const uint8_t UID = 0x10;

//受信したコマンド(0で初期化)
uint8_t rx_cmd = 0x00;

//出力ピン
const uint8_t PIN_DO1 = 18;
const uint8_t PIN_DO2 = 19;
const uint8_t PIN_DO3 = 4;

/*** setup procedure (run once at cold boot) */
void setup() {

	// the twelite main class
	the_twelite
		<< TWENET::appid(APP_ID)    // set application ID (identify network group)
		<< TWENET::channel(CHANNEL) // set channel (pysical channel)
		<< TWENET::rx_when_idle();  // open receive circuit (if not set, it can't listen packts from others)

	// Register Network
	auto&& nwk = the_twelite.network.use<NWK_SIMPLE>();
	nwk	<< NWK_SIMPLE::logical_id(UID); // set Logical ID.

	the_twelite.begin(); // start twelite!

	/*** INIT message */
	Serial << "--- Scratch act ---" << mwx::crlf;

	//ピン設定
	pinMode(PIN_DO1, OUTPUT);
    digitalWrite(PIN_DO1, HIGH);

    pinMode(PIN_DO2, OUTPUT);
    digitalWrite(PIN_DO2, HIGH);

    pinMode(PIN_DO3, OUTPUT);
    digitalWrite(PIN_DO3, HIGH);

}

/*** begin procedure (called once at boot) */
void begin() {
	Serial << "..begin (run once at boot)" << mwx::crlf;
}

/*** loop procedure (called every event) */
void loop() {

	// packet
	if (the_twelite.receiver.available()) {
		auto&& rx = the_twelite.receiver.read();

		// just dump a packet.
		Serial << format("rx from %08x/%d", rx.get_addr_src_long(), rx.get_addr_src_lid()) << mwx::crlf;

		expand_bytes(rx.get_payload().begin(), rx.get_payload().end(),
					rx_cmd //コマンド種別(1バイト)
		);

		Serial << format("rx_cmd is %02x",rx_cmd) << mwx::crlf;
	}

	switch(rx_cmd){
		case 0x00:
			//光らせない場合の処理
			digitalWrite(PIN_DO1, HIGH);
        	digitalWrite(PIN_DO2, HIGH);
        	digitalWrite(PIN_DO3, HIGH);
			break;
		case 0x01:
			//赤色を光らせる場合の処理
			digitalWrite(PIN_DO1, LOW);
        	digitalWrite(PIN_DO2, HIGH);
        	digitalWrite(PIN_DO3, HIGH);
			break;
		case 0x02:
			//黄色を光らせる場合の処理
        	digitalWrite(PIN_DO1, HIGH);
			digitalWrite(PIN_DO2, LOW);
        	digitalWrite(PIN_DO3, HIGH);
			break;
		case 0x03:
			//緑を光らせる場合の処理
        	digitalWrite(PIN_DO1, HIGH);
        	digitalWrite(PIN_DO2, HIGH);
			digitalWrite(PIN_DO3, LOW);
			break;
		default:
			//nothing do
			break;
	}

}



/* Copyright (C) 2019-2020 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE        *
 * AGREEMENT).                                                        */