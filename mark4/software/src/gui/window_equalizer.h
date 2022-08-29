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

#include "datastructures.h"
#include "theme_manager.h"
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define	WINDOW_EQUALIZER_WIDTH	275
#define	WINDOW_EQUALIZER_HEIGHT	116


#define	BAR_NUM	11		// 1 preamp+10 bands
typedef struct _tWindowEqualizerStatus
{
	eOnOff equalizer;
	eOnOff automatic;

	int bar[BAR_NUM];		// -100...0...100
	int barY[BAR_NUM];
} tWindowEqualizerStatus;

typedef struct _tHandleWindowEqualizer
{
	// gtk related bureaucracy
	GtkApplication *app;
	GdkPixbuf *pixbuf;
	GtkWidget *picture;
	GtkWidget *window;
	GtkGesture *gesture_click;
	GtkGesture *gesture_drag;

	// pixbufs to draw on
	GdkPixbuf *pixbufBackground;
	tHandleThemeManager *pHandleThemeManager;
	void *pControllerContext;
	ePressable lastPressed;
	int pressedX;
	int pressedY;

	tWindowEqualizerStatus status;

} tHandleWindowEqualizer;

int window_equalizer_init(tHandleWindowEqualizer* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app);
int window_equalizer_signal_new_theme(tHandleWindowEqualizer* pThis);
int window_equalizer_show(tHandleWindowEqualizer *pThis);
int window_equalizer_hide(tHandleWindowEqualizer *pThis);


#endif