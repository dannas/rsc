all: snake

snake : snake.cpp
	g++ `pkg-config ncurses --cflags` -Wall -std=c++14 snake.cpp -o snake `pkg-config ncurses --libs`
