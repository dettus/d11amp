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
#include <out123.h>
#include <mpg123.h>
#include <unistd.h>
#include "audio.h"

int audio_startfile(tHandleAudio* pThis,char *filename)
{
	pthread_mutex_lock(&pThis->mPauseAudio);
	if (pThis->fptr!=NULL) 
	{
		fclose(pThis->fptr);	
	}
	pThis->fptr=fopen(filename,"rb");
	if (pThis->fptr==NULL)
	{
		return AUDIO_NOK;
	}
	pthread_mutex_unlock(&pThis->mPauseAudio);
	return AUDIO_OK;
}

void* audio_thread(void* hAudio)
{
	tHandleAudio* pThis=(tHandleAudio*)hAudio;
	int i;
	size_t n;
	off_t frame_num;
	unsigned char inbuf[1024];
	size_t audiobytes;
	unsigned char *audio;

	while (1)
	{
		pthread_mutex_lock(&pThis->mPauseAudio);
		if (pThis->fptr!=NULL && !feof(pThis->fptr))
		{
			n=fread(inbuf,sizeof(char),sizeof(inbuf),pThis->fptr);
			mpg123_feed((mpg123_handle*)pThis->mpg123handle,inbuf,n);
			mpg123_decode_frame((mpg123_handle*)pThis->mpg123handle,&frame_num,&audio,&audiobytes);
			{
				long rate;
				int channels;
				int encoding;
				mpg123_getformat((mpg123_handle*)pThis->mpg123handle,&rate,&channels,&encoding);

				if (rate!=pThis->cur_rate || channels!=pThis->cur_channels || encoding!=pThis->cur_encoding)
				{
					out123_stop((out123_handle*)pThis->out123handle);
					pThis->cur_rate=rate;;
					pThis->cur_channels=channels;
					pThis->cur_encoding=encoding;
					if (rate!=0)
					{
						out123_start((out123_handle*)pThis->out123handle,rate,channels,encoding);
					}
				}
			}
			out123_play((out123_handle*)pThis->out123handle,audio,audiobytes);
			if (feof(pThis->fptr))
			{
				out123_stop((out123_handle*)pThis->out123handle);
				pThis->cur_rate=0;
				pThis->cur_channels=0;
				pThis->cur_encoding=0;
			}
		}
		pthread_mutex_unlock(&pThis->mPauseAudio);
		usleep(10);
	}	
}

int audio_init(tHandleAudio* pThis)
{
	int err;

	pthread_mutex_init(&pThis->mPauseAudio,NULL);

	pThis->fptr=NULL;

	pThis->cur_channels=0;
	pThis->cur_rate=0;
	pThis->cur_encoding=0;
	pThis->out123handle=(void*)out123_new();
	out123_open((out123_handle*)pThis->out123handle,NULL,NULL);


	pThis->mpg123handle=NULL;
	err=mpg123_init();
//	printf("init err:%d %d\n",err,MPG123_OK);
	pThis->mpg123handle=(void*)mpg123_new(NULL,&err);   
	err=mpg123_open_feed((mpg123_handle*)pThis->mpg123handle);

	pthread_create(&pThis->audiothread,NULL, &audio_thread,(void*)pThis);
	return AUDIO_OK;
}
