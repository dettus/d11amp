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
INSTALLSHARE=$(PREFIX)/share/games
INSTALLMAN=$(PREFIX)/share/man

CC?=gcc
AR?=ar
CFLAGS=-g -O0
#CFLAGS?=-O3
CFLAGS+=-Wall 
#CFLAGS+=-Werror
#CPPFLAGS+=-DEXPERIMENTAL_SAVEGAME_SLOTS
PROJ_HOME=./
INCFLAGS= -I$(PROJ_HOME)src/skinhandler/
LINK=$(CC)

# Library: gtk+-3.0
LDFLAGS+= `pkg-config    --libs gtk+-2.0`
CFLAGS+=  `pkg-config  --cflags gtk+-2.0`
#LDFLAGS+= `pkg-config    --libs gdk-3.0`
#CFLAGS+=  `pkg-config  --cflags gdk-3.0`

# Library: libMPG123
LDFLAGS+= `pkg-config    --libs libmpg123`
CFLAGS+=  `pkg-config  --cflags libmpg123`

# Library: libOUT123
LDFLAGS+= `pkg-config    --libs libout123`
CFLAGS+=  `pkg-config  --cflags libout123`

SOURCES_SKINHANDLER=	\
	src/skinhandler/elements.c	\
	src/skinhandler/pixbufloader.c

SOURCES_MAIN=	\
	src/main/main.c

OBJ_SKINHANDLER=${SOURCES_SKINHANDLER:.c=.o}
OBJ_MAIN=${SOURCES_MAIN:.c=.o}

OBJFILES=$(OBJ_SKINHANDLER) $(OBJ_MAIN)

all: d11amp
	

clean:
	rm -rf d11amp
	rm -rf $(OBJFILES)

d11amp:	$(OBJFILES)
	$(LINK) -o $@ $(OBJFILES) $(LDFLAGS)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CFLAGS_EXTRA) $(INCFLAGS) -c -o $@ $< 
	

