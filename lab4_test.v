// name:  test
// desc:  test four bit ripple carry adder/subtracter
// date:  10/16/08
//   by:  Chad Cordero, Timothy Covel, Aaron Smith

module test;
  reg [3:0] a;
  reg [3:0] b;
  reg m;
  reg Cen;
  wire s;
  wire c4;

  // create instance of adder
  four_bit_adder_subtracter add_sub(s, c4, a, b, m, Cen);

  // set up the monitoring

  initial
  begin
    $display("A      B     M     Cen   C4    S     Time");
    $monitor("%b  %b   %b      %b      %b      %b  %d", a, b, m, Cen, c4, s, $time);
  end

  // run through a series of numbers
  initial
  begin
        a=4'b0100; b=4'b0000; m=1'b1; Cen=1'b1;
    #10 a=4'b0100; b=4'b0011; m=1'b1; Cen=1'b1;
    #10 a=4'b1100; b=4'b0011; m=1'b1; Cen=1'b1;
    
    #10 a=4'b0100; b=4'b0000; m=1'b0; Cen=1'b1;
    #10 a=4'b0100; b=4'b0011; m=1'b0; Cen=1'b1;
    #10 a=4'b0100; b=4'b0011; m=1'b0; Cen=1'b1;

    #10 a=4'b0100; b=4'b0000; m=1'b0; Cen=1'b0;
    #10 a=4'b0100; b=4'b0011; m=1'b0; Cen=1'b0;
    #10 a=4'b1100; b=4'b0011; m=1'b0; Cen=1'b0;
    
    #10 a=4'b0100; b=4'b0000; m=1'b1; Cen=1'b0;
    #10 a=4'b0100; b=4'b0011; m=1'b1; Cen=1'b0;
    #10 a=4'b1100; b=4'b0011; m=1'b1; Cen=1'b0;
    #10 $finish;
  end
endmodule


