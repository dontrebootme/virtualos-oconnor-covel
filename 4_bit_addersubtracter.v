module half_adder (S, C, x, y);
 output S, C;
 input x,y;

 xor (S, x, y);
 and (C, x, y);

endmodule

module full_adder (S, C, x, y, z);
 output S, C;
 input x, y, z;
 wire S1, C1, C2;

 half_adder HA1 (S1, C1, x, y);
 half_adder HA2 (S, C2, S1, z);
 or G1 (C, C2, C1);

endmodule

module ripple_carry_4_bit_adder ( Sum, C4, A, B, C0);
 output [3: 0] Sum;
 output C4;
 input [3: 0] A, B;
 input C0;
 
 wire	C1, C2, C3;

 full_adder	FA0 (Sum[0], C1, A[0], B[0], C0), 
    FA1 (Sum[1], C2, A[1], B[1], C1),
		FA2 (Sum[2], C3, A[2], B[2], C2),
		FA3 (Sum[3], C4, A[3], B[3], C3);
    
endmodule

//module four_bit_alu(S, Cout, A, B, M, Cen);
//  input M, Cen;
//  input [3:0] A, B;
//  output Cout;
//  output [3:0] S;
//  wire C0, C1, C2; // Intermediate carries
//  wire [3:0] T; // XOR outputs
  
//  xor    X0(T[0], M, B[0]),
//         X1(T[1], M, B[1]),
//         X2(T[2], M, B[2]),
//         X3(T[3], M, B[3]);
         
  // insert AND gates and Full Adders here
  
  // These are the four AND gates required by Lab4
//  and a1( );
//  and a2(C0 );
//  and a3(C1 );
//  and a4(C2 );

  // These are our four FULL ADDERS required by Lab4
//  full_adder FA0

//endmodule

module four_bit_adder_subtracter(S,Cout,A,B,M, Cen);
 input M;
 input [3:0] A, B;
 input Cen;
 output Cout;
 output [3:0] S;
 wire C0,C1,C2;
 wire v0,v1,v2,v3;
 
 wire [3:0] T;
 
 xor    X0(T[0], M, B[0]),
        X1(T[1], M, B[1]),
        X2(T[2], M, B[2]),
        X3(T[3], M, B[3]);
        
and  a0(v0, M, Cen),
     a1(v1, C0, Cen),
     a2(v2, C1, Cen),
     a3(v3, C2, Cen);

 full_adder  FA0 (S[0],c0,A[0],T[0], v0),
             FA1 (S[1],c1,A[1],T[1],v1),
             FA2 (S[2],C2,A[2],T[2],v2),
             FA3 (S[3],Cout,A[3],T[3], v3);
endmodule
                      