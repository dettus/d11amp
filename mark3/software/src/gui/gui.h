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
#ifndef	GUI_H
#define	GUI_H
#include <gtk/gtk.h>

#include "audiooutput.h"
#include "decoder.h"
#include "datastructures.h"
#include "theme_manager.h"
#include "window_equalizer.h"
#include "window_main.h"

typedef struct _tHandleGUI
{
	tHandleThemeManager handleThemeManager;
	tHandleWindowEqualizer   handleWindowEqualizer;
	tHandleWindowMain   handleWindowMain;
} tHandleGUI;

int gui_init(tHandleGUI* pThis,GtkApplication *app,tHandleAudioOutput *pHandleAudioOutput,tHandleDecoder *pHandleDecoder);
int gui_show(tHandleGUI* pThis);
int gui_get_shuffle_repeat(tHandleGUI* pThis,int* pShuffle,int* pRepeat);
#endif
