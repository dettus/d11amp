#!/usr/bin/make -f 
#
#Copyright 2022, dettus@dettus.net
#
#Redistribution and use in source and binary forms, with or without modification,
#are permitted provided that the following conditions are met:
#
#1. Redistributions of source code must retain the above copyright notice, this 
#   list of conditions and the following disclaimer.
#
#2. Redistributions in binary form must reproduce the above copyright notice, 
#   this list of conditions and the following disclaimer in the documentation 
#   and/or other materials provided with the distribution.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
#FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
#DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
#SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
#OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

PREFIX?=/usr/local
INSTALLBIN=$(PREFIX)/bin
INSTALLSHARE=$(PREFIX)/share/
INSTALLMAN=$(PREFIX)/share/man

CC?=gcc
AR?=ar
CFLAGS=-g -O0 #-DD11AMP_DEBUGGING
#CFLAGS?=-O3
CFLAGS+=-Wall 
#CFLAGS+=-Werror
PROJ_HOME=./
INCFLAGS= -I$(PROJ_HOME)src/
LINK=$(CC)

# Library: gtk+-3.0
LDFLAGS+= `pkg-config    --libs gtk4`
CFLAGS+=  `pkg-config  --cflags gtk4`

# Library gdk-pixbuf
LDFLAGS+= `pkg-config    --libs gdk-pixbuf-2.0`
CFLAGS+=  `pkg-config  --cflags gdk-pixbuf-2.0`

# Library: libMPG123
LDFLAGS+= `pkg-config    --libs libmpg123`
CFLAGS+=  `pkg-config  --cflags libmpg123`

# Library: portaudio
LDFLAGS+= `pkg-config    --libs portaudio-2.0`
CFLAGS+=  `pkg-config  --cflags portaudio-2.0`


LDFLAGS+=-lpthread

DIRECTORY_AUDIOOUTPUT=src/audiooutput
SOURCES_AUDIOOUTPUT=	\
	$(DIRECTORY_AUDIOOUTPUT)/audiooutput.c	\
	$(DIRECTORY_AUDIOOUTPUT)/audiooutput_portaudio.c
OBJ_AUDIOOUTPUT=${SOURCES_AUDIOOUTPUT:.c=.o}

DIRECTORY_DECODER=src/decoder
SOURCES_DECODER=	\
	$(DIRECTORY_DECODER)/decoder.c		\
	$(DIRECTORY_DECODER)/decoder_mp3.c		
OBJ_DECODER=${SOURCES_DECODER:.c=.o}

DIRECTORY_GUI=src/gui
SOURCES_GUI=	\
	$(DIRECTORY_GUI)/gui.c	\
	$(DIRECTORY_GUI)/theme_elements.c	\
	$(DIRECTORY_GUI)/theme_manager.c	\
	$(DIRECTORY_GUI)/visualizer.c		\
	$(DIRECTORY_GUI)/window_equalizer.c		\
	$(DIRECTORY_GUI)/window_main.c
OBJ_GUI=${SOURCES_GUI:.c=.o}

DIRECTORY_MAIN=src/main
SOURCES_MAIN=	\
	$(DIRECTORY_MAIN)/main.c
OBJ_MAIN=${SOURCES_MAIN:.c=.o}


DIRECTORY_SHARED=src/shared
SOURCES_SHARED=	\
	$(DIRECTORY_SHARED)/constants.c
OBJ_SHARED=${SOURCES_SHARED:.c=.o}


OBJFILES=$(OBJ_AUDIOOUTPUT) $(OBJ_DECODER) $(OBJ_GUI) $(OBJ_MAIN) $(OBJ_SHARED)
INCFLAGS+=-I$(DIRECTORY_AUDIOOUTPUT) -I$(DIRECTORY_DECODER) -I$(DIRECTORY_GUI) -I$(DIRECTORY_MAIN) -I$(DIRECTORY_SHARED)

all: d11amp
	

clean:
	rm -rf d11amp
	rm -rf $(OBJFILES)

d11amp:	$(OBJFILES)
	$(LINK) -o $@ $(OBJFILES) $(LDFLAGS)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CFLAGS_EXTRA) $(INCFLAGS) -c -o $@ $< 
	

