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

#include "datastructures.h"
#include <pthread.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "audiooutput.h"
#include "pixbufloader.h"
#include "decoder.h"
#include "visualizer.h"

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
	unsigned int magic;
// gtk related
	GdkPixbuf *pixbufMain;		// the drawing area
	GdkPixbuf *pixbufScaled;	// scaled drawing area
	GdkPixbuf *pixbufSongInfo;	// song info
	GdkPixbuf *pixbufBitrate;	// bitrate in kbit/s
	GdkPixbuf *pixbufSamplerate;	// samplerate in kHz
	GdkPixbuf *pixbufVisualizer;	// oscilloscope, FFT or waterfall

	GtkWidget *widgetMainWindow;
	GtkWidget *widgetMainImage;
	GtkWidget *widgetMainLayout;

	tHandlePixbufLoader *pHandlePixbufLoader;
	tHandleDecoder *pHandleDecoder;
	tHandleAudioOutput *pHandleAudioOutput;


	tSongInfo songInfo_drawn;	// song info which has already been drawn
	int scrollpos;			// when the info is too long, it needs to scroll
	int scrolllen;			// for this many pixels

	tHandleVisualizer handleVisualizer;

	eMainWindowPressed lastPressed;


// miscellaneous
	int scaleFactor;		//
	int statusMonoSter;		//
	int statusPlayPause;
	int statusTitleBar;
	int statusClutterBar;
	int statusTimeDigits[4];	//
	int statusVolume;
	int statusBalance;
	int statusEqualizer;
	int statusPlaylist;
	int statusSongPos;		//
	int statusShuffle;
	int statusRepeat;


// window geometry
	int geometryX;
	int geometryY;
	int geometryWidth;
	int geometryHeight;

// since the animation should become fluidly, it is best to do it in its own thread
	pthread_t threadWindowMain;
	pthread_mutex_t mutex;	
	pthread_mutex_t mutex_click;	
} tHandleWindowMain;

int window_main_init(tHandleWindowMain* pThis,tHandlePixbufLoader* pHandlePixbufLoader,tHandleDecoder* pHandleDecoder,tHandleAudioOutput* pHandleAudiooutput);
int window_main_refresh_songinfo(tHandleWindowMain* pThis,tSongInfo songInfo);
int window_main_update_pcmsamples(tHandleWindowMain* pThis, tPcmSink *pPcmSink);
int window_main_run(tHandleWindowMain* pThis);
int window_main_get_shuffle_repeat(tHandleWindowMain* pThis,int* pShuffle,int* pRepeat);

#endif

