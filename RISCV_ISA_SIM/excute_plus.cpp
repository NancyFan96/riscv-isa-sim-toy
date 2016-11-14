//
//  excute_plus.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/11.
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

void instruction::execute_R4(){
    //FMADD.D rs1 * rs2 + rs3;
    xcode rm = ((sim_regs.getFCSR()&FUNCT3) >> 12);
    if(getfunc2() == 1)
    {
        
        switch(getfunc3())
        {
            case 0: //Round to Nearest, ties to Even
                sim_regs.writeFloatReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                break;
            case 1: //Round towards Zero
                sim_regs.writeFloatReg(getrd(),(int)(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                break;
            case 2: //Round Down (towards -wuqiong)
                sim_regs.writeFloatReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                break;
            case 3: //Round Up (towards +wuqiong)
                sim_regs.writeFloatReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                break;
            case 4: //Round to Nearest, ties to Max Magnitude
                sim_regs.writeFloatReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                break;
            case 7:
                switch(rm){
                    case 0: //Round to Nearest, ties to Even
                        
                        sim_regs.writeFloatReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                        break;
                    case 1: //Round towards Zero
                        sim_regs.writeFloatReg(getrd(),(int)(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                        break;
                    case 2: //Round Down (towards -wuqiong)
                        sim_regs.writeFloatReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                        break;
                    case 3: //Round Up (towards +wuqiong)
                        sim_regs.writeFloatReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                        break;
                    case 4: //Round to Nearest, ties to Max Magnitude
                        sim_regs.writeFloatReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())*sim_regs.readFloatReg(getrs2()) + sim_regs.readFloatReg(getrs3())));
                        break;
                    case 5:
                    case 6:
                    case 7://
                        printf("undefined instruction in R4.Invaild rounding mode!\n");
                        break;
                }
                break;
            default:
                printf("undefined instruction in R4.Invaild rounding mode!\n");
                
        }
    }
    
    
}


void instruction::execute_FExt(){
    if(opcode == 0x07){
        // FLW, FLD rd, rs1, imm, from mem to rd
        memAddress mem_addr;
        if(getfunc3()==2){//FLW
            mem_addr = sim_regs.readReg(getrs1())+immediate;
            sim_regs.writeFloatReg(getrd(), (f32)sim_mem.get_memory_f32(mem_addr));
            if(verbose) print_ins("FLW", getrd(), getrs1(), immediate);
        }
        else if(getfunc3()==3){
            mem_addr = sim_regs.readReg(getrs1())+immediate;
            sim_regs.writeFloatReg(getrd(), (f64)sim_mem.get_memory_f64(mem_addr));
            if(verbose) print_ins("FLD", getrd(), getrs1(), immediate);
        }
        else{
            printf("undefined instruction with opcode = 0x07\n");
        }
    }
    else if(opcode == 0x27){
        // FSW, FSW rs1, rs2, imm, from rs2 to mem
        memAddress mem_addr;
        if(getfunc3()==2){//FSW
            mem_addr = sim_regs.readReg(getrs1())+immediate;
            sim_mem.set_memory_f32(mem_addr, (f32)sim_regs.readFloatReg(getrs2()));
            if(verbose) print_ins("FSW", getrs1(), getrs2(), immediate);
        }
        else if(getfunc3()==3){
            mem_addr = sim_regs.readReg(getrs1())+immediate;
            sim_mem.set_memory_f64(mem_addr, sim_regs.readFloatReg(getrs2()));
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
        xcode rm = ((sim_regs.getFCSR()&FUNCT3) >> 12);
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
                    if(((f32)sim_regs.readFloatReg(getrs1())-(f32)sim_regs.readFloatReg(getrs2()))<FDIFF)
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FEQ.S", getrd(), getrs1(), getrs2());
                }
                else if(getfunc3()==1){//FLT
                    if((f32)sim_regs.readFloatReg(getrs1())<(f32)sim_regs.readFloatReg(getrs2()))
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FLT.S", getrd(), getrs1(), getrs2());
                }
                else if(getfunc3()==0){//FLE
                    if((f32)sim_regs.readFloatReg(getrs1())<=(f32)sim_regs.readFloatReg(getrs2()))
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FLE.S", getrd(), getrs1(), getrs2());
                }
                else{
                    printf("undefined instruction with opcode=0x53, func7=0x50\n");
                }
                break;
            case 0x51:
                // FEQ.D, FLT.D, FLE.D
                if(getfunc3()==2){//FEQ
                    if((sim_regs.readFloatReg(getrs1())-sim_regs.readFloatReg(getrs2()))<FDIFF)
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
                    if((sim_regs.readFloatReg(getrs1())<sim_regs.readFloatReg(getrs2()))||(sim_regs.readFloatReg(getrs1())-sim_regs.readFloatReg(getrs2()))<FDIFF)
                        sim_regs.writeReg(getrd(), 1);
                    else
                        sim_regs.writeReg(getrd(), 0);
                    if(verbose) print_ins("FLE.D", getrd(), getrs1(), getrs2());
                }
                else{
                    printf("undefined instruction with opcode=0x53, func7=0x51\n");
                }
                break;
            case 0x60:
                //FCVT.L.S rs1,rs2 FCVT.W.S single float -> 32/64 signed integer //notice: set fcsr
                
                if(getrs2() == 0)
                {//w.s
                    switch(getfunc3())
                    {
                        case 0: //Round to Nearest, ties to Even
                            
                            sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 1: //Round towards Zero
                            sim_regs.writeReg(getrd(),(int)sim_regs.readFloatReg(getrs1()));
                            break;
                        case 2: //Round Down (towards -wuqiong)
                            sim_regs.writeReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 3: //Round Up (towards +wuqiong)
                            sim_regs.writeReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 4: //Round to Nearest, ties to Max Magnitude
                            sim_regs.writeReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 5:
                        case 6: //
                            printf("undefined instruction with opcode=0x53, func7=0x60.Invaild rounding mode!\n");
                            break;
                        case 7:
                            
                            switch(rm)
                        {
                            case 0: //Round to Nearest, ties to Even
                                sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 1: //Round towards Zero
                                sim_regs.writeReg(getrd(),(int)sim_regs.readFloatReg(getrs1()));
                                break;
                            case 2: //Round Down (towards -wuqiong)
                                sim_regs.writeReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 3: //Round Up (towards +wuqiong)
                                sim_regs.writeReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 4: //Round to Nearest, ties to Max Magnitude
                                sim_regs.writeReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())));
                                break;
                            default: //fcsr
                                printf("undefined instruction with opcode=0x53, func7=0x60.Invaild rounding mode!\n");
                        }
                            break;
                        default:
                            printf("undefined instruction with opcode=0x53, func7=0x60.\n");
                            
                    }
                }
                else if(getrs2() == 2) //l.s
                {
                    switch(getfunc3())
                    {
                        case 0: //Round to Nearest, ties to Even
                            
                            sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 1: //Round towards Zero
                            sim_regs.writeReg(getrd(),(int)sim_regs.readFloatReg(getrs1()));
                            break;
                        case 2: //Round Down (towards -wuqiong)
                            sim_regs.writeReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 3: //Round Up (towards +wuqiong)
                            sim_regs.writeReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 4: //Round to Nearest, ties to Max Magnitude
                            sim_regs.writeReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 5:
                        case 6: //
                            printf("undefined instruction with opcode=0x53, func7=0x60.Invaild rounding mode!\n");
                            break;
                        case 7:
                            switch(rm)
                        {
                            case 0: //Round to Nearest, ties to Even
                                sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 1: //Round towards Zero
                                sim_regs.writeReg(getrd(),(int)sim_regs.readFloatReg(getrs1()));
                                break;
                            case 2: //Round Down (towards -wuqiong)
                                sim_regs.writeReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 3: //Round Up (towards +wuqiong)
                                sim_regs.writeReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 4: //Round to Nearest, ties to Max Magnitude
                                sim_regs.writeReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())));
                                break;
                            default: //fcsr
                                printf("undefined instruction with opcode=0x53, func7=0x60.Invaild rounding mode!\n");
                        }
                            break;
                        default:
                            printf("undefined instruction with opcode=0x53, func7=0x60.\n");
                            
                    }
                }
                break;
            case 0x68:
                //FCVT.S.L rs1,rs2 FCVT.S.W 32/64 signed integer -> single float
                if(getrs2() == 0) //s.w
                {
                    sim_regs.writeFloatReg(getrd(),(f32)sim_regs.readReg(getrs1()));
                }
                else if(getrs2() == 2) //s.l
                    sim_regs.writeFloatReg(getrd(),(f32)sim_regs.readReg(getrs1()));
                break;
            case 0x11:
                //FSGNJ.D FSGNJN.D
                if(getfunc3() == 0)// FSGNJ.D
                    sim_regs.writeFloatReg(getrd(),sim_regs.readFloatReg(getrs1()));
                else if(getfunc3() == 1)
                    sim_regs.writeFloatReg(getrd(),0.0-sim_regs.readFloatReg(getrs1()));
                break;
            case 0x20:
                //FCVT.S.D rs1,rs2 double-precision -> single-precision
                if(getrs2() == 1)
                    sim_regs.writeFloatReg(getrd(),(f32)sim_regs.readFloatReg(getrs1()));
                break;
            case 0x21:
                //FCVT.D.S rs1,rs2 single-precision -> double-precision never round
                if(getrs2() == 0)
                    sim_regs.writeFloatReg(getrd(),(f32)sim_regs.readFloatReg(getrs1()));
                break;
            case 0x61:
                //FCVT.W.D FCVT.WU.D  double-precision -> 32 (un)signed integer
                if(getrs2() == 0)//FCVT.W.D
                {
                    switch(getfunc3())
                    {
                        case 0: //Round to Nearest, ties to Even
                            
                            sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 1: //Round towards Zero
                            sim_regs.writeReg(getrd(),(int)sim_regs.readFloatReg(getrs1()));
                            break;
                        case 2: //Round Down (towards -wuqiong)
                            sim_regs.writeReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 3: //Round Up (towards +wuqiong)
                            sim_regs.writeReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 4: //Round to Nearest, ties to Max Magnitude
                            sim_regs.writeReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 5:
                        case 6: //
                            printf("undefined instruction with opcode=0x53, func7=0x60.Invaild rounding mode!\n");
                            break;
                        case 7:
                            switch(rm)
                        {
                            case 0: //Round to Nearest, ties to Even
                                sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 1: //Round towards Zero
                                sim_regs.writeReg(getrd(),(int)sim_regs.readFloatReg(getrs1()));
                                break;
                            case 2: //Round Down (towards -wuqiong)
                                sim_regs.writeReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 3: //Round Up (towards +wuqiong)
                                sim_regs.writeReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 4: //Round to Nearest, ties to Max Magnitude
                                sim_regs.writeReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())));
                                break;
                            default: //fcsr
                                printf("undefined instruction with opcode=0x53, func7=0x60.Invaild rounding mode!\n");
                        }
                            break;
                        default:
                            printf("undefined instruction with opcode=0x53, func7=0x60.\n");
                            
                    }
                }
                if(getrs2() == 1)//FCVT.WU.D
                {
                    switch(getfunc3())
                    {
                        case 0: //Round to Nearest, ties to Even
                            
                            sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 1: //Round towards Zero
                            sim_regs.writeReg(getrd(),(int)sim_regs.readFloatReg(getrs1()));
                            break;
                        case 2: //Round Down (towards -wuqiong)
                            sim_regs.writeReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 3: //Round Up (towards +wuqiong)
                            sim_regs.writeReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 4: //Round to Nearest, ties to Max Magnitude
                            sim_regs.writeReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())));
                            break;
                        case 5:
                        case 6: //
                            printf("undefined instruction with opcode=0x53, func7=0x60.Invaild rounding mode!\n");
                            break;
                        case 7:
                            switch(rm)
                        {
                            case 0: //Round to Nearest, ties to Even
                                sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 1: //Round towards Zero
                                sim_regs.writeReg(getrd(),(int)sim_regs.readFloatReg(getrs1()));
                                break;
                            case 2: //Round Down (towards -wuqiong)
                                sim_regs.writeReg(getrd(),(signed64)floor(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 3: //Round Up (towards +wuqiong)
                                sim_regs.writeReg(getrd(),(signed64)ceil(sim_regs.readFloatReg(getrs1())));
                                break;
                            case 4: //Round to Nearest, ties to Max Magnitude
                                sim_regs.writeReg(getrd(),(signed64)round(sim_regs.readFloatReg(getrs1())));
                                break;
                            default: //fcsr
                                printf("undefined instruction with opcode=0x53, func7=0x60.Invaild rounding mode!\n");
                        }
                            break;
                        default:
                            printf("undefined instruction with opcode=0x53, func7=0x60.\n");
                            
                    }
                }
                break;
            case 0x69:
                //FCVT.D.W FCVT.D.WU 32/64 (un)signed integer -> double-precision//is unaffected by round
                if(getrs2() == 0)//FCVT.D.W
                    sim_regs.writeFloatReg(getrd(),(f32)sim_regs.readReg(getrs1()));
                if(getrs2() == 1)//FCVT.D.WU
                    sim_regs.writeFloatReg(getrd(),(f32)sim_regs.readReg(getrs1()));
                break;
            case 0x71: //round = 000
                //FMV.X.D  floating-point register rs1 -> IEEE 754-2008 integer register rd; rs1 single
                if(getrs2() == 0)
                    sim_regs.writeReg(getrd(),RNE(sim_regs.readFloatReg(getrs1())));
                break;
            case 0x79: //round = 000
                //FMV.D.X IEEE 754-2008 integer register rs1 -> the floating-point register rd.
                if(getrs2() == 0)
                    sim_regs.writeFloatReg(getrd(),(f32)sim_regs.readReg(getrs1()));
                break;
            default:
                break;
        }
    }
    else{
        printf("undefined instruction in FExt\n");
    }
    
}

