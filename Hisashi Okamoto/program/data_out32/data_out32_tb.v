`timescale 1ns / 1ns
module data_out32_tb; 
	reg CLK;					/* システムクロック */
	reg rst;					/* rst = 1:初期化 */
	reg [2:0] st;			/* 現在の状態 */
	reg [255:0] PDI;		/* データ入力(β256bit) */
	wire [31:0] DO0;		/* データ出力(β32bit) */
	wire [31:0] DO1;		/* データ出力(β32bit) */
	wire [31:0] DO2;		/* データ出力(β32bit) */
	wire [31:0] DO3;		/* データ出力(β32bit) */
	wire [31:0] DO4;		/* データ出力(β32bit) */
	wire [31:0] DO5;		/* データ出力(β32bit) */
	wire [31:0] DO6;		/* データ出力(β32bit) */
	wire [31:0] DO7;		/* データ出力(β32bit) */
  
	TopModule DUT(.CLK(CLK), .rst(rst), .st(st), .PDI(PDI), .DO0(DO0), .DO1(DO1), .DO2(DO2), .DO3(DO3), .DO4(DO4), .DO5(DO5), .DO6(DO6), .DO7(DO7));

  /* CLK生成 */
	always begin #5
		CLK = ~CLK;
	end
	
	initial begin
		CLK = 1'b0;
		rst = 1'b1;
		st  = 3'b000;
		PDI[31:0] <= $unsigned($random);
		PDI[63:32] <= $unsigned($random);
		PDI[95:64] <= $unsigned($random);
		PDI[127:96] <= $unsigned($random);
		PDI[159:128] <= $unsigned($random);
		PDI[181:160] <= $unsigned($random);
		PDI[223:182] <= $unsigned($random);
		PDI[255:224] <= $unsigned($random);
		#10
		rst = 1'b0;
		#10
		st  = 3'b001;
		#10
		st  = 3'b010;
		#10
		st  = 3'b011;
		#10
		st  = 3'b100;
		#10
		st  = 3'b101;
	#50 $stop;
	end
		
endmodule
