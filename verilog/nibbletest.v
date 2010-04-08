module testTc;
 reg [3:0] data_in;
 reg read_en, write_en;
 reg [1:0] read_add;
 reg [1:0] write_add;

 wire [3:0] data_out;

 Register_File NR1(data_out, data_in, read_add, read_en, write_add, write_en);

initial
 begin
  data_in = 4'b0001;
  read_add = 2'b00;
  write_add = 2'b00;
  read_en = 1;
  write_en = 1;
 end

initial
 begin
  #1 write_en = 0;
  #1 write_en = 1;
  
  #5 write_add = 2'b01;
     data_in = 4'b0010;

  #1 write_en = 0;
  #1 write_en = 1;

  #5 write_add = 2'b10;
     data_in = 4'b0100;

  #1 write_en = 0;
  #1 write_en = 1;

  #5 write_add = 2'b11;
     data_in = 4'b1000;

  #1 write_en = 0;
  #1 write_en = 1;

  #5 read_en = 1'b0;

  #5 read_add = 2'b00;
  #5 read_add = 2'b01;
  #5 read_add = 2'b10;
  #5 read_add = 2'b11;
 end

initial
 begin
 $monitor("data_in: %b  read_add: %b read_en: %b write_add: %b write_en: %b data_out: %b" , data_in, read_add, read_en, write_add, write_en, data_out);
 end

endmodule