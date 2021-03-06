// fp_lt_arria10.v

// Generated using ACDS version 16.0 211

`timescale 1 ps / 1 ps
module fp_lt_arria10 (
		input  wire [31:0] a,      //      a.a
		input  wire        areset, // areset.reset
		input  wire [31:0] b,      //      b.b
		input  wire        clk,    //    clk.clk
		output wire [0:0]  q       //      q.q
	);

	fp_lt_arria10_altera_fp_functions_160_wduzk3i fp_functions_0 (
		.clk    (clk),    //    clk.clk
		.areset (areset), // areset.reset
		.a      (a),      //      a.a
		.b      (b),      //      b.b
		.q      (q)       //      q.q
	);

endmodule
