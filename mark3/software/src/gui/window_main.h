/*

Copyright 2022, dettus@dettus.net

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/

#ifndef	WINDOW_MAIN_H
#define	WINDOW_MAIN_H
#include <gtk/gtk.h>
#include <pthread.h>
#include "audiooutput.h"
#include "decoder.h"
#include "theme_manager.h"
#include "visualizer.h"

#define	WINDOW_MAIN_WIDTH	275
#define	WINDOW_MAIN_HEIGHT	116
typedef enum
{
        PRESSED_NONE=0,
        PRESSED_CLUTTERBAR_O,
        PRESSED_CLUTTERBAR_A,
        PRESSED_CLUTTERBAR_I,
        PRESSED_CLUTTERBAR_D,
        PRESSED_CLUTTERBAR_V,
        PRESSED_VOLUME_SLIDER,
        PRESSED_BALANCE_SLIDER,
        PRESSED_EQUALIZER,
        PRESSED_PLAYLIST,
        PRESSED_SONGPOS,
        PRESSED_PREV,
        PRESSED_PLAY,
        PRESSED_PAUSE,
        PRESSED_STOP,
        PRESSED_NEXT,
        PRESSED_OPEN,
        PRESSED_SHUFFLE,
        PRESSED_REPEAT,
	PRESSED_INFO
} eMainWindowPressed;
#define	MAGIC	0x68654879	// 'yHeh'

typedef struct _tHandleWindowMain
{
// GTK related widgets
	GtkWidget *windowMain;	// this is the main window
	GtkWidget *picture;	// this is the picture widget, housing the pixbuf
	GtkGesture *gesture;	// this one is handling the user events
	GdkPixbuf *pixbuf;	// this one is what is being drawn upon
	GdkPixbuf *pixbufSongInfo;
	GdkPixbuf *pixbufBitrate;
	GdkPixbuf *pixbufSamplerate;

	int scaleFactor;

	tHandleThemeManager *pHandleThemeManager;	// pointer to the theme loader
	tHandleAudioOutput *pHandleAudioOutput;
	tHandleDecoder* pHandleDecoder;

	tHandleVisualizer handleVisualizer;


	// GUI status
	eMainWindowPressed lastPressed;
	int statusTimeDigits[4];
	int statusClutterBar;
	int statusMonoSter;
	int statusPlayPause;
	int statusTitleBar;
	int statusVolume;
	int statusBalance;
	int statusEqualizer;
	int statusPlaylist;
	int statusSongPos;
	int statusShuffle;
	int statusRepeat;


	// displaying song info
	tSongInfo songInfo;
	int scrolllen;
	int scrollpos;



	// background threads
	pthread_mutex_t mutex;
	pthread_t thread;	
	
} tHandleWindowMain;

int window_main_init(GtkApplication* app,tHandleWindowMain* pThis,tHandleThemeManager* pHandleThemeManager,tHandleAudioOutput* pHandleAudioOutput,tHandleDecoder *pHandleDecoder);	// to be called from the "activate" callback
int window_main_show(tHandleWindowMain* pThis);

#endif
