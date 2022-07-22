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
typedef struct _tHandleMainWindow
{
	GdkPixbuf* mainPixbuf;	// the drawing area
	
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



	// interactive elements
	int titlebar_active;
	int clutterbar_shown;
	char clutterbar_pressed;
	int time_digit[4];
	int playpause_state;
	int mono_stereo;
	
	int volume_setting;	// 0..28
	int volume_slider;

	int balance_setting;	// -18..0..18
	int balance_pressed;

	
	int equalizer_pressed;
	int equalizer_active;
	int playlist_pressed;
	int playlist_active;

	int songpos;
	int songpos_pressed;

	int cbutton_pressed;

	int shuffle_pressed;
	int shuffle_active;
	int repeat_pressed;
	int repeat_active;

} tHandleMainWindow;
int mainwindow_init(tHandleMainWindow* pThis,tHandlePixbufLoader *pPixbuf);
#define	MAINWINDOW_OK	0
#define	MAINWINDOW_NOK	-1
#endif

