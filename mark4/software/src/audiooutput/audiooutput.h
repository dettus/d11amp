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

#ifndef AUDIOOUTPUT_H
#define	AUDIOOUTPUT_H
#include <gtk/gtk.h>
#include "config.h"
#include "datastructures.h"
#include "audiooutput_portaudio.h"
#include "audiooutput_preferences.h"

typedef enum
{
	eAUDIOBACKEND_NONE=0,
	eAUDIOBACKEND_PORTAUDIO
} eAudioBackend;
typedef struct _tHandleAudioOutput
{
	tHandleConfig handleConfig;
	tHandleAudioOutputPortaudio handleAudioOutputPortaudio;
	tHandleAudioOutputPreferences handleAudioOutputPreferences;
	int volume;
	eAudioBackend audioBackend;
	void* pControllerContext;
	
} tHandleAudioOutput;

int audiooutput_init(tHandleAudioOutput *pThis,void* pControllerContext);
int audiooutput_activate(tHandleAudioOutput *pThis);
int audiooutput_push(tHandleAudioOutput *pThis,tPcmSink *pPcmSink);
int audiooutput_stop(tHandleAudioOutput *pThis);
int audiooutput_signal_volume(tHandleAudioOutput *pThis,int volume);
int audiooutput_signal_balance(tHandleAudioOutput *pThis,int balance);
int audiooutput_getVolume(tHandleAudioOutput *pThis,int* pVolume,int* pBalance);
int audiooutput_getLastSamples(tHandleAudioOutput *pThis,signed short *pPcm,int n);
int audiooutput_commandline_option(tHandleAudioOutput* pThis,char* argument);
void audiooutput_help();
int audiooutput_get_preferences_widget(tHandleAudioOutput* pThis,GtkWidget** pWidget);
int audiooutput_activate_preferences(tHandleAudioOutput* pThis);
int audiooutput_apply_preferences(tHandleAudioOutput* pThis);

#endif

