//
//  system.h
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef system_h
#define system_h
#endif /* system_h */

//THIS IS RISCV R64IFM

#define RRXLEN 64		// the rrx registers are all 64 bits wide


typedef unsigned long int	reg64;
typedef unsigned long int	regPC;
typedef unsigned int	 	reg32;
typedef unsigned char       regID;
typedef unsigned int 		ins;
typedef long int      imm;
typedef unsigned char xcode;
typedef unsigned char insType;

typedef unsigned char byte;

/* ------- define registers file ------- *
 *	rrx[0]    zero    Hard-wired zero
 *	rrx[1]    ra      Return address
 *	rrx[2]    s0/fp   Saved register/frame pointer
 *	rrx[3]    s1      Saved registers
 *	rrx[4]    s2      ..
 *	rrx[5]    s3      ..
 *	rrx[6]    s4      ..
 *	rrx[7]    s5      ..
 *	rrx[8]    s6      ..
 *	rrx[9]    s7      ..
 *	rrx[10]   s8      ..
 *	rrx[11]   s9      ..
 *	rrx[12]   s10     ..
 *	rrx[13]   s11     ..
 *	rrx[14]   sp      Stack pointer
 *	rrx[15]   tp      Thread pointer
 *	rrx[16]   v0      Return values
 *	rrx[17]   v1      ..
 *	rrx[18]   a0      Function arguments
 *	rrx[19]   a1      ..
 *	rrx[20]   a2      ..
 *	rrx[21]   a3      ..
 *	rrx[22]   a4      ..
 *	rrx[23]   a5      ..
 *	rrx[24]   a6      ..
 *	rrx[25]   a7      ..
 *	rrx[26]   t0      Temporaries
 *	rrx[27]   t1      ..
 *	rrx[28]   t2      ..
 *	rrx[29]   t3      ..
 *	rrx[30]   t4      ..
 *	rrx[31]   gp      Global pointer
 */
class registers {
public:
    regPC PC;
    reg64 rrx[32];
    
public:
    reg64 readReg();
    reg64 readReg(regID regDst);
    bool writeReg(regID regDst, reg64 value);       // if success return true, else return false
    bool writeReg(regID regDst, regID regSrc);
    reg64 getPC();
    bool setPC(reg64 newPC);
};
/* ------- END define registers file ------- */

