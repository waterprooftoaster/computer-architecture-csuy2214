`include "four_bit_adder.v"

module alu(A, B, Op, S);

   input [3:0]  A;
   input [3:0]  B;
   input [2:0]  Op;

   output [3:0]sum;
   output [3:0] S;

   wire [3:0] temp_B;
   wire carry_out;

   assign temp_B = Op[2] ? ~B : B;

   four_bit_adder adder_instance(
      A, 
      temp_B, 
      Op[2], 
      sum, 
      carry_out
   );

   assign S = Op[1] ? (Op[0] ? ( {3'b000,sum[3]}) : (sum)) :  (Op[0] ? (A | temp_B ): (A & temp_B)); 
   
endmodule