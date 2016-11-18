//
//  sim.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan, Kejing Yang,Yao Lu on 16/11/5.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

/*-------------------------------------------------
 sim.cpp contains several parts of RISCV simulator.
 1. load_program - read an ELF file and load program.
 2. fetch - fetch an instruction
 3. decode - decode an instruction
 4. execute - simulate instruction executing process
 5. exit - program exits
 
 An extra GDB mode is also provided.
 
 --------------------------------------------------*/

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include "system.h"
#include "register.hpp"
#include "memory.hpp"
#include "decode.hpp"
#include "sim.hpp"

using namespace std;

bool GDB_MODE =false;
bool verbose = false;
bool IS_TO_EXIT = false;
bool IS_TO_DO = false;
long int COUNT_INS = 0;

static int GDB_TYPE;
static bool VALID_BREAKPOINT=false;
static memAddress breakpoint = 0;
static bool IS_ENTER_STEP = false;
static bool IS_FIRST_GDB = false;
static bool IS_NOP = false;
static bool WAIT = false;

static memAddress currentPC;
static memAddress main_virtual_address = 0;

extern long int COUNTS[HOW_MANY_INSTS]; // CHANGE

void help()
{
    printf("This is a simulator to execute riscv ELF!\n");
    printf("    Usage: ./sim <filename> [--verbose|--debug]\n");
    printf("           ./sim --help\n");
    printf("Multiple ELFs is NOT supported!\n\n");
    
}

bool load_program(char const *file_name)
{
    /*-----------read ELF file-----------*/
    FILE * file_in=NULL;
    unsigned long file_size;
    
    file_in=fopen(file_name,"rb");
    if (file_in==NULL) {
        if(GDB_MODE)    printf("> ");
        printf("cannot open the file : %s \n",file_name);
        return false;
    }
    fseek(file_in,0,SEEK_END);                                              //set file pointer to the end of file
    file_size=ftell(file_in);                                               //get file size
    fseek(file_in,0,SEEK_SET);                                              //set file pointer to the begining of file
    
    unsigned char* file_buffer;
    file_buffer=(unsigned char*)malloc(sizeof(unsigned char)*file_size);    //read file into file_buffer
    if(fread(file_buffer,sizeof(char),file_size,file_in)<file_size)
    {
        if(GDB_MODE)    printf("> ");
        printf("Fail to read the entire file!\n");
        return false;
        
    }
    else
    {
        if(GDB_MODE)    printf("> ");
        printf("Read file successfully!\n");
    }
    
    /*copy segments in ELF file to simulator memory*/
    
    Elf64_Ehdr* elf_header;
    elf_header=(Elf64_Ehdr*)file_buffer;
    Elf64_Shdr* sec_header=(Elf64_Shdr*)((unsigned char*)elf_header+elf_header->e_shoff);                       //locate to first section
    Elf64_Half sec_header_entry_size=elf_header->e_shentsize;
    
    sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size);                                 //locate to section .text
    memAddress program_entry_offset=(memAddress)(sec_header->sh_addr);                                          //runtime virtual address
    byte*  cur_p_mem=sim_mem.get_memory_p_address(program_entry_offset);
    Elf64_Half seg_num=elf_header->e_phnum;                                                                     //number of segments

    
    Elf64_Phdr* seg_header = (Elf64_Phdr*)((unsigned char*)elf_header + elf_header->e_phoff);                   //locate to the first entry of program header table
    Elf64_Half  seg_header_entry_size=elf_header->e_phentsize;                                                  //Program header table entry size
    
    for(int cnt=0;cnt<seg_num;++cnt)
    {
        unsigned char* seg_in_file=(unsigned char*)elf_header+seg_header->p_offset;
        Elf64_Xword seg_size_in_mem=seg_header->p_memsz;                                                        //segment size
        memcpy(cur_p_mem,seg_in_file,seg_size_in_mem);                                                          //copy segment to sim_mem
        cur_p_mem =(byte*)cur_p_mem + seg_size_in_mem;
        seg_header=(Elf64_Phdr*)((unsigned char*)seg_header+seg_header_entry_size);                             //next segment entry
        
    }
    /*------------- end of segments copy----------------*/
    
    Elf64_Half  sec_num=elf_header->e_shnum;                                                                    //number of sections
    sec_header=(Elf64_Shdr*)((unsigned char*)elf_header+elf_header->e_shoff);                                   //locate to first section
    sec_header_entry_size=elf_header->e_shentsize;
    Elf64_Shdr* strtab_sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size*(sec_num-1));  //locate to section header of string table
    Elf64_Off strtab_offset=strtab_sec_header->sh_offset;                                                       //read string table offset offset
    unsigned char* p_strtab=(unsigned char*)elf_header+strtab_offset;                                           //point to the beginning of string table
    
    
    for(int cnt=0;cnt<sec_num;++cnt)                                                                            //for each section
    {
        Elf64_Word section_type=sec_header->sh_type;
        if(section_type==2)                                                                                     //if section type==SYMTAB
        {
            Elf64_Off symtab_offset=sec_header->sh_offset;
            Elf64_Xword   sh_size=sec_header->sh_size;
            long int symtab_num=sh_size/sizeof(Elf64_Sym);                                                      //number of entries in symbol table
            Elf64_Sym* p_symtab=(Elf64_Sym*)((unsigned char*)elf_header+symtab_offset);                         //locate to the first entry
            for(int entry_cnt=0;entry_cnt<symtab_num;++entry_cnt)
            {
                Elf64_Word    st_name=p_symtab->st_name;
                Elf64_Addr    st_value=p_symtab->st_value;
                if(st_name!=0)
                {
                    unsigned char* p_name=p_strtab+st_name;
                    char* test_name=(char*)malloc(sizeof(char)*20);
                    int name_cnt=0;
                    while (*p_name!='\0'&&name_cnt<20)
                    {
                        test_name[name_cnt]=*p_name;
                        p_name++;
                        name_cnt++;
                    }
                    test_name[name_cnt]='\0';
                    if(strcmp(test_name,"main")==0)
                    {
                        main_virtual_address=(memAddress)st_value;
                        break;
                    }
                    
                }//end of if
                p_symtab++;
            }//end of for
            break;
        }//end of if
        sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size);//next section
    }
    

    fclose(file_in);                                //close ELF file
    free(file_buffer);                              //free buffer
    
    /* ---- init regs ------*/
    if((byte *)main_virtual_address==NULL)
        printf("main virtual address invalid!\n");
    sim_regs.setPC(program_entry_offset);          //start from program entry
    sim_regs.writeReg(zero, 0);
    sim_regs.writeReg(sp, STACK_TOP);

    return true;
}

