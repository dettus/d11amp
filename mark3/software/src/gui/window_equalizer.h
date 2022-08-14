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

#ifndef	WINDOW_EQUALIZER_H
#define	WINDOW_EQUALIZER_H
#include <gtk/gtk.h>
#include <pthread.h>
#include "datastructures.h"
#include "decoder.h"
#include "theme_manager.h"

#define	WINDOW_EQUALIZER_WIDTH 275
#define	WINDOW_EQUALIZER_HEIGHT 116

#define	BAR_NUM		11

typedef struct _tHandleWindowEqualizer
{
	GtkWidget *windowEqualizer;
	GtkWidget *picture;
	GtkGesture *gesture;
	GdkPixbuf *pixbuf;
	int scaleFactor;

	tHandleDecoder* pHandleDecoder;
	tHandleThemeManager* pHandleThemeManager;
	pthread_mutex_t mutex;

	int statusDB[BAR_NUM];		// -14...0..14
	int statusSpline[113];


	int statusTitlebar;
	int onOff;
	int automaticOnOff;
} tHandleWindowEqualizer;


int window_equalizer_init(GtkApplication* app,tHandleWindowEqualizer* pThis,tHandleThemeManager* pHandleThemeManager,tHandleDecoder *pHandleDecoder);
int window_equalizer_show(tHandleWindowEqualizer* pThis);
int window_equalizer_hide(tHandleWindowEqualizer* pThis);
#endif
