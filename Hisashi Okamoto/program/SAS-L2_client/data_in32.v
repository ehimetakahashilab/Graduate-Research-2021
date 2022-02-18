module data_in32(CLK, rst, st, DI0, DI1, DI2, DI3, DI4, DI5, DI6, DI7, PDO);
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
	
	reg [255:0] PDO;
	
	always @ ( posedge CLK )
		begin
			if( rst == 1'b1 )
			begin
				PDO <= 256'b0;
			end
			else if( st == 3'b001 )
			begin
				PDO[31:0] <= DI0;
				PDO[63:32] <= DI1;
				PDO[95:64] <= DI2;
				PDO[127:96] <= DI3;
				PDO[159:128] <= DI4;
				PDO[191:160] <= DI5;
				PDO[223:192] <= DI6;
				PDO[255:224] <= DI7;
			end
		end

endmodule