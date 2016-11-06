//
//  decode.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include "decode.hpp"

#ifndef register_hpp
#define register_hpp
#include "register.hpp"
#endif

#ifndef memory_hpp
#define memory_hpp
#include "memory.hpp"
#endif

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
}
void instruction::print_ins(const char* inst_name, regID r1, regID r2, imm imm0){
    printf("instruction:\t %s %d, %d, %d\n", inst_name, r1, r2, imm0);
    sim_regs.readReg();
}
void instruction::print_ins(const char* inst_name, regID rx, imm imm0){
    printf("instruction:\t %s %d, %d\n", inst_name, rx, imm0);
    sim_regs.readReg();
}
void instruction::print_ins(const char* inst_name, regID rx){
    printf("instruction:\t %s %d\n", inst_name, rx);
    sim_regs.readReg();
}
void instruction::print_ins(const char* inst_name){
    printf("instruction:\t %s\n", inst_name);
    sim_regs.readReg();
}

bool instruction::getType(ins inst){
    if(inst == 0x00000073){
        optype = SCALL;
        tag = 0;
        return true;
    }
    opcode = inst&OPCODE;
    switch(opcode)
    {
        case 0x3B:      // b0111011
        case 0x33:      // b0110011
            optype =  R_TYPE;
            tag = 62;   // b111110 | 32+16+8+4+2
            return true;
            
        case 0x67:      // b1100111
        case 0x03:      // b0000011
        case 0x13:      // b0010011
        case 0x1B:
            //case 0x73:      // b1110011
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

// Notice all the extend is sign-extend
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
    switch(opcode){
            /*---- RV32I-----*/
        case 0x23:      // b0100011,SB,SH,SW,SD
            execute_SX();
            break;
        case 0x33:      // b0110011,ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND
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
            execute_R64();
            break;
        case 0x63:      // b1100011,BEQ,BNE,BLT,BGE,BLTU,BGEU
            execute_UX();
            break;
        default:;
    }
    
}

void instruction::execute_O()
{
    switch(opcode)
    {
        case 0x37://LUI
            sim_regs.writeReg(getrd(),(reg64)immediate);
            break;
        case 0x17://AUIPC
            reg32 newPC;
            newPC = (reg32)(sim_regs.getPC()+immediate);
            sim_regs.setPC(newPC);
            sim_regs.writeReg(getrd(),(reg64)newPC);
            break;
        default:;
    }
}
//execute R type
void instruction::execute_R()
{
    if(getfunc7() == 0){
        switch(getfunc3()){
            case 0x00: //b000 add
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1())+sim_regs.readReg(getrs2()));
                break;
            case 0x01: //b001 sll
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) << (sim_regs.readReg(getrs2())&0x3f));
                break;
            case 0x02: //b010 slt
                if((long int)sim_regs.readReg(getrs1()) < (long int)sim_regs.readReg(getrs2()))
                    sim_regs.writeReg(getrd(), 1);
                else
                    sim_regs.writeReg(getrd(), 0);
                break;
            case 0x03: //b011 sltu
                if(sim_regs.readReg(getrs1()) < sim_regs.readReg(getrs2()))
                    sim_regs.writeReg(getrd(), 1);
                else
                    sim_regs.writeReg(getrd(), 0);
                break;
            case 0x04: //b100 xor
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) ^ sim_regs.readReg(getrs2()));
                break;
            case 0x05: //b101 srl
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) >> (sim_regs.readReg(getrs2())&0x3f));
                break;
            case 0x06: //b110 or
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) | sim_regs.readReg(getrs2()));
                break;
            case 0x07: //b111 and
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) & sim_regs.readReg(getrs2()));
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=0\n");
                break;
        }
    }
    else if(opcode==0x33 && getfunc7()==0x20){
        switch(getfunc3()){
            case 0x00: //b000 sub
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) - sim_regs.readReg(getrs2()));
                break;
            case 0x05: //b101 sra
                sim_regs.writeReg(getrd(), (long)(sim_regs.readReg(getrs1())) >> (sim_regs.readReg(getrs2())&0x3f));
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=0x20\n");
                break;
        }
    }
}

