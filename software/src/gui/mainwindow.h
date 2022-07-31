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

#ifndef	MAINWINDOW_H
#define	MAINWINDOW_H
#include <gtk/gtk.h>
#include "elements.h"
#include "pixbufloader.h"
#include "decoder_main.h"

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
	PRESSED_REPEAT
} eMainWindowPressed;
typedef struct _tHandleMainWindow
{
	GdkPixbuf* mainPixbuf;	// the drawing area
	GdkPixbuf* scaledPixbuf;	// scaled drawing area
	GdkPixbuf* songInfoPixbuf;	// for the song info
	int songInfo_scrollpos;
	int songInfo_scrolllen;
	
	// gtk related 
	GtkWidget* mainWindow;
	GtkWidget* mainImage;
	GtkWidget* layout;

	// status
	int posx;
	int posy;
	int width;
	int height;
	int scalefactor;

	// pointer to other handles
	tHandlePixbufLoader *pHandlePixbufLoader;
	tHandleDecoderMain *pHandleDecoderMain;



	// interactive elements
	int titlebar_active;
	int clutterbar_shown;
	int time_digit[4];
	int playpause_state;
	int mono_stereo;
	
	int volume_setting;	// 0..28
	int volume_slider;

	int balance_setting;	// -18..0..18

	
	int equalizer_active;
	int playlist_active;

	int songpos;


	int shuffle_active;
	int repeat_active;

	eMainWindowPressed pressed;

	int file_len_seconds;
	int file_pos_seconds;

} tHandleMainWindow;
int mainwindow_init(tHandleMainWindow* pThis,tHandlePixbufLoader *pPixbuf,tHandleDecoderMain *pHandleDecoderMain);
int mainwindow_setpos(tHandleMainWindow* pThis,int file_len,int file_pos);

#define	MAINWINDOW_OK	0
#define	MAINWINDOW_NOK	-1
#endif

