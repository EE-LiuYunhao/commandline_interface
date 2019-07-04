
 # HU Qiyun   3035449396
 # LIU Yunhao 3035448079

interpreter:main.o History.o
	g++ main.o History.o -o interpreter
main.o: main.cpp History.cpp History.h Command.h
	g++ -c main.cpp
History.o:History.cpp History.h Command.h
	g++ -c History.cpp
Debugger:main.cpp History.cpp History.h Command.h
	g++ -c History.cpp -g
	g++ -c main.cpp -g
	g++ main.o History.o -o Debugger -g
	gdb ./Debugger
run:
	./interpreter