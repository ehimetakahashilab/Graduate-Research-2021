module TopModule(CLK, RST, call, PD, MD, suc, RES, st, rst, rdwr);
	input CLK;					/* システムクロック */
	input RST;					/* リセット信号 */
	input call;					/* 認証要請 */
	input suc;					/* 認証の成否 */
	input [255:0] PD;			/* シフトレジスタからのデータ(α) */
	input [255:0] MD;			/* メモリからのデータ(An, Mn) */
	output [255:0] RES;		/* 演算結果 */
	output rst;
	output [2:0] st;			/* 状態 */
	output rdwr;				/* rdwr = 0:メモリからの読み出し,rdwr = 1:メモリへの書き込み */
	
	wire [2:0] w_st;
	
	ALU u0(.CLK(CLK), .RST(RST), .call(call), .suc(suc), .PD(PD), .MD(MD), .RES(RES), .st(st), .rst(rst), .rdwr(rdwr));//,adr);
	
endmodule