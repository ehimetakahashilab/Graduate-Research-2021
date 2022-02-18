module TopModule(CLK, rst, st, DI0, DI1, DI2, DI3, DI4, DI5, DI6, DI7, PDO);
	input CLK;					/* システムクロック */
	input rst;					/* rst = 1:初期化 */
	input [2:0] st;			/* 現在の状態 */
	input [31:0] DI0;			/* データ入力(α32bit) */
	input [31:0] DI1;			/* データ入力(α32bit) */
	input [31:0] DI2;			/* データ入力(α32bit) */
	input [31:0] DI3;			/* データ入力(α32bit) */
	input [31:0] DI4;			/* データ入力(α32bit) */
	input [31:0] DI5;			/* データ入力(α32bit) */
	input [31:0] DI6;			/* データ入力(α32bit) */
	input [31:0] DI7;			/* データ入力(α32bit) */
	output [255:0] PDO;		/* データ出力(α256bit) */
	
	data_in32 u0(.CLK(CLK), .rst(rst), .st(st), .DI0(DI0), .DI1(DI1), .DI2(DI2), .DI3(DI3), .DI4(DI4), .DI5(DI5), .DI6(DI6), .DI7(DI7), .PDO(PDO));
	
endmodule
