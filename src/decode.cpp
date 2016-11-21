//
//  decode.cpp
//  RISCV_ISA_SIM
//
//  Decode instructions, get meaningful part like rd, rs, imm
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu Wang  on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include <string.h>
#include <math.h>
#include "system.h"
#include "decode.hpp"
#include "register.hpp"
#include "memory.hpp"


instruction::instruction()
{
    opcode = 0;
    optype = 0;
    tag = 0;        //#important
    func3 = 0;
    func7 = 0;
    immediate = 0;
    rd = 0;
    rs1 = 0;
    rs2 = 0;
}
void instruction::print_ins(const char* inst_name, regID rd, regID rs1, regID rs2, regID rs3){
    // FMADD.D
    if(GDB_MODE)    printf("> ");
    printf("instruction:\t %s %d, %d, %d, %d\n", inst_name, rd, rs1, rs2, rs3);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}

void instruction::print_ins(const char* inst_name, regID rd, regID rs1, regID rs2){
    // 建议都另外写小函数识别计数 
    // 要补全decode.hpp里面的define， define里面.都改为_, 如FMADD.D 就是FMADD_D
    // 改一下system.h里面HOW_MANY_INSTS的值，是大致的指令总数
    // 在sim.cpp的“BYE”之前打印COUNTS表
    // 即可在--verbose下查看指令数统计结果
    // ADD, SLL, SLT, SLTU, XOR, SRL, OR, AND, SUB, SRA, MUL, MULH, MULHSH, MULHU, DIV, DIVU, REM, REMU, ADDW, SLLW, SRLW, SUBW, SRAW, MULW,DIVW, DIVUW, REMW, REMUW
    // FADD.S FADD.D FSUB.S FSUB.D FMUL.S FMUL.D FDIV.S FDIV.D FEQ.S, FLT.S, FLE.S FEQ.D, FLT.D, FLE.D
    if(GDB_MODE)    printf("> ");
    printf("instruction:\t %s %d, %d, %d\n", inst_name, rd, rs1, rs2);
    sim_regs.readReg();
    sim_regs.readFloatReg();
    
}
void instruction::print_ins(const char* inst_name, regID r1, regID r2, imm imm0){
    // JALR, LB, LH, LW, LBU, LHU, LWU, LD, ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADDIW, SLLIW, SRLIW, SRAIW, SB,SH,SW,SD, BEQ,BNE,BLT,BGE,BLTU,BGEU
    // FLW, FLD, FSW, FSD
    if(GDB_MODE)    printf("> ");
    printf("instruction:\t %s %d, %d, %ld\n", inst_name, r1, r2, imm0);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}
void instruction::print_ins(const char* inst_name, regID r1, regID r2){
    //FSGNJ.D FSGNJN.D FCVT.S.D FCVT.D.S FCVT.W.S FCVT.WU.S FCVT.W.D FCVT.WU.D FCVT.S.W FCVT.S.WU FCVT.D.W FCVT.D.WU FMV.X.D FMV.D.X
    if(GDB_MODE)    printf("> ");
    printf("instruction:\t %s %d, %d\n", inst_name, r1, r2);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}

void instruction::print_ins(const char* inst_name, regID rx, imm imm0){
    // LUI, AUIPC, JAL
    printf("instruction:\t %s %d, 0x%lx\n", inst_name, rx, imm0);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}
void instruction::print_ins(const char* inst_name, regID rx){
    printf("instruction:\t %s %d\n", inst_name, rx);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}
void instruction::print_ins(const char* inst_name){
    // READ ERITE GETTIMEOFDAY
    printf("instruction:\t %s\n", inst_name);
    sim_regs.readReg();
    sim_regs.readFloatReg();
    
}

bool instruction::getType(ins inst){
    if(inst == 0x73){
        optype = SCALL;
        tag = 0;
        return true;
    }
    opcode = inst&OPCODE;
    switch(opcode)
    {
        case 0x3B:      // b0111011
        case 0x33:      // b0110011
        case 0x53:      // b1010011 -- F extension
            optype =  R_TYPE;
            tag = 62;   // b111110 | 32+16+8+4+2
            return true;
            
        case 0x67:      // b1100111
        case 0x03:      // b0000011
        case 0x13:      // b0010011
        case 0x1B:
        case 0x07:      // -- F extension
            //case 0x73:      // b1110011
            optype =  I_TYPE;
            tag = 27;    // b011011 | 16+8+2+1
            return true;
            
        case 0x23:      // b0100011
        case 0x27:      // b0100111 -- F extension
            optype =  S_TYPE;
            tag = 51;    // b110011 | 32+16+2+1
            return true;
            
        case 0x63:      // b1100011
            optype =  SB_TYPE;
            tag = 51;    // b110011
            return true;
            
        case 0x37:      // b0110111
        case 0x17:      // b0010111
            optype =  U_TYPE;
            tag = 9;    // b001001 | 8+1
            return true;
            
        case 0x6F:      // b1101111
            optype =  UJ_TYPE;
            tag = 9;    // b001001
            return true;
        case 0x43:
        case 0x47:
        case 0x4B:
        case 0x4F:
            optype = R4_TYPE;
            tag = 250;     // b11111010
            return true;
        default:
            return false;
    }
}

