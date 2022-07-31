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
#include <string.h>
#include "mp3decoder.h"

int mp3decode_init(tHandleMp3Decode* pThis)
{
	int err;
	pThis->done=1;
	pThis->audioFormat.channels=0;
	pThis->audioFormat.rate=0;
	pThis->audioFormat.encoding=0;
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
	off_t audio_bytes_num;
	int err;
	int l;
	
	if (!pThis->done)
	{
		mpg123_close((mpg123_handle*)pThis->mpg123handle);
		pThis->done=1;
	}
	err=mpg123_open((mpg123_handle*)pThis->mpg123handle,filename);
	l=strlen(filename);
	if (l>1023) l=1023;
	memcpy(pThis->filename,filename,l+1);
	pThis->filename[1023]=0;
	
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
	pThis->audioFormat.channels=channels;
	pThis->audioFormat.rate=rate;
	pThis->audioFormat.encoding=encoding;

	err=mpg123_seek((mpg123_handle*)pThis->mpg123handle,0,SEEK_END);		// go to the end of the file
	audio_bytes_num=mpg123_tell((mpg123_handle*)pThis->mpg123handle);
	pThis->file_len_seconds=((int)audio_bytes_num/(int)rate);

	err=mpg123_seek((mpg123_handle*)pThis->mpg123handle,pThis->startpos,SEEK_SET);		// go to start position
	audio_bytes_num=mpg123_tell((mpg123_handle*)pThis->mpg123handle);
	pThis->file_pos_seconds=((int)audio_bytes_num/(int)rate);

	pThis->done=0;
	return DECODER_OK;
}



int mp3decode_process(tHandleMp3Decode* pThis,tPcmSink *pPcmSink,int* file_len_seconds,int* file_pos_seconds)
{
	unsigned char *pAudioPtr;
	off_t framenum;
	size_t audio_bytes_num;
	long rate;
	int channels;
	int encoding;

	int retval;
	int err;

	retval=DECODER_OK;

	err=mpg123_decode_frame((mpg123_handle*)pThis->mpg123handle,&framenum,&pAudioPtr,&audio_bytes_num);
	pPcmSink->pcmSamples=pAudioPtr;
	pPcmSink->audio_bytes_num=(int)audio_bytes_num;
	if (err==MPG123_DONE)
	{
		pThis->done=1;
		pThis->startpos=0;
		mpg123_close((mpg123_handle*)pThis->mpg123handle);
		retval=DECODER_FILE_DONE;
	}
		
	mpg123_getformat((mpg123_handle*)pThis->mpg123handle,&rate,&channels,&encoding);
	pThis->audioFormat.channels=channels;
	pThis->audioFormat.rate=rate;
	pThis->audioFormat.encoding=encoding;

	audio_bytes_num=mpg123_tell((mpg123_handle*)pThis->mpg123handle);
	pThis->file_pos_seconds=((int)audio_bytes_num/(int)rate);


	*file_len_seconds=pThis->file_len_seconds;
	*file_pos_seconds=pThis->file_pos_seconds;

	//memcpy(pPcmSink->pcmbuf,pAudioPtr,(int)audio_bytes_num);
	pPcmSink->audioFormat=pThis->audioFormat;
	if (retval==DECODER_FILE_DONE)
	{
		mp3decode_jump(pThis,0);	
	}
	
	return retval;
}

int mp3decode_jump(tHandleMp3Decode* pThis,int second)
{
	int err;
	if (!pThis->done)
	{
		err=mpg123_seek((mpg123_handle*)pThis->mpg123handle,second*pThis->audioFormat.rate,SEEK_SET);		// go to the beginning of the file
		pThis->file_pos_seconds=second;
			
	} else {
		pThis->startpos=second;
	}
	return DECODER_OK;
}

int mp3decode_getsonginfo(tHandleMp3Decode* pThis,char** songinfo,int* bitrate,int* rate,int* channels)
{
	struct mpg123_frameinfo frameInfo;
	mpg123_info((mpg123_handle*)pThis->mpg123handle,&frameInfo);
	*rate=frameInfo.rate;
	*bitrate=frameInfo.bitrate;
	*songinfo=pThis->filename;
	*channels=pThis->audioFormat.channels;

	return 0;
}
