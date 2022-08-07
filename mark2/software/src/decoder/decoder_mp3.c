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
#include <stdio.h>
#include <string.h>
#include "decoder_mp3.h"
#include "decoder_shared.h"

// helper functions
int decoder_mp3_convert_audioencoding(int mpg123_encoding,eAudioEncoding *pAudioCoding)
{
	int retval;

	retval=DECODER_OK;
	switch(mpg123_encoding)
	{
		case 0xd0:	*pAudioCoding=eAUDIO_ENCODING_S16LE;break;	// TODO: why 0xd0?
		default:	retval=DECODER_NOK;break;
	}	
	return retval;
}


/// API functions
int decoder_mp3_init(tHandleDecoderMp3 *pThis)
{
	int err;
	memset(pThis,0,sizeof(tHandleDecoderMp3));
	pThis->pHandleMPG123=(void*)mpg123_new(NULL,&err);

	return DECODER_OK;
}
int decoder_mp3_open(tHandleDecoderMp3 *pThis,char* filename,tSongInfo *pSongInfo)
{
	long rate;
	int channels;
	int encoding;

	size_t audioBytesNum;
	unsigned char *pAudioPtr;
	off_t frameNum;

	struct mpg123_frameinfo frameInfo;

	int l;
	int err;


	printf("open\n");	
	if (pThis->statusOpen && pThis->pHandleMPG123!=NULL)
	{
		mpg123_close((mpg123_handle*)pThis->pHandleMPG123);
		pThis->statusOpen=0;
	}
	err=mpg123_open((mpg123_handle*)pThis->pHandleMPG123,filename);
	if (err)	// TODO: better check
	{
		return DECODER_NOK;
	}
// the file has been opened. it is time to populate the song info	
	l=strlen(filename)+1;
	if (l>sizeof(pSongInfo->filename))
	{
		l=sizeof(pSongInfo->filename);
	}
	memcpy(pSongInfo->filename,filename,l);
// audio encoding
	err=mpg123_decode_frame((mpg123_handle*)pThis->pHandleMPG123,&frameNum,&pAudioPtr,&audioBytesNum);
	if (err==MPG123_DONE)
	{
		return DECODER_EOF;
	}
	err=mpg123_getformat((mpg123_handle*)pThis->pHandleMPG123,&rate,&channels,&encoding);
	pThis->audioFormat.channels=channels;
	pThis->audioFormat.rate=rate;
	if (decoder_mp3_convert_audioencoding(encoding,&pThis->audioFormat.encoding))
	{
		printf("UNKNOWN ENCODING %d. Please contact dettus@dettus.net\n",encoding);
		return DECODER_NOK;
	}
// bitrate
	mpg123_info((mpg123_handle*)pThis->pHandleMPG123,&frameInfo);
	pSongInfo->bitrate=frameInfo.bitrate;
	
// length of the file
	err=mpg123_seek((mpg123_handle*)pThis->pHandleMPG123,0,SEEK_END);
	audioBytesNum=mpg123_tell((mpg123_handle*)pThis->pHandleMPG123);
	pThis->songLen=pSongInfo->len=(int)audioBytesNum/(int)rate;
	err=mpg123_seek((mpg123_handle*)pThis->pHandleMPG123,0,SEEK_SET);
	audioBytesNum=mpg123_tell((mpg123_handle*)pThis->pHandleMPG123);
	pSongInfo->pos=(int)audioBytesNum/(int)rate;
	
	pThis->statusOpen=1;
	
	return DECODER_OK;
}
int decoder_mp3_process(tHandleDecoderMp3 *pThis,tSongInfo *pSongInfo,tPcmSink *pPcmSink)
{
	int retval;
	off_t frameNum;
	int err;
	
	int audioBytesNum;
	size_t tmp;
	long rate;
	int encoding;
	retval=DECODER_OK;
	
// start off with the pcm samples
	err=mpg123_decode_frame((mpg123_handle*)pThis->pHandleMPG123,&frameNum,&(pPcmSink->pAudioData),&tmp);
	pPcmSink->audio_bytes_num=(int)tmp;
	if (err==MPG123_DONE)
	{
		retval=DECODER_EOF;
	}

	mpg123_getformat((mpg123_handle*)pThis->pHandleMPG123,&rate,&pThis->audioFormat.channels,&encoding);
	pThis->audioFormat.rate=rate;
	if (decoder_mp3_convert_audioencoding(encoding,&pThis->audioFormat.encoding))
	{
		printf("UNKNOWN ENCODING %d. Please contact dettus@dettus.net\n",encoding);
		return DECODER_NOK;
	}
	pPcmSink->audioFormat.rate=pThis->audioFormat.rate;
	pPcmSink->audioFormat.channels=pThis->audioFormat.channels;
	pPcmSink->audioFormat.encoding=pThis->audioFormat.encoding;

	audioBytesNum=mpg123_tell((mpg123_handle*)pThis->pHandleMPG123);
	pSongInfo->pos=(int)audioBytesNum/(int)pThis->audioFormat.rate;
	pSongInfo->len=pThis->songLen;

	if (retval==DECODER_EOF)
	{
		decoder_mp3_jump(pThis,pSongInfo,0);	
	}
		
	

	return retval;
}
int decoder_mp3_jump(tHandleDecoderMp3 *pThis,tSongInfo* pSongInfo,int second)
{
	int retval;
	int err;

	retval=DECODER_OK;
	if (pThis->statusOpen)
	{
		err=mpg123_seek((mpg123_handle*)pThis->pHandleMPG123,second*pThis->audioFormat.rate,SEEK_SET);
		if (err)
		{
			retval=DECODER_NOK;	
		} else if (pSongInfo!=NULL) {
			pSongInfo->pos=second;
		}
	}
	return retval;
}

