#!/bin/bash
cc main.c -g -std=c99 -c -I /opt/raylib/src -o main.o && \
cc main.o -s -Wall -std=c99 -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 && \
./a.out
