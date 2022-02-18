module data_out32(CLK, rst, st, PDI, DO0, DO1, DO2, DO3, DO4, DO5, DO6, DO7);
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
	
	reg [255:0] data;
	
	assign DO0 = data[31:0];
	assign DO1 = data[63:32];
	assign DO2 = data[95:64];
	assign DO3 = data[127:96];
	assign DO4 = data[159:128];
	assign DO5 = data[191:160];
	assign DO6 = data[223:192];
	assign DO7 = data[255:224];
	
	always @ ( posedge CLK )
		begin
			if( rst == 1'b1 )
			begin
				data <= 256'b0;
			end
			else if( st == 3'b101 )
			begin
				data <= PDI;
			end
		end

endmodule