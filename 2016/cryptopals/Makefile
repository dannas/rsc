CPPFLAGS= -Wall -g -std=c++14

PROGRAMS := $(patsubst %.cpp, %, $(wildcard *.cpp))

all: $(PROGRAMS)

%: %.cpp
	g++ $(CPPFLAGS) $^ -o $@ 

clean:
	rm $(PROGRAMS)
