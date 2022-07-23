#!/bin/sh

cc -O0 -g -c -o audio.o audio.c  `pkg-config --cflags libout123` `pkg-config --cflags libmpg123`
cc -O0 -g -c -o audiotest.o audiotest.c 
cc -O0 -g -o audiotest.app audio.o audiotest.o `pkg-config --libs libout123` `pkg-config --libs libmpg123` -lpthread
