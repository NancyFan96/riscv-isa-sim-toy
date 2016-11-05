//
//  register.hpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef register_hpp
#define register_hpp

#ifndef stdio
#define stdio
#include <stdio.h>
#endif

#ifndef system_h
#define system_h
#include "system.h"
#endif

#endif /* register_hpp */

/* ------- define registers file ------- */
#define zero 0  //	rrx[0]    zero    Hard-wired zero
#define ra 1    //	rrx[1]    ra      Return address
#define s0 2    //	rrx[2]    s0/fp   Saved register/frame pointer
#define fp 2
#define s1 3    //	rrx[3]    s1      Saved registers
#define s2 4    //	rrx[4]    s2      ..
#define s3 5    //	rrx[5]    s3      ..
#define s4 6    //	rrx[6]    s4      ..
#define s5 7    //	rrx[7]    s5      ..
#define s6 8    //	rrx[8]    s6      ..
#define s7 9    //	rrx[9]    s7      ..
#define s8 10   //	rrx[10]   s8      ..
#define s9 11   //	rrx[11]   s9      ..
#define s10 12  //	rrx[12]   s10     ..
#define s11 13  //	rrx[13]   s11     ..
#define sp 14   //	rrx[14]   sp      Stack pointer
#define tp 15   //	rrx[15]   tp      Thread pointer
#define v0 16   //	rrx[16]   v0      Return values
#define v1 17   //	rrx[17]   v1      ..
#define a0 18   //	rrx[18]   a0      Function arguments
#define a1 19   //	rrx[19]   a1      ..
#define a2 20   //	rrx[20]   a2      ..
#define a3 21   //	rrx[21]   a3      ..
#define a4 22   //	rrx[22]   a4      ..
#define a5 23   //	rrx[23]   a5      ..
#define a6 24   //	rrx[24]   a6      ..
#define a7 25   //	rrx[25]   a7      ..
#define t0 26   //	rrx[26]   t0      Temporaries
#define t1 27   //	rrx[27]   t1      ..
#define t2 28   //	rrx[28]   t2      ..
#define t3 29   //	rrx[29]   t3      ..
#define t4 30   //	rrx[30]   t4      ..
#define gp 31   //	rrx[31]   gp      Global pointer


class registers {
public:
    byte* PC;
    reg64 rrx[32];
    
public:
    registers();
    void readReg();          // for debug                      //
    reg64 readReg(regID regDst);                    //
    bool writeReg(regID regDst, reg64 value);       // if success return true, else return false
    //bool writeReg(regID regDst, regID regSrc);      // if success return true, else return false
    reg32 getPC();                                  //
    bool setPC(reg32 newPC);                        // if success return true, else return false
    void incPC();                                   // PC to next instruction
};
/* ------- END define registers file ------- */

static registers sim_regs;

