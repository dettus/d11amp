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

#ifndef	AUDIOOUTPUT_PREFERENCES_H
#define	AUDIOOUTPUT_PREFERENCES_H

#include <gtk/gtk.h>

typedef struct _tAudioOutputPreferences
{
	int deviceIdx;
} tAudioOutputPreferences;

typedef struct _tHandleAudioOutputPreferences
{
	GtkWidget *dropDown;
	GtkStringList* audioDeviceList;

	tAudioOutputPreferences current_preferences;
	tAudioOutputPreferences next_preferences;
} tHandleAudioOutputPreferences;
int audiooutput_preferences_init(tHandleAudioOutputPreferences* pThis);
int audiooutput_preferences_get_widget(tHandleAudioOutputPreferences* pThis,GtkWidget **pWidget);
int audiooutput_preferences_activate(tHandleAudioOutputPreferences* pThis,int currDevIdx);
int audiooutput_preferences_apply(tHandleAudioOutputPreferences* pThis,tAudioOutputPreferences *pNextPrefs);


#endif
