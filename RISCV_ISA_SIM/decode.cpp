//
//  decode.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include "decode.hpp"

instruction::instruction()
{
    opcode = 0;
    optype = 0;
    tag = 0;        //#important
    func3 = 0;
    func7 = 0;
    immediate = 0;
}

void instruction::print_ins(const char* inst_name, regID rd, regID rs1, regID rs2){
    printf("instruction:\t %s %d %d %d\n", inst_name, rd, rs1, rs2);
}
void instruction::print_ins(const char* inst_name, regID r1, regID r2, imm imm0){
    printf("instruction:\t %s %d %d %d\n", inst_name, r1, r2, imm0);
}
void instruction::print_ins(const char* inst_name, regID rx, imm imm0){
    printf("instruction:\t %s %d %d\n", inst_name, rx, imm0);
}
void instruction::print_ins(const char* inst_name, regID rx){
    printf("instruction:\t %s %d\n", inst_name, rx);
}
void instruction::print_ins(const char* inst_name){
    printf("instruction:\t %s\n", inst_name);
}

bool instruction::getType(ins inst){
    opcode = inst&OPCODE;
    switch(opcode)
    {
        case 0x33:      // b0110011
            optype =  R_TYPE;
            tag = 62;   // b111110 | 32+16+8+4+2
            return true;
            
        case 0x67:      // b1100111
        case 0x03:      // b0000011
        case 0x13:      // b0010011
        case 0x73:      // b1110011
            optype =  I_TYPE;
            tag = 27;    // b011011 | 16+8+2+1
            return true;
            
        case 0x23:      // b0100011
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
            
        default:
            return false;
    }
}

bool instruction::setIMM(ins inst){
    switch(optype)
    {
            /*R type
             no immediate*/
        case R_TYPE:
            return true;
            /*I type
             31----------20 imm[0]~imm[11]
             shamt SRAI SRLI SLLI*/
        case I_TYPE:
            immediate = (inst&ONES(31,20) >> 20) | (IMM_SIGN(inst)*ONES(31,11));
            return true;
            /*S type
             31-----25-------------11---7
             imm[11]~imm[5]        imm[4]~imm[0]*/
        case S_TYPE:
            immediate = (inst&ONES(11,7) >> 7) | (inst&ONES(31,25) >> 20) | (IMM_SIGN(inst)*ONES(31,11));
            return true;
            /*SB type
             31 imm[12] 30----25 imm[10]~imm[5] 11----8 imm[4]~imm[1] 7 imm[11]*/
        case SB_TYPE:
            immediate = (((inst&ONES(11,8))>>7) | ((inst&ONES(30,25))>>20) | ((inst&ONES(7,7))<<4) | (IMM_SIGN(inst)*ONES(31,12)));
            return true;
            /*U type
             31----12 imm[31]~imm[12]*/
        case U_TYPE:
            immediate = (inst&ONES(31,12));
            return true;
            /*UJ type
             31 imm[20] 30----21 imm[10]~imm[1] 20 imm[11] 19----12 imm[19]~imm[12]*/
        case UJ_TYPE:
            immediate = (((inst&ONES(30,21))>>20) | ((inst&ONES(20,20))>>9) | (inst&ONES(19,12)) | (IMM_SIGN(inst)*ONES(31,20)));
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
            /* rs2,rs1,rd,func7,func3*/
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
    if(tag&16)   return rd;
    else{
        printf("Warning: Invalid rs1 in instruction is used!\n");
        return rs1;
    }
}

regID instruction:: getrs2(){
    if(tag&32)   return rd;
    else{
        printf("Warning: Invalid rs2 in instruction is used!\n");
        return rs2;
    }
}

bool instruction::Is_exit(){
    // EXIT FLAG is J to itself
    if(opcode == 111 && rd == 0 && immediate == -2) //b1101111 | 64+32+15 = 96+15 = 111
        return true;
    return false;
}

// only valid instruction will enter this function
void instruction::execute(){
    switch(optype)
    {
        case R_TYPE:
            execute_R();
            break;
        case I_TYPE:
            execute_I();
            break;
        case S_TYPE:
            execute_S();
            break;
        case SB_TYPE:
            execute_SB();
            break;
        case U_TYPE:
            execute_U();
            break;
        case UJ_TYPE:
            execute_UJ();
            break;
        default:;
    }
}

void instruction::execute_R(){
    // contain opcode, func3, rd, rs1, imm
    
}
void instruction::execute_I(){
    // contain opcode, func3, rd, rs1, imm
    
}
void instruction::execute_S(){
    // contain opcode, func3, rd, rs1, imm
    
}
void instruction::execute_SB(){
    // contain opcode, func3, rd, rs1, imm
    
}
void instruction::execute_U(){
    // contain opcode, func3, rd, rs1, imm
    
}
void instruction::execute_UJ(){
    // contain opcode, func3, rd, rs1, imm
    
}







