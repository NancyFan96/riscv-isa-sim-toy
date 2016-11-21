//
//  decode.hpp
//  RISCV_ISA_SIM
//
//  Decode instructions, get meaningful part like rd, rs, imm
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu Wang  on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef decode_hpp
#define decode_hpp

#include <stdio.h>
#include "system.h"
#include "register.hpp"
#include "memory.hpp"


/* define some useful function to decode the instruction */
#define R_TYPE   0
#define I_TYPE   1
#define S_TYPE   2
#define SB_TYPE  3
#define U_TYPE   4
#define UJ_TYPE  5
#define R4_TYPE  6
#define SCALL    7


/*          create a binary MASK like
 *     value:  000... 00000111...1111000...000
 *  position:  31...       x  ...   y ...  210
 *  for all k if (31>=x>=k>=y>=0), bit(k) = 1,
 *                      otherwise, bit(k) = 0
 */
#define ONES(x,y)       (reg64) ((((unsigned long)1<<x)-1)+((unsigned long)1<<x) -(((unsigned long)1<<y)-1))
#define OPCODE          ONES(6,0)      // 7
#define FUNCT2          ONES(26,25)    // 2
#define FUNCT3          ONES(14,12)    // 3
#define FUNCT7          ONES(31,25)    // 7
#define RD              ONES(11,7)     // 5
#define RS1             ONES(19,15)    // 5
#define RS2             ONES(24,20)    // 5
#define RS3             ONES(31,27)    // 5
#define SHAMT           ONES(25,20)    // 6, RV64I
#define IMM_SIGN(inst)  ((inst>>31)&1) // sign bit of immediate



/* ------- RV64I BASE INTEGER INSTRUCTION SET -------*/
class instruction {
public:
    xcode opcode;           // inst[0-6]
    insType optype;
    byte tag;               // bit0 set if immediate is valid,
                            // bit1 set if func3 is valid,
                            // bit2 set if func7 is valid，
                            // bit3 set if rd is valid
                            // bit4 set if rs1 is valid
                            // bit5 set if rs2 is valid
                            // bit6 set if func2 is valid
                            // bit7 set if rs3 is valid
    imm immediate;
    xcode func2;            // inst[25-26]
    xcode func3;            // inst[12-14]
    xcode func7;            // inst[25-31]
    regID rd;               // inst[7-11]
    regID rs1;              // inst[15-19]
    regID rs2;              // inst[20-24]
    regID rs3;              // inst[27-31]
    
public:
    instruction();
    bool getType(ins inst);         // if success return true, else return false
    bool setIMM(ins inst);          // (set immediate) Notice need switch, AND BE CAREFUL OF IMM BIT ORDER
    bool decode(ins inst);          // set rx, (func3), (func7), (and call getIMM)
    
    // These gets are designed to avoid using invalid parts of instruction
    imm getImm();
    xcode getfunc3();
    xcode getfunc7();
    xcode getfunc2();
    regID getrd();
    regID getrs1();
    regID getrs2();
    regID getrs3();
    
    void execute();
    void execute_R64();
    void execute_R();
    void execute_I();
    void execute_SX();
    void execute_UX();
    void execute_O();
    void execute_R4();
    void execute_FExt();
    
    void print_ins(const char* inst_name, regID rd, regID rs1, regID rs2, regID rs3);
    void print_ins(const char* inst_name, regID rd, regID rs1, regID rs2);
    void print_ins(const char* inst_name, regID r1, regID r2, imm imm0);
    void print_ins(const char* inst_name, regID r1, regID r2);
    void print_ins(const char* inst_name, regID rx, imm imm0);
    void print_ins(const char* inst_name, regID rx);
    void print_ins(const char* inst_name);
};

/* ------- END define riscv instruction  ------- */

extern bool verbose;
extern bool debug;
extern bool IS_TO_EXIT;
extern registers sim_regs;
extern memory sim_mem;
extern long int COUNT_INS;
extern bool GDB_MODE;


#endif /* decode_hpp */

