module addertest;

 reg [3:0] A, B;
 reg C0;

 wire [3:0] S;
 wire C4;

 ripple_carry_4_bit_adder AT (S, C4, A, B, C0);

 initial
  begin

   C0 = 1'b0;
   A = 4'b0000;
   B = 4'b0011;

   repeat (15)
     #10 A = A + 1'b1;

   #10
     C0 = 1'b0;
     A = 4'b0011;
     B = 4'b0000;

   repeat (15)
     #10 B = B + 1'b1;
     
     
    #10
      C0 = 1'b1;
      A = 4'b0000;
      B = 4'b0011;
    
    repeat (15)
     #10 A = A + 1'b1;
 
      
    #10
      C0 = 1'b1;
      A = 4'b0011;
      B = 4'b0000;      
      
    repeat (15);
     #10 B = B + 1'b1;
       
  end

 initial
   $monitor("A = %4b, B = %4b, C0 = %b, S = %4b, C4 = %b", A, B, C0, S, C4);

endmodule