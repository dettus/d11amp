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

#ifndef	GUI_TOP_H
#define	GUI_TOP_H

#include "config.h"
#include "datastructures.h"
#include "playlist.h"
#include "theme_manager.h"
#include "window_equalizer.h"
#include "window_main.h"
#include "window_playlist.h"
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

typedef struct _tHandleGuiTop
{
	// gtk related bureaucracy
	GtkApplication *app;
	tHandleConfig handleConfig;

		
	tHandleThemeManager handleThemeManager;
	tHandleWindowEqualizer handleWindowEqualizer;
	tHandleWindowMain handleWindowMain;
	tHandleWindowPlaylist handleWindowPlaylist;

	void *pControllerContext;
} tHandleGuiTop;

int gui_top_init(tHandleGuiTop* pThis,void* pControllerContext,GtkApplication* app,tHandlePlayList* pHandlePlayList);
int gui_top_start(tHandleGuiTop* pThis);
int gui_top_signal_scale(tHandleGuiTop* pThis,int scaleFactor);
int gui_top_signal_new_theme(tHandleGuiTop* pThis);
int gui_top_commandline_option(tHandleGuiTop* pThis,char* argument);

#endif
