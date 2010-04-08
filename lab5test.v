module test;
  reg a;
  reg b;
  reg z;
  wire [7:0] S;
  
  decoder_3x8_gates decode(S,a,b,z);
  
  initial
  begin
  $display("Z  A  B  S time");
  $monitor("%b  %b  %b  %b  %d", z, a, b, S, $time);
  end
  
  initial
  begin
  
       z=1'b0; a=1'b0; b=1'b0;
   #10 z=1'b0; a=1'b0; b=1'b1;
   #10 z=1'b0; a=1'b1; b=1'b0;
  
   #10 z=1'b0; a=1'b1; b=1'b1;
   #10 z=1'b1; a=1'b0; b=1'b0;

   #10 z=1'b1; a=1'b0; b=1'b1;
   #10 z=1'b1; a=1'b1; b=1'b0;
   #10 z=1'b1; a=1'b1; b=1'b1;
   #10 $finish;
  
  end
endmodule