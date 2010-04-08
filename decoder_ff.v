//decoder.v

module decoder_df (
  output [3:0] D,
  input [1:0] A, enable
);

 assign  D[0] = ~(~A[1] & ~A[0] & ~enable),
         D[1] = ~(~A[1] & A[0] & ~enable),
         D[2] = ~(A[1] & ~A[0] &~enable),
         D[3] = ~(A[1] & A[0] & ~enable);

endmodule
