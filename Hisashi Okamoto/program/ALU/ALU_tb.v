`timescale 1ns / 1ns
module ALU_tb; 
	reg CLK;					/* システムクロック */
	reg RST;					/* リセット信号 */
	reg call;
	reg suc;
//	input [2:0] st;			/* 現在の状態 */
	reg [255:0] PD;			/* シフトレジスタからのデータ(α) */
	reg [255:0] MD;			/* メモリからのデータ(An, Mn) */
	wire [255:0] RES;		/* 演算結果 */
	wire [2:0] st;		/* FSMへのレスポンス */
	wire rst;
	wire rdwr;				/* rdwr = 0:メモリからの読み出し,rdwr = 1:メモリへの書き込み */
//	output adr ;  /* adr = 0:An, 1:Mn */

	TopModule DUT(.CLK(CLK), .RST(RST), .call(call), .PD(PD), .MD(MD), .suc(suc), .RES(RES), .st(st), .rst(rst), .rdwr(rdwr)); 

  /* CLK生成 */
	always begin #5 
		CLK = ~CLK;
	end
	
	initial begin
	CLK = 1'b0;
	RST = 1'b1;
	call = 1'b0;
	suc = 1'b0;
//	PD = 256'b0;
//	MD = 256'b1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111;
	PD[31:0] <= $unsigned($random);
	PD[63:32] <= $unsigned($random);
	PD[95:64] <= $unsigned($random);
	PD[127:96] <= $unsigned($random);
	PD[159:128] <= $unsigned($random);
	PD[181:160] <= $unsigned($random);
	PD[213:182] <= $unsigned($random);
	PD[255:214] <= $unsigned($random);
	MD[31:0] <= $unsigned($random);
	MD[63:32] <= $unsigned($random);
	MD[95:64] <= $unsigned($random);
	MD[127:96] <= $unsigned($random);
	MD[159:128] <= $unsigned($random);
	MD[181:160] <= $unsigned($random);
	MD[213:182] <= $unsigned($random);
	MD[255:214] <= $unsigned($random);
	#10
	RST = 1'b0;
	call = 1'b1;
	#10
	call = 1'b0;
	#100
	suc = 1'b1;
	#10
	suc = 1'b0;
	#50 $stop;
	end
		
endmodule
