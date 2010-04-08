//nibblereg.v

module Nibble_Reg(data_out, data_in, load, out_en);
input [3:0] data_in;
input load, out_en;
output [3:0] data_out;

// wires between flip-flops and tri-state gates
wire [3:0] dff_out;

// instantiate tri-state gates to do output enable
bufif0 tri33(data_out[3], dff_out[3], out_en);
bufif0 tri22(data_out[2], dff_out[2], out_en);
bufif0 tri11(data_out[1], dff_out[1], out_en);
bufif0 tri00(data_out[0], dff_out[0], out_en);

//instantiate D flip-flops here
D_FF Reg_Bit_3(dff_out[3], data_in[3], load);
D_FF Reg_Bit_2(dff_out[2], data_in[2], load);
D_FF Reg_Bit_1(dff_out[1], data_in[1], load);
D_FF Reg_Bit_0(dff_out[0], data_in[0], load);

endmodule
