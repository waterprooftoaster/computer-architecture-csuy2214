`timescale 1ns/100ps
`default_nettype none

`include "alu.v"

module alu_tb();

   reg signed [3:0]  A = 4'b0;
   reg signed [3:0]  B = 4'b0;

   reg[2:0]        Op = 3'b0;

   wire [3:0] S_expected;
   wire [3:0] S_actual;

   task check;
      input [2:0] op;
      input signed [3:0] a;
      input signed [3:0] b;
      input signed [3:0] actual;
      input signed [3:0] expected;
      if (expected !== actual) begin 
         $display("When %s %d and %d, got %d, but expected %d",
         (op==3'b000)?"anding":
         (op==3'b001)?"oring":
         (op==3'b010)?"adding":
         (op==3'b100)?"anding(not)":
         (op==3'b101)?"oring(not)":
         (op==3'b110)?"subtracting":
         (op==3'b111)?"slt":"?", a,b,actual,expected); 
      end
   endtask

   task test;
      input[2:0] op;
      input signed [3:0] a;
      input signed [3:0] b;
      begin
         #20 A <= a; B <= b; Op <= op;
         #20 check(op, a, b, S_actual, S_expected);

         // $display("A=%d B=%d Op=%d Sactual=%d Sexpected=%d",a,b,op,S_actual,S_expected);

      end
   endtask
   
   assign {S_expected} = (Op==3'b000) ? A & B : 
                         (Op==3'b001) ? A | B :
                         (Op==3'b010) ? A + B :
                         (Op==3'b100) ? A & ~B :
                         (Op==3'b101) ? A | ~B :
                         (Op==3'b110) ? A - B :
                         (Op==3'b111) ? A < B : 0;

   alu my_alu(A, B, Op, S_actual);

   integer ia = 0;
   integer ib = 0;
   integer iop = 0;

   initial begin

      $dumpfile("alu_tb.vcd");
      $dumpvars(0, alu_tb);

      for (iop=0; iop<=3'b111; iop=iop+1)
        if (iop!=3'b011)
          for (ia=-8; ia<=7; ia=ia+1)
            for (ib=-8; ib<=7; ib=ib+1)
               if (!(iop==3'b111 && (ia-ib>7 || ia-ib<-8)))
                  #20 test(iop, ia, ib); 

      #20 $finish;

   end

endmodule
