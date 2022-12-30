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

#include "audiooutput.h"
#include "controller.h"
#include <string.h>
#include <stdio.h>

int audiooutput_init(tHandleAudioOutput *pThis,void* pControllerContext)
{
	int retval;
	memset(pThis,0,sizeof(tHandleAudioOutput));
	retval=RETVAL_NOK;
	pThis->pControllerContext=pControllerContext;
	pThis->audioBackend=eAUDIOBACKEND_PORTAUDIO;

	switch (pThis->audioBackend)
	{
		case eAUDIOBACKEND_PORTAUDIO:
			retval=audiooutput_portaudio_init(&(pThis->handleAudioOutputPortaudio));
			break;
		default:
			break;
	}

	audiooutput_preferences_init(&(pThis->handleAudioOutputPreferences));
	return retval;
}
int audiooutput_activate(tHandleAudioOutput *pThis)
{
	int retval;
	tPayload payload;

	
	retval=RETVAL_NOK;
	switch (pThis->audioBackend)
	{
		case eAUDIOBACKEND_PORTAUDIO:
			retval=audiooutput_portaudio_activate(&(pThis->handleAudioOutputPortaudio));
			break;
		default:
			break;
	}
	config_init(&(pThis->handleConfig),pThis->pControllerContext,"audiooutout.config");
	config_getint(&(pThis->handleConfig),"volume",&(payload.volume),100);
	controller_event(pThis->pControllerContext,eEVENT_SET_VOLUME,&payload);
	config_getint(&(pThis->handleConfig),"balance",&(payload.balance),0);
	controller_event(pThis->pControllerContext,eEVENT_SET_BALANCE,&payload);
	return retval;
}
int audiooutput_push(tHandleAudioOutput *pThis,tPcmSink *pPcmSink)
{
	int retval;

	retval=RETVAL_OK;
	switch(pThis->audioBackend)
	{
		case eAUDIOBACKEND_PORTAUDIO:
			retval=audiooutput_portaudio_push(&(pThis->handleAudioOutputPortaudio),pPcmSink->pAudioData,pPcmSink->audio_bytes_num,pPcmSink->audioFormat);
			break;
		default:
			retval=RETVAL_NOK;
			break;
	}
	return retval;
}
int audiooutput_stop(tHandleAudioOutput *pThis)
{
	int retval;
	retval=RETVAL_OK;
	switch(pThis->audioBackend)
	{
		case eAUDIOBACKEND_PORTAUDIO:
			retval=audiooutput_portaudio_stop(&(pThis->handleAudioOutputPortaudio));
			break;
		default:
			retval=RETVAL_NOK;
			break;
	}
	return retval;
}
int audiooutput_signal_volume(tHandleAudioOutput *pThis,int volume)
{
	config_setint(&(pThis->handleConfig),"volume",volume);
	return audiooutput_portaudio_setVolume(&(pThis->handleAudioOutputPortaudio),volume);
}
int audiooutput_signal_balance(tHandleAudioOutput *pThis,int balance)
{
	config_setint(&(pThis->handleConfig),"balance",balance);
	return audiooutput_portaudio_setBalance(&(pThis->handleAudioOutputPortaudio),balance);
}
int audiooutput_getVolume(tHandleAudioOutput *pThis,int* pVolume,int* pBalance)
{
	return audiooutput_portaudio_getVolume(&(pThis->handleAudioOutputPortaudio),pVolume,pBalance);
}
int audiooutput_getLastSamples(tHandleAudioOutput *pThis,signed short *pPcm,int n)
{
	return audiooutput_portaudio_getLastSamples(&(pThis->handleAudioOutputPortaudio),pPcm,n);
}
int audiooutput_commandline_option(tHandleAudioOutput* pThis,char* argument)
{
	int retval;
	int l;
	
	retval=RETVAL_NOK_COMMANDLINE;
	l=strlen(argument);
	if (strncmp("--audiooutput.portaudio.",argument,24)==0 && l>24)
	{
		retval=audiooutput_portaudio_commandline_option(&(pThis->handleAudioOutputPortaudio),argument);
	}
	return retval;
	
}
int audiooutput_get_preferences_widget(tHandleAudioOutput* pThis,GtkWidget** pWidget)
{
	int retval;
	retval=RETVAL_OK;

	retval=audiooutput_preferences_get_widget(&(pThis->handleAudioOutputPreferences),pWidget);
	
	return retval;
}

