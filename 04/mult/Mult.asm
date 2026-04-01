// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Check if either operand is zero
@R2
M=0

@R0
D=M
@END
D;JEQ

@R1
D=M
@END
D;JEQ

//Test values of operands
@R0
D=M
@R1
D=D-M
@SWAP
D;JGT

@LOOP
0;JMP

//Swap values
(SWAP)
@R0
D=M
@R2
M=D
@R1
D=M
@R0
M=D
@R2
D=M
@R1
M=D
@R2
M=0


//Loop
(LOOP)
@R2
D=M
@R1
D=D+M
@R2
M=D
@R0
M=M-1
D=M
@LOOP
D;JGT


(END)
@END
0;JMP