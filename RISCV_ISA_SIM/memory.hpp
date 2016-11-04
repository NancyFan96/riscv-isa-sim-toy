//
//  memory.hpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef memory_hpp
#define memory_hpp

#include <stdio.h>
#include "system.h"

#endif /* memory_hpp */

#define MEM_SIZE 1<<27           // 0x0800 0000
#define STACK_BOTTOM 0x6000000   // bottom address of stack


/* ------- define memory ------- */
class memory {
public:
    unsigned long int size = MEM_SIZE;
    memAddress * mem_zero;
    bool OUT_OF_MEM = false;
    
public:
    memory();
    bool  set_memory_reg8(memAddress addr, reg8 value);         // if success return true, else return false
    bool  set_memory_reg16(memAddress addr, reg16 value);      // if success return true, else return false
    bool  set_memory_reg32(memAddress addr, reg32 value);      // if success return true, else return false
    bool  set_memory_reg64(memAddress addr, reg64 value);      // if success return true, else return false
    reg64 get_memory_reg64(memAddress addr);                   // if success return true, else return -1
    reg32 get_memory_reg32(memAddress addr);                   // if success return true, else return -1
    reg16 get_memory_reg16(memAddress addr);                   // if success return true, else return -1
    reg8  get_memory_reg8(memAddress addr);                    // if success return true, else return -1
    
};
