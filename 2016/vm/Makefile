CPPFLAGS= -Wall -g -std=c++14 -Wswitch-enum -Wswitch-default
LIBS =-lpthread

PROGRAMS := $(patsubst %.cpp, %, $(wildcard *.cpp))

all: $(PROGRAMS)

%: %.cpp
	g++ $(CPPFLAGS) $^ -o $@ $(LIBS)

