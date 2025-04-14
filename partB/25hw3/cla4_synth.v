/////////////////////////////////////////////////////////////
// Created by: Synopsys DC Expert(TM) in wire load mode
// Version   : T-2022.03-SP2
// Date      : Fri Apr 14 23:04:21 2023
/////////////////////////////////////////////////////////////


module HA_0 ( a, b, sum, cout, clk );
  input a, b, clk;
  output sum, cout;
  wire   N0, N1;

  DFFPOSX1 sum_reg ( .D(N0), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N1), .CLK(clk), .Q(cout) );
  XOR2X1 U4 ( .A(b), .B(a), .Y(N0) );
  AND2X1 U3 ( .A(b), .B(a), .Y(N1) );
endmodule


module HA_7 ( a, b, sum, cout, clk );
  input a, b, clk;
  output sum, cout;
  wire   N0, N1;

  DFFPOSX1 sum_reg ( .D(N0), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N1), .CLK(clk), .Q(cout) );
  XOR2X1 U4 ( .A(b), .B(a), .Y(N0) );
  AND2X1 U3 ( .A(b), .B(a), .Y(N1) );
endmodule


module FA_0 ( a, b, cin, sum, cout, clk );
  input a, b, cin, clk;
  output sum, cout;
  wire   ab_sum, ab_cout, abc_sum, abc_cout, N0;

  HA_0 ha_a_b ( .a(a), .b(b), .sum(ab_sum), .cout(ab_cout), .clk(clk) );
  HA_7 ha_ab_c ( .a(ab_sum), .b(cin), .sum(abc_sum), .cout(abc_cout), .clk(clk) );
  DFFPOSX1 sum_reg ( .D(abc_sum), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N0), .CLK(clk), .Q(cout) );
  OR2X1 U3 ( .A(ab_cout), .B(abc_cout), .Y(N0) );
endmodule


module HA_1 ( a, b, sum, cout, clk );
  input a, b, clk;
  output sum, cout;
  wire   N0, N1;

  DFFPOSX1 sum_reg ( .D(N0), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N1), .CLK(clk), .Q(cout) );
  XOR2X1 U4 ( .A(b), .B(a), .Y(N0) );
  AND2X1 U3 ( .A(b), .B(a), .Y(N1) );
endmodule


module HA_2 ( a, b, sum, cout, clk );
  input a, b, clk;
  output sum, cout;
  wire   N0, N1;

  DFFPOSX1 sum_reg ( .D(N0), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N1), .CLK(clk), .Q(cout) );
  XOR2X1 U4 ( .A(b), .B(a), .Y(N0) );
  AND2X1 U3 ( .A(b), .B(a), .Y(N1) );
endmodule


module FA_1 ( a, b, cin, sum, cout, clk );
  input a, b, cin, clk;
  output sum, cout;
  wire   ab_sum, ab_cout, abc_sum, abc_cout, N0;

  HA_2 ha_a_b ( .a(a), .b(b), .sum(ab_sum), .cout(ab_cout), .clk(clk) );
  HA_1 ha_ab_c ( .a(ab_sum), .b(cin), .sum(abc_sum), .cout(abc_cout), .clk(clk) );
  DFFPOSX1 sum_reg ( .D(abc_sum), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N0), .CLK(clk), .Q(cout) );
  OR2X1 U3 ( .A(ab_cout), .B(abc_cout), .Y(N0) );
endmodule


module HA_3 ( a, b, sum, cout, clk );
  input a, b, clk;
  output sum, cout;
  wire   N0, N1;

  DFFPOSX1 sum_reg ( .D(N0), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N1), .CLK(clk), .Q(cout) );
  XOR2X1 U4 ( .A(b), .B(a), .Y(N0) );
  AND2X1 U3 ( .A(b), .B(a), .Y(N1) );
endmodule


module HA_4 ( a, b, sum, cout, clk );
  input a, b, clk;
  output sum, cout;
  wire   N0, N1;

  DFFPOSX1 sum_reg ( .D(N0), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N1), .CLK(clk), .Q(cout) );
  XOR2X1 U4 ( .A(b), .B(a), .Y(N0) );
  AND2X1 U3 ( .A(b), .B(a), .Y(N1) );
