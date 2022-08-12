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
#include "theme_manager.h"

#define	WINDOW_MAIN_WIDTH	275
#define	WINDOW_MAIN_HEIGHT	116

typedef struct _tHandleWindowMain
{
// GTK related widgets
	GtkWidget *windowMain;	// this is the main window
	GtkWidget *picture;	// this is the picture widget, housing the pixbuf
	GtkGesture *gesture;	// this one is handling the user events
	GdkPixbuf *pixbuf;	// this one is what is being drawn upon
	int scaleFactor;

	tHandleThemeManager *pHandleThemeManager;	// pointer to the theme loader
} tHandleWindowMain;

int window_main_init(GtkApplication* app,tHandleWindowMain* pThis,tHandleThemeManager *pHandleThemeManager);	// to be called from the "activate" callback
int window_main_show(tHandleWindowMain* pThis);

#endif