// Notice all the extend is sign-extend
bool instruction::setIMM(ins inst){
    switch(optype)
    {
            /*R,R4,SCALL type
             no immediate*/
        case R_TYPE:
        case SCALL:
        case R4_TYPE:
            return true;
            /*I type
             31----------20 imm[0]~imm[11]
             shamt SRAI SRLI SLLI*/
        case I_TYPE:
            immediate = ((inst&ONES(31,20)) >> 20) | (IMM_SIGN(inst)*ONES(63, 11));
            return true;
            /*S type
             31-----25-------------11---7
             imm[11]~imm[5]        imm[4]~imm[0]*/
        case S_TYPE:
            immediate = ((inst&ONES(11,7)) >> 7) | ((inst&ONES(31,25)) >> 20) | (IMM_SIGN(inst)*ONES(63, 11));
            return true;
            /*SB type
             31 imm[12] 30----25 imm[10]~imm[5] 11----8 imm[4]~imm[1] 7 imm[11]*/
        case SB_TYPE:
            immediate = ((inst&ONES(11,8))>>7) | ((inst&ONES(30,25))>>20) | ((inst&ONES(7,7))<<4)| (IMM_SIGN(inst)*ONES(63, 12));
            return true;
            /*U type
             31----12 imm[31]~imm[12]*/
        case U_TYPE:
            immediate = (inst&ONES(31,12))| (IMM_SIGN(inst)*ONES(63, 31));
            return true;
            /*UJ type
             31 imm[20] 30----21 imm[10]~imm[1] 20 imm[11] 19----12 imm[19]~imm[12]*/
        case UJ_TYPE:
            immediate = ((inst&ONES(30,21))>>20) | ((inst&ONES(20,20))>>9) | (inst&ONES(19,12))| (IMM_SIGN(inst)*ONES(63, 20));
            return true;
        default:
            return false;
    }
}

bool instruction::decode(ins inst){
    if(getType(inst) != true || setIMM(inst) != true){
        printf("DECODE ERROR in FIRST STAGE! Oops!\n");
        return false;
    }
    switch(optype)
    {
        case SCALL:
            return true;
            /* rs2,rs1,rd,func7,func3*/
        case R4_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            func2 = ((inst&FUNCT2) >> 25);
            rd = ((inst&RD) >> 7);
            rs1 = ((inst&RS1) >> 15);
            rs2 = ((inst&RS2) >> 20);
            rs3 = ((inst&RS3) >> 27);
            return true;
        case R_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            func7 = ((inst&FUNCT7) >> 25);
            rd = ((inst&RD) >> 7);
            rs1 = ((inst&RS1) >> 15);
            rs2 = ((inst&RS2) >> 20);
            return true;
            /*rs1,func3,rd*/
        case I_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            rd = ((inst&RD) >> 7);
            rs1 = ((inst&RS1) >> 15);
            return true;
            /*rs2,rs1,func3*/
        case S_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            rs1 = ((inst&RS1) >> 15);
            rs2 = ((inst&RS2) >> 20);
            return true;
            /*rs2,rs1,func3*/
        case SB_TYPE:
            func3 = ((inst&FUNCT3) >> 12);
            rs1 = ((inst&RS1) >> 15);
            rs2 = ((inst&RS2) >> 20);
            return true;
            /*rd*/
        case U_TYPE:
            rd = ((inst&RD) >> 7);
            return true;
            /*rd*/
        case UJ_TYPE:
            rd = ((inst&RD) >> 7);
            return true;
        default:
            return false;
            
    }
}

imm instruction:: getImm(){
    if(tag&1)   return immediate;
    else{
        printf("Warning: Invalid immediate in instruction is used!\n");
        return immediate;
    }
}
xcode instruction:: getfunc2(){
    if(tag&64)   return func2;
    else{
        printf("Warning: Invalid func2 in instruction is used!\n");
        return func2;
    }
}

xcode instruction:: getfunc3(){
    if(tag&2)   return func3;
    else{
        printf("Warning: Invalid func3 in instruction is used!\n");
        return func3;
    }
}
xcode instruction:: getfunc7(){
    if(tag&4)   return func7;
    else{
        printf("Warning: Invalid func7 in instruction is used!\n");
        return func7;
    }
}
regID instruction:: getrd(){
    if(tag&8)   return rd;
    else{
        printf("Warning: Invalid rd in instruction is used!\n");
        return rd;
    }
}

regID instruction:: getrs1(){
    if(tag&16)   return rs1;
    else{
        printf("Warning: Invalid rs1 in instruction is used!\n");
        return rs1;
    }
}

regID instruction:: getrs2(){
    if(tag&32)   return rs2;
    else{
        printf("Warning: Invalid rs2 in instruction is used!\n");
        return rs2;
    }
}
regID instruction:: getrs3(){
    if(tag&128)   return rs3;
    else{
        printf("Warning: Invalid rs3 in instruction is used!\n");
        return rs3;
    }
}


