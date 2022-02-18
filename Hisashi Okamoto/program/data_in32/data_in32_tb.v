`timescale 1ns / 1ns
module data_in32_tb; 
	reg CLK;					/* システムクロック */
	reg rst;					/* rst = 1:初期化 */
	reg [2:0] st;			/* 現在の状態 */
	reg [31:0] DI0;			/* データ入力(α32bit) */
	reg [31:0] DI1;			/* データ入力(α32bit) */
	reg [31:0] DI2;			/* データ入力(α32bit) */
	reg [31:0] DI3;			/* データ入力(α32bit) */
	reg [31:0] DI4;			/* データ入力(α32bit) */
	reg [31:0] DI5;			/* データ入力(α32bit) */
	reg [31:0] DI6;			/* データ入力(α32bit) */
	reg [31:0] DI7;			/* データ入力(α32bit) */
	wire [255:0] PDO;		/* データ出力(α256bit) */
  
	TopModule DUT(.CLK(CLK), .rst(rst), .st(st), .DI0(DI0), .DI1(DI1), .DI2(DI2), .DI3(DI3), .DI4(DI4), .DI5(DI5), .DI6(DI6), .DI7(DI7), .PDO(PDO));

  /* CLK生成 */
	always begin #5
		CLK = ~CLK;
	end
	
	initial begin
		CLK = 1'b0;
		rst = 1'b1;
		st  = 3'b000;
		DI0 <= $unsigned($random);
		DI1 <= $unsigned($random);
		DI2 <= $unsigned($random);
		DI3 <= $unsigned($random);
		DI4 <= $unsigned($random);
		DI5 <= $unsigned($random);
		DI6 <= $unsigned($random);
		DI7 <= $unsigned($random);
		#50
		rst = 1'b0;
		st = 3'b001;
	#50 $stop;
	end
		
endmodule
