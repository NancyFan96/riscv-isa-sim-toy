//
//  register.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include "register.hpp"

registers::registers(){
    PC = 0;
    for(int i = 0;i < 32;i++)
        rrx[i] = 0;
}

// for debug
void registers::readReg(){
    for(int i = 0;i<32;i++)
    {
        printf("rrx %d :%lu	",i ,rrx[i]);
        if(i == 7 || i == 15 || i== 23 || i== 31)
            printf("\n");
    }
    printf("PC: %lu\n",(reg64)PC);
}

reg64 registers::readReg(regID regDst){
    return rrx[regDst];
}

bool registers::writeReg(regID regDst, reg64 value){
    if(regDst <0 || regDst > 31 )
        return false;
    if(regDst == 0)
    {
        printf("Warning: $zero was tried to be written\n");
        return true;
    }
    rrx[regDst] = value;
    return true;
    
}

reg32 registers::getPC(){
    reg32 PC_value =(reg32)(unsigned long)PC;
    return PC_value;
}

bool registers::setPC(reg32 newPC){
    //need PC range
    reg64 new_PC_value = (unsigned long)newPC;
    PC = (byte *)new_PC_value;
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
