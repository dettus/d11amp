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

#include "decoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *decoder_thread(void* handle)
{
	int retval;
	FILE *f;

	f=fopen("debug.pcm","wb");
	tHandleDecoder* pThis=(tHandleDecoder*)handle;
	while (1)
	{	
		pthread_mutex_lock(&pThis->mutex);
		if (pThis->state==DECODER_PLAY)
		{
			switch(pThis->fileType)
			{
				case FILETYPE_MP3:
					retval=decoder_mp3_process(&pThis->handleDecoderMp3,&(pThis->songInfo),&(pThis->pcmSink));
					if (retval>=RETVAL_NOK)
					{
				//		printf("%3d/%3d> audio bytes:%d\n",pThis->songInfo.pos,pThis->songInfo.len,pThis->pcmSink.audio_bytes_num);
						fwrite(pThis->pcmSink.pAudioData,sizeof(char),pThis->pcmSink.audio_bytes_num,f);
					}
					if (retval==RETVAL_DECODER_EOF)
					{
						pThis->state=DECODER_EOF;	
					}
					break;
				default:
					break;
			}	
		} else {
			usleep(1000);
		}
		pthread_mutex_unlock(&pThis->mutex);
		usleep(1000);
	}
}
int decoder_init(tHandleDecoder* pThis,void* pControllerContext)
{
	int retval;

	memset(pThis,0,sizeof(tHandleDecoder));
	pThis->pControllerContext=pControllerContext;



	retval=RETVAL_OK;
	retval|=decoder_mp3_init(&pThis->handleDecoderMp3);

	pthread_mutex_init(&pThis->mutex,NULL);
	pthread_create(&pThis->thread,NULL,&decoder_thread,(void*)pThis);

	return retval;	
}
int decoder_open_file(tHandleDecoder* pThis,char* filename)
{

	int retval;

	retval=RETVAL_OK;
	pthread_mutex_lock(&pThis->mutex);
	// TODO: DETERMINE THE FILETYPE
	pThis->fileType=FILETYPE_MP3;
	switch(pThis->fileType)
	{
		case FILETYPE_MP3:
			retval|=decoder_mp3_open_file(&pThis->handleDecoderMp3,filename,&pThis->songInfo);	
			break;
		default:
			pThis->state=DECODER_NONE;
			break;
	}
	if (retval==RETVAL_OK)
	{
		pThis->state=DECODER_PAUSE;
	}
	pthread_mutex_unlock(&pThis->mutex);

	return retval;
}
int decoder_play(tHandleDecoder* pThis)
{
	int retval;

	retval=RETVAL_OK;
	pthread_mutex_lock(&pThis->mutex);
	if (pThis->state==DECODER_NONE)
	{
		retval=RETVAL_NOK;
	} else {
		pThis->state=DECODER_PLAY;
	}	
	pthread_mutex_unlock(&pThis->mutex);
	return retval;
}
int decoder_pause(tHandleDecoder* pThis)
{
	int retval;

	retval=RETVAL_OK;
	pthread_mutex_lock(&pThis->mutex);
	if (pThis->state==DECODER_NONE)
	{
		retval=RETVAL_NOK;
	}
	else if (pThis->state==DECODER_PLAY)
	{
		pThis->state=DECODER_PAUSE;
	}	
	pthread_mutex_unlock(&pThis->mutex);
	return retval;
}
int decoder_jump(tHandleDecoder* pThis,int newSongPos)
{
	int retval;
	retval=RETVAL_OK;
	pthread_mutex_lock(&pThis->mutex);
	if (newSongPos>pThis->songInfo.len)
	{
		newSongPos=pThis->songInfo.len;
	} 
	if (newSongPos<0)
	{
		newSongPos=0;
	}
	switch(pThis->fileType)
	{
		case FILETYPE_MP3:
			retval|=decoder_mp3_jump(&pThis->handleDecoderMp3,&(pThis->songInfo),newSongPos);
			break;
		default:
			pThis->state=DECODER_NONE;
			break;
	}
	pthread_mutex_unlock(&pThis->mutex);
	return retval;
}
