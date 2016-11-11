# riscv-isa-sim-toy

这是体系实习的LAB2作业

由小组合作完成

要求实现RISCV64I 的模拟器


## 当前进度

：）

**TO DO:**

* close把输出直接关了？！
* complete static variable init (已经实现?)，结合scanf的问题来看并没有（不过好像是RISCV-gcc本身的问题，spike也会挂？）
* need to realize MALLOC\* (已经实现？，相传有爆堆错误)
* M, F extension 校对，浮点扩展的rounding mode还没管
* 标测 quicksort, ijk, ack + drystone(实现drystone还需要补充一些指令的实现)

DONE:

* start PC from 必须从entry point开始，否则从\<main\>【错误】==> mac下可以，linux下segmemtation fault
* complete ecall to realize exit√, printf\* and so on//进入了ecall但没有调用成功

* gdb

## LOGs

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


## 文件结构

- README.md 
- ....xcodeproj
- RISCV_ISA_SIM

	-- system.h
	模拟器的通用头文件，重命名了一些数据类型
	
	* 地址宽也都是64bit，但只有32位有效
	
	-- decode.hpp/cpp
	 模拟器解释执行reisv可执行文件的核心
	 
	 包括：instruction 类
	 
	 * instruction 结构
	 * 指令执行的decode,excute(== excute+mem+writeback)
	 
	-- memory.hpp/cpp
	 模拟器模拟的内存结构，装载进了ELF文件，<mark>高地址处预备实现stack</mark>
	 
	 包括：memory 类
	 
	 * memory 宽为32bit	
	 * 主要函数功能为load、store
	 * 注意ELF装载时，直接将其虚拟地址，作为自定义memory的index使用，因此多写地址时要注意加上基地址mem\_zero, 已定义函数get\_memory\_p\_address, 和get\_memory\_offset来转化index和实际地址
	 
	-- register.hpp/cpp
	 模拟器模拟的register files，包括PC寄存器，浮点寄存器
	 
	 包括：registers 类
	 
	 * 32个通用寄存器，寄存器宽都是64bit
	 * 通用寄存器的名字都已定义在宏
	 
	-- exe.hpp/cpp
	 模拟器main文件
	 
	 包括：load_program, fetch, exit\_program, main和help等


