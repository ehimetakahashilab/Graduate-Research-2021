module TopModule(st, adr);
input [2:0] st;
	output adr;			/* adr = 0:An, 1:Mn */
	
	reg data;
	
	assign adr = data;
	
	always @ ( st )
		begin
			case( st )
				3'b000: data = 1'b1;
				3'b001: data = 1'b0;
				3'b010: data = 1'b0;
				3'b011: data = 1'b0;
				3'b100: data = 1'b1;
				3'b101: data = 1'b0;
				3'b110: data = 1'b0;
				3'b111: data = 1'b0;
				default: data = 1'b0;
			endcase
		end
	
endmodule
