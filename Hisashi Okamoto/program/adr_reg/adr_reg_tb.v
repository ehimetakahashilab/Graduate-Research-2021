`timescale 1ns / 1ns
module adr_reg_tb; 
 
  wire    adr;
  reg  [2:0]  st;
  
  TopModule  DUT(.adr(adr) ,.st (st)); 



// "Repeater Pattern" Repeat 1 times
// Start Time = 0 ns, End Time = 300 ns, Period = 50 ns
  initial
  begin
		st = 3'b000;
		#50	st = 3'b001;
	   #50	st = 3'b010;
	   #50   st = 3'b011;
	   #50   st = 3'b100;
	   #50   st = 3'b101;
	   #50   st = 3'b110;
	   #50   st = 3'b111;
	   #50;
// 300 ns, repeat pattern in loop.
  end

  initial
	#600 $stop;
endmodule
