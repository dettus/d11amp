#!/bin/sh
gcc -O0 -g  -c -o theme_elements.o ../software/src/gui/theme_elements.c -I../software/src/gui/ `pkg-config --cflags gdk-pixbuf-2.0`
gcc -O0 -g  -c -o themedump.o themedump.c -I../software/src/gui/ `pkg-config --cflags gdk-pixbuf-2.0`
gcc -O0 -g  -o  themedump.app themedump.o theme_elements.o `pkg-config --libs gdk-pixbuf-2.0`

