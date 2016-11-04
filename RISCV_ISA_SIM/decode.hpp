//
//  decode.hpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef decode_hpp
#define decode_hpp

#include <stdio.h>
#include "system.h"

#endif /* decode_hpp */

/* define some useful function to decode the instruction */
#define R_TYPE   0
#define I_TYPE   1
#define S_TYPE   2
#define SB_TYPE  3
#define U_TYPE   4
#define UJ_TYPE  5

/*          create a binary MASK like           */
/*     value:  000... 00000111...1111000...000  */
/*  position:  31...       y  ...   x ...  210  */
/*  for all k if (31>=x>=k>=y>=0), bit(k) = 1,  */
/*                      otherwise, bit(k) = 0   */
#define ONES(x,y)       (ins) (((1<<(x+1))-1)-((1<<y)-1))

/* masks */                                          // bit LEN
#define OPCODE     ONES(6,0)      // 7
#define FUNCT3     ONES(14,12)    // 3
#define FUNCT7     ONES(31,25)    // 7
#define RD         ONES(11,7)     // 5
#define RS1        ONES(19,15)    // 5
#define RS2        ONES(24,20)    // 5
#define SHAMT      ONES(25,20)    // 6, RV64I
/*------------------------- END define useful functions --------------------------*/



/* ------- define riscv instruction  ------- */
/* ------- RV64I BASE INTEGER INSTRUCTION SET -------*/
class instruction {
public:
    ins inst;
    xcode opcode;           // inst[0-6]
    insType optype;
    byte tag;               // bit0 set if immediate is valid,
                            // bit1 set if func3 is valid,
                            // bit2 sit if func7 is valid
    xcode func3;            // inst[12-14]
    xcode func7;            // inst[25-31]
    imm immediate;
    union DECODE{
        struct insR{
            regID rd;       // inst[7-11]
            //xcode funct3;   // inst[12-14]
            regID rs1;      // inst[15-19]
            regID rs2;      // inst[20-24]
            //xcode funct7;   // inst[25-31]
        };
        struct insI{
            regID rd;       // inst[7-11]
            //xcode funct3;   // inst[12-14]
            regID rs1;      // inst[15-19]
            //imm immediate0;  // inst[20-31], imm[0-11]
        };
        struct insS{
            //imm immediate0;// inst[7-11], imm[0-4]
            //xcode funct3;   // inst[12-14]
            regID rs1;      // inst[15-19]
            regID rs2;      // inst[20-24]
            //imm immediate1; // inst[25-31], imm[5-11]
        };
        struct insSB{
            //imm immediate0;// inst[7-11], imm[11|0-4]
            //xcode funct3;   // inst[12-14]
            regID rs1;      // inst[15-19]
            regID rs2;      // inst[20-24]
            //imm immediate1; // inst[25-31],imm[5-10|12]
        };
        struct insU{
            regID rd;       // inst[7-11]
            //imm immediate0;  // inst[12-31], imm[12-31]
        };
        struct insUJ{
            regID rd;       // inst[7-11]
            //imm immediate0;  // inst[12-31], imm[12-19|11|1-10|20]
        };
    }insX;
public:
    instruction();
    bool getType();         // if success return true, else return false
    bool getIMM();          // (set immediate) Notice need swith, AND BE CAREFUL OF IMM BIT ORDER
    bool decode();          // set rx, (func3), (func7), (and call getIMM)
};
/* ------- END define riscv instruction  ------- */