void exit_program(){
    free(sim_mem.mem_zero);
}

ins fetch(){
    ins inst;
    if(IS_NOP){
        inst = sim_mem.get_memory_32(currentPC);
    }
    else{
        currentPC = sim_regs.getPC();
        inst = sim_mem.get_memory_32(currentPC);
        sim_regs.incPC();
    }
    return inst;
}

//gdb mode execute function
bool gdb_mode_func(void)
{
    char cmd[20];
    memAddress break_addr=0;
    memAddress debug_mem=0;
    reg32 mem_content=0;
    if(IS_FIRST_GDB)
    {
        printf(">\n");
        printf("> main function start at %lx\n",main_virtual_address);
        printf("> select a mode you want to run with:\n");
        printf("> break: set breakpoint\n");
        printf("> delete: delete breakpoint\n");
        printf("> continue: continue running\n");
        printf("> step: step mode\n");
        printf("> memory: print memory content\n");
        printf("> register: print register file info\n");
        printf("> quit: quit gdb mode\n>\n>\n");
    }
    
    printf("> ");
    scanf("%s",cmd);        //read command
    fflush(stdin);          //clean stdin buffer
    char cmd_char = 'a';
    if (strcmp(cmd,"break")==0)
        cmd_char = 'b';
    else if(strcmp(cmd,"delete")==0)
        cmd_char = 'd';
    else if(strcmp(cmd,"continue")==0)
        cmd_char = 'c';
    else if(strcmp(cmd,"step")==0)
        cmd_char = 's';
    else if(strcmp(cmd,"memory")==0)
        cmd_char = 'm';
    else if(strcmp(cmd,"register")==0)
        cmd_char = 'r';
    else if(strcmp(cmd,"quit")==0)
        cmd_char = 'q';
    
    switch (cmd_char)
    {
        case 'b':
            GDB_TYPE = set_breakpoint;
            printf("> set breakpoint address\n");
            printf("> ");
            scanf("%lx",&break_addr);   //set breakpoint addresss
            VALID_BREAKPOINT=true;
            breakpoint=break_addr;
            return true;
        case 'd':
            GDB_TYPE = delete_breakpoint;
            printf("> delete breakpoint(%lx)\n", breakpoint);
            VALID_BREAKPOINT = false;
            WAIT = true;
            IS_NOP = true;
            return true;
        case 'c':
            GDB_TYPE = continue_run;
            printf("> continue running...\n");
            return true;
        case 's':
            if(GDB_TYPE != step)
                IS_ENTER_STEP = true;
            if(IS_ENTER_STEP)
            {
                printf(">\n");
                printf("> --------------------step mode--------------------\n");
                IS_ENTER_STEP = false;
            }
            GDB_TYPE = step;
            VALID_BREAKPOINT=true;
            breakpoint=sim_regs.getPC();
            return true;
        case 'm':
            GDB_TYPE = print_mem;
            IS_NOP = true;
            WAIT = true;
            printf("> set memory address\n");
            printf("> ");
            scanf("%lx",&debug_mem);
            for(int row=0;row<4;++row){
                printf("> ");
                for(int col=0;col<4;++col){
                    mem_content=sim_mem.get_memory_32(debug_mem);
                    printf("0x%08x",mem_content);
                    printf("    ");
                    debug_mem+=4;
                }
                printf("\n");
            }
            return true;
        case 'r':
            GDB_TYPE = print_reg;
            IS_NOP = true;
            WAIT = true;
            sim_regs.readReg();
            sim_regs.readFloatReg();
            return true;
        case 'q':
            GDB_MODE=false;
            GDB_TYPE = quit_gdb;
            verbose = false;
            return false;
        default:
            printf("> invalid command\n");
            //GDB_MODE=false;
            GDB_TYPE = undefined_gdb;
            IS_NOP = true;
            verbose = false;
            return true;
    }
    
}

