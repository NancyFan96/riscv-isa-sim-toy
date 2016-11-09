//
//  decode.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include <string.h>
#include <unistd.h>
#include <fenv.h>
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

void instruction::print_ins(const char* inst_name, regID rd, regID rs1, regID rs2){
    printf("instruction:\t %s %d, %d, %d\n", inst_name, rd, rs1, rs2);
    sim_regs.readReg();
    sim_regs.readFloatReg();
}
void instruction::print_ins(const char* inst_name, regID r1, regID r2, imm imm0){
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
        if(sim_regs.readReg(a7) == 93 && sim_regs.readReg(a1) == 0 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            // exit_program
            IS_TO_EXIT = true;
            if(verbose) print_ins("Prog Exited!");
            else
                printf("Prog Exited!");
        }
        if(sim_regs.readReg(a7) == 63 && sim_regs.readReg(a3) == 0){
            // read
            printf("read.. ");
			int fd = (int)sim_regs.readReg(a0);
			void * buf = (void*)sim_regs.readReg(a1);
			size_t count = (size_t)sim_regs.readReg(a2);
			read(fd, buf, count);
            printf("read\n");
        }
        if(sim_regs.readReg(a7) == 64 && sim_regs.readReg(a3) == 0){
            // write
            printf("write.. ");
			int fd = (int)sim_regs.readReg(a0);
			void * buf = (void*)sim_regs.readReg(a1);
			size_t count = (size_t)sim_regs.readReg(a2);
            printf("args: fd = %d, buf_p = %lx, count(byte) = %d\n", fd, (reg64)buf, (int)count);
			write(fd, buf, count);
            printf("write\n");
        }
        return;
    }
    
    if(optype == R4_TYPE){
        /*------RV64F-I.-----*/
        execute_R4();
        printf("Warning: R4 hasn't been finished\n");
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

void instruction::execute_R4(){
    
}

void instruction::execute_FExt(){
    if(opcode == 0x07){
        // FLW, FLD rd, rs1, imm, from mem to rd
        memAddress mem_addr;
        if(getfunc3()==2){//FLW
            mem_addr = sim_regs.readReg(getrs1())+immediate;
            sim_regs.writeFloatReg(getrd(), (f32)sim_mem.get_memory_32(mem_addr));
            if(verbose) print_ins("FLW", getrd(), getrs1(), immediate);
        }
        else if(getfunc3()==3){
            mem_addr = sim_regs.readReg(getrs1())+immediate;
            sim_regs.writeFloatReg(getrd(), (f64)sim_mem.get_memory_64(mem_addr));
            if(verbose) print_ins("FLD", getrd(), getrs1(), immediate);
        }
        else{
            printf("undefined instruction with opcode = 0x07\n");
        }
    }
    else if(opcode == 0x27){
        // FSW, FSW rs1, rs2, imm, from rs2 to mem
        memAddress mem_addr;
        if(getfunc3()==2){//FLW
            mem_addr = sim_regs.readReg(getrs1())+immediate;
            sim_mem.set_memory_32(mem_addr, (f32)sim_regs.readFloatReg(getrs2()));
            if(verbose) print_ins("FSW", getrs1(), getrs2(), immediate);
        }
        else if(getfunc3()==3){
            mem_addr = sim_regs.readReg(getrs1())+immediate;
            sim_mem.set_memory_64(mem_addr, sim_regs.readFloatReg(getrs2()));
            if(verbose) print_ins("FSD", getrs1(), getrs2(), immediate);
        }
        else{
            printf("undefined instruction with opcode = 0x27\n");
        }
    }
    else if(opcode == 0x53){
        //int roundingmode = 0 | getfunc3();
        //fesetenv((fenv_t)roundingmode);
        // XXX rd, rs1, rs2
        switch (getfunc7()) {
            case 0:
                // FADD.S
                sim_regs.writeFloatReg(getrd(), (f32)sim_regs.readFloatReg(getrs1())+(f32)sim_regs.readFloatReg(getrs2()));
                if(verbose) print_ins("FADD.S", getrd(), getrs1(), getrs2());
                break;
            case 1:
                // FADD.D
                sim_regs.writeFloatReg(getrd(), (f64)sim_regs.readFloatReg(getrs1())+(f64)sim_regs.readFloatReg(getrs2()));
                if(verbose) print_ins("FADD.D", getrd(), getrs1(), getrs2());
                break;
            case 4:
                // FSUB.S
                sim_regs.writeFloatReg(getrd(), (f32)sim_regs.readFloatReg(getrs1())-(f32)sim_regs.readFloatReg(getrs2()));
                if(verbose) print_ins("FSUB.S", getrd(), getrs1(), getrs2());
                break;
            case 5:
                // FSUB.D
                sim_regs.writeFloatReg(getrd(), (f64)sim_regs.readFloatReg(getrs1())-(f64)sim_regs.readFloatReg(getrs2()));
                if(verbose) print_ins("FSUB.D", getrd(), getrs1(), getrs2());
                break;
            case 8:
                // FMUL.S
                sim_regs.writeFloatReg(getrd(), (f32)sim_regs.readFloatReg(getrs1())*(f32)sim_regs.readFloatReg(getrs2()));
                if(verbose) print_ins("FMUL.S", getrd(), getrs1(), getrs2());
                break;
            case 9:
                // FMUL.D
                sim_regs.writeFloatReg(getrd(), (f64)sim_regs.readFloatReg(getrs1())*(f64)sim_regs.readFloatReg(getrs2()));
                if(verbose) print_ins("FMUL.D", getrd(), getrs1(), getrs2());
                break;
            case 12:
                // FDIV.S
                sim_regs.writeFloatReg(getrd(), (f32)sim_regs.readFloatReg(getrs1())/(f32)sim_regs.readFloatReg(getrs2()));
                if(verbose) print_ins("FDIV.S", getrd(), getrs1(), getrs2());
                break;
            case 13:
                // FDIV.D
                sim_regs.writeFloatReg(getrd(), (f64)sim_regs.readFloatReg(getrs1())/(f64)sim_regs.readFloatReg(getrs2()));
                if(verbose) print_ins("FDIV.D", getrd(), getrs1(), getrs2());
                break;
            case 0x50:
                // FEQ.S, FLT.S, FLE.S
                if(getfunc3()==2){//FEQ
                    if((f32)sim_regs.readFloatReg(getrs1())==(f32)sim_regs.readFloatReg(getrs2()))
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FEQ.D", getrd(), getrs1(), getrs2());
                }
                else if(getfunc3()==1){//FLT
                    if((f32)sim_regs.readFloatReg(getrs1())<(f32)sim_regs.readFloatReg(getrs2()))
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FLT.D", getrd(), getrs1(), getrs2());
                }
                else if(getfunc3()==0){//FLE
                    if((f32)sim_regs.readFloatReg(getrs1())<=(f32)sim_regs.readFloatReg(getrs2()))
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FLE.D", getrd(), getrs1(), getrs2());
                }
                else{
                    printf("undefined instruction with opcode=0x53, func7=0x50\n");
                }
                break;
            case 0x51:
                // FEQ.D, FLT.D, FLE.D
                if(getfunc3()==2){//FEQ
                    if(sim_regs.readFloatReg(getrs1())==sim_regs.readFloatReg(getrs2()))
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FEQ.D", getrd(), getrs1(), getrs2());
                }
                else if(getfunc3()==1){//FLT
                    if(sim_regs.readFloatReg(getrs1())<sim_regs.readFloatReg(getrs2()))
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FLT.D", getrd(), getrs1(), getrs2());
                }
                else if(getfunc3()==0){//FLE
                    if(sim_regs.readFloatReg(getrs1())<=sim_regs.readFloatReg(getrs2()))
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FLE.D", getrd(), getrs1(), getrs2());
                }
                else{
                    printf("undefined instruction with opcode=0x53, func7=0x51\n");
                }

            default:
                break;
        }
    }
    else{
        printf("undefined instruction in FExt\n");
    }
    
}

void instruction::execute_O()
{
    switch(opcode)
    {
        case 0x37://LUI
            sim_regs.writeReg(getrd(),immediate);
            if(verbose) print_ins("LUI", getrd(), immediate);
            break;
        case 0x17://AUIPC
            sim_regs.writeReg(getrd(),(signed64)sim_regs.getPC()+immediate-4);//!!!
            if(verbose) print_ins("AUIPC", getrd(), immediate);
            break;
        default:;
    }
}
//execute R type
void instruction::execute_R()
{
    /*****************NEED TO DO*******************/
    /*// MUL, MULH, MULHSH, MULHU, DIV, DIVU, REM, REMU*/
    if(getfunc7() == 0){
        switch(getfunc3()){
            case 0x00: //b000 add
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1())+sim_regs.readReg(getrs2()));
                if(verbose) print_ins("ADD", getrd(), getrs1(), getrs2());
                break;
            case 0x01: //b001 sll
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) << (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SLL", getrd(), getrs1(), getrs2());
                break;
            case 0x02: //b010 slt
                if((long int)sim_regs.readReg(getrs1()) < (long int)sim_regs.readReg(getrs2()))
                    sim_regs.writeReg(getrd(), 1);
                else
                    sim_regs.writeReg(getrd(), 0);
                if(verbose) print_ins("SLT", getrd(), getrs1(), getrs2());
                break;
            case 0x03: //b011 sltu
                if(sim_regs.readReg(getrs1()) < sim_regs.readReg(getrs2()))
                    sim_regs.writeReg(getrd(), 1);
                else
                    sim_regs.writeReg(getrd(), 0);
                if(verbose) print_ins("SLTU", getrd(), getrs1(), getrs2());
                break;
            case 0x04: //b100 xor
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) ^ sim_regs.readReg(getrs2()));
                if(verbose) print_ins("XOR", getrd(), getrs1(), getrs2());
                break;
            case 0x05: //b101 srl
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) >> (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SRL", getrd(), getrs1(), getrs2());
                break;
            case 0x06: //b110 or
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) | sim_regs.readReg(getrs2()));
                if(verbose) print_ins("OR", getrd(), getrs1(), getrs2());
                break;
            case 0x07: //b111 and
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) & sim_regs.readReg(getrs2()));
                if(verbose) print_ins("AND", getrd(), getrs1(), getrs2());
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=0\n");
                break;
        }
    }
    else if(getfunc7()==0x20){
        switch(getfunc3()){
            case 0x00: //b000 sub
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) - sim_regs.readReg(getrs2()));
                if(verbose) print_ins("SUB", getrd(), getrs1(), getrs2());
                break;
            case 0x05: //b101 sra
                sim_regs.writeReg(getrd(), (long)(sim_regs.readReg(getrs1())) >> (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SRA", getrd(), getrs1(), getrs2());
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=0x20\n");
                break;
        }
    }
}

