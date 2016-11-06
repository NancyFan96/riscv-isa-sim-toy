//
//  exe.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/5.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "system.h"
#include "register.hpp"
#include "memory.hpp"
#include "decode.hpp"
#include "exe.hpp"

using namespace std;

#ifdef GDB
static memAddress breakpoint;
static bool ONESTEP = false;
#endif

void help()
{
    printf("This is a simulator to execute riscv ELF!\n");
    printf("    Usage: ./exe <filename> [--verbose|--debug]\n");
    printf("           ./exe --help\n");
    printf("Multiple ELFs is NOT supported!\n\n");
    
}

bool load_program(char const *file_name)
{
    /*-----------read ELF file-----------*/
    FILE * file_in=NULL;
    unsigned long file_size;
    
    file_in=fopen(file_name,"rb");
    if (file_in==NULL) {
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
        printf("fail to read the entire file!\n");
        return false;
        
    }
    else
    {
        printf("read file successfully!\n");
    }
    
    /*copy segments in ELF file to simulator memory*/
    
    //define data type
    Elf64_Ehdr* elf_header;
    elf_header=(Elf64_Ehdr*)file_buffer;
    Elf64_Shdr* sec_header=(Elf64_Shdr*)((unsigned char*)elf_header+elf_header->e_shoff);
    Elf64_Half sec_header_entry_size=elf_header->e_shentsize;
    sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size);         //locate to section .text
    memAddress program_entry_offset=(memAddress)(sec_header->sh_addr);                  //runtime virtual address aka. program entry address 0x10000
    //unsigned int code_size=(unsigned int)sec_header->sh_size;
    //EXIT_POINT = program_entry_offset+code_size;                                       //set EXIT_POINT
    byte*  cur_p_mem=sim_mem.get_memory_p_address(program_entry_offset);        //copy segment from program entry offset
    
    
    Elf64_Half seg_num=elf_header->e_phnum;//number of segments
    
    //printf("number of segments: %u :\n",seg_num);
    
    Elf64_Phdr* seg_header = (Elf64_Phdr*)((unsigned char*)elf_header + elf_header->e_phoff);//locate to the first entry of program header table
    Elf64_Half  seg_header_entry_size=elf_header->e_phentsize;//Program header table entry size
    
    for(int cnt=0;cnt<seg_num;++cnt)//copy each segment
    {
        unsigned char* seg_in_file=(unsigned char*)elf_header+seg_header->p_offset;
        Elf64_Xword seg_size_in_mem=seg_header->p_memsz;//segment size
        memcpy(cur_p_mem,seg_in_file,seg_size_in_mem);//copy segment to sim_mem
        cur_p_mem =(byte*)cur_p_mem + seg_size_in_mem;
        
        //printf("segment size is: %ld\n",seg_size_in_mem);
        
        seg_header=(Elf64_Phdr*)((unsigned char*)seg_header+seg_header_entry_size);//next segment entry
        
    }
    printf("end of segment copy\n");
    
    fclose(file_in);//close ELF file
    free(file_buffer);//free buffer
    
    /* ---- init regs ------*/
    sim_regs.setPC(program_entry_offset);                                       //set PC register
    sim_regs.writeReg(zero, 0);
    sim_regs.writeReg(sp, STACK_TOP);
    //sim_regs.writeReg(gp, <#reg64 value#>);
    
    
    

    return true;
}

// Unfinished!!
bool exit_program(){
    free(sim_mem.mem_zero);
    return true;
}

ins fetch(){
    memAddress PC = sim_regs.getPC();              // 32bit
    ins inst = sim_mem.get_memory_32(PC);
    sim_regs.incPC();
    return inst;
}


int main(int argc, char * argv[]){
    /*--------- prase ---------*/
    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        help();
        return 0;
    }
    if(argc == 3) {
#ifdef GDB
        if (strcmp(argv[2], "--debug") == 0){
            printf("Debug mode\n\n");
            breakpoint = sim_regs.getPC();
            verbose = 1;
        }
#endif
        if (strcmp(argv[2], "--verbose") == 0)
            verbose = 1;
    }
    const char * file_name = argv[1];
    
    /*---------END prase ---------*/
    
    
    if(load_program(file_name)==true){
        cout << "EXCUTE "<< file_name <<"..." << endl;
    }
    else{
        cout << "LOAD ERROR!" << endl;
        return -1;
    }
    
    //reg32 lastPC = -1;
    //reg32 curPC = -1;
    while(1){
        ins inst = fetch();
        instruction fetched_inst;
        if(fetched_inst.decode(inst) == true){
            if(fetched_inst.Is_exit())
                break;
            fetched_inst.execute();
        }
        else{
            cout << "DECODE ERROR!" << endl;
        }
    }
    
    if(exit_program() == true){
        cout << "EXIT "<< file_name<< " BYE!"<< endl;
        return 0;
    }
    else{
        cout << "EXIT ERROR!" << endl;
        return -1;
    }

}
