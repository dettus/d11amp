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
#include "gui_top.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define	MAGIC	0x68654879		// 'yHeh'
typedef struct _tControllerContext
{
	int magic;
	GtkApplication *app;
// handle to the top level modules
	tHandleAudioOutput handleAudioOutput;
	tHandleGuiTop handleGuiTop;
	void *pHandleDecoder;		// TODO: use the proper structure

	pthread_mutex_t mutex;
} tControllerContext;

int controller_getBytes(int* bytes)
{
	*bytes=sizeof(tControllerContext);
	return RETVAL_OK;
}

int controller_init(void* pControllerContext,void *pGtkApp)
{
	int retval;
	tControllerContext *pThis=(tControllerContext*)pControllerContext;

	retval=RETVAL_OK;
	pThis->magic=MAGIC;
	pThis->app=(GtkApplication*)pGtkApp;
	retval|=gui_top_init(&(pThis->handleGuiTop),pControllerContext,pThis->app);

	
	
	pthread_mutex_init(&(pThis->mutex),NULL);


	
	return retval;
}

int controller_event(void* pControllerContext,eControllerEvent event,void* payload)
{
	tControllerContext *pThis=(tControllerContext*)pControllerContext;
	if (pThis->magic!=MAGIC)
	{
		fprintf(stderr,"Memory corruption detected\n");
		exit(1);
		return RETVAL_NOK;
	}
	// the idea is that one module is sending out an event, which is forwarded to the modules on which it has an effect.
	
	pthread_mutex_lock(&(pThis->mutex));	// one evenent triggering another event is discouraged
	switch(event)
	{
		case eEVENT_ACTIVATE:
			gui_top_signal_new_theme(&(pThis->handleGuiTop));
			break;
		case eEVENT_PLAY_NEXT_FILE:
			// repeat=gui_check_repeat_button();
			// random=gui_check_random_button();
			// filename=playlist_get_next_filename(repeat,random);
			// if filename!=NULL decoder_open_file(filename);
			// else decoder_stop();
			break;
		case eEVENT_PLAY_PREV_FILE:
			// repeat=gui_check_repeat_button();
			// filename=playlist_get_prev_filename(repeat);
			// if filename!=NULL decoder_open_file(filename);
			// else decoder_stop();
			break;
		case eEVENT_NEW_THEME:
			gui_top_signal_new_theme(&(pThis->handleGuiTop));
			break;
		default:
			printf("TODO: handle event %d\n",(int)event);
			break;	
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return RETVAL_OK;
}

