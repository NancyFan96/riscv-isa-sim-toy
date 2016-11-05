//
//  memory.hpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef memory_hpp
#define memory_hpp

#ifndef stdio
#define stdio
#include <stdio.h>
#endif

#ifndef stdlib
#define stdlib
#include <stdlib.h>
#endif

#ifndef system_h
#define system_h
#include "system.h"
#endif
#endif /* memory_hpp */

#define MEM_SIZE 1<<27           // 0x0800 0000, 128M
#define STACK_BOTTOM 0x6000000   // bottom address of stack
#define STACK_TOP    0x7ffffff   // top address of stack


/* ------- define memory ------- */
class memory {
public:
    unsigned long int size = MEM_SIZE;
    memAddress * mem_zero;
    //bool OUT_OF_MEM = false;
    
public:
    memory();
    memAddress* get_memory_p_address(memAddress offset);           // return mem_zero + offset, offset is directly from ELF
    memAddress get_memory_offset(memAddress *my_mem_Paddr);       // return my_mem_address - offset
    bool  set_memory_8(memAddress addr, reg8 value);         // if success return true, else return false
    bool  set_memory_16(memAddress addr, reg16 value);      // if success return true, else return false
    bool  set_memory_32(memAddress addr, reg32 value);      // if success return true, else return false
    bool  set_memory_64(memAddress addr, reg64 value);      // if success return true, else return false
    reg64 get_memory_64(memAddress offset);
    reg32 get_memory_32(memAddress offset);
    reg16 get_memory_16(memAddress offset);
    reg8  get_memory_8(memAddress offset);
    
};