void instruction::execute_R64()
{
    if(getfunc7()==0x00){
        switch(getfunc3()){
            case 0x00: //b000 addw
                sim_regs.writeReg(getrd(), (int)sim_regs.readReg(getrs1()) + (int)sim_regs.readReg(getrs2()) );
                break;
            case 0x01: //b001 sllw
                sim_regs.writeReg(getrd(), (int)sim_regs.readReg(getrs1()) << (sim_regs.readReg(getrs2())&0x3f));
                break;
            case 0x05: //b101 srlw
                sim_regs.writeReg(getrd(), (int)sim_regs.readReg(getrs1()) >> (sim_regs.readReg(getrs2())&0x3f));
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
                break;
            case 0x05: //b101 sraw
                sim_regs.writeReg(getrd(), (int)(sim_regs.readReg(getrs1())) >> (sim_regs.readReg(getrs2())&0x3f));
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
            newPC = (reg32)(sim_regs.getPC()+immediate);
            sim_regs.setPC(newPC);
            break;
        case 0x67: // b110 0111
            // JALR rd, rs1, imm
            switch (getfunc3()){
                    reg32 newPC;
                case 0:
                    sim_regs.writeReg(getrd(), sim_regs.getPC());
                    newPC = (reg32)((sim_regs.readReg(getrs1())+immediate)&~1);
                    sim_regs.setPC(newPC);
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
            switch (getfunc3()) {
                case 0:             // LB rd, rs1, imm
                    mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_8(mem_addr));
                    break;
                case 1:             // LH rd, rs1, imm
                    mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_16(mem_addr));
                    break;
                case 2:             // LW rd, rs1, imm
                    mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_32(mem_addr));
                    break;
                case 4:             // LBU rd, rs1, imm
                    immediate = immediate & ~ONES(31, 12);
                    mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_8(mem_addr));
                    break;
                case 5:             // LHU rd, rs1, imm
                    immediate = immediate & ~ONES(31, 12);
                    mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_16(mem_addr));
                    break;
                case 6:             // LWU rd, rs1, imm
                    immediate = immediate & ~ONES(31, 12);
                    mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_32(mem_addr));
                    break;
                case 3:             // LD rd, rs1, imm
                    immediate = immediate & ~ONES(31, 12);
                    mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_64(mem_addr));
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
                    break;
                case 2:             // SLTI rd, rs1, imm
                    if(sim_regs.readReg(getrs1())<immediate)
                        ALUZ64 = 1;
                    else
                        ALUZ64 = 0;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    break;
                case 3:             // SLTIU rd, rs1, imm
                    if(sim_regs.readReg(getrs1())<(reg64)(unsigned int)immediate)
                        ALUZ64 = 1;
                    else
                        ALUZ64 = 0;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    break;
                case 4:             // XORI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())^immediate; // 符号扩展
                    sim_regs.writeReg(getrd(), ALUZ64);
                    break;
                case 6:             // ORI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())|immediate; // 符号扩展
                    sim_regs.writeReg(getrd(), ALUZ64);
                    break;
                case 7:             // ANDI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())&immediate; // 符号扩展
                    sim_regs.writeReg(getrd(), ALUZ64);
                    break;
                case 1:             // SLLI rd, rs1, shamt ?
                    shamt = immediate & ONES(5, 0);
                    ALUZ64 = sim_regs.readReg(getrs1()) << shamt;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    break;
                case 5:             // SRLI, SRAI rd, rs1, shamt ?
                    shamt = immediate & ONES(5, 0);
                    ALUZ64 = sim_regs.readReg(getrs1()) >> shamt;
                    sim_regs.writeReg(getrd(), ALUZ64);
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
                    ALUZ32 = (int)sim_regs.readReg(getrs1())+immediate;
                    sim_regs.writeReg(getrd(), ALUZ32);
                    break;
                case 1:             // SLLIW rd, rs1, shamt
                    shamt = immediate & ONES(4, 0);
                    ALUZ32 = ((int)sim_regs.readReg(getrs1())) << shamt;
                    sim_regs.writeReg(getrd(), ALUZ32);
                    break;
                case 5:             // SRLIW, SRAIW rd, rs1, shamt
                    shamt = immediate & ONES(4, 0);
                    ALUZ32 = ((int)sim_regs.readReg(getrs1())) >> shamt;
                    sim_regs.writeReg(getrd(), ALUZ32);
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
        case 0://SB
            mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
            sim_mem.set_memory_8(mem_addr, (reg8)getrd());
            break;
        case 1://SH
            mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
            sim_mem.set_memory_16(mem_addr, (reg16)getrd());
            break;
        case 2://SW
            mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
            sim_mem.set_memory_32(mem_addr, (reg32)getrd());
            break;
        case 3://SD
            mem_addr = (reg32)sim_regs.readReg(getrs1())+ immediate;
            sim_mem.set_memory_64(mem_addr, (reg64)getrd());
            break;
    }
    
}
//BEQ,BNE,BLT,BGE,BLTU,BGEU rs1,rs2,imm
void instruction::execute_UX(){
    switch(getfunc3())
    {
        case 0://BEQ
            if(getrs1() == getrs2())
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate);
                sim_regs.setPC(newPC);
            }
            break;
        case 1://BNE
            if(getrs1() != getrs2())
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate);
                sim_regs.setPC(newPC);
            }
            break;
        case 4://BLT
            if((long)getrs1() < (long)getrs2())
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate);
                sim_regs.setPC(newPC); 
            }
            break;
        case 5://BGE
            if((long)getrs1() >= (long)getrs2())
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate);
                sim_regs.setPC(newPC); 
            }
            break;
        case 6://BLTU
            if(getrs1() < getrs2())
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate);
                sim_regs.setPC(newPC); 
            }
            break;
        case 7://BGEU
            if(getrs1() >= getrs2())
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate);
                sim_regs.setPC(newPC); 
            }
            break;
        default:;
    }
}







