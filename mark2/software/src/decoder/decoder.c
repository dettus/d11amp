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
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "audiooutput.h"
#include "decoder.h"
#include "decoder_mp3.h"
#include "decoder_shared.h"

void *decoder_thread(void* handle)
{
	tHandleDecoder *pThis=(tHandleDecoder*)handle;
	FILE *f_debug;

	f_debug=fopen("debug.pcm","wb");
	while (1)
	{
		int retval;
		pthread_mutex_lock(&pThis->mutex);
		retval=DECODER_NODATA;
		if (pThis->state==STATE_PLAY)
		{
			switch(pThis->fileType)
			{
				case FILETYPE_MP3:
					retval=decoder_mp3_process(&(pThis->handleDecoderMp3),&pThis->songInfo,&pThis->pcmSink);
					break;	
				default:
					retval=DECODER_EOF;
					break;
			}
		}

		if (retval==DECODER_OK)
		{
			audiooutput_push(pThis->pHandleAudioOutput,&pThis->pcmSink);
			printf("decoded %d bytes\n",pThis->pcmSink.audio_bytes_num);
			fwrite(pThis->pcmSink.pAudioData,sizeof(char),pThis->pcmSink.audio_bytes_num,f_debug);
			
		}
		if (retval==DECODER_EOF)
		{
			printf("end of file\n");
			pThis->state=STATE_EOF;
		}	

		pthread_mutex_unlock(&pThis->mutex);
		usleep(1000);
	}
	fclose(f_debug);
}
int decoder_init(tHandleDecoder* pThis,tHandleAudioOutput* pHandleAudioOutput)
{
	printf("INIT %p\n",pThis);
	memset(pThis,0,sizeof(tHandleDecoder));
	pThis->pHandleAudioOutput=pHandleAudioOutput;
	pThis->state=STATE_NONE;
	pThis->fileType=FILETYPE_NONE;

	decoder_mp3_init(&(pThis->handleDecoderMp3));

	pthread_mutex_init(&pThis->mutex,NULL);
	pthread_create(&pThis->threadDecoder,NULL,&decoder_thread,(void*)pThis);
	return DECODER_OK;
}
int decoder_openfile(tHandleDecoder* pThis,char* filename)
{
	int retval;
	printf("OPEN %p\n",pThis);
	pThis->fileType=FILETYPE_MP3;

	pthread_mutex_lock(&pThis->mutex);
	retval=DECODER_OK;
	switch(pThis->fileType)
	{
		case FILETYPE_MP3:
			retval=decoder_mp3_open(&(pThis->handleDecoderMp3),filename,&pThis->songInfo);
			if (!retval)
			{
				pThis->state=STATE_STOP;
			}
			break;
		default:
			retval=DECODER_NOK;
			break;
	}
	pthread_mutex_unlock(&pThis->mutex);
	return retval;
}
int decoder_seek(tHandleDecoder* pThis,int second)
{
	int retval;
	retval=DECODER_OK;
	switch(pThis->fileType)
	{
		case FILETYPE_MP3:
			retval=decoder_mp3_jump(&(pThis->handleDecoderMp3),&(pThis->songInfo),second);
			pThis->songInfo.pos=second;
			break;
		default:
			retval=DECODER_NOK;
			break;
	}
	return retval;
}
int decoder_set_state(tHandleDecoder* pThis,eDecoderState nextState)
{
	int retval=DECODER_OK;
	pthread_mutex_lock(&pThis->mutex);

	switch(nextState)
	{
		case STATE_PLAY:
			if (pThis->state==STATE_NONE)
			{
				retval=DECODER_NOK;
			} else {
				if (pThis->state!=STATE_PAUSE)
				{
					decoder_seek(pThis,0);
				}
				pThis->state=STATE_PLAY;
			}
			break;
		case STATE_PAUSE:
			if (pThis->state==STATE_PLAY)
			{
				pThis->state=STATE_PAUSE;
			}
			break;
		case STATE_STOP:
			if (pThis->state!=STATE_NONE)
			{
				decoder_seek(pThis,0);
			}	
			pThis->state=STATE_STOP;
			break;
		default:
			break;
	
	}
	pthread_mutex_unlock(&pThis->mutex);
	return retval;	
}
int decoder_set_songPos(tHandleDecoder* pThis,int second)
{
	int retval=DECODER_OK;
	pthread_mutex_lock(&pThis->mutex);
	if (second<pThis->songInfo.len)
	{
		retval=decoder_seek(pThis,second);
	} else {
		retval=DECODER_NOK;
	}
	pthread_mutex_unlock(&pThis->mutex);
	return retval;
}
int decoder_get_songInfo(tHandleDecoder* pThis,tSongInfo* pSongInfo)
{
	pthread_mutex_lock(&pThis->mutex);
	memcpy(pSongInfo,&(pThis->songInfo),sizeof(tSongInfo));
	pthread_mutex_unlock(&pThis->mutex);
	return DECODER_OK;
	
}
