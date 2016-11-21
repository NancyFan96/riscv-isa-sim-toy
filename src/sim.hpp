//
//  exe.hpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu Wang on 16/11/5.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef exe_hpp
#define exe_hpp

#include "system.h"
#include "decode.hpp"
#include "register.hpp"
#include "memory.hpp"
#include <map>
#include <string>



bool load_program(const char* filename);    // if success return true, else return false
ins fetch();                                // return instruction and PC inc
void exit_program();                        // if success return true, else return false


/*------------- define ELF --------------*/
//standard ELF TYPES
//64-bit only
typedef unsigned long int   Elf64_Addr;     //type of addresses u64
typedef unsigned short int  Elf64_Half;     //u16
typedef unsigned long int   Elf64_Off;      //type of file offsets u64
typedef int                 Elf64_Sword;    //s32
typedef unsigned int        Elf64_Word;     //u32
typedef unsigned long int   Elf64_Xword;    //u64
typedef signed long int     Elf64_Sxword;   //s64
typedef unsigned short int  Elf64_Section;  //u16

/*ELF file header.
 This appears at the start of every ELF file
 64 bytes in total
 装载和运行程序需要关注以下信息：
 e_entry      程序入口地址
 e_phoff      segment header偏移
 e_phnum   segment数量*/

#define EI_NIDENT (16)

typedef struct{
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
 It describes size and location for each section*/
typedef struct{
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

/* Program segment header.*/
typedef struct{
    Elf64_Word    p_type;                 /* Segment type */
    Elf64_Word    p_flags;                /* Segment flags */
    Elf64_Off     p_offset;               /* Segment file offset */
    Elf64_Addr    p_vaddr;                /* Segment virtual address */
    Elf64_Addr    p_paddr;                /* Segment physical address */
    Elf64_Xword   p_filesz;               /* Segment size in file */
    Elf64_Xword   p_memsz;                /* Segment size in memory */
    Elf64_Xword   p_align;                /* Segment alignment */
} Elf64_Phdr;

/*symbol table entry*/
typedef struct{
    /* Symbol name (string tbl index)
     st_name   This member holds an index into the object file's symbol
     string table, which holds character representations of the
     symbol names.  If the value is nonzero, it represents a
     string table index that gives the symbol name.  Otherwise,
     the symbol has no name.*/
    Elf64_Word    st_name;
    unsigned char st_info;                /* Symbol type and binding */
    unsigned char st_other;               /* Symbol visibility */
    Elf64_Section st_shndx;               /* Section index */
    Elf64_Addr    st_value;               /* Symbol value  This member gives the value of the associated symbol.*/
    Elf64_Xword   st_size;                /* Symbol size */
} Elf64_Sym;



#define continue_run 0
#define step 1
#define set_breakpoint 2
#define delete_breakpoint 3
#define print_mem 4
#define print_reg 5
#define quit_gdb 6
#define undefined_gdb 7



//initialize map
std::map<std::string,int> COUNTS;



#endif /* exe_hpp */


/*------------ END define ELF --------------*/
