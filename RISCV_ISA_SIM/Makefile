CC = g++
PROC = sim

SRC=	sim.cpp memory.cpp register.cpp decode.cpp execute.cpp execute_F.cpp
HDR=	system.h sim.hpp memory.hpp register.hpp decode.hpp


all: 	$(PROC)

sim: 	$(SRC) $(HDR)
	$(CC) -o $(PROC) *.cpp -Wall


clean:
	-rm $(PROC) *.o
