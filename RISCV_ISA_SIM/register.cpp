//
//  register.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include "system.h"
#include "register.hpp"

registers sim_regs;

registers::registers(){
    PC = 0;
    for(int i = 0;i < 32;i++)
        rrx[i] = 0;
}

// for debug
void registers::readReg(){
    for(int i = 0;i<32;i++)
    {
        if(i == sp||i== gp)
            printf("rrx%d: 0x%lx\t",i ,rrx[i]);
        else
            printf("rrx%d: %lu\t",i ,rrx[i]);
        if(i == 7 || i == 15 || i== 23 || i== 31)
            printf("\n");
    }
    printf("PC: 0x%lx\n\n",(reg64)PC);
}

reg64 registers::readReg(regID regDst){
    return rrx[regDst];
}

bool registers::writeReg(regID regDst, reg64 value){
    if(regDst <0 || regDst > 31 )
        return false;
    if(regDst == 0)
    {
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

    //else{
    //    printf("PC invalid! Oops!\n");
    //   return false;
    //}
}

// PC to next instruction
void registers::incPC(){
    PC += 4;
}
