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
#include "decoder.h"
#include "gui_top.h"
#include "playlist.h"
#include "window_equalizer.h"
#include "window_main.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define	MAGIC	0x68654879		// 'yHeh'
#define	PCMRINGBUFSIZE	8192
#define	ENTRY_RING_SIZE	128
typedef struct _tControllerContext
{
	int magic;
	GtkApplication *app;
// handle to the top level modules
	tHandleAudioOutput handleAudioOutput;
	tHandleGuiTop handleGuiTop;
	tHandleDecoder handleDecoder;
	tHandlePlayList handlePlayList;

	pthread_mutex_t mutex;

	signed short pcmRingBuf[PCMRINGBUFSIZE];
	int pcmIdx;

	int entryRingBuf[ENTRY_RING_SIZE];
	int entryRingIdx;
	int scalefactor;
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
	pThis->scalefactor=1;
	retval|=playlist_init(&(pThis->handlePlayList));	// initializer BEFORE the gui
	retval|=gui_top_init(&(pThis->handleGuiTop),pControllerContext,pThis->app,&(pThis->handlePlayList));
	retval|=audiooutput_init(&(pThis->handleAudioOutput));
	retval|=decoder_init(&(pThis->handleDecoder),pControllerContext);
	



	
	srand(time(NULL));   // Initialization, should only be called once.
	pthread_mutex_init(&(pThis->mutex),NULL);


	
	return retval;
}
int controller_commandline_options(void* pControllerContext,tArguments *pArguments)
{
	tControllerContext *pThis=(tControllerContext*)pControllerContext;
	int i;
	int retval;
	int l;

	retval=RETVAL_OK;

	for (i=0;i<pArguments->argc;i++)
	{
		l=strlen(pArguments->argv[i]);
		if (strncmp(pArguments->argv[i],"--gui.",6)==0 && l>6)
		{
			retval|=gui_top_commandline_option(&(pThis->handlePlayList),pArguments->argv[i]);
		}
		if (strncmp(pArguments->argv[i],"--playlist.",11)==0 && l>11)
		{
			retval|=playlist_commandline_option(&(pThis->handlePlayList),pArguments->argv[i]);
		}
	}
	return retval;
}

int controller_event(void* pControllerContext,eControllerEvent event,tPayload* pPayload)
{
	tControllerContext *pThis=(tControllerContext*)pControllerContext;
	eDecoderState decoderState;
	int numberOfEntries;
	int currentEntry;
	int shuffle,repeat;
	tSongInfo songInfo;
	if (pThis->magic!=MAGIC)
	{
		fprintf(stderr,"Memory corruption detected\n");
		exit(1);
		return RETVAL_NOK;
	}
	// the idea is that one module is sending out an event, which is forwarded to the modules on which it has an effect.
	
	pthread_mutex_lock(&(pThis->mutex));	// one evenent triggering another event is discouraged
	decoder_pull_state(&(pThis->handleDecoder),&decoderState);
	playlist_get_numbers(&(pThis->handlePlayList),&numberOfEntries,&currentEntry);
	window_main_pull_shuffle_repeat(&(pThis->handleGuiTop.handleWindowMain),&shuffle,&repeat);
	switch(event)
	{
		case eEVENT_ACTIVATE:
			playlist_read_entry(&(pThis->handlePlayList),0,&songInfo,NULL);
			if (songInfo.filename[0])
			{
				decoder_open_file(&(pThis->handleDecoder),songInfo.filename);
			}
			gui_top_signal_new_theme(&(pThis->handleGuiTop));
			window_main_signal_volume(&(pThis->handleGuiTop.handleWindowMain),100);
			audiooutput_signal_volume(&(pThis->handleAudioOutput),100);
			window_main_signal_balance(&(pThis->handleGuiTop.handleWindowMain),0);
			audiooutput_signal_balance(&(pThis->handleAudioOutput),0);
			break;
		case eEVENT_EOF:
			{
				window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_END_OF_SONG);
				decoderState=DECODER_PLAY;
			}// implicitly, the end of a song triggers also a "NEXT SONG"
		case eEVENT_PLAY_NEXT_FILE:
			if (numberOfEntries)	// if there is a playlist
			{
				tSongInfo songInfo;
				int retrynum;
				int retval;
				retrynum=numberOfEntries;	// find a file which can be opened. retry this many times
				do
				{
					pThis->entryRingBuf[pThis->entryRingIdx]=currentEntry;
					pThis->entryRingIdx=(pThis->entryRingIdx+1)%ENTRY_RING_SIZE;
					if (repeat || ((currentEntry+1)<numberOfEntries))	// this is emulating a "feature" which I encountered: when the last entry in the playlist has been played, it stops. even in shuffle mode.
					{
						if (shuffle)
						{
							currentEntry=rand();
						} else {
							currentEntry=(currentEntry+1);
						}
						currentEntry%=numberOfEntries;
					}
					playlist_set_current_entry(&(pThis->handlePlayList),currentEntry);
					playlist_read_entry(&(pThis->handlePlayList),currentEntry,&songInfo,NULL);
					window_playlist_signal_jump_to_entry(&(pThis->handleGuiTop.handleWindowPlaylist),currentEntry);
					retval=decoder_open_file(&(pThis->handleDecoder),songInfo.filename);
					if (retval!=RETVAL_OK)
					{
						retrynum--;
					}
				}
				while ((retrynum>0) && retval!=RETVAL_OK);
				if (retval==RETVAL_OK)
				{
					decoder_jump(&(pThis->handleDecoder),0);
					if (decoderState==DECODER_PLAY)
					{
						decoder_play(&(pThis->handleDecoder));
						window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_PLAY);
					} else {
						window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_START_OF_SONG);	
					}
				}
			}
			else if (repeat && decoderState!=DECODER_NONE)
			{
				decoder_jump(&(pThis->handleDecoder),0);
				decoder_play(&(pThis->handleDecoder));
				window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_PLAY);
			}
			break;
		case eEVENT_PLAY_PREV_FILE:
			if (numberOfEntries)
			{
				tSongInfo songInfo;
				int retrynum;
				int retval;
				retrynum=ENTRY_RING_SIZE;	// go through the list of previous entries. try loading the file in there. retry this many times
				do
				{
					if (shuffle)
					{
						pThis->entryRingIdx--;
						if (pThis->entryRingIdx<0)
						{
							pThis->entryRingIdx+=ENTRY_RING_SIZE;
						}
						window_playlist_signal_jump_to_entry(&(pThis->handleGuiTop.handleWindowPlaylist),currentEntry);
						currentEntry=pThis->entryRingBuf[pThis->entryRingIdx]%numberOfEntries;
					} else {
						if (currentEntry==0)
						{
							currentEntry=numberOfEntries;
						}
						currentEntry--;
						if (currentEntry<0)
						{
							currentEntry=0;
						}
					}
					playlist_set_current_entry(&(pThis->handlePlayList),currentEntry);
					playlist_read_entry(&(pThis->handlePlayList),currentEntry,&songInfo,NULL);
					window_playlist_signal_jump_to_entry(&(pThis->handleGuiTop.handleWindowPlaylist),currentEntry);
					retval=decoder_open_file(&(pThis->handleDecoder),songInfo.filename);
					if (retval!=RETVAL_OK)
					{
						retrynum--;
					}
				}
				while ((retrynum>0) && retval!=RETVAL_OK);
				if (retval==RETVAL_OK)
				{
					decoder_jump(&(pThis->handleDecoder),0);
					if (decoderState==DECODER_PLAY)
					{
						decoder_play(&(pThis->handleDecoder));
						window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_PLAY);
					} else {
						window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_START_OF_SONG);	
					}
				}
			}
			else if (decoderState!=DECODER_NONE)
			{
				decoder_jump(&(pThis->handleDecoder),0);
				decoder_play(&(pThis->handleDecoder));
				window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_PLAY);
			}
			break;
		case eEVENT_NEW_THEME:
			gui_top_signal_new_theme(&(pThis->handleGuiTop));
			break;
		case eEVENT_SET_VOLUME:
			{
				window_main_signal_volume(&(pThis->handleGuiTop.handleWindowMain),pPayload->volume);
				audiooutput_signal_volume(&(pThis->handleAudioOutput),pPayload->volume);
			}
			break;
		case eEVENT_SET_BALANCE:
			{
				window_main_signal_balance(&(pThis->handleGuiTop.handleWindowMain),pPayload->balance);
				audiooutput_signal_balance(&(pThis->handleAudioOutput),pPayload->balance);
			}
			break;
		case eEVENT_SET_EQUALIZER:
			{
				window_equalizer_signal_bars(&(pThis->handleGuiTop.handleWindowEqualizer),pPayload->equalizer.bar,pPayload->equalizer.value);
			}
			break;

		case eEVENT_OPEN_FILE:
			{
				if (decoder_open_file(&(pThis->handleDecoder),pPayload->filename)==RETVAL_OK)
				{
					window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),(decoderState==DECODER_PLAY)?eINDICATOR_PLAY:eINDICATOR_START_OF_SONG);	
				} else {
					window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_NONE);
				}
			}	
			break;
		case eEVENT_PLAY:
			{
				decoder_play(&(pThis->handleDecoder));
				window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_PLAY);
			}
			break;
		case eEVENT_PAUSE:
			{
				decoder_pause(&(pThis->handleDecoder));
				audiooutput_stop(&(pThis->handleAudioOutput));
				window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_PAUSE);
			}
			break;
		case eEVENT_STOP:
			{
				decoder_pause(&(pThis->handleDecoder));
				decoder_jump(&(pThis->handleDecoder),0);
				audiooutput_stop(&(pThis->handleAudioOutput));
				window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),eINDICATOR_STOP);
			}
			break;
		case eEVENT_JUMP:
			{
				decoder_jump(&(pThis->handleDecoder),pPayload->newSongPos);
				if (pPayload->newSongPos==0)
				{
					window_main_signal_indicator(&(pThis->handleGuiTop.handleWindowMain),(decoderState==DECODER_PLAY)?eINDICATOR_PLAY:eINDICATOR_START_OF_SONG);	
				}
			}
			break;	
		case eEVENT_SCALE:
			{
				pThis->scalefactor*=2;
				if (pThis->scalefactor>=8)
				{
					pThis->scalefactor=1;
				}

				window_equalizer_signal_scalefactor(&(pThis->handleGuiTop.handleWindowEqualizer),pThis->scalefactor);
				window_main_signal_scalefactor(&(pThis->handleGuiTop.handleWindowMain),pThis->scalefactor);
				window_playlist_signal_scalefactor(&(pThis->handleGuiTop.handleWindowPlaylist),pThis->scalefactor);
				
			}
			break;
		default:
			printf("TODO: handle event %d\n",(int)event);
			break;	
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return RETVAL_OK;
}

void controller_pushpcm(void* pControllerContext,tPcmSink *pPcmSink)
{
	int i;
	int idx;
	signed short *ptr;
	tControllerContext *pThis=(tControllerContext*)pControllerContext;
	pthread_mutex_lock(&(pThis->mutex));
	audiooutput_push(&(pThis->handleAudioOutput),pPcmSink);

	idx=pThis->pcmIdx;
	// TODO: conversion
	ptr=(signed short*)pPcmSink->pAudioData;
	for (i=0;i<pPcmSink->audio_bytes_num/sizeof(short);i++)
	{
		pThis->pcmRingBuf[idx]=*ptr;
		ptr++;
		idx=(idx+1)%PCMRINGBUFSIZE;
	}
	pThis->pcmIdx=idx;
	
	pthread_mutex_unlock(&(pThis->mutex));
}

void controller_pull_songInfo(void* pControllerContext,tSongInfo *pSongInfo)
{
	tControllerContext *pThis=(tControllerContext*)pControllerContext;
	pthread_mutex_lock(&(pThis->mutex));
	decoder_pull_songInfo(&(pThis->handleDecoder),pSongInfo);
	pthread_mutex_unlock(&(pThis->mutex));

}
void controller_pull_pcm(void* pControllerContext,signed short* pPcmDestination,int num)
{
	int i;
	int idx;
	tControllerContext *pThis=(tControllerContext*)pControllerContext;
	pthread_mutex_lock(&(pThis->mutex));
	idx=pThis->pcmIdx-num;
	if (idx<0) idx+=PCMRINGBUFSIZE;

	for (i=0;i<num;i++)
	{
		pPcmDestination[i]=pThis->pcmRingBuf[idx];
		idx=(idx+1)%PCMRINGBUFSIZE;
	}
	pthread_mutex_unlock(&(pThis->mutex));
}

