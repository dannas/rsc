CXXFLAGS = -g -Og -Wall -std=c++14
PROGRAMS = rr-v1 hello
all: $(PROGRAMS)

rr-v1: rr-v1.cpp util.h util.cpp
	g++ $(CXXFLAGS) rr-v1.cpp util.h util.cpp -o rr-v1

hello: hello.c
	gcc -nostdlib hello.c -o hello


clean:
	rm $(PROGRAMS)
