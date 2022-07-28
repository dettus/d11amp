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
#include <mpg123.h>
#include <unistd.h>
#include "mp3decoder.h"

int mp3decode_init(tHandleMp3Decode* pThis)
{
	pThis->done=1;
	pThis->audioformat.channels=0;
	pThis->audioformat.rate=0;
	pThis->audioformat.encoding=0;
	pThis->cur_rate=0;
	pThis->cur_encoding=0;
	pThis->mpg123handle=NULL;
	pThis->mpg123handle=(void*)mpg123_new(NULL,&err);
	pThis->file_len_seconds=0;
	pThis->file_pos_seconds=0;
	pThis->startpos=0;

	return	DECODER_OK;
}

int mp3decode_fileopen(tHandleMp3Decode* pThis,char* filename)
{
	long rate;
	int channels;
	int encoding;
	size_t audiobytes;
	unsigned char *pAudioPtr;
	off_t framenum;
	off_t audio_samples_num;
	int err;
	
	if (!pThis->done)
	{
		mpg123_close((mpg123_handle*)pThis->mpg123handle);
		pThis->done=1;
	}
	err=mpg123_open(pThis->handle,filename);
	if (err)	// TODO: better check
	{
		return DECODER_NOK;
	}
	err=mpg123_decode_frame((mpg123_handle*)pThis->mpg123handle,&framenum,&pAudioPtr,&audiobytes);
	if (err==MPG123_DONE)
	{
		return DECODER_FILE_DONE;
	}
	err=mpg123_getformat((mpg123_handle*)pThis->mpg123handle,&rate,&channels,&encoding);
	pThis->audioformat.channels=channels;
	pThis->audioformat.rate=rate;
	pThis->audioformat.encoding=encoding;

	err=mpg123_seek((mpg123_handle*)pThis->mpg123handle,0,SEEK_END);		// go to the end of the file
	audio_samples_num=mpg123_tell((mpg123_handle*)pThis->mpg123handle);
	pThis->file_len_seconds=((int)audio_samples_num/(int)rate);

	err=mpg123_seek((mpg123_handle*)pThis->mpg123handle,pThis->startpos,SEEK_SET);		// go to start position
	audio_samples_num=mpg123_tell((mpg123_handle*)pThis->mpg123handle);
	pThis->file_pos_seconds=((int)audio_samples_num/(int)rate);

	pThis->done=0;
	return DECODER_OK;
}



int mp3decode_process(tHandleMp3Decode* pThis,tPCMSink *pPcmSink,int* file_len_seconds,int* file_pos_seconds)
{
	unsigned char *pAudioPtr;
	off_t framenum;
	off_t audio_bytes_num;
	long rate;
	int channels;
	int encoding;

	int retval;

	retval=DECODER_OK;

	err=mpg123_decode_frame((mpg123_handle*)pThis->mpg123handle,&frame_num,&pAudioPtr,&audio_bytes_num);
	if (err==MPG123_DONE)
	{
		pThis->done=1;
		pThis->startpos=0;
		mpg123_close((mpg123_handle*)pThis->mpg123handle);
		retval=DECODER_FILE_DONE;
	}
		
	mpg123_getformat((mpg123_handle*)pThis->mpg123handle,&rate,&channels,&encoding);
	pThis->audioformat.channels=channels;
	pThis->audioformat.rate=rate;
	pThis->audioformat.encoding=encoding;

	audio_samples_num=mpg123_tell((mpg123_handle*)pThis->mpg123handle);
	pThis->file_pos_seconds=((int)audio_samples_num/(int)rate);


	*file_len_seconds=pThis->file_len_seconds;
	*file_pos_seconds=pThis->file_pos_seconds;

	memcpy(pPcmSink->pcmbuf,audio,(int)audio_bytes_num);
	pPcmSink->audio_bytes_num=(int)audio_bytes_num;
	pPcmSink->audioformat=pThis->audioformat;
	
	return retval;
}

int mp3decode_jump(tHandleMp3Decode* pThis,int second)
{
	if (!pThis->done)
	{
		err=mpg123_seek((mpg123_handle*)pThis->mpg123handle,second,SEEK_SET);		// go to the beginning of the file
		pThis->file_pos_seconds=second;
			
	} else {
		pThis->startpos=second;
	}
	return DECODER_OK;
}