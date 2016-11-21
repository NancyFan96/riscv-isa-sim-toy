//
//  execute_F.cpp
//  RISCV_ISA_SIM
//
//  execution for RISCV64F (only for std tests)
//
//  Created by Nancy Fan, Yang Kejing, Wang Luyao  on 16/11/11.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include <string.h>
#include <map>
#include <string>
#include <math.h>
#include "system.h"
#include "decode.hpp"
#include "register.hpp"
#include "memory.hpp"

extern std::map<std::string,int> COUNTS;
extern bool IS_DYCOUNT;

void instruction::execute_R4(){
    //FMADD.D rs1 * rs2 + rs3;
    if(getfunc2() == 1){
        f64 f64Z = sim_regs.readFloatRegD(rs1)* sim_regs.readFloatRegD(rs2)+ sim_regs.readFloatRegD(rs3);
        sim_regs.writeFloatRegD(rd, f64Z);
        if(verbose) print_ins("FMADD.D", rd, rs1, rs2, rs3);
        if(IS_DYCOUNT)COUNTS["FMADD_D"]++;
    }
    else{
        printf("Warning: undefined instruction in R4 type\n");
    }
    
}


void instruction::execute_FExt(){
    if(opcode == 0x07){
        // FLW, FLD rd, rs1, imm, from mem to rd
        memAddress mem_addr;
        if(getfunc3()==2){//FLW
            mem_addr = sim_regs.readReg(rs1)+immediate;
            sim_regs.writeFloatRegS(rd, sim_mem.get_memory_f32(mem_addr));
            if(verbose) print_ins("FLW", rd, rs1, immediate);
            if(IS_DYCOUNT)COUNTS["FLW"]++;
        }
        else if(getfunc3()==3){
            mem_addr = sim_regs.readReg(rs1)+immediate;
            sim_regs.writeFloatRegD(rd, sim_mem.get_memory_f64(mem_addr));
            if(verbose) print_ins("FLD", rd, rs1, immediate);
            if(IS_DYCOUNT)COUNTS["FLD"]++;
        }
        else{
            printf("undefined instruction with opcode = 0x07\n");
        }
    }
    else if(opcode == 0x27){
        // FSW, FSD rs1, rs2, imm, from rs2 to mem
        memAddress mem_addr;
        if(getfunc3()==2){//FSW
            mem_addr = sim_regs.readReg(rs1)+immediate;
            sim_mem.set_memory_f32(mem_addr, sim_regs.readFloatRegS(rs2));
            if(verbose) print_ins("FSW", rs1, rs2, immediate);
            if(IS_DYCOUNT)COUNTS["FSW"]++;
        }
        else if(getfunc3()==3){//FSD
            mem_addr = sim_regs.readReg(rs1)+immediate;
            sim_mem.set_memory_f64(mem_addr, sim_regs.readFloatRegD(rs2));
            if(verbose) print_ins("FSD", rs1, rs2, immediate);
            if(IS_DYCOUNT)COUNTS["FSD"]++;
        }
        else{
            printf("undefined instruction with opcode = 0x27\n");
        }
    }
    else if(opcode == 0x53){
        // XXX rd, rs1, rs2
        switch (getfunc7()) {
            case 0:
                // FADD.S
                sim_regs.writeFloatRegS(rd, sim_regs.readFloatRegS(rs1)+sim_regs.readFloatRegS(rs2));
                if(verbose) print_ins("FADD.S", rd, rs1, rs2);
                if(IS_DYCOUNT)COUNTS["FADD_S"]++;
                break;
            case 1:
                // FADD.D
                sim_regs.writeFloatRegD(rd, sim_regs.readFloatRegD(rs1)+sim_regs.readFloatRegD(rs2));
                if(verbose) print_ins("FADD.D", rd, rs1, rs2);
                if(IS_DYCOUNT)COUNTS["FADD_D"]++;
                break;
            case 4:
                // FSUB.S
                sim_regs.writeFloatRegS(rd, sim_regs.readFloatRegS(rs1)-sim_regs.readFloatRegS(rs2));
                if(verbose) print_ins("FSUB.S", rd, rs1, rs2);
                if(IS_DYCOUNT)COUNTS["FSUB_S"]++;
                break;
            case 5:
                // FSUB.D
                sim_regs.writeFloatRegD(rd, sim_regs.readFloatRegD(rs1)-sim_regs.readFloatRegD(rs2));
                if(verbose) print_ins("FSUB.D", rd, rs1, rs2);
                if(IS_DYCOUNT)COUNTS["FSUB_D"]++;
                break;
            case 8:
                // FMUL.S
                sim_regs.writeFloatRegS(rd, sim_regs.readFloatRegS(rs1)*sim_regs.readFloatRegS(rs2));
                if(verbose) print_ins("FMUL.S", rd, rs1, rs2);
                if(IS_DYCOUNT)COUNTS["FMUL_S"]++;
                break;
            case 9:
                // FMUL.D
                sim_regs.writeFloatRegD(rd, sim_regs.readFloatRegD(rs1)*sim_regs.readFloatRegD(rs2));
                if(verbose) print_ins("FMUL.D", rd, rs1, rs2);
                if(IS_DYCOUNT)COUNTS["FMUL_D"]++;
                break;
            case 12:
                // FDIV.S
                sim_regs.writeFloatRegS(rd, sim_regs.readFloatRegS(rs1)/sim_regs.readFloatRegS(rs2));
                if(verbose) print_ins("FDIV.S", rd, rs1, rs2);
                if(IS_DYCOUNT)COUNTS["FDIV_S"]++;
                break;
            case 13:
                // FDIV.D
                sim_regs.writeFloatRegD(rd, sim_regs.readFloatRegD(rs1)/sim_regs.readFloatRegD(rs2));
                if(verbose) print_ins("FDIV.D", rd, rs1, rs2);
                if(IS_DYCOUNT)COUNTS["FDIV_D"]++;
                break;
            case 0x50:
                // FEQ.S, FLT.S, FLE.S
                if(getfunc3()==2){//FEQ
                    if(((sim_regs.readFloatRegS(rs1)-sim_regs.readFloatRegS(rs2))<FDIFF) && ((sim_regs.readFloatRegS(rs2)-sim_regs.readFloatRegS(rs1)) < FDIFF))
                        sim_regs.writeReg(rd, 1);
                    else
                        sim_regs.writeReg(rd, 0);
                    if(verbose) print_ins("FEQ.S", rd, rs1, rs2);
                    if(IS_DYCOUNT)COUNTS["FEQ_S"]++;
                }
                else if(getfunc3()==1){//FLT
                    if(sim_regs.readFloatRegS(rs1)<sim_regs.readFloatRegS(rs2))
                        sim_regs.writeReg(rd, 1);
                    else
                        sim_regs.writeReg(rd, 0);
                    if(verbose) print_ins("FLT.S", rd, rs1, rs2);
                    if(IS_DYCOUNT)COUNTS["FLT_S"]++;
                }
                else if(getfunc3()==0){//FLE
                    if((sim_regs.readFloatRegS(rs1)-sim_regs.readFloatRegS(rs2))<=FDIFF)
                        sim_regs.writeReg(rd, 1);
                    else
                        sim_regs.writeReg(rd, 0);
                    if(verbose) print_ins("FLE.S", rd, rs1, rs2);
                    if(IS_DYCOUNT)COUNTS["FLE_S"]++;
                }
                else{
                    printf("undefined instruction with opcode=0x53, func7=0x50\n");
                }
                break;
            case 0x51:
                // FEQ.D, FLT.D, FLE.D
                if(getfunc3()==2){//FEQ
                    if(((sim_regs.readFloatRegD(rs1)-sim_regs.readFloatRegD(rs2))<FDIFF)&&((sim_regs.readFloatRegD(rs2)-sim_regs.readFloatRegD(rs1))<FDIFF))
                        sim_regs.writeReg(rd, 1);
                    else
                        sim_regs.writeReg(rd, 0);
                    if(verbose) print_ins("FEQ.D", rd, rs1, rs2);
                    if(IS_DYCOUNT)COUNTS["FEQ_D"]++;
                }
                else if(getfunc3()==1){//FLT
                    if(sim_regs.readFloatRegD(rs1)<sim_regs.readFloatRegD(rs2))
                        sim_regs.writeReg(rd, 1);
                    else
                        sim_regs.writeReg(rd, 0);
                    if(verbose) print_ins("FLT.D", rd, rs1, rs2);
                    if(IS_DYCOUNT)COUNTS["FLT_D"]++;
                }
                else if(getfunc3()==0){//FLE
                    if((sim_regs.readFloatRegD(rs1)<sim_regs.readFloatRegD(rs2))||(sim_regs.readFloatRegD(rs1)-sim_regs.readFloatRegD(rs2))<FDIFF)
                        sim_regs.writeReg(rd, 1);
                    else
                        sim_regs.writeReg(rd, 0);
                    if(verbose) print_ins("FLE.D", rd, rs1, rs2);
                    if(IS_DYCOUNT)COUNTS["FLE_D"]++;
                }
                else{
                    printf("undefined instruction with opcode=0x53, func7=0x51\n");
                }
                break;
            /*--------------------------- F&D extension convert instruction begin ---------------------------*/
            case 0x11:
                //FSGNJ.D FSGNJN.D rd, rs1
                if(getfunc3() == 0){// FSGNJ.D
                    sim_regs.writeFloatRegD(rd,sim_regs.readFloatRegD(rs1));
                    if(verbose) print_ins("FSGNJ.D", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FSGNJ_D"]++;
                }
                else if(getfunc3() == 1){
                    sim_regs.writeFloatRegD(rd,0.0-sim_regs.readFloatRegD(rs1));
                    if(verbose) print_ins("FSGNJN.D", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FSGNJN_D"]++;
               }
                else{
                    printf("undefined instruction in FExt, func7 = 0x11\n");
                }                break;
            case 0x20:
                //FCVT.S.D rd,rs1 double ==> float
                if(rs2 == 1){
                    sim_regs.writeFloatRegS(rd,(f32)sim_regs.readFloatRegD(rs1));
                    if(verbose) print_ins("FCVT.S.D", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_S_D"]++;
                }
                else{
                    printf("undefined instruction in FExt, func7 = 0x20\n");
                }
                break;
            case 0x21:
                //FCVT.D.S rd,rs1 float ==> double
                if(rs2 == 0){
                    sim_regs.writeFloatRegD(rd,(f64)sim_regs.readFloatRegS(rs1));
                    if(verbose) print_ins("FCVT.D.S", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_D_S"]++;
                }
                else{
                    printf("undefined instruction in FExt, func7 = 0x21\n");
                }
                break;
            case 0x60:
                // 0        FCVT.W.S rd, rs1    -- float rs1 ==> signed32 rd
                // 1        FCVT.WU.S rd, rs1   -- float rs1 ==> reg32 rd     NO_USE
                // (10)2    FCVT.L.S rd, rs1    -- float rs1 ==> signed64 rd
                // (11)2    FCVT.LU.S rd, rs1   -- float rs1 ==> reg64 rd     NO_USE
                if(rs2 == 0){//w.s
                    f32 value = sim_regs.readFloatRegS(rs1);
                    sim_regs.writeReg(rd, (signed32)value);
                    if(verbose) print_ins("FCVT.W.S", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_W_S"]++;
                }
                /*else if(rs2 == 1){//wu.s
                    f32 value = (f32)sim_regs.readFloatRegS(rs1);
                    sim_regs.writeReg(rd, (reg32)value);
                    if(verbose) print_ins("FCVT.WU.S", rd, rs1);
                }*/
                else if(rs2 == 2){ //l.s
                    f32 value = sim_regs.readFloatRegS(rs1);
                    sim_regs.writeReg(rd, (signed64)value);
                    if(verbose)  print_ins("FCVT.L.S", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_L_S"]++;
                }
                /*else if(rs2 == 3){//lu.s
                    f32 value = (f32)sim_regs.readFloatRegS(rs1);
                    sim_regs.writeReg(rd, (reg64)value);
                    if(verbose) print_ins("FCVT.LU.S", rd, rs1);
                }*/
                else{
                    printf("undefined instruction in FExt, func7 = 0x60\n");
                }
                break;
            case 0x61:
                // 0        FCVT.W.D rd, rs1    -- double rs1 ==> signed32 rd
                // 1        FCVT.WU.D rd, rs1   -- double rs1 ==> reg32 rd     NO_USE
                // (10)2    FCVT.L.D rd, rs1    -- double rs1 ==> signed64 rd
                // (11)2    FCVT.LU.D rd, rs1   -- double rs1 ==> reg64 rd     NO_USE
                if(rs2 == 0){//FCVT.W.D rd, rs1  double ==> signed32
                    f64 value = sim_regs.readFloatRegD(rs1);
                    sim_regs.writeReg(rd, (signed32)value);
                    if(verbose) print_ins("FCVT.W.D", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_W_D"]++;
                }
                /*else if(rs2 == 1){//FCVT.WU.D rd, rs1  double ==> reg32
                    f64 value = sim_regs.readFloatRegD(rs1);
                    sim_regs.writeReg(rd, (reg32)value);
                    if(verbose) print_ins("FCVT.WU.D", rd, rs1);
                }*/
                else if(rs2 == 2){//FCVT.L.D rd, rs1  double ==> signed64
                    f64 value = sim_regs.readFloatRegD(rs1);
                    sim_regs.writeReg(rd, (signed64)value);
                    if(verbose) print_ins("FCVT.L.D", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_L_D"]++;
                }
                /*else if(rs2 == 3){//FCVT.LU.D rd, rs1  double ==> reg64
                    f64 value = sim_regs.readFloatRegD(rs1);
                    sim_regs.writeReg(rd, (reg64)value);
                    if(verbose) print_ins("FCVT.LU.D", rd, rs1);
                }*/
                else{
                    printf("undefined instruction in FExt, func7 = 0x61\n");
                }
                break;
            case 0x68:
                // 0        FCVT.S.W rd, rs1    -- signed32 rs1 ==> float rd
                // 1        FCVT.S.WU rd, rs1   -- reg32 rs1 ==> float rd       NO_USE
                // (10)2    FCVT.S.L rd, rs1    -- signed64 rs1 ==> float rd
                // (11)2    FCVT.S.LU rd, rs1   -- reg64 rs1 ==> float rd       NO_USE
                if(rs2 == 0) //s.w
                {
                    sim_regs.writeFloatRegS(rd,(f32)(signed32)sim_regs.readReg(rs1));
                    if(verbose) print_ins("FCVT.S.W", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_S_W"]++;
                }
                else if(rs2 == 2){ //s.l
                    sim_regs.writeFloatRegS(rd,(f32)(signed64)sim_regs.readReg(rs1));
                    if(verbose) print_ins("FCVT.S.L", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_S_L"]++;
                }
                else{
                    printf("undefined instruction in FExt, func7 = 0x68\n");
                }
                break;
            case 0x69:
                // 0        FCVT.D.W rd, rs1    -- signed32  ==> double
                // 1        FCVT.D.WU rd, rs1   -- reg32  ==> double
                // (10)2    FCVT.D.L rd, rs1    -- signed64 rs1 ==> double rd     NO_USE
                // (11)2    FCVT.D.LU rd, rs1   -- reg64 rs1 ==> double rd        NO_USE
                if(rs2 == 0){//FCVT.D.W
                    sim_regs.writeFloatRegD(rd,(f64)(signed64)sim_regs.readReg(rs1));
                    if(verbose) print_ins("FCVT.D.W", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_D_W"]++;
                }
                else if(rs2 == 1){//FCVT.D.WU
                    sim_regs.writeFloatRegD(rd,(f64)(reg64)sim_regs.readReg(rs1));
                    if(verbose) print_ins("FCVT.D.WU", rd, rs1);
                    if(IS_DYCOUNT)COUNTS["FCVT_D_WU"]++;
                    printf("unused FCVT.D.WU");
                }
                else{
                    printf("undefined instruction in FExt, func7 = 0x69\n");
                }
                break;                
            case 0x71: //round = 000
                //FMV.X.D  floating-point register rs1 -> IEEE 754-2008 integer register rd; rs1 single
                if(rs2 == 0){
                    f64 f_value =  sim_regs.readFloatRegD(rs1);      //read float point from float reg
                    f64* fptr = (f64*)malloc(sizeof(f64)*1);
                    *fptr = f_value;
                    reg64 value = *((reg64*)fptr);
                    sim_regs.writeReg(rd,value);
                    if(IS_DYCOUNT)COUNTS["FMV_X_D"]++;
                }
                else{
                    printf("undefined instruction in FExt\n");
                }
                break;
            case 0x79: //round = 000
                //FMV.D.X IEEE 754-2008 integer register rs1 -> the floating-point register rd.
                if(rs2 == 0){
                    reg64 int_value = sim_regs.readReg(rs1);
                    reg64* intp = (reg64 *)malloc(sizeof(reg64)*1);
                    *intp = int_value;
                    f64 value = *((f64*)intp);
                    sim_regs.writeFloatRegD(rd,value);
                    if(IS_DYCOUNT)COUNTS["FMV_D_X"]++;
                }
                else{
                    printf("undefined instruction in FExt\n");
                }
                break;
        }
    }
    else{
        printf("undefined instruction in FExt\n");
    }
    
}

