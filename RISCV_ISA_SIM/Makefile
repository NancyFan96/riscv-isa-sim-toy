CC = g++
PROC = exe

all: $(PROC)

exe: exe.cpp exe.hpp decode.cpp decode.hpp memory.cpp memory.hpp register.cpp register.hpp system.h
	$(CC) -o $(PROC) *.cpp


clean:
	-rm $(PROC) *.o
