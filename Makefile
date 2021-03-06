bobjects = PhsStuDim dim_stu_command

CC = gcc
CXX = g++

CFLAGS = -g -Wall -I./dim -I./
CXXFLAGS = -g -Wall
CXXFLAGS += -I./dim -I./dim/dim -I./ 

all : $(bobjects)

PhsStuDim : dim_phos.cxx STU_API.o SocketClient_API.o

	$(CXX) $(CXXFLAGS) -L./dim/ -ldim -lpthread -o $@  STU_API.o SocketClient_API.o dim_phos.cxx ./dim/libdim.a

STU_API.o : STU_API.cpp STU_API.h
	$(CXX) $(CFLAGS) -c STU_API.cpp

SocketClient_API.o : SocketClient_API.cpp SocketClient_API.h
	$(CXX) $(CFLAGS) -c SocketClient_API.cpp

dim_stu_command : dim_stu_command.c
	$(CC) $(CXXFLAGS) -L./dim/ -ldim -lpthread -o $@  dim_stu_command.c ./dim/libdim.a

clean: 
	rm -f $(bobjects)
	rm -f *.o
	rm -f M*-* 

distclean: 
	rm -f ./*~

install: all
	ln -s $(HOME)/PHOSControl_5/M*-* ./