objs = Command.o Emulator.o Memory.o main.o
CPPFLAGS = -std=c++11 -Wwrite-strings

install : $(objs)
	g++ -o emulator $(objs)

clean :
	rm $(objs)