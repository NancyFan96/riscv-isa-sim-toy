//
//  exe.hpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/5.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef exe_hpp
#define exe_hpp

#include "system.h"
#include "decode.hpp"
#include "register.hpp"
#include "memory.hpp"


bool load_program(const char* filename);    // if success return true, else return false
ins fetch();                        // return instruction and PC inc
bool exit_program();                        // if success return true, else return false


/*------------- define ELF --------------*/
//
//  load_program.hpp
//
//
//  Created by 杨可静 on 11/5/16.
//
//


#include <stdio.h>

//standard ELF TYPES
//64-bit only
typedef unsigned long int   Elf64_Addr;//type of addresses u64
typedef unsigned short int  Elf64_Half;//u16
typedef unsigned long int   Elf64_Off;//type of file offsets u64
typedef int                 Elf64_Sword;//s32
typedef unsigned int        Elf64_Word;//u32
typedef unsigned long int   Elf64_Xword;//u64
typedef signed long int     Elf64_Sxword;//s64
typedef unsigned short int  Elf64_Section;//u16

/*ELF file header.
 This appears at the start of every ELF file
 64 bytes in total
 装载和运行程序需要关注以下信息：
 e_entry      程序入口地址
 e_phoff      segment header偏移
 e_phnum   segment数量*/

#define EI_NIDENT (16)

typedef struct
{
    unsigned char e_ident[EI_NIDENT];     /* Magic number and other info */
    Elf64_Half    e_type;                 /* Object file type */
    Elf64_Half    e_machine;              /* Architecture */
    Elf64_Word    e_version;              /* Object file version */
    Elf64_Addr    e_entry;                /* Entry point virtual address */
    Elf64_Off     e_phoff;                /* Program header table file offset */
    Elf64_Off     e_shoff;                /* Section header table file offset */
    Elf64_Word    e_flags;                /* Processor-specific flags */
    Elf64_Half    e_ehsize;               /* ELF header size in bytes */
    Elf64_Half    e_phentsize;            /* Program header table entry size */
    Elf64_Half    e_phnum;                /* Program header table entry count */
    Elf64_Half    e_shentsize;            /* Section header table entry size */
    Elf64_Half    e_shnum;                /* Section header table entry count */
    Elf64_Half    e_shstrndx;             /* Section header string table index */
} Elf64_Ehdr;


/*Section header helps to reference every section
 It describes size and location for each section
 从链接的角度描述ELF文件*/

typedef struct
{
    Elf64_Word    sh_name;                /* Section name (string tbl index) */
    Elf64_Word    sh_type;                /* Section type */
    Elf64_Xword   sh_flags;               /* Section flags */
    Elf64_Addr    sh_addr;                /* Section virtual addr at execution */
    Elf64_Off     sh_offset;              /* Section file offset */
    Elf64_Xword   sh_size;                /* Section size in bytes */
    Elf64_Word    sh_link;                /* Link to another section */
    Elf64_Word    sh_info;                /* Additional section information */
    Elf64_Xword   sh_addralign;           /* Section alignment */
    Elf64_Xword   sh_entsize;             /* Entry size if section holds table */
} Elf64_Shdr;


/* Program segment header.
 从链接的角度来描述ELF文件，告诉系统如何建立一个内存映像
 可重定位文件不需要这个头表*/
typedef struct
{
    Elf64_Word    p_type;                 /* Segment type */
    Elf64_Word    p_flags;                /* Segment flags */
    Elf64_Off     p_offset;               /* Segment file offset */
    Elf64_Addr    p_vaddr;                /* Segment virtual address */
    Elf64_Addr    p_paddr;                /* Segment physical address */
    Elf64_Xword   p_filesz;               /* Segment size in file */
    Elf64_Xword   p_memsz;                /* Segment size in memory */
    Elf64_Xword   p_align;                /* Segment alignment */
} Elf64_Phdr;


#endif /* exe_hpp */


/*------------ END define ELF --------------*/
