#!/usr/bin/make -f
#
#Copyright 2023, dettus@dettus.net
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
MYPREFIX=$(DESTDIR)$(PREFIX)


INSTALLBIN=$(MYPREFIX)/bin
INSTALLMAN=$(MYPREFIX)/share/man


CC?=gcc
AR?=ar
CFLAGS?= -O3 -Os
#CFLAGS?=-O0 -g
CFLAGS+=-Wall
#CFLAGS+=-Werror
PROJ_HOME=./
INCPATH= -I$(PROJ_HOME)src/
LINK=$(CC)

LDFLAGS?=-Wl,--as-needed

LIBRARIES=gdk-pixbuf-2.0 gtk4 libmpg123 portaudio-2.0 libzip

LIBS=    `pkg-config   --libs $(LIBRARIES)`
INCPATH+=`pkg-config --cflags $(LIBRARIES)`

LIBS+=-lpthread -lm


####
DIRECTORY_AUDIOOUTPUT=src/audiooutput
SOURCES_AUDIOOUTPUT=    \
	$(DIRECTORY_AUDIOOUTPUT)/audiooutput.c  \
	$(DIRECTORY_AUDIOOUTPUT)/audiooutput_portaudio.c	\
	$(DIRECTORY_AUDIOOUTPUT)/audiooutput_preferences.c
OBJ_AUDIOOUTPUT=${SOURCES_AUDIOOUTPUT:.c=.o}

DIRECTORY_DECODER=src/decoder
SOURCES_DECODER=   \
	$(DIRECTORY_DECODER)/decoder.c	\
	$(DIRECTORY_DECODER)/decoder_mp3.c
OBJ_DECODER=${SOURCES_DECODER:.c=.o}


DIRECTORY_GUI=src/gui
SOURCES_GUI=	\
	$(DIRECTORY_GUI)/default_theme.c	\
	$(DIRECTORY_GUI)/gui_helpers.c	\
	$(DIRECTORY_GUI)/gui_top.c	\
	$(DIRECTORY_GUI)/theme_elements.c	\
	$(DIRECTORY_GUI)/theme_manager.c	\
	$(DIRECTORY_GUI)/visualizer.c		\
	$(DIRECTORY_GUI)/window_equalizer.c	\
	$(DIRECTORY_GUI)/window_license.c	\
	$(DIRECTORY_GUI)/window_main.c		\
	$(DIRECTORY_GUI)/window_playlist.c	\
	$(DIRECTORY_GUI)/window_preferences.c
OBJ_GUI=${SOURCES_GUI:.c=.o}

DIRECTORY_MAIN=src/main
SOURCES_MAIN=   \
	$(DIRECTORY_MAIN)/controller.c	\
	$(DIRECTORY_MAIN)/main.c	\
	$(DIRECTORY_MAIN)/print_screens.c
OBJ_MAIN=${SOURCES_MAIN:.c=.o}

DIRECTORY_PLAYLIST=src/playlist
SOURCES_PLAYLIST=   \
	$(DIRECTORY_PLAYLIST)/playlist.c	
OBJ_PLAYLIST=${SOURCES_PLAYLIST:.c=.o}

DIRECTORY_SHARED=src/shared
SOURCES_SHARED=   \
	$(DIRECTORY_SHARED)/config.c	\
	$(DIRECTORY_SHARED)/license.c
OBJ_SHARED=${SOURCES_SHARED:.c=.o}


OBJFILES=$(OBJ_AUDIOOUTPUT) $(OBJ_DECODER) $(OBJ_GUI) $(OBJ_MAIN) $(OBJ_PLAYLIST) $(OBJ_SHARED)
INCPATH+=-I$(DIRECTORY_AUDIOOUTPUT) -I$(DIRECTORY_DECODER) -I$(DIRECTORY_GUI) -I$(DIRECTORY_MAIN) -I$(DIRECTORY_PLAYLIST) -I$(DIRECTORY_SHARED)
####



all: d11amp

clean: check-clean
	rm -rf d11amp
	rm -rf $(OBJFILES)

install: d11amp d11amp.1
	install -d -m 755 $(INSTALLBIN)
	install -m 755 d11amp   $(INSTALLBIN)/d11amp
	install -d -m 755 $(INSTALLMAN)/man1
	install -m 644 d11amp.1 $(INSTALLMAN)/man1/d11amp.1

d11amp: $(OBJFILES)
	$(LINK) -o $@ $(OBJFILES) $(LDFLAGS) $(LIBS)

d11amp.wsz: d11amp
	mkdir -p d11amp_wsz_tmp/
	./d11amp --gui.theme.dumpdefault=d11amp_wsz_tmp/
	zip -j d11amp.wsz d11amp_wsz_tmp/*

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CFLAGS_EXTRA) $(INCPATH) -c -o $@ $<

	


## in case some post-compilation checks are needed
## if not, please comment out this line
include ./checks.mk
