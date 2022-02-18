`timescale 1ns / 1ns
module ram_tb; 
	reg CLK;
//	reg RST;
//	reg call;
//	reg suc;
	reg [255:0] PD;
//	wire [255:0] q;
//	wire [255:0] RES;
//	wire [2:0] st;
//	wire adr;
//	wire rw;
	reg rdwr;
	reg [2:0] st;
	wire [255:0] q;
	
//	TopModule DUT(.CLK(CLK), .RST(RST), .call(call), .suc(suc), .PD(PD), .q(q), .RES(RES), .st(st), .adr(adr), .rw(rw));
	TopModule DUT(.CLK(CLK), .PD(PD), .rdwr(rdwr), .st(st), .q(q));
  
  /* CLK生成 */
	always begin #5 
		CLK = ~CLK;
	end
	
	initial begin
		CLK = 1'b0;
		rdwr = 1'b0;
		st = 3'b000;
		PD <= 256'b0;
		#50
		st = 3'b001;
		#50
		st = 3'b010;
		#50
		st = 3'b011;
		#50
		st = 3'b100;
		#50
		st = 3'b101;
		#50
		st = 3'b000;
		#50
		rdwr = 1'b1;
		st = 3'b110;
		#50
		st = 3'b111;
		#50
		rdwr = 1'b0;
		st = 3'b000;
		#50
		st = 3'b001;
	#100 $stop;
	end
	
//	initial begin
//		CLK = 1'b0;
//		RST= 1'b1;
//		call = 1'b0;
//		#10
//		RST = 1'b0;
////		PD <= 256'b1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111;
//		PD[31:0] <= $unsigned($random);
//		PD[63:32] <= $unsigned($random);
//		PD[95:64] <= $unsigned($random);
//		PD[127:96] <= $unsigned($random);
//		PD[159:128] <= $unsigned($random);
//		PD[191:160] <= $unsigned($random);
//		PD[223:192] <= $unsigned($random);
//		PD[255:224] <= $unsigned($random);
//		call = 1'b1;
//		#60
//		suc = 1'b1;
//		#50
//		RST = 1'b1;
//		#10
//		RST = 1'b0;
//		#50
//		call = 1'b1;
//		#10
//		call = 1'b0;
//	#100 $stop;
//	end
		
endmodule
