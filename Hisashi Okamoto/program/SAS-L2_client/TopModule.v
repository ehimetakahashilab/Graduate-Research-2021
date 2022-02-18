module TopModule(CLK, RST, DI0, DI1, DI2, DI3, DI4, DI5, DI6, DI7, call, suc, DO0, DO1, DO2, DO3, DO4, DO5, DO6, DO7, st);
	input CLK;		/* システムクロック */
	input RST;		/* リセット */
	input [31:0] DI0;			/* データ入力(α32bit) */
	input [31:0] DI1;			/* データ入力(α32bit) */
	input [31:0] DI2;			/* データ入力(α32bit) */
	input [31:0] DI3;			/* データ入力(α32bit) */
	input [31:0] DI4;			/* データ入力(α32bit) */
	input [31:0] DI5;			/* データ入力(α32bit) */
	input [31:0] DI6;			/* データ入力(α32bit) */
	input [31:0] DI7;			/* データ入力(α32bit) */
	input call;		/* 認証要請 */
	input suc;		/* 認証の成否 */
	output [31:0] DO0;			/* データ出力(β32bit) */
	output [31:0] DO1;			/* データ出力(β32bit) */
	output [31:0] DO2;			/* データ出力(β32bit) */
	output [31:0] DO3;			/* データ出力(β32bit) */
	output [31:0] DO4;			/* データ出力(β32bit) */
	output [31:0] DO5;			/* データ出力(β32bit) */
	output [31:0] DO6;			/* データ出力(β32bit) */
	output [31:0] DO7;			/* データ出力(β32bit) */
	output [2:0] st;
	
	wire rst;
	wire [2:0] w_st;
	wire [2:0] out_st;
	wire [255:0] PDO;
	wire [255:0] RES;
	wire [255:0] MD;
	wire adr;
	wire rdwr;
	
	assign st = w_st;

//	FSM u1(.CLK(CLK), .RST(RST), .call(call), .suc(suc), .in_st(out_st), .st(w_st), .rst(rst));
//	shiftreg_sp u2(.CLK(CLK), .rst(rst), .st(w_st), .SDI(DI), .PDO(PDO));
	data_in32 u2(.CLK(CLK), .rst(rst), .st(w_st), .DI0(DI0), .DI1(DI1), .DI2(DI2), .DI3(DI3), .DI4(DI4), .DI5(DI5), .DI6(DI6), .DI7(DI7), .PDO(PDO));
	ALU u3(.CLK(CLK), .RST(RST), .call(call), .suc(suc), .PD(PDO), .MD(MD), .RES(RES), .rst(rst), .st(w_st), .rdwr(rdwr));//,.adr(adr));
//	shiftreg_ps u4(.CLK(CLK), .rst(rst), .st(w_st), .PDI(RES), .SDO(DO));
	data_out32 u4(.CLK(CLK), .rst(rst), .st(w_st), .PDI(RES), .DO0(DO0), .DO1(DO1), .DO2(DO2), .DO3(DO3), .DO4(DO4), .DO5(DO5), .DO6(DO6), .DO7(DO7));
	adr_reg u5(.st(w_st), .adr(adr));
	ram u6(.address(adr), .clock(CLK), .data(RES), .rden(~rdwr), .wren(rdwr), .q(MD));
	
endmodule