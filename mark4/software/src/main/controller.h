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

#ifndef	CONTROLLER_H
#define	CONTROLLER_H

#include "datastructures.h"


// this is a list of events that could happen.
// they have effects on all the modules, and can be triggered by any one of them.
typedef enum
{
	eEVENT_NONE=0,
	eEVENT_ACTIVATE,
	eEVENT_PLAY_NEXT_FILE,
	eEVENT_PLAY_PREV_FILE,
	eEVENT_NEW_THEME,
	eEVENT_SET_VOLUME,
	eEVENT_SET_BALANCE,
	eEVENT_SET_EQUALIZER,

	eEVENT_OPEN_FILE,
	eEVENT_PLAY,
	eEVENT_PAUSE,
	eEVENT_STOP,
	eEVENT_JUMP
} eControllerEvent;

typedef union _tPayload
{
	int volume;
	int balance;
	struct 
	{
		int bar;
		int value;
	} equalizer;
	char* filename;
	int newSongPos;	// in seconds
} tPayload;

int controller_getBytes(int* bytes);
int controller_init(void* pControllerContext,void *pGtkApp);
int controller_event(void* pControllerContext,eControllerEvent event,tPayload* pPayload);
void controller_pushpcm(void* pControllerContext,tPcmSink *pPcmSink);
#endif

