CC = g++
PROC = exe

SRC=	exe.cpp memory.cpp register.cpp decode.cpp execute.cpp excute_plus.cpp
HDR=	system.h exe.hpp memory.hpp register.hpp decode.hpp


all: 	$(PROC)

exe: 	$(SRC) $(HDR)
	$(CC) -o $(PROC) *.cpp -Wall


clean:
	-rm $(PROC) *.o
