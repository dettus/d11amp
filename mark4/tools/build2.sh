#!/bin/sh

(
	cd ../software/src
	make
)

cc -c -o previewtest.o previewtest.c -I../software/src/gui	-I../software/src/shared -I../software/src/playlist \
	`pkg-config --cflags gdk-pixbuf-2.0 gtk4 libmpg123 portaudio-2.0 libzip`
cc -o previewtest.apl previewtest.o \
	../software/src/decoder/decoder.o	\
	../software/src/decoder/decoder_mp3.o	\
	../software/src/audiooutput/audiooutput.o	\
	../software/src/audiooutput/audiooutput_portaudio.o	\
	../software/src/audiooutput/audiooutput_preferences.o	\
	../software/src/playlist/playlist.o	\
	../software/src/main/controller.o	\
	../software/src/main/print_screens.o	\
	../software/src/shared/config.o	\
	../software/src/shared/license.o	\
	../software/src/gui/default_theme.o	\
	../software/src/gui/gui_helpers.o	\
	../software/src/gui/gui_top.o	\
	../software/src/gui/visualizer.o	\
	../software/src/gui/theme_manager.o \
	../software/src/gui/theme_elements.o	\
	../software/src/gui/window_license.o	\
	../software/src/gui/window_main.o	\
	../software/src/gui/window_playlist.o	\
	../software/src/gui/window_preferences.o	\
	../software/src/gui/window_equalizer.o	\
	`pkg-config --libs gdk-pixbuf-2.0 gtk4 libmpg123 portaudio-2.0 libzip` -lm -lpthread
	

