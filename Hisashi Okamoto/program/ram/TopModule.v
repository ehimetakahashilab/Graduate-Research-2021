//module TopModule(CLK, RST, call, suc, PD, q, RES, st, adr, rw);
//	input CLK;
//	input RST;
//	input	call;
//	input suc;
//	input [255:0] PD;
//	output [255:0] q;
//	output [255:0] RES;
//	output [2:0] st;
//	output adr;
//	output rw;
//	
//	wire rst;
//	wire [2:0] w_st;
//	wire w_adr;
//	wire [255:0] RES;
//	wire [255:0] w_MD;
//	wire rdwr;
//	
//	assign adr = w_adr;
//	assign st = w_st;
//	assign rw = rdwr;
	
//	ALU u0(.CLK(CLK), .RST(RST), .call(call), .suc(suc), .PD(PD), .MD(q), .RES(RES), .st(w_st), .rst(rst), .rdwr(rdwr));
//	adr_reg u1(.st(w_st), .adr(w_adr));
module TopModule(CLK, PD, rdwr, st, q);
	input CLK;
	input [255:0] PD;
	input rdwr;
	input [2:0] st;
	output [255:0] q;
	
	wire adr;
	
	adr_reg u1(.st(st), .adr(adr));
	ram u2(.address(adr), .clock(CLK), .data(PD), .rden(~rdwr), .wren(rdwr), .q(q));
	
endmodule
