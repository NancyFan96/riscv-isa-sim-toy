//
//  decode.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include<sys/stat.h>
#include <fenv.h>
#include <math.h>
#include "system.h"
#include "decode.hpp"
#include "register.hpp"
#include "memory.hpp"

extern const float FDIFF;
extern bool GDB_MODE;

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

void instruction::print_ins(const char* inst_name, regID rd, regID rs1, regID rs2){
    if(GDB_MODE)    printf("> ");
    printf("instruction:\t %s %d, %d, %d\n", inst_name, rd, rs1, rs2);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}
void instruction::print_ins(const char* inst_name, regID r1, regID r2, imm imm0){
    if(GDB_MODE)    printf("> ");
    printf("instruction:\t %s %d, %d, %ld\n", inst_name, r1, r2, imm0);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}
void instruction::print_ins(const char* inst_name, regID rx, imm imm0){
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
    printf("instruction:\t %s\n", inst_name);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}

signed64 instruction::RNE(f64 d){
    if((signed64)d % 2 == 0){
        if(d > 0.0)
            return (signed64)round(d) - 1;
        else
            return (signed64)round(d) + 1;
    }
    else
        return (signed64)round(d);
}

signed64 instruction::RTZ(f64 d){
    return (signed64)d;
}

signed64 instruction::RDN(f64 d){
    return (signed64)floor(d);
}

signed64 instruction::RUP(f64 d){
    return (signed64)ceil(d);
}

signed64 instruction::RMM(f64 d){
    return (signed64)ceil(d);
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


// only valid instruction will enter this function
void instruction::execute(){
    if(optype == SCALL){
        // _exit, read, write, gettimeofday, sbrk, fstat, lseek, close
        if(sim_regs.readReg(a7) == 93 && sim_regs.readReg(a1) == 0 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            // exit_program
            IS_TO_EXIT = true;
            if(verbose) print_ins("\nProg Exited!");
            else{
                if(GDB_MODE)    printf("> ");
                printf("\nProg Exited!\n");
            }
        }
        else if(sim_regs.readReg(a7) == 62 &&  sim_regs.readReg(a3) == 0){
            //lseek
            sim_regs.writeReg(a0, lseek((int)sim_regs.readReg(a0), (off_t)sim_regs.readReg(a1), (int)sim_regs.readReg(a2)));
        }
        else if(sim_regs.readReg(a7) == 63 && sim_regs.readReg(a3) == 0){
            // read
            //printf("read.. ");
			int fd = (int)sim_regs.readReg(a0);
			void * buf = (void*)sim_regs.readReg(a1);
			size_t count = (size_t)sim_regs.readReg(a2);
            //printf("args: fd = %d, buf_p = %lx, count(byte) = %d ...\n", fd, (reg64)buf, (int)count);
            buf = sim_mem.get_memory_p_address((reg64)buf);
			count = read(fd, buf, count);
            sim_regs.writeReg(a0, count);
            if(verbose) print_ins("READ");
        }
        else if(sim_regs.readReg(a7) == 64 && sim_regs.readReg(a3) == 0){
            // write
            //printf("write.. ");
			int fd = (int)sim_regs.readReg(a0);
			void * buf = (void*)sim_regs.readReg(a1);
			size_t count = (size_t)sim_regs.readReg(a2);
            //fflush(stdout);
            //printf("args: fd = %d, buf_p = %lx, count(byte) = %d ...\n", fd, (reg64)buf, (int)count);
			buf = sim_mem.get_memory_p_address((reg64)buf);
			count = write(fd, buf, count);
            sim_regs.writeReg(a0, count);
            if(verbose) print_ins("WRITE");
        }
        else if(sim_regs.readReg(a7) == 169 &&  sim_regs.readReg(a1) == 0 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            // gettimeofday
            printf("Before:\n");
            print_ins("GETTIMEOFDAY");
            struct  timeval  *tv_p = (struct timeval *)sim_mem.get_memory_p_address(sim_regs.readReg(a0));
            sim_regs.writeReg(a0,  gettimeofday(tv_p,NULL));
            printf("After:\n");
            print_ins("GETTIMEOFDAY");
        }
        else if(sim_regs.readReg(a7) == 214 &&  sim_regs.readReg(a1) == 0 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            //sbrk
        }
        else if(sim_regs.readReg(a7) == 57 &&  sim_regs.readReg(a1) == 0 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
             //close
            //int fd = (int)sim_regs.readReg(a0);
            //sim_regs.writeReg(a0, close(fd));
            //if(verbose) print_ins("CLOSE");
         }
        else if(sim_regs.readReg(a7) == 80 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            //fstat
            //int fd = (int)sim_regs.readReg(a0);
            //struct stat * buf = (struct stat*)sim_regs.readReg(a1);
            //sim_regs.writeReg(a0, (reg64)fstat(fd,buf));
            //if(verbose) print_ins("FSTAT");
        }
        else
            printf("Undefined scall\n");
        return;
    }
    
    if(optype == R4_TYPE){
        /*------RV64F-I.-----*/
        execute_R4();
        return;
    }
    switch(opcode){
            /*---- RV32I-----*/
        case 0x23:      // b0100011,SB,SH,SW,SD
            execute_SX();
            break;
        case 0x33:      // b0110011,ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND
                        // MUL, MULH, MULHSH, MULHU, DIV, DIVU, REM, REMU
            execute_R();
            break;
        case 0x37:      // b0110111, LUI
        case 0x17:      // b0010111, AUIPC
            execute_O();//other
            break;
        case 0x1B:      // b0011011,ADDIW,SLLIW,SRLIW,SRAIW
        case 0x13:      // b0010011,SLLI,SRLI,SRAI,ADDI,SLTI,SLTIU,XORI,ORI,ANDI,SLLI,SRLI,SRAI
        case 0x03:      // b0000011,LB,LH,LW,LBU,LHU,LWU,LD
        case 0x67:      // b1100111,JALR
        case 0x6F:      // b1101111,JAL
            execute_I();
            break;
        case 0x3B:      // b0111011,ADDW,SUBW,SLLW,SRLW,SRAW
                        // MULW,DIVW, DIVUW, REMW, REMUW
            execute_R64();
            break;
        case 0x63:      // b1100011,BEQ,BNE,BLT,BGE,BLTU,BGEU
            execute_UX();
            break;
            
            /*------RV64F-II.-----*/
        case 0x07:
        case 0x27:
        case 0x53:
            execute_FExt();
            break;
        default:;
    }
    
}

