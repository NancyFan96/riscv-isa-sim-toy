//
//  register.cpp
//  RISCV_ISA_SIM
//
//  Read and write function for registers
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu Wang on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include "system.h"
#include "register.hpp"

registers sim_regs;
extern bool GDB_MODE;
registers::registers(){
    PC = 0;
    fcsr = 0;
    for(int i = 0;i < 32;i++){
        rrx[i] = 0;
        frx[i] = 0;
    }
}


void registers::readReg(){
    for(int i = 0;i<32;i++)
    {
        if(GDB_MODE && i%8==0)    printf("> ");

        if(i == sp||i== gp||i == ra ||i == fp)
            printf("rrx%d: 0x%lx\t",i ,rrx[i]);
        else
            printf("rrx%d: %lu\t",i ,rrx[i]);
        if(i%8==7)
            printf("\n");
    }
    if(GDB_MODE)    printf("> ");
    printf("PC: 0x%lx\n",(reg64)PC);
    if(GDB_MODE)    printf("> ");
    printf("\n");
}

reg64 registers::readReg(regID regDst){
    return rrx[regDst];
}

bool registers::writeReg(regID regDst, reg64 value){
    if(regDst <0 || regDst > 31 )
        return false;
    if(regDst == 0){
        //printf("Warning: $zero was tried to be written\n");
        return true;
    }
    rrx[regDst] = value;
    return true;
    
}

reg64 registers::getPC(){
    return (reg64)PC;
}

bool registers::setPC(reg64 newPC){
    PC = (byte *)newPC;
    return true;

}

// PC to next instruction
void registers::incPC(){
    PC += 4;
}

/* --------- F extension ---------- */
void registers::readFloatReg(){
    for(int i = 0;i<32;i++){
        if(GDB_MODE && i%8==0)    printf("> ");
        printf("frx%d: %lf\t",i ,frx[i]);
        if(i%8==7)
            printf("\n");
    }
    if(GDB_MODE)    printf("> ");
    printf("FCSR: 0x%x\n",fcsr);
    if(GDB_MODE)    printf("> ");
    printf("\n");
}

f64 registers::readFloatRegD(regID regDst){
    return frx[regDst];
}

f32 registers::readFloatRegS(regID regDst){
    f32 * value_f32_ptr = (f32 *)(&(frx[regDst]));
    return *(value_f32_ptr);
}

bool registers::writeFloatRegD(regID regDst, f64 value){
    if(regDst <0 || regDst > 31 )
        return false;
    if(regDst == 0){
        //printf("Warning: $zero was tried to be written\n");
        return true;
    }
    frx[regDst] = value;
    return true;
    
}
bool registers::writeFloatRegS(regID regDst, f32 value){
    if(regDst <0 || regDst > 31 )
        return false;
    if(regDst == 0){
        //printf("Warning: $zero was tried to be written\n");
        return true;
    }
    f32 * value_f32_ptr = (f32 *)(&(frx[regDst]));
    * value_f32_ptr = value;
    return true;
    
}


reg32 registers::getFCSR(){ //luyao
    return fcsr;
}

bool registers::setFCSR(reg32 newFCSR){ //luyao
    fcsr = newFCSR;
    return true;
}
