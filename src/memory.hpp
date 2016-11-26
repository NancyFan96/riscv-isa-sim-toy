//
//  memory.hpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu Wang  on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef memory_hpp
#define memory_hpp

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "system.h"

#define MEM_SIZE 1<<28           // 0x1000 0000, 256M
#define STACK_BOTTOM 0x6000000   // bottom address of stack
#define STACK_TOP    0xfffffff   // top address of stack


/* ------- define memory ------- */
class memory {
public:
    byte * mem_zero;                    // 8bit
    std::ofstream trace_out;            //trace file
    
public:
    memory();
    byte* get_memory_p_address(memAddress offset);           // return mem_zero + offset, offset is directly from ELF
    memAddress get_memory_offset(byte *my_mem_Paddr);       // return my_mem_address - offset
    bool  set_memory_8(memAddress addr, reg8 value);         // if success return true, else return false
    bool  set_memory_16(memAddress addr, reg16 value);      // if success return true, else return false
    bool  set_memory_32(memAddress addr, reg32 value);      // if success return true, else return false
    bool  set_memory_64(memAddress addr, reg64 value);      // if success return true, else return false
    reg64 get_memory_64(memAddress offset);
    reg32 get_memory_32(memAddress offset);
    reg16 get_memory_16(memAddress offset);
    reg8  get_memory_8(memAddress offset);
    
    /* ----------------add float point instruction ----------------*/
    f32   get_memory_f32(memAddress offset);                //get float point from memory
    f64   get_memory_f64(memAddress offset);
    bool  set_memory_f32(memAddress addr,f32 value);
    bool  set_memory_f64(memAddress addr,f64 value);
    
};

extern memory sim_mem;

#endif /* memory_hpp */
