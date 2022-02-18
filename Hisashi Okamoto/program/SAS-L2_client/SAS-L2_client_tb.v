`timescale 1ps / 1ps
module SASL2_client_tb; 
	reg CLK;		/* システムクロック */
	reg RST;		/* リセット */
	reg [31:0] DI0;			/* データ入力(α32bit) */
	reg [31:0] DI1;			/* データ入力(α32bit) */
	reg [31:0] DI2;			/* データ入力(α32bit) */
	reg [31:0] DI3;			/* データ入力(α32bit) */
	reg [31:0] DI4;			/* データ入力(α32bit) */
	reg [31:0] DI5;			/* データ入力(α32bit) */
	reg [31:0] DI6;			/* データ入力(α32bit) */
	reg [31:0] DI7;			/* データ入力(α32bit) */
	reg call;		/* 認証要請 */
	reg suc;		/* 認証の成否 */
	wire [31:0] DO0;			/* データ出力(β32bit) */
	wire [31:0] DO1;			/* データ出力(β32bit) */
	wire [31:0] DO2;			/* データ出力(β32bit) */
	wire [31:0] DO3;			/* データ出力(β32bit) */
	wire [31:0] DO4;			/* データ出力(β32bit) */
	wire [31:0] DO5;			/* データ出力(β32bit) */
	wire [31:0] DO6;			/* データ出力(β32bit) */
	wire [31:0] DO7;			/* データ出力(β32bit) */
	wire [2:0] st;
  
	TopModule DUT(.CLK(CLK), .RST(RST), .DI0(DI0), .DI1(DI1), .DI2(DI2), .DI3(DI3), .DI4(DI4), .DI5(DI5), .DI6(DI6), .DI7(DI7), .call(call), .suc(suc), .DO0(DO0), .DO1(DO1), .DO2(DO2), .DO3(DO3), .DO4(DO4), .DO5(DO5), .DO6(DO6), .DO7(DO7), .st(st));

  /* CLK生成 */
	always begin #5
		CLK = ~CLK;
	end
	
	initial begin
		CLK = 1'b0;
		RST = 1'b1;
		DI0 <= $unsigned($random);
		DI1 <= $unsigned($random);
		DI2 <= $unsigned($random);
		DI3 <= $unsigned($random);
		DI4 <= $unsigned($random);
		DI5 <= $unsigned($random);
		DI6 <= $unsigned($random);
		DI7 <= $unsigned($random);
		#10
		RST = 1'b0;
		call = 1'b1;
		#10
		call = 1'b0;
		#100
		suc = 1'b1;
		#10
		suc = 1'b0;
	#100 $stop;
	end
		
endmodule
