//
//  execute.cpp
//  RISCV_ISA_SIM
//
//  execution for RISCV64I, RISCV64M (only for std tests)
//
//  Created by Nancy Fan, Yang Kejing, Wang Luyao  on 16/11/11.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <map>
#include <string>
#include <math.h>
#include "system.h"
#include "decode.hpp"
#include "register.hpp"
#include "memory.hpp"

extern std::map<std::string,int> COUNTS;

// only valid instruction will enter this function
void instruction::execute(){
    if(optype == SCALL){
        COUNTS["ECALL"]++;
        // _exit, read, write, gettimeofday, sbrk, (fstat), (lseek), (close)
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
            int fd = (int)sim_regs.readReg(a0);
            void * buf = (void*)sim_regs.readReg(a1);
            size_t count = (size_t)sim_regs.readReg(a2);
            //printf("args: fd = %d, buf_p = %lx, count(byte) = %d ...\n", fd, (reg64)buf, (int)count);
            buf = sim_mem.get_memory_p_address((reg64)buf);
            count = write(fd, buf, count);
            sim_regs.writeReg(a0, count);
            fflush(stdout);
            if(verbose) print_ins("WRITE");
        }
        else if(sim_regs.readReg(a7) == 169 &&  sim_regs.readReg(a1) == 0 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            // gettimeofday
            struct  timeval  *tv_p = (struct timeval *)sim_mem.get_memory_p_address(sim_regs.readReg(a0));
            sim_regs.writeReg(a0,  gettimeofday(tv_p,NULL));
            //time_t * tm = (time_t *)sim_mem.get_memory_p_address(sim_regs.readReg(a0));
            //sim_regs.writeReg(a0,  time(tm));
            if(verbose) print_ins("GETTIMEOFDAY");
        }
        else if(sim_regs.readReg(a7) == 214 &&  sim_regs.readReg(a1) == 0 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            //sbrk
            //byte * mem_addr = sim_mem.get_memory_p_address(sim_regs.readReg(a0));
            //sim_regs.writeReg(a0, (reg64)mem_addr);
        }
        else if(sim_regs.readReg(a7) == 57 &&  sim_regs.readReg(a1) == 0 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            //close
        }
        else if(sim_regs.readReg(a7) == 80 && sim_regs.readReg(a2) == 0 && sim_regs.readReg(a3) == 0){
            //fstat
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

void instruction::execute_O()
{
    switch(opcode)
    {
        case 0x37://LUI
            sim_regs.writeReg(getrd(),immediate);
            if(verbose) print_ins("LUI", getrd(), immediate);
            COUNTS["LUI"]++;
            break;
        case 0x17://AUIPC
            sim_regs.writeReg(getrd(),(signed64)sim_regs.getPC()+immediate-4);
            if(verbose) print_ins("AUIPC", getrd(), immediate);
            COUNTS["AUIPC"]++;
            break;
        default:
            printf("undefined instruction in execute_O\n");
            
    }
}
//execute R type
void instruction::execute_R()
{
    if(getfunc7() == 0){
        switch(getfunc3()){
            case 0x00: //b000 add
                sim_regs.writeReg(getrd(), (signed64)sim_regs.readReg(getrs1())+(signed64)sim_regs.readReg(getrs2()));
                if(verbose) print_ins("ADD", getrd(), getrs1(), getrs2());
                COUNTS["ADD"]++;
                break;
            case 0x01: //b001 sll
                //sim_regs.readReg();
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) << ((sim_regs.readReg(getrs2())&0x3f)));
                //sim_regs.readReg();
                if(verbose)
                    print_ins("SLL", getrd(), getrs1(), getrs2());
                COUNTS["SLL"]++;
                break;
            case 0x02: //b010 slt
                if((signed64)sim_regs.readReg(getrs1()) < (signed64)sim_regs.readReg(getrs2()))
                    sim_regs.writeReg(getrd(), 1);
                else
                    sim_regs.writeReg(getrd(), 0);
                if(verbose) print_ins("SLT", getrd(), getrs1(), getrs2());
                COUNTS["SLT"]++;
                break;
            case 0x03: //b011 sltu
                if(sim_regs.readReg(getrs1()) < sim_regs.readReg(getrs2()))
                    sim_regs.writeReg(getrd(), 1);
                else
                    sim_regs.writeReg(getrd(), 0);
                if(verbose) print_ins("SLTU", getrd(), getrs1(), getrs2());
                COUNTS["SLTU"]++;
                break;
            case 0x04: //b100 xor
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) ^ sim_regs.readReg(getrs2()));
                if(verbose) print_ins("XOR", getrd(), getrs1(), getrs2());
                COUNTS["XOR"]++;
                break;
            case 0x05: //b101 srl
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) >> (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SRL", getrd(), getrs1(), getrs2());
                COUNTS["SRL"]++;
                break;
            case 0x06: //b110 or
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) | sim_regs.readReg(getrs2()));
                if(verbose) print_ins("OR", getrd(), getrs1(), getrs2());
                COUNTS["OR"]++;
                break;
            case 0x07: //b111 and
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) & sim_regs.readReg(getrs2()));
                if(verbose) print_ins("AND", getrd(), getrs1(), getrs2());
                COUNTS["AND"]++;
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=0\n");
                break;
        }
    }
    else if(getfunc7()==0x20){
        switch(getfunc3()){
            case 0x00: //b000 sub
                sim_regs.writeReg(getrd(), (signed64)sim_regs.readReg(getrs1()) - (signed64)sim_regs.readReg(getrs2()));
                if(verbose) print_ins("SUB", getrd(), getrs1(), getrs2());
                COUNTS["SUB"]++;
                break;
            case 0x05: //b101 sra
                sim_regs.writeReg(getrd(), (long)(sim_regs.readReg(getrs1())) >> (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SRA", getrd(), getrs1(), getrs2());
                COUNTS["SRA"]++;
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=0x20\n");
                break;
        }
    }
    else if(getfunc7()==0x01){
        // MUL, MULH, MULHSH, MULHU, DIV, DIVU, REM, REMU
        reg64 reg1_l = 0;
        reg64 reg1_h = 0;
        reg64 reg2_l = 0;
        reg64 reg2_h = 0;
        reg64 mul_h = 0;
        reg64 mul_m = 0;
        reg64 mul_l = 0;
        signed32 neg = 0;
        reg64 reg1 = 0,reg2 = 0;
        switch(getfunc3()){
            case 0x00: //b000 mul
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1())*sim_regs.readReg(getrs2()));
                if(verbose) print_ins("MUL", getrd(), getrs1(), getrs2());
                COUNTS["MUL"]++;
                break;
            case 0x01: //b001 mulh ?
                
                if((signed64)sim_regs.readReg(getrs1()) < 0)
                {
                    reg1 = ~sim_regs.readReg(getrs1())+1;
                    neg++;
                }
                else
                    reg1 = sim_regs.readReg(getrs1());
                if((signed64)sim_regs.readReg(getrs2()) < 0)
                {
                    reg2 = ~sim_regs.readReg(getrs2())+1;
                    neg++;
                }
                else
                    reg2 = sim_regs.readReg(getrs2());
                reg1_l = (reg32)reg1;
                reg1_h = reg1 >> 32;
                reg2_l = (reg32)reg2;
                reg2_h = reg2 >> 32;
                mul_h = reg1_h * reg2_h;
                mul_m = reg1_h * reg2_l + reg1_l * reg2_h;
                mul_l = reg1_l * reg2_l;
                if(neg == 0 || neg == 2)
                    sim_regs.writeReg(getrd(), (reg64) (mul_h + (mul_m >> 32)));
                else if(neg == 1)
                {
                    if((reg64) (mul_l + (mul_m << 32)) == 0)
                        sim_regs.writeReg(getrd(), (reg64) (~((mul_h + (mul_m >> 32))-1)));
                    else
                        sim_regs.writeReg(getrd(), (reg64)(~(mul_h + (mul_m >> 32))));
                }
                else;
                if(verbose) print_ins("MULH", getrd(), getrs1(), getrs2());
                COUNTS["MULH"]++;
                break;
            case 0x02: //b010 mulhsu
                reg1_l = (reg32)sim_regs.readReg(getrs1());
                reg1_h = sim_regs.readReg(getrs1()) >> 32;
                reg2_l = (reg32)sim_regs.readReg(getrs2());
                reg2_h = sim_regs.readReg(getrs2()) >> 32;
                mul_h = reg1_h * reg2_h;
                mul_m = reg1_h * reg2_l + reg1_l * reg2_h;
                sim_regs.writeReg(getrd(), (mul_h + (mul_m >> 32)));
                if(verbose) print_ins("MULHSU", getrd(), getrs1(), getrs2());
                COUNTS["MULHSU"]++;
                break;
            case 0x03: //b011 mulhu
                reg1_l = (reg32)sim_regs.readReg(getrs1());
                reg1_h = sim_regs.readReg(getrs1()) >> 32;
                reg2_l = (reg32)sim_regs.readReg(getrs2());
                reg2_h = sim_regs.readReg(getrs2()) >> 32;
                mul_h = reg1_h * reg2_h;
                mul_m = reg1_h * reg2_l + reg1_l * reg2_h;
                sim_regs.writeReg(getrd(), (mul_h + (mul_m >> 32)));
                if(verbose) print_ins("MULHU", getrd(), getrs1(), getrs2());
                COUNTS["MULHU"]++;
                break;
            case 0x04: //b100 div
                sim_regs.writeReg(getrd(), (signed64)sim_regs.readReg(getrs1()) / (signed64)sim_regs.readReg(getrs2()));
                if(verbose) print_ins("DIV", getrd(), getrs1(), getrs2());
                COUNTS["DIV"]++;
                break;
            case 0x05: //b101 divu
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) / (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("DIVU", getrd(), getrs1(), getrs2());
                COUNTS["DIVU"]++;
                break;
            case 0x06: //b110 rem
                sim_regs.writeReg(getrd(), (signed64)sim_regs.readReg(getrs1()) % (signed64)sim_regs.readReg(getrs2()));
                if(verbose) print_ins("REM", getrd(), getrs1(), getrs2());
                COUNTS["REM"]++;
                break;
            case 0x07: //b111 remu
                sim_regs.writeReg(getrd(), sim_regs.readReg(getrs1()) % sim_regs.readReg(getrs2()));
                if(verbose) print_ins("REMU", getrd(), getrs1(), getrs2());
                COUNTS["REMU"]++;
                break;
            default:
                printf("undefined instruction with opcode=011 0011, func7=1\n");
                break;
        }
    }
    else{
        printf("undefined instruction in R-type\n");
    }
}