int main(int argc, char * argv[]){
    const char * file_name;
    
    if(argc < 2 || argc > 3 || strcmp(argv[1],"--help") == 0){
        help();
        return 0;
    }
    file_name = argv[1];
    if(argc == 3) {
        if (strcmp(argv[2], "--debug") == 0){
            printf("> (gdb mode)\n>\n");
            IS_FIRST_GDB = true;
            IS_NOP = true;
            VALID_BREAKPOINT=true;
            breakpoint=currentPC;      //break point <- current PC; fetched but not executed!
            GDB_MODE=true;
            WAIT = true;
        }
        else if (strcmp(argv[2], "--verbose") == 0)
            verbose = true;
        else{
            printf("\nINVALID COMMAND!\n");
            help();
            return 0;
        }
    }
    
    if(load_program(file_name)==true){
        if(GDB_MODE)    printf("> ");
        printf("sp = %lx  PC = %lx\n", sim_regs.readReg(sp),sim_regs.getPC());
        if(GDB_MODE)    printf("> ");
        cout << "EXCUTE "<< file_name << endl;
        if(GDB_MODE)    printf("> ");
        printf("\n");
    }
    else{
        cout << "LOAD ERROR!" << endl;
        return -1;
    }

    memset(COUNTS, 0, sizeof(long int)*HOW_MANY_INSTS);
    while(1){
        ins inst = fetch();
        instruction fetched_inst;
        
        if(GDB_MODE&&VALID_BREAKPOINT&&breakpoint == currentPC){
            verbose = true;
            WAIT = true;
            
        }
        
        if(!IS_NOP){
            if(verbose){
                printf("> -------------AFTER LASTST INSTRUCTION(VERBOSE)--------------------\n");
                printf("> currentPC = %lx\n", currentPC);
                printf("> instruction = %x\n", inst);
            }
            if(fetched_inst.decode(inst) == true){
                fetched_inst.execute();
                if(IS_TO_EXIT)  break;
            }else{
                cout << "DECODE ERROR!" << endl;
                printf("> currentPC = %lx\n", currentPC);
                printf("> instruction = %x\n", inst);
                printf("current register(current instruction not run yet)...\n");
                sim_regs.readReg();
                sim_regs.readFloatReg();
                return -1;
            }
        }
        else
            IS_NOP = false;

        /*-----------gdb mode------------*/
        if(GDB_MODE && WAIT){
            if(gdb_mode_func()==false){
                printf("> quit gdb mode\n\n");
                return -1;
            }//enter gdb mode function
            
            if(IS_FIRST_GDB)    IS_FIRST_GDB = false;
            
            if(GDB_TYPE!=print_mem && GDB_TYPE != print_reg && GDB_TYPE != delete_breakpoint){
                verbose = false;
                WAIT = false;
            }
        }

    }
    
    exit_program();

    // 这里打一张COUNTS的表 CHANGE

    cout << "\nBYE "<< file_name<< " !"<< endl<< endl;
    return 0;
}
