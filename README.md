# riscv-isa-sim-toy

这是体系实习的LAB2作业

由小组合作完成

要求实现RISCV64I 的模拟器


## 当前进度


DONE ：）

## QuikStart

### Help info

```
    This is a simulator to execute riscv ELF!
        Usage: ./sim <filename> [--verbose|--debug]
               ./sim --help
    Multiple ELFs is NOT supported!
```

## 文件结构

- README.md 
- ....xcodeproj, sim 中间调试文件
- RISCV_ISA_SIM

```
├── readme.md   
├── src/    
│   ├── system.h 					# THIS IS RISCV R64IFM
│   ├── sim.hpp 						# Contains several parts of RISCV simulator and MAIN function
│   ├── sim.cpp     				# Details can be referred below
│   ├── decode.hpp 					# Decode instructions, get meaningful part like rd, rs, imm
│   ├── decode.cpp 					
│   ├── execute.cpp 				# Execution for RISCV64I, RISCV64M (only for std tests)
│   ├── execute_F.cpp 				# Execution for RISCV64F (only for std tests)
│   ├── register.hpp 				# Read and write functions for Registers
│   ├── register.cpp
│   ├── memory.hpp 					# Read and write functions for Memory
│   ├── memory.cpp
│   ├── Makefile						# would create executable file -- sim
│   ├── test/ 
│   	├── Itest/ 
│   	├── IOtest/ 
│   	├── Ftest/ 
│   	├── stdtest/ 
│   		├── dhry2.1/ 
│   		├── ijk 					# input: m, n, t, A[][], B[][]
│   		├── ijk.cpp
│   		├── qs 					# input: size, A[]
│   		├── quickSort.cpp 
│   		├── ack					# input: m, n
│   		├── ackermann.cpp 
│   	├── time 
│   	├── time.cpp 
│   	├── time.objdump

```
### What sim.cpp CONTAINS?

* sim.cpp contains several parts of RISCV simulator.
 
 0. help - print some help info to use this simulator
 1. load_program - read an ELF file and load program.
 2. fetch - fetch an instruction
 3. decode - decode an instruction
 4. execute - simulate instruction executing process
 5. exit_program - program exits, free the simulator memory space
 6. FLAG verbose - if set, print verbose info
 
* An extra GDB mode is also provided. This include
 1. gdb\_mode_func - wait and process a gdb instruction
 2. Sepecial FLAGS like
    -- VALID_BREAKPOINT
    -- IS_ENTER_STEP
    -- IS_FIRST_GDB
    -- IS_NOP
    -- WAIT
 
* An extra count function is also provided. This include
 1. print_ins_cnt_init - set clear
 2. print_ins_cnt - print result table
 This function count every single instruction that has been executed
 and their ratio of the entire program





## LOGs

### 重要记录

* Float register关于float和double的读写要分开用不同的函数，编码不同
* F ext校对！（心酸
* fload, fstore修正，需要memory类中新开用浮点指针存取的函数，不然强制转换会有问题
* need to realize MALLOC==> brk不能直接调用linux系统函数实现，因为可能会在模拟的内存外新开访问空间，实际riscv指令不能访问到(?)
* close把输出直接关了==>不用实现；原因猜测是riscv语义和linux不一致，后面有空再去翻系统函数源码...
* start PC from 必须从entry point开始，否则从\<main\>【错误】==> mac下可以，linux下segmemtation fault
* complete ecall to realize exit√, printf\* and so on
* gdb

### 部分提交记录

8.2 commit: final versioin

7.4 commit: fix some bugs in gdb, 现在断点是平时所用断点,查找到浮点错误是fmv语义错，另一个可能是float和double类型有问题

7.3 commit: fix the bugs in FLW, FLD, FSW, FSD

7.2 commit: printf run, add some printf in dhry\_1.c, pinpoint dugs in F extension CVT and ? 

6.3 commit: printf can work, scanf has bugs with initalization, 指针一定要初始化！

6.2 commit: Add M extesion, fixed gdb & ecall's args, still gdb input bugs and read&write bug

6.1 commit: Add gdb.1.0, some float extension(still have bugs

5.2 commit: fixed some bugs, j,branch,store指令语义错(当前PC)，store手癌存了寄存器编号

5.1 commit: fixed some bugs in decode like &, >>的优先级,指令含义，手癌等

tt, tiny, try用spike执行时有warning

	warning: only got 1127219200 bytes of target mem (wanted 4294967296)


4.1 commit, memAddress==>64bit unsigned, get/setPC use reg64, imm==>64bit signed

	add some signed type named signed8/16/32/64

4th commit, finish excute in decode

3.2 commit, fix memory and pointer conflic, change the struct of instruction and add a function to tell the END of a prog

## 作业要求

完成 RISC-V R64IFM(64 位基本集+浮点扩展+乘除扩展)ISA 的模拟,需完成:

* 至少包含指令译码、寄存器堆和存储(包括指令和数据)等部分。
* 需实现的指令见附件1和附件2,浮点和乘除扩展不限。
* 需利用RISC-V工具链完成用户程序的加载,并能够自动结束用户程序的运行。 
* 能够显式的看到运行结果。
* 具备运行模式和调试模式(可选)。
* 具备良好的扩展性,可方便的添加任意新指令(可选)。

最终提交内容:

* 实验报告(模板见附件3)
* 指令级模拟器代码
* 以lab1.1中用C编写的快速排序、矩阵乘法、求Ackermann函数为测试程序 最终评测标准,通过助教提供的用户程序的模拟。