// opcode = 3B
void instruction::execute_R64()
{
    if(getfunc7()==0x00){
        switch(getfunc3()){
            case 0x00: //b000 addw
                sim_regs.writeReg(getrd(), (signed32)sim_regs.readReg(getrs1()) + (signed32)sim_regs.readReg(getrs2()) );
                if(verbose) print_ins("ADDW", getrd(), getrs1(), getrs2());
                COUNTS["ADDW"]++;
                break;
            case 0x01: //b001 sllw
                sim_regs.writeReg(getrd(), (signed32)sim_regs.readReg(getrs1()) << (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SLLW", getrd(), getrs1(), getrs2());
                COUNTS["SLLW"]++;
                break;
            case 0x05: //b101 srlw
                sim_regs.writeReg(getrd(), (signed32)sim_regs.readReg(getrs1()) >> (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SRLW", getrd(), getrs1(), getrs2());
                COUNTS["SRLW"]++;
                break;
            default:
                printf("undefined instruction with opcode =011 1011, func7=0\n");
                break;
        }
    }
    else if(getfunc7()==0x20){
        switch(getfunc3()){
            case 0x00: //b000 subw
                sim_regs.writeReg(getrd(), (signed32)sim_regs.readReg(getrs1()) - (signed32)sim_regs.readReg(getrs2()) );
                if(verbose) print_ins("SUBW", getrd(), getrs1(), getrs2());
                COUNTS["SUBW"]++;
                break;
            case 0x05: //b101 sraw
                sim_regs.writeReg(getrd(), (signed32)(sim_regs.readReg(getrs1())) >> (sim_regs.readReg(getrs2())&0x3f));
                if(verbose) print_ins("SRAW", getrd(), getrs1(), getrs2());
                COUNTS["SRAW"]++;
                break;
            default:
                printf("undefined instruction with opcode = 011 1011, func7=0x20\n");
                break;
        }
    }
    else if(getfunc7()==0x01){
        // MULW,DIVW, DIVUW, REMW, REMUW
        switch(getfunc3()){
            case 0x00: //b000 mulw
                sim_regs.writeReg(getrd(), (reg64)((signed32)sim_regs.readReg(getrs1()) * (signed32)sim_regs.readReg(getrs2())) );
                if(verbose) print_ins("MULW", getrd(), getrs1(), getrs2());
                COUNTS["MULW"]++;
                break;
            case 0x04: //b100 divw
                sim_regs.writeReg(getrd(), (reg64)((signed32)sim_regs.readReg(getrs1()) / (signed32)sim_regs.readReg(getrs2())) );
                if(verbose) print_ins("DIVW", getrd(), getrs1(), getrs2());
                COUNTS["DIVW"]++;
                break;
            case 0x05: //b101 divuw
                sim_regs.writeReg(getrd(), (reg64)((reg32)sim_regs.readReg(getrs1()) / (reg32)sim_regs.readReg(getrs2())) );
                if(verbose) print_ins("DIVUW", getrd(), getrs1(), getrs2());
                COUNTS["DIVUW"]++;
                break;
            case 0x06: //b110 remw
                sim_regs.writeReg(getrd(), (reg64)((signed32)sim_regs.readReg(getrs1()) % (signed32)sim_regs.readReg(getrs2())) );
                if(verbose) print_ins("REMW", getrd(), getrs1(), getrs2());
                COUNTS["REMW"]++;
                break;
            case 0x07: //b111 remuw
                sim_regs.writeReg(getrd(), (reg64)((reg32)sim_regs.readReg(getrs1()) % (reg32)sim_regs.readReg(getrs2())) );
                if(verbose) print_ins("REMUW", getrd(), getrs1(), getrs2());
                COUNTS["REMUW"]++;
                break;
            default:
                printf("undefined instruction with opcode =011 1011, func7=1\n");
                break;
        }
    }
    else
        printf("undefined instruction in R64-TYPE\n");
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
            COUNTS["JAL"]++;
            break;
        case 0x67: // b110 0111
            // JALR rd, rs1, imm
            switch (getfunc3()){
                case 0:
                    sim_regs.writeReg(getrd(), sim_regs.getPC());
                    sim_regs.setPC((reg32)((sim_regs.readReg(getrs1())+immediate)&~1));
                    if(verbose) print_ins("JALR", getrd(), getrs1(), immediate);
                    COUNTS["JALR"]++;
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
                    mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), (signed8)sim_mem.get_memory_8(mem_addr));
                    if(verbose) print_ins("LB", getrd(), getrs1(), immediate);
                    COUNTS["LB"]++;
                    break;
                case 1:             // LH rd, rs1, imm
                    mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), (signed16)sim_mem.get_memory_16(mem_addr));
                    if(verbose) print_ins("LH", getrd(), getrs1(), immediate);
                    COUNTS["LH"]++;
                    break;
                case 2:             // LW rd, rs1, imm
                    mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), (signed32)sim_mem.get_memory_32(mem_addr));
                    if(verbose) print_ins("LW", getrd(), getrs1(), immediate);
                    COUNTS["LW"]++;
                    break;
                case 4:             // LBU rd, rs1, imm
                    mem_addr = sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_8(mem_addr));
                    if(verbose) print_ins("LBU", getrd(), getrs1(), immediate);
                    COUNTS["LBU"]++;
                    break;
                case 5:             // LHU rd, rs1, imm
                    mem_addr =  sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_16(mem_addr));
                    if(verbose) print_ins("LHU", getrd(), getrs1(), immediate);
                    COUNTS["LHU"]++;
                    break;
                case 6:             // LWU rd, rs1, imm
                    mem_addr = sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), sim_mem.get_memory_32(mem_addr));
                    if(verbose) print_ins("LWU", getrd(), getrs1(), immediate);
                    COUNTS["LWU"]++;
                    break;
                case 3:             // LD rd, rs1, imm
                    mem_addr = sim_regs.readReg(getrs1())+ immediate;
                    sim_regs.writeReg(getrd(), (signed64)sim_mem.get_memory_64(mem_addr));
                    if(verbose) print_ins("LD", getrd(), getrs1(), immediate);
                    COUNTS["LD"]++;
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
                    COUNTS["ADDI"]++;
                    break;
                case 2:             // SLTI rd, rs1, imm
                    if((signed64)sim_regs.readReg(getrs1())<immediate)
                        ALUZ64 = 1;
                    else
                        ALUZ64 = 0;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("SLTI", getrd(), getrs1(), immediate);
                    COUNTS["SLTI"]++;
                    break;
                case 3:             // SLTIU rd, rs1, imm
                    if(sim_regs.readReg(getrs1())<(reg64)immediate)
                        ALUZ64 = 1;
                    else
                        ALUZ64 = 0;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("SLTIU", getrd(), getrs1(), immediate);
                    COUNTS["SLTIU"]++;
                    break;
                case 4:             // XORI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())^immediate;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("XORI", getrd(), getrs1(), immediate);
                    COUNTS["XORI"]++;
                    break;
                case 6:             // ORI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())|immediate;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("ORI", getrd(), getrs1(), immediate);
                    COUNTS["ORI"]++;
                    break;
                case 7:             // ANDI rd, rs1, imm
                    ALUZ64 = sim_regs.readReg(getrs1())&immediate;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("ANDI", getrd(), getrs1(), immediate);
                    COUNTS["ANDI"]++;
                    break;
                case 1:             // SLLI rd, rs1, shamt
                    shamt = immediate & ONES(5, 0);
                    ALUZ64 = sim_regs.readReg(getrs1()) << shamt;
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose) print_ins("SLLI", getrd(), getrs1(), shamt);
                    COUNTS["SLLI"]++;
                    break;
                case 5:             // SRLI, SRAI rd, rs1, shamt
                    shamt = immediate & ONES(5, 0);
                    if(immediate & ~ONES(5,0)){ //SRAI
                        ALUZ64 = ((signed64)sim_regs.readReg(getrs1())) >> shamt;
                         COUNTS["SRAI"]++;
                    }
                    else{                   // SRLI
                        ALUZ64 = sim_regs.readReg(getrs1()) >> shamt;
                        COUNTS["SRLI"]++;
                    }
                    sim_regs.writeReg(getrd(), ALUZ64);
                    if(verbose){
                        if(immediate & ~ONES(5,0)){
                            print_ins("SRAI", getrd(), getrs1(), shamt);
                        }
                        
                        else{
                            print_ins("SRLI", getrd(), getrs1(), shamt);
                            
                        }
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
                    if(verbose) print_ins("ADDIW", getrd(), getrs1(), immediate);
                    COUNTS["ADDIW"]++;
                    break;
                case 1:             // SLLIW rd, rs1, shamt
                    shamt = immediate & ONES(4, 0);
                    ALUZ32 = ((signed32)sim_regs.readReg(getrs1())) << shamt;
                    sim_regs.writeReg(getrd(), ALUZ32);
                    if(verbose) print_ins("SLLIW", getrd(), getrs1(), shamt);
                    COUNTS["SLLIW"]++;
                    break;
                case 5:             // SRLIW, SRAIW rd, rs1, shamt
                    shamt = immediate & ONES(4, 0);
                    if(immediate & ~ONES(4,0)){ //SRAIW
                        ALUZ32 = ((signed32)sim_regs.readReg(getrs1()) >> shamt);
                        COUNTS["SRAIW"]++;
                    }
                    else{                       //SRLIW
                        ALUZ32 = ((reg32)sim_regs.readReg(getrs1())) >> shamt;
                        COUNTS["SRLIW"]++;
                    }
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
        default:
            printf("Undefined instruction in excute_I\n");
    }
    
}
//SB,SH,SW,SD rs1,rs2,imm
void instruction::execute_SX(){
    memAddress mem_addr;
    switch(getfunc3())
    {
        case 0://SB rs1, rs2, imm
            mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
            sim_mem.set_memory_8(mem_addr, (reg8)sim_regs.readReg(getrs2()));
            if(verbose) print_ins("SB", getrs1(), getrs2(), immediate);
            COUNTS["SB"]++;
            break;
        case 1://SH
            mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
            sim_mem.set_memory_16(mem_addr, (reg16)sim_regs.readReg(getrs2()));
            if(verbose) print_ins("SH", getrs1(), getrs2(), immediate);
            COUNTS["SH"]++;
            break;
        case 2://SW
            mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
            sim_mem.set_memory_32(mem_addr, (reg32)sim_regs.readReg(getrs2()));
            if(verbose) print_ins("SW", getrs1(), getrs2(), immediate);
            COUNTS["SW"]++;
            break;
        case 3://SD rs1, rs2, imm
            mem_addr = (signed64)sim_regs.readReg(getrs1())+ immediate;
            sim_mem.set_memory_64(mem_addr, (reg64)sim_regs.readReg(getrs2()));
            if(verbose) print_ins("SD", getrs1(), getrs2(), immediate);
            COUNTS["SD"]++;
            break;
        default:
            printf("Undefined excute in excute_SX\n");
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
            COUNTS["BEQ"]++;
            break;
        case 1://BNE
            if((signed64)sim_regs.readReg(getrs1())  != (signed64)sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BNE", getrs1(), getrs2(), immediate);
            COUNTS["BNE"]++;
            break;
        case 4://BLT
            if((signed64)sim_regs.readReg(getrs1())  < (signed64)sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BLT", getrs1(), getrs2(), immediate);
            COUNTS["BLT"]++;
            break;
        case 5://BGE
            if((signed64)sim_regs.readReg(getrs1())  >= (signed64)sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BGE", getrs1(), getrs2(), immediate);
            COUNTS["BGE"]++;
            break;
        case 6://BLTU
            if(sim_regs.readReg(getrs1())  < sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BLTU", getrs1(), getrs2(), immediate);
            COUNTS["BLTU"]++;
            break;
        case 7://BGEU
            if(sim_regs.readReg(getrs1())  >= sim_regs.readReg(getrs2()))
            {
                reg32 newPC;
                newPC = (reg32)(sim_regs.getPC()+immediate-4);//!!
                sim_regs.setPC(newPC);
            }
            if(verbose) print_ins("BGEU", getrs1(), getrs2(), immediate);
            COUNTS["BGEU"]++;
            break;
        default:
            printf("Undefined excute in excute_SX\n");
    }
}