endmodule


module FA_2 ( a, b, cin, sum, cout, clk );
  input a, b, cin, clk;
  output sum, cout;
  wire   ab_sum, ab_cout, abc_sum, abc_cout, N0;

  HA_4 ha_a_b ( .a(a), .b(b), .sum(ab_sum), .cout(ab_cout), .clk(clk) );
  HA_3 ha_ab_c ( .a(ab_sum), .b(cin), .sum(abc_sum), .cout(abc_cout), .clk(clk) );
  DFFPOSX1 sum_reg ( .D(abc_sum), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N0), .CLK(clk), .Q(cout) );
  OR2X1 U3 ( .A(ab_cout), .B(abc_cout), .Y(N0) );
endmodule


module HA_5 ( a, b, sum, cout, clk );
  input a, b, clk;
  output sum, cout;
  wire   N0, N1;

  DFFPOSX1 sum_reg ( .D(N0), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N1), .CLK(clk), .Q(cout) );
  XOR2X1 U4 ( .A(b), .B(a), .Y(N0) );
  AND2X1 U3 ( .A(b), .B(a), .Y(N1) );
endmodule


module HA_6 ( a, b, sum, cout, clk );
  input a, b, clk;
  output sum, cout;
  wire   N0, N1;

  DFFPOSX1 sum_reg ( .D(N0), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N1), .CLK(clk), .Q(cout) );
  XOR2X1 U4 ( .A(b), .B(a), .Y(N0) );
  AND2X1 U3 ( .A(b), .B(a), .Y(N1) );
endmodule


module FA_3 ( a, b, cin, sum, cout, clk );
  input a, b, cin, clk;
  output sum, cout;
  wire   ab_sum, ab_cout, abc_sum, abc_cout, N0;

  HA_6 ha_a_b ( .a(a), .b(b), .sum(ab_sum), .cout(ab_cout), .clk(clk) );
  HA_5 ha_ab_c ( .a(ab_sum), .b(cin), .sum(abc_sum), .cout(abc_cout), .clk(clk) );
  DFFPOSX1 sum_reg ( .D(abc_sum), .CLK(clk), .Q(sum) );
  DFFPOSX1 cout_reg ( .D(N0), .CLK(clk), .Q(cout) );
  OR2X1 U3 ( .A(ab_cout), .B(abc_cout), .Y(N0) );
endmodule


module cla4 ( a, b, cin, sum, cout, clk );
  input [3:0] a;
  input [3:0] b;
  output [3:0] sum;
  input cin, clk;
  output cout;
  wire   adder0_sum, adder0_cout, adder1_sum, adder1_cout, adder2_sum,
         adder2_cout, adder3_sum, adder3_cout;

  FA_0 adder0 ( .a(a[0]), .b(b[0]), .cin(cin), .sum(adder0_sum), .cout(
        adder0_cout), .clk(clk) );
  FA_3 adder1 ( .a(a[1]), .b(b[1]), .cin(adder0_cout), .sum(adder1_sum), 
        .cout(adder1_cout), .clk(clk) );
  FA_2 adder2 ( .a(a[2]), .b(b[2]), .cin(adder1_cout), .sum(adder2_sum), 
        .cout(adder2_cout), .clk(clk) );
  FA_1 adder3 ( .a(a[3]), .b(b[3]), .cin(adder2_cout), .sum(adder3_sum), 
        .cout(adder3_cout), .clk(clk) );
  DFFPOSX1 \sum_reg[3]  ( .D(adder3_sum), .CLK(clk), .Q(sum[3]) );
  DFFPOSX1 \sum_reg[2]  ( .D(adder2_sum), .CLK(clk), .Q(sum[2]) );
  DFFPOSX1 \sum_reg[1]  ( .D(adder1_sum), .CLK(clk), .Q(sum[1]) );
  DFFPOSX1 \sum_reg[0]  ( .D(adder0_sum), .CLK(clk), .Q(sum[0]) );
  DFFPOSX1 cout_reg ( .D(adder3_cout), .CLK(clk), .Q(cout) );
endmodule

