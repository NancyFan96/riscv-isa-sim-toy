//
//  exe.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/5.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include "exe.hpp"
#include <iostream>
#include <stdlib.h>
#include <cstring>


using namespace std;

static registers sim_regs;
static memory   sim_mem;

ins fetch(){
    reg32 PC = sim_regs.getPC();                         // 64bit
    PC = sim_mem.get_memory_address(PC);
    ins inst = sim_mem.get_memory_32(PC);
    
    return inst;
}

bool load_program(char const *file_name)
{
    /*读入ELF文件*/
    FILE * file_in=NULL;
    unsigned long file_size;
    
    file_in=fopen(file_name,"rb");//二进制读文件
    if (file_in==NULL) {
        printf("cannot open the file : %s \n",file_name);
        return false;
    }
    fseek(file_in,0,SEEK_END);//定位文件指针到文件末
    file_size=ftell(file_in);//获得当前文件指针到文件头的位置，即文件大小
    fseek(file_in,0,SEEK_SET);//重新将文件指针定位到文件头
    
    unsigned char* file_buffer;
    file_buffer=(unsigned char*)malloc(sizeof(unsigned char)*file_size);//read file into file_buffer
    if(fread(file_buffer,sizeof(char),file_size,file_in)<file_size)
    {
        printf("fail to read the entire file!\n");
        return false;
        
    }
    else
    {
        printf("read file successfully!\n");
    }
    
    /*将segment拷贝到模拟器定义的内存*/
    //define data type
    Elf64_Ehdr* elf_header;
    elf_header=(Elf64_Ehdr*)file_buffer;
    Elf64_Shdr* sec_header=(Elf64_Shdr*)((unsigned char*)elf_header+elf_header->e_shoff);
    Elf64_Half sec_header_entry_size=elf_header->e_shentsize;
    sec_header=(Elf64_Shdr*)((unsigned char*)sec_header+sec_header_entry_size); //定位到序号为1的section(.text)
    memAddress program_entry_offset=(memAddress)(sec_header->sh_addr);          //代码段在运行时的虚拟地址，即程序入口地址0x10000
    sim_regs.setPC(program_entry_offset);                                       //set PC register
    
    memAddress cur_mem=sim_mem.get_memory_address(program_entry_offset);//从偏移0x10000处开始存放segment的二进制内容
    
    
    Elf64_Half seg_num=elf_header->e_phnum;//number of segments
    
    //printf("number of segments: %u :\n",seg_num);
    
    Elf64_Phdr* seg_header = (Elf64_Phdr*)((unsigned char*)elf_header + elf_header->e_phoff);//根据segment header offset定位到第一个entry
    Elf64_Half  seg_header_entry_size=elf_header->e_phentsize;//Program header table entry size
    
    for(int cnt=0;cnt<seg_num;++cnt)//读取各个segment的信息；一个程序一般只有一个segment
    {
        unsigned char* seg_in_file=(unsigned char*)elf_header+seg_header->p_offset;//指向二进制文件里要复制到内存中的segment内容
        Elf64_Xword seg_size_in_mem=seg_header->p_memsz;//segment的大小
        memAddress * p_cur=(memAddress* )cur_mem;
        memcpy(p_cur,seg_in_file,seg_size_in_mem);//copy segment to sim_mem
        cur_mem+=seg_size_in_mem;//指针指向拷贝后空的内存开始处
        
        //printf("segment size is: %ld\n",seg_size_in_mem);
        
        seg_header=(Elf64_Phdr*)((unsigned char*)seg_header+seg_header_entry_size);//定位到下一个segment entry
        
    }
    printf("end of segment copy\n");
    
    fclose(file_in);//关闭打开的ELF文件
    free(file_buffer);//释放存放ELF文件的内存
    return true;
}

bool exit_program(){
    return false;
}


int main(int argc, char * argv[]){
    /*--------- prase ---------*/
    const char * file_name;
    
    
    /*---------END prase ---------*/
    
    
    if(load_program(file_name)==true){
        cout << "EXCUTE "<< file_name <<"..." << endl;
    }
    else{
        cout << "LOAD ERROR!" << endl;
        return -1;
    }
    
    ins inst = fetch();
    instruction fetched_inst;
    fetched_inst.decode(inst);
    fetched_inst.execute();
    
    if(exit_program() == true){
        cout << "EXIT "<< file_name<< " BYE!"<< endl;
        return 0;
    }
    else{
        cout << "EXIT ERROR!" << endl;
        return -1;
    }

}
