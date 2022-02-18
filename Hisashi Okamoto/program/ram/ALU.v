module ALU(CLK, RST, call, suc, PD, MD, RES, st, rst, rdwr);//,adr);
	input CLK;					/* システムクロック */
	input RST;					/* リセット信号 */
	input call;
	input suc;
//	input [2:0] st;			/* 現在の状態 */
	input [255:0] PD;			/* シフトレジスタからのデータ(α) */
	input [255:0] MD;			/* メモリからのデータ(An, Mn) */
	output [255:0] RES;		/* 演算結果 */
	output [2:0] st;		/* FSMへのレスポンス */
	output rst;
	output rdwr;				/* rdwr = 0:メモリからの読み出し,rdwr = 1:メモリへの書き込み */
//	output adr ;  /* adr = 0:An, 1:Mn */
	
	
	reg [2:0] st;
//	reg adr;
	reg [255:0] reg1, reg2, reg3, reg4;
	
	parameter
		IDLE = 3'b000,		/* 待機 */
		RECV = 3'b001,		/* α受信 */
		NGEN = 3'b010,		/* N = α XOR An */
		AGEN = 3'b011,		/* An+1 = N XOR Mn */
		BGEN = 3'b100,		/* β生成 */
		SEND = 3'b101,		/* β送信 */
		MUPD = 3'b110,		/* Mn更新 */
		AUPD = 3'b111;		/* An更新 */
	
	assign rst = ( RST == 1'b1 ) ? 1'b1 : 1'b0;
	
	assign rdwr = ( st == 3'b110 ) ? 1'b1 :
						( st == 3'b111 ) ? 1'b1 : 1'b0;
						
	assign RES = reg1;
	
	always @ ( posedge CLK )
		begin
			if ( RST == 1'b1 )
			begin
				reg1 <= 256'b0;
				reg2 <= 256'b0;
				reg3 <= 256'b0;
				reg4 <= 256'b0;
				st <= IDLE;
			end
			else	begin
		
			case (st)
				IDLE: /* st = 3'b000 */
						if( call == 1'b1 ) 
						begin 
							st <= RECV;		
//							adr <= 1'b1;		
						end
						else if( suc == 1'b1 ) st <= MUPD;
						else st <= IDLE;
				RECV:/* st = 3'b001: reg1 = α */
							begin
								reg1 <= PD;		/* reg1 = α */
//								adr <= 1'b0;
								st <= NGEN;
							end							
				NGEN: /* st = 3'b010: N = α XOR An */
							begin
								reg1 <= (MD ^ reg1);
								reg2=MD; //reg2=An
//								adr <= 1'b0;
								st <= AGEN;
							end
				AGEN: /* st = 3'b011: An+1 = N XOR Mn */
							begin
								reg1 <= (MD ^ reg1); 	/* reg1 = An+1 */
								reg3 <= MD; //reg3=Mn
//								adr <= 1'b0;
								st <= BGEN;
								
							end
				BGEN: /* st = 3'b100: β = An+1 + An */
							begin
								reg1 <= reg1 + MD;
								reg4 <= reg1; //An+1
//								adr <= 1'b1;
								st <= SEND;	
							end
				SEND: /* st = 3'b101:  */ 
							begin
//								adr <= 1'b0;
								st <= IDLE;
							end						
				MUPD:	/* st = 3'b110: Mn+1 = An + Mn */
							begin
							reg1 <= reg2 + reg3; //reg1=An + Mn
//								adr <= 1'b0;
								st <= AUPD;

							end
				AUPD: /* st = 3'b111: An更新 */
							begin
							reg1 <= reg4;
//								adr <= 1'b0;
								st <= IDLE;
							end
				default: st <= IDLE;
				endcase
			end
		end
endmodule