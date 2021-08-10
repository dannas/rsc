PROGRAMS = nm size
LIBRARIES = libshared.so
all: $(PROGRAMS)

nm: nm.c
	gcc -std=c99 -O0 -g -Wall nm.c -o nm

size: size.c
	gcc -std=c99 -O0 -g -Wall size.c -o size

libshared.so: shared.cpp
	g++ -fPIC -shared shared.cpp -o libshared.so

clean:
	rm $(PROGRAMS) $(LIBRARIES)
