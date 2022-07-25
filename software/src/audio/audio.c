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
#include <string.h>
#include "audio.h"

int audio_startfile(tHandleAudio* pThis,char *filename)
{
	pthread_mutex_lock(&pThis->mPauseAudio);
	memcpy(pThis->cur_filename,filename,strlen(filename)+1);
	pThis->cur_state=AUDIOSTATE_START;
	pthread_mutex_unlock(&pThis->mPauseAudio);
	return AUDIO_OK;
}

void* audio_thread(void* hAudio)
{
	tHandleAudio* pThis=(tHandleAudio*)hAudio;
	off_t frame_num;
	off_t sample_num;
	size_t audiobytes;
	unsigned char *audio;
	FILE *f_debug;

	int isopen;
	int err;

	long rate;
	int channels;
	int encoding;

	isopen=0;
	f_debug=fopen("debug.pcm","wb");
	while (1)
	{
		pthread_mutex_lock(&pThis->mPauseAudio);
		switch(pThis->cur_state)
		{
			case AUDIOSTATE_START:
				{
					if (isopen)
					{
						mpg123_close(pThis->mpg123handle);			
					}
					pThis->cur_channels=0;
					pThis->cur_rate=0;
					pThis->cur_encoding=0;

					err=mpg123_open((mpg123_handle*)pThis->mpg123handle,pThis->cur_filename);
					if (!err)
					{
						// find out the encoding
						err=mpg123_decode_frame((mpg123_handle*)pThis->mpg123handle,&frame_num,&audio,&audiobytes);
						err=mpg123_getformat((mpg123_handle*)pThis->mpg123handle,&rate,&channels,&encoding);
						pThis->cur_rate=rate;
						pThis->cur_channels=channels;
						pThis->cur_encoding=encoding;

						// find out the length of the file

						err=mpg123_seek((mpg123_handle*)pThis->mpg123handle,0,SEEK_END);
						sample_num=mpg123_tell((mpg123_handle*)pThis->mpg123handle);
						err=mpg123_seek((mpg123_handle*)pThis->mpg123handle,0,SEEK_SET);

						pThis->song_len_seconds=(int)sample_num/((int)rate);
						pThis->song_pos_seconds=0;

						isopen=1;
						pThis->cur_state=AUDIOSTATE_PLAY;
					} else {
						isopen=0;
					}
				}
				break;
			case AUDIOSTATE_PLAY:
				{
					err=mpg123_decode_frame((mpg123_handle*)pThis->mpg123handle,&frame_num,&audio,&audiobytes);
					if (err==-12)	// TODO: apparently, this is the error code for the end of the song
					{
						pThis->cur_state=AUDIOSTATE_STOP;
						out123_stop((out123_handle*)pThis->out123handle);
						pThis->cur_rate=0;
						pThis->cur_channels=0;
						pThis->cur_encoding=0;
					} else {
						// play. make sure that the output driver has the correct configuration
						size_t n;
						err=mpg123_getformat((mpg123_handle*)pThis->mpg123handle,&rate,&channels,&encoding);
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
						fwrite(audio,sizeof(char),(int)audiobytes,f_debug);
						n=out123_play((out123_handle*)pThis->out123handle,audio,audiobytes);


						// find out where we are
						sample_num=mpg123_tell((mpg123_handle*)pThis->mpg123handle);
						pThis->song_pos_seconds=(int)sample_num/((int)rate);

						printf("%3d/%3d> frame_num:%6d audiobytes:%d->%d rate:%d channels:%d encoding:%d\n",pThis->song_pos_seconds,pThis->song_len_seconds,(int)frame_num,(int)audiobytes,(int)n,(int)rate,channels,encoding);
					}
				}
				break;
			default:
				usleep(10);	// take it easy...
				break;
		}
		pthread_mutex_unlock(&pThis->mPauseAudio);
	}
}

int audio_init(tHandleAudio* pThis)
{
	int err;

	pthread_mutex_init(&pThis->mPauseAudio,NULL);


	memset(pThis->cur_filename,0,sizeof(pThis->cur_filename));
	pThis->cur_state=AUDIOSTATE_STOP;
	pThis->cur_channels=0;
	pThis->cur_rate=0;
	pThis->cur_encoding=0;
	pThis->out123handle=(void*)out123_new();
	out123_open((out123_handle*)pThis->out123handle,NULL,NULL);


	pThis->mpg123handle=NULL;
	err=mpg123_init();
	pThis->mpg123handle=(void*)mpg123_new(NULL,&err);   
	err=mpg123_open_feed((mpg123_handle*)pThis->mpg123handle);

	pthread_create(&pThis->audiothread,NULL, &audio_thread,(void*)pThis);
	return AUDIO_OK;
}
