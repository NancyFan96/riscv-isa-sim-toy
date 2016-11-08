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
#include <string.h>
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

bool verbose = false;
bool debug = false;
bool _exit = false;

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
    Elf64_Shdr* sec_header=(Elf64_Shdr*)((unsigned char*)elf_header+elf_header->e_shoff);//locate to first section
    Elf64_Half sec_header_entry_size=elf_header->e_shentsize;
    
    sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size);         //locate to section .text
    memAddress program_entry_offset=(memAddress)(sec_header->sh_addr);                  //runtime virtual address aka. program entry address 0x10000
    //unsigned int code_size=(unsigned int)sec_header->sh_size;
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
    
    Elf64_Half  sec_num=elf_header->e_shnum;//number of sections
    sec_header=(Elf64_Shdr*)((unsigned char*)elf_header+elf_header->e_shoff);//locate to first section
    sec_header_entry_size=elf_header->e_shentsize;
    Elf64_Shdr* strtab_sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size*(sec_num-1));//locate to section header of string table
    Elf64_Off strtab_offset=strtab_sec_header->sh_offset;//read string table offset offset
    unsigned char* p_strtab=(unsigned char*)elf_header+strtab_offset;//point to the beginning of string table
    memAddress main_virtual_address; // change
    
    for(int cnt=0;cnt<sec_num;++cnt)//for each section
    {
        Elf64_Word section_type=sec_header->sh_type;
        if(section_type==2)//if section type==SYMTAB
        {
            Elf64_Off symtab_offset=sec_header->sh_offset;
            Elf64_Xword   sh_size=sec_header->sh_size;
            unsigned long int symtab_num=sh_size/sizeof(Elf64_Sym);//number of entries in symbol table
            Elf64_Sym* p_symtab=(Elf64_Sym*)((unsigned char*)elf_header+symtab_offset);//locate to the first entry
            for(int entry_cnt=0;entry_cnt<symtab_num;++entry_cnt)//for each entry
            {
                Elf64_Word    st_name=p_symtab->st_name;//index to string table
                Elf64_Addr    st_value=p_symtab->st_value;//virtual address for 'main' entry
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
                    test_name[name_cnt]='\0';//get symbol name
                    if(strcmp(test_name,"main")==0)
                    {
                        printf("main found!\n");
                        printf("main virtual address is:%lx\n",st_value);
                        main_virtual_address=(memAddress)st_value;//st_value is an unsigned long int
                        break;
                    }
                    
                }//end of if
                p_symtab++;
            }//end of for
            break;
        }//end of if
        sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size);//next section
    }
    
    
    
    fclose(file_in);//close ELF file
    free(file_buffer);//free buffer
    
    /* ---- init regs ------*/
    if((byte *)main_virtual_address==NULL)
        printf("main virtual address invalid!\n");
    sim_regs.setPC(main_virtual_address);                                       //set PC register
    //sim_regs.setPC(program_entry_offset);
    sim_regs.writeReg(zero, 0);
    sim_regs.writeReg(sp, STACK_TOP);
    sim_regs.writeReg(ra, 65604); //0x10044
    printf("sp = %lx\n", sim_regs.readReg(sp));
    
    
    
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
            verbose = true;
        }
#endif
        if (strcmp(argv[2], "--verbose") == 0)
            verbose = true;
    }
    const char * file_name = argv[1];
    
    /*---------END prase ---------*/
    
    printf("spid = %d, sp = %lx\n", sp, sim_regs.readReg(sp));

    if(load_program(file_name)==true){
        cout << "EXCUTE "<< file_name <<"..." << endl;
    }
    else{
        cout << "LOAD ERROR!" << endl;
        return -1;
    }
    printf("sp = %lx\n", sim_regs.readReg(sp));
    sim_regs.readReg();

    //reg32 lastPC = -1;
    //reg32 curPC = -1;
    while(1){
        ins inst = fetch();
        printf("%x\n", inst);//debug
        instruction fetched_inst;
        if(fetched_inst.decode(inst) == true){
            printf("start excute\n");
            fetched_inst.execute();
            if(_exit)
                break;
        }
        else{
            cout << "DECODE ERROR!" << endl;
            return -1;
        }
    }
    
    if(exit_program() == true){
        cout << "BYE "<< file_name<< " !"<< endl<< endl;
        return 0;
    }
    else{
        cout << "EXIT ERROR!" << endl;
        return -1;
    }

}
