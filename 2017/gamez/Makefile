all: snake pong

snake : snake.cpp
	g++ -g -O2 `pkg-config ncurses --cflags` -Wall -std=c++14 snake.cpp -o snake `pkg-config ncurses --libs`

pong : pong.cpp
	g++ -g -O2 `pkg-config ncurses --cflags` -Wall -std=c++14 pong.cpp -o pong `pkg-config ncurses --libs`
