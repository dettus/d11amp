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

#include "mp3decoder.h"

void* decode_thread(void* handle)
{
	tHandleDecoderMain* pThis=(tHandleDecoderMain*)pThis;
	while (1)
	{
		int retval;
		pthread_mutex_lock(&(pThis->mutex));
		if (pThis->state==STATE_PLAY)
		{
			switch (pThis->current_filetype)
			{
				case FILETYPE_MP3:
					retval=mp3decode_process(&(pThis->handlemp3decoder),&(pThis->pcmSink),&(pThis->file_len_seconds),&(pThis->file_pos_seconds));
					break;
				default:
					break;
			}
			if (retval==DECODER_FILE_DONE)
			{
				pThis->state=STATE_STOP;
				pThis->done=1;
			}
			// TODO: audio_push_sink_in_a_blocking_way(pThis->pcmSink);		
			
		}


		pthread_mutex_unlock(&(pThis->mutex));
		if (pThis->state!=STATE_PLAY)
		{
			usleep(1000);	// TODO: find a better prime
		}
	}
}



int decode_init(tHandleDecoderMain* pThis)
{

	pThis->current_filetype=FILETYPE_NONE;
	mp3decoder_init(&(pThis->handlemp3decoder));
	pThis->done=1;
	pThis->state=STATE_NONE;	
	pThis->file_len_seconds=0;
	pThis->file_pos_seconds=0;


        pthread_mutex_init(&pThis->mutex,NULL);
	pthread_create(&pThis->audiothread,NULL, &audio_thread,(void*)pThis);

	return	DECODE_OK;
}

int decode_fileopen(tHandleDecoderMain* pThis,char* filename)
{
	pthread_mutex_lock(&(pThis->mutex));
	// TODO: pThis->current_filetype=detect_filetype(filename)
	switch (pThis->current_filetype)
	{
		case FILETYPE_MP3:
			if (!mp3decode_fileopen(&(pThis->handlemp3decoder),filename))
			{
				if (pThis->state==STATE_NONE)
				{
					pThis->state=STATE_STOP;
					pThis->done=0;
				}
			}
			break;
		default:
			break;
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return	DECODE_OK;
}
int decode_fileplay(tHandleDecoderMain* pThis)
{
	pthread_mutex_lock(&(pThis->mutex));
	if (pThis->state!=STATE_NONE)
	{
		pThis->state=STATE_PLAY;
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return	DECODE_OK;
}
int decode_filestop(tHandleDecoderMain* pThis)
{
	pthread_mutex_lock(&(pThis->mutex));
	if (pThis->state!=STATE_NONE)
	{
		mp3decode_jump(&(pThis->handlemp3decoder),0);	// next time somebody presses "STOP", the file is alread at the beginning
		pThis->state=STATE_STOP;
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return	DECODE_OK;
}
int decode_filepause(tHandleDecoderMain* pThis)
{
	pthread_mutex_lock(&(pThis->mutex));
	if (pThis->state!=STATE_NONE)
	{
		pThis->state=STATE_PAUSE;
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return	DECODE_OK;
}
int decode_filejump(tHandleDecoderMain* pThis,int second)
{
	pthread_mutex_lock(&(pThis->mutex));
	if (pThis->state!=STATE_NONE)
	{
		mp3decode_jump(&(pThis->handlemp3decoder),second);
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return	DECODE_OK;
}
int decode_getpos(tHandleDecoderMain* pThis,int* file_len_seconds,int* file_pos_seconds)
{
	*file_len_seconds=0;
	*file_pos_seconds=0;
	pthread_mutex_lock(&(pThis->mutex));
	if (pThis->state!=STATE_NONE)
	{
		*file_len_seconds=pThis->file_len_seconds;
		*file_pos_seconds=pThis->file_pos_seconds;
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return DECODE_OK;
}
int decode_getdone(tHandleDecoderMain* pThis,int* done)
{
	pthread_mutex_lock(&(pThis->mutex));
	if (pThis->state!=STATE_NONE)
	{
		*done=pThis->file_len_seconds;
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return DECODE_OK;
}