// opcode = 3B
void instruction::execute_R64()
{
    /*****************NEED TO DO*******************/
    /*// MULW,DIVW, DIVUW, REMW, REMUW*/
    
    if(getfunc7()==0x00){
        switch(getfunc3()){
            case 0x00: //b000 addw
                sim_regs.writeReg(getrd(), (int)sim_regs.readReg(getrs1()) + (int)sim_regs.readReg(getrs2()) );
                if(verbose) print_ins("ADDW", getrd(), getrs1(), getrs2());
                break;
            case 0x01: //b001 sllw
                sim_regs.writeReg(getrd(), (int)sim_regs.readReg(getrs1()) << (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("ALLW", getrd(), getrs1(), getrs2());
                break;
            case 0x05: //b101 srlw
                sim_regs.writeReg(getrd(), (int)sim_regs.readReg(getrs1()) >> (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("ARLW", getrd(), getrs1(), getrs2());
                break;
            default:
                printf("undefined instruction with opcode =011 1011, func7=0\n");
                break;
        }
    }
    else if(getfunc7()==0x20){
        switch(getfunc3()){
            case 0x00: //b000 subw
                sim_regs.writeReg(getrd(), (int)sim_regs.readReg(getrs1()) - (int)sim_regs.readReg(getrs2()) );
                if(verbose) print_ins("SUBW", getrd(), getrs1(), getrs2());
                break;
            case 0x05: //b101 sraw
                sim_regs.writeReg(getrd(), (int)(sim_regs.readReg(getrs1())) >> (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SRAW", getrd(), getrs1(), getrs2());
                break;
            default:
                printf("undefined instruction with opcode = 011 1011, func7=0x20\n");
                break;
        }
    }
    else printf("undefined instruction in R-TYPE\n");
}

void instruction::execute_I(){
    // contain opcode, func3, rd, rs1, imm
    switch (opcode) {
        case 0x6F: //JAL rd,imm
            reg32 newPC;
            sim_regs.writeReg(getrd(), sim_regs.getPC()); //PC+4?
            newPC = (reg32)(sim_regs.getPC()+immediate-4);
            sim_regs.setPC(newPC);
            if(verbose) print_ins("JAL", getrd(), immediate);
            break;
        case 0x67: // b110 0111
            // JALR rd, rs1, imm
            switch (getfunc3()){
                case 0:
                    sim_regs.writeReg(getrd(), sim_regs.getPC());
                    sim_regs.setPC((reg32)((sim_regs.readReg(getrs1())+immediate)&~1));
                    if(verbose) print_ins("JALR", getrd(), getrs1(), immediate);
                    break;
                default:
                    printf("Undefined instruction with opcode = 110 0111\n");
                    break;
            }
            break;
        case 0x03: // b000 0011
            // LB, LH, LW, LBU, LHU rd, rs1, imm
            // LWU, LD rd, rs1, imm
            memAddress mem_addr;
            //printf("enter 0x03\n");
            switch (getfunc3()) {
                case 0:             // LB rd, rs1, imm
                    mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
                    //printf("mem_addr = 0x%lx\n", mem_addr);
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_8(mem_addr));
                    if(verbose) print_ins("LB", getrd(), getrs1(), immediate);
                    break;
                case 1:             // LH rd, rs1, imm
                    mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
                    //printf("mem_addr = 0x%lx\n", mem_addr);
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_16(mem_addr));
                    if(verbose) print_ins("LH", getrd(), getrs1(), immediate);
                    break;
                case 2:             // LW rd, rs1, imm
                    mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
                    //printf("mem_addr = 0x%lx\n", mem_addr);
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_32(mem_addr));
                    if(verbose) print_ins("LW", getrd(), getrs1(), immediate);
                    break;
                case 4:             // LBU rd, rs1, imm
                    immediate = immediate & ~ONES(63, 12);
                    mem_addr = sim_regs.readReg(getrs1())+ immediate;
                    //printf("mem_addr = 0x%lx\n", mem_addr);
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_8(mem_addr));
                    if(verbose) print_ins("LBU", getrd(), getrs1(), immediate);
                    break;
                case 5:             // LHU rd, rs1, imm
                    immediate = immediate & ~ONES(63, 12);
                    mem_addr =  sim_regs.readReg(getrs1())+ immediate;
                    //printf("mem_addr = 0x%lx\n", mem_addr);
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_16(mem_addr));
                    if(verbose) print_ins("LHU", getrd(), getrs1(), immediate);
                    break;
                case 6:             // LWU rd, rs1, imm
                    immediate = immediate & ~ONES(63, 12);
                    mem_addr = sim_regs.readReg(getrs1())+ immediate;
                    //printf("mem_addr = 0x%lx\n", mem_addr);
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_32(mem_addr));
                    if(verbose) print_ins("LWU", getrd(), getrs1(), immediate);
                    break;
                case 3:             // LD rd, rs1, imm
                    //printf("start this inst..\n");
                    mem_addr = sim_regs.readReg(getrs1())+ immediate;
                    //printf("mem_addr = 0x%lx\n", mem_addr);
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_64(mem_addr));
                    if(verbose) print_ins("LD", getrd(), getrs1(), immediate);
                    break;
                default:
                    printf("Undefined instruction with opcode = 000 0011\n");
                    break;
            }
            break;
        case 0x13: // b001 0011
            // ADDI, SLTI, SLTIU, XORI, ORI, ANDI rd, rs1, imm
            // SLLI, SRLI, SRAI rd, rs1, shamt
            signed64 ALUZ64;
            byte shamt;
            switch (getfunc3()) {
                case 0:             // ADDI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())+immediate;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("ADDI", getrd(), getrs1(), immediate);
                    break;
                case 2:             // SLTI rd, rs1, imm
                    if(sim_regs.readReg(getrs1())<immediate)
                        ALUZ64 = 1;
                    else
                        ALUZ64 = 0;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("SLTI", getrd(), getrs1(), immediate);
                    break;
                case 3:             // SLTIU rd, rs1, imm
                    if(sim_regs.readReg(getrs1())<(reg64)immediate)
                        ALUZ64 = 1;
                    else
                        ALUZ64 = 0;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("SLTIU", getrd(), getrs1(), immediate);
                    break;
                case 4:             // XORI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())^immediate;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("XORI", getrd(), getrs1(), immediate);
                    break;
                case 6:             // ORI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())|immediate;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("ORI", getrd(), getrs1(), immediate);
                    break;
                case 7:             // ANDI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())&immediate;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("ANDI", getrd(), getrs1(), immediate);
                    break;
                case 1:             // SLLI rd, rs1, shamt
                    shamt = immediate & ONES(5, 0);
                    ALUZ64 = sim_regs.readReg(getrs1()) << shamt;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("SLLI", getrd(), getrs1(), shamt);
                    break;
                case 5:             // SRLI, SRAI rd, rs1, shamt
                    shamt = immediate & ONES(5, 0);
                    if(immediate & ~ONES(5,0))  //SRAI
                        ALUZ64 = ((signed64)sim_regs.readReg(getrs1())) >> shamt;
                    else                       // SRLI
                        ALUZ64 = sim_regs.readReg(getrs1()) >> shamt;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose){
                        if(immediate & ~ONES(5,0))
                            print_ins("SRAI", getrd(), getrs1(), shamt);
                        else
                            print_ins("SRLI", getrd(), getrs1(), shamt);
                    }
                    break;
                default:
                    printf("Undefined instruction with opcode = 001 0011\n");
                    break;
            }
            break;
        case 0x1B:  //b001 1011
            // ADDIW, SLLIW, SRLIW, SRAIW rd, rs1, shamt[longer]
            signed32 ALUZ32;
            switch (getfunc3()) {
                case 0:             // ADDIW rd, rs1, imm
                    ALUZ32 = (signed32)sim_regs.readReg(getrs1())+(signed32)immediate;
                    sim_regs.writeReg(getrd(), ALUZ32);
                    if(verbose) print_ins("ADDIW", getrd(), getrs1(), shamt);
                    break;
                case 1:             // SLLIW rd, rs1, shamt
                    shamt = immediate & ONES(4, 0);
                    ALUZ32 = ((signed32)sim_regs.readReg(getrs1())) << shamt;
                    sim_regs.writeReg(getrd(), ALUZ32);
                    if(verbose) print_ins("SLLIW", getrd(), getrs1(), shamt);
                    break;
                case 5:             // SRLIW, SRAIW rd, rs1, shamt
                    shamt = immediate & ONES(4, 0);
                    if(immediate & ~ONES(4,0)) //SRAIW
                        ALUZ32 = (int)((signed32)sim_regs.readReg(getrs1()) >> shamt);
                    else                       //SRLIW
                        ALUZ32 = ((reg32)sim_regs.readReg(getrs1())) >> shamt;
                    sim_regs.writeReg(getrd(), ALUZ32);
                    if(verbose){
                        if(immediate & ~ONES(4,0))
                            print_ins("SRAIW", getrd(), getrs1(), shamt);
                        else
                            print_ins("SRLIW", getrd(), getrs1(), shamt);
                    }
                    break;
                default:
                    printf("Undefined instruction with opcode = 001 1011\n");
                    break;
            }
            break;
        default:;
    }
    
}
//SB,SH,SW,SD rs1,rs2,imm
void instruction::execute_SX(){
    memAddress mem_addr;
    switch(getfunc3())
    {
        case 0://SB rs1, rs2, imm
            mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
            //printf("mem_addr = 0x%lx\n", mem_addr);
            sim_mem.set_memory_8(mem_addr, (reg8)sim_regs.readReg(getrs2()));
            if(verbose) print_ins("SB", getrs1(), getrs2(), immediate);
            break;
        case 1://SH
            mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
            //printf("mem_addr = 0x%lx\n", mem_addr);
            sim_mem.set_memory_16(mem_addr, (reg16)sim_regs.readReg(getrs2()));
            if(verbose) print_ins("SH", getrs1(), getrs2(), immediate);
            break;
        case 2://SW
            mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
            //printf("mem_addr = 0x%lx\n", mem_addr);
            sim_mem.set_memory_32(mem_addr, (reg32)sim_regs.readReg(getrs2()));
            if(verbose) print_ins("SW", getrs1(), getrs2(), immediate);
            break;
        case 3://SD rs1, rs2, imm
            mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
            //printf("mem_addr = 0x%lx\n", mem_addr);
            sim_mem.set_memory_64(mem_addr, (reg64)sim_regs.readReg(getrs2()));
            if(verbose) print_ins("SD", getrs1(), getrs2(), immediate);
            break;
    }
    
}
//BEQ,BNE,BLT,BGE,BLTU,BGEU rs1,rs2,imm
void instruction::execute_UX(){
    switch(getfunc3())
    {
        case 0://BEQ
            if((signed64)sim_regs.readReg(getrs1())  == (signed64)sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BEQ", getrs1(), getrs2(), immediate);
            break;
        case 1://BNE
            if((signed64)sim_regs.readReg(getrs1())  != (signed64)sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BNE", getrs1(), getrs2(), immediate);
            break;
        case 4://BLT
            if((signed64)sim_regs.readReg(getrs1())  < (signed64)sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BLT", getrs1(), getrs2(), immediate);
            break;
        case 5://BGE
            if((signed64)sim_regs.readReg(getrs1())  >= (signed64)sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BGE", getrs1(), getrs2(), immediate);
            break;
        case 6://BLTU
            if(sim_regs.readReg(getrs1())  < sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BLTU", getrs1(), getrs2(), immediate);
            break;
        case 7://BGEU
            if(sim_regs.readReg(getrs1())  >= sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BGEU", getrs1(), getrs2(), immediate);
            break;
        default:;
    }
}







