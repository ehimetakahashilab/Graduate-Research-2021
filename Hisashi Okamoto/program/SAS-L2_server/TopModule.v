module TopModule(CLK);
	input CLK;		/* システムクロック */
	
	wire RST;
	wire [31:0] DI0;			/* データ入力(α32bit) */
	wire [31:0] DI1;			/* データ入力(α32bit) */
	wire [31:0] DI2;			/* データ入力(α32bit) */
	wire [31:0] DI3;			/* データ入力(α32bit) */
	wire [31:0] DI4;			/* データ入力(α32bit) */
	wire [31:0] DI5;			/* データ入力(α32bit) */
	wire [31:0] DI6;			/* データ入力(α32bit) */
	wire [31:0] DI7;			/* データ入力(α32bit) */
	wire call;
	wire suc;
	wire [31:0] DO0;			/* データ出力(β32bit) */
	wire [31:0] DO1;			/* データ出力(β32bit) */
	wire [31:0] DO2;			/* データ出力(β32bit) */
	wire [31:0] DO3;			/* データ出力(β32bit) */
	wire [31:0] DO4;			/* データ出力(β32bit) */
	wire [31:0] DO5;			/* データ出力(β32bit) */
	wire [31:0] DO6;			/* データ出力(β32bit) */
	wire [31:0] DO7;			/* データ出力(β32bit) */
	wire [2:0] st;
	
    SASL2 u0 (
        .call_external_connection_export (call), // call_external_connection.export
        .clk_clk                         (CLK),                         //                      clk.clk
        .di_external_connection_export   (DI0),   //   di_external_connection.export
        .do_external_connection_export   (DO0),   //   do_external_connection.export
        .reset_reset_n                   (1'b1),                   //                    reset.reset_n
        .rst_external_connection_export  (RST),  //  rst_external_connection.export
        .st_external_connection_export   (st),   //   st_external_connection.export
        .suc_external_connection_export  (suc),  //  suc_external_connection.export
        .di7_external_connection_export  (DI7),  //  di7_external_connection.export
        .di6_external_connection_export  (DI6),  //  di6_external_connection.export
        .di5_external_connection_export  (DI5),  //  di5_external_connection.export
        .di4_external_connection_export  (DI4),  //  di4_external_connection.export
        .di3_external_connection_export  (DI3),  //  di3_external_connection.export
        .di2_external_connection_export  (DI2),  //  di2_external_connection.export
        .di1_external_connection_export  (DI1),  //  di1_external_connection.export
        .do7_external_connection_export  (DO7),  //  do7_external_connection.export
        .do6_external_connection_export  (DO6),  //  do6_external_connection.export
        .do5_external_connection_export  (DO5),  //  do5_external_connection.export
        .do4_external_connection_export  (DO4),  //  do4_external_connection.export
        .do3_external_connection_export  (DO3),  //  do3_external_connection.export
        .do2_external_connection_export  (DO2),  //  do2_external_connection.export
        .do1_external_connection_export  (DO1)   //  do1_external_connection.export
    );



	SAS_L2_client u1(.CLK(CLK), .RST(RST), .DI0(DI0), .DI1(DI1), .DI2(DI2), .DI3(DI3), .DI4(DI4), .DI5(DI5), .DI6(DI6), .DI7(DI7), .call(call), .suc(suc), .DO0(DO0), .DO1(DO1), .DO2(DO2), .DO3(DO3), .DO4(DO4), .DO5(DO5), .DO6(DO6), .DO7(DO7), .st(st));
	
endmodule