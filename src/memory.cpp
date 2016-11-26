//
//  memory.cpp
//  RISCV_ISA_SIM
//
//  Read and write functions for Memory
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu Wang  on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include <string.h>
#include "system.h"
#include "memory.hpp"

memory sim_mem;


memory::memory()//initiaiize
{
    mem_zero=(byte *)malloc(sizeof(char)*MEM_SIZE);
    memset(mem_zero,0,MEM_SIZE);
    
    trace_out.open("trace.txt",std::ios::out|std::ios::trunc);    //open a new file or clean the existed file
    if (!trace_out.is_open())
        printf("cannot open trace file!\n");
    else
        printf("open a trace file!\n");
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
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
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
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    
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
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
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
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    
    reg64* p_u64=(reg64*)v_addr;
    *p_u64=value;
    return true;
}
reg64 memory::get_memory_64(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    //printf("0x%lx\n", (reg64)v_addr);
    //printf("*v_addr = 0x%x", *v_addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return -1;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    
    reg64* p_u64=(reg64*)v_addr;
    //printf("*p_u64 = 0x%lx\n", *p_u64);
    return (*p_u64);
    
}
reg32 memory::get_memory_32(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL){
        printf("invalid memory address\n");
        return -1;
    }
    
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    
    reg32* p_u32=(reg32*)v_addr;
    return (*p_u32);
    
}
reg16 memory::get_memory_16(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL){
        printf("invalid memory address\n");
        return -1;
    }
    
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    
    reg16* p_u16=(reg16*)v_addr;
    return (*p_u16);
    
    
}
reg8  memory::get_memory_8(memAddress addr)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL){
        printf("invalid memory address\n");
        return -1;
    }
    
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    reg8* p_u8=(reg8*)v_addr;
    return (*p_u8);
    
}

f32   memory::get_memory_f32(memAddress addr)
{
    byte* v_addr = get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return -1;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    float* p_f32 = (float*)v_addr;
    return *p_f32;
    
}
f64  memory::get_memory_f64(memAddress addr)
{
    byte* v_addr = get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return -1;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"r\t"<<trace_addr<<"\n";
    
    double* p_f64 = (double*)v_addr;
    return *p_f64;
}
bool  memory::set_memory_f32(memAddress addr,f32 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    float* fp = (float*)v_addr;
    *fp = value;
    return true;
}
bool  memory::set_memory_f64(memAddress addr,f64 value)
{
    byte* v_addr=get_memory_p_address(addr);
    if(v_addr==NULL)
    {
        printf("invalid memory address\n");
        return false;
    }
    char trace_addr[35];
    sprintf(trace_addr, "%lu", addr);
    trace_out<<"w\t"<<trace_addr<<"\n";
    
    double* fp = (double*)v_addr;
    *fp = value;
    return true;
}
