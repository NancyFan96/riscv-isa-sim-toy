//
//  memory.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include "memory.hpp"

memory::memory()//initiaiize
{
    mem_zero=(byte *)malloc(sizeof(char)*size);
}

byte* memory::get_memory_p_address(memAddress offset)
{
    return (mem_zero+offset);
}

memAddress memory::get_memory_offset(byte *my_mem_Paddr){
    return (memAddress)(my_mem_Paddr-mem_zero); // 64bit - 64bit
}


bool  memory::set_memory_8(memAddress addr, reg8 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    
    reg8* p_u8=(reg8*)v_addr;
    *p_u8=value;
    return true;
}
bool  memory::set_memory_16(memAddress addr, reg16 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    
    reg16* p_u16=(reg16*)v_addr;
    *p_u16=value;
    return true;
}
bool  memory::set_memory_32(memAddress addr, reg32 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    reg32* p_u32=(reg32*)v_addr;
    *p_u32=value;
    return true;
    
}
bool  memory::set_memory_64(memAddress addr, reg64 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("unvalid memory address\n");
        return false;
    }
    reg64* p_u64=(reg64*)v_addr;
    *p_u64=value;
    return true;
}
reg64 memory::get_memory_64(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return -1;
    }
    reg64* p_u64=(reg64*)v_addr;
    return (*p_u64);
    
}
reg32 memory::get_memory_32(memAddress addr)
{
    {
        byte* v_addr=get_memory_p_address(addr);
        if(v_addr==NULL)
        {
            printf("invalid memory address\n");
            return -1;
        }
        reg32* p_u32=(reg32*)v_addr;
        return (*p_u32);
        
    }
}
reg16 memory::get_memory_16(memAddress addr)
{
    {
        byte* v_addr=get_memory_p_address(addr);
        if(v_addr==NULL)
        {
            printf("invalid memory address\n");
            return -1;
        }
        reg16* p_u16=(reg16*)v_addr;
        return (*p_u16);
        
    }
}
reg8  memory::get_memory_8(memAddress addr)
{
    {
        byte* v_addr=get_memory_p_address(addr);
        if(v_addr==NULL)
        {
            printf("invalid memory address\n");
            return -1;
        }
        reg8* p_u8=(reg8*)v_addr;
        return (*p_u8);
        
    }
}
