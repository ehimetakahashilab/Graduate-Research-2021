module TopModule(CLK, rst, st, PDI, DO0, DO1, DO2, DO3, DO4, DO5, DO6, DO7);
	input CLK;					/* システムクロック */
	input rst;					/* rst = 1:初期化 */
	input [2:0] st;			/* 現在の状態 */
	input [255:0] PDI;		/* データ入力(β256bit) */
	output [31:0] DO0;		/* データ出力(β32bit) */
	output [31:0] DO1;		/* データ出力(β32bit) */
	output [31:0] DO2;		/* データ出力(β32bit) */
	output [31:0] DO3;		/* データ出力(β32bit) */
	output [31:0] DO4;		/* データ出力(β32bit) */
	output [31:0] DO5;		/* データ出力(β32bit) */
	output [31:0] DO6;		/* データ出力(β32bit) */
	output [31:0] DO7;		/* データ出力(β32bit) */
	
	data_out32 u0(.CLK(CLK), .rst(rst), .st(st), .PDI(PDI), .DO0(DO0), .DO1(DO1), .DO2(DO2), .DO3(DO3), .DO4(DO4), .DO5(DO5), .DO6(DO6), .DO7(DO7));

endmodule
