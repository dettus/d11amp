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

#include "decoder_mp3.h"
#include <math.h>
#include <mpg123.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// helper functions
int decoder_mp3_convert_audioencoding(int mpg123_encoding,eAudioEncoding *pAudioCoding)
{
	int retval;

	retval=RETVAL_OK;
	switch(mpg123_encoding)
	{
		case 0xd0:	*pAudioCoding=eAUDIO_ENCODING_S16LE;break;	// TODO: why 0xd0?
		default:	retval=RETVAL_NOK;break;
	}	
	return retval;
}



int decoder_mp3_init(tHandleDecoderMp3 *pThis)
{
	int err;
	int retval;
	retval=RETVAL_OK;
	memset(pThis,0,sizeof(tHandleDecoderMp3));
	pThis->pHandleMPG123=(void*)mpg123_new(NULL,&err);
	pThis->opened=0;

	if (err!=MPG123_OK)
	{
		retval=RETVAL_NOK;
	}
	return retval;
}
int decoder_mp3_open_file(tHandleDecoderMp3 *pThis,char* filename,tSongInfo *pSongInfo)
{
	int retval;
	int err;
	retval=RETVAL_OK;

	if (pThis->opened)
	{
		mpg123_close((mpg123_handle*)pThis->pHandleMPG123);
		pThis->opened=0;
	}
	err=mpg123_open((mpg123_handle*)pThis->pHandleMPG123,filename);
	if (err!=MPG123_OK)
	{
		retval=RETVAL_NOK;
	} else {
		long rate;
		int channels;
		int encoding;

		size_t audioBytesNum;
		unsigned char *pAudioPtr;
		off_t frameNum;

		struct mpg123_frameinfo frameInfo;

		int l;

		l=strlen(filename)+1;
		if (l>sizeof(pSongInfo->filename))
		{
			l=sizeof(pSongInfo->filename);
		}
		memcpy(pSongInfo->filename,filename,l);
		err=mpg123_decode_frame((mpg123_handle*)pThis->pHandleMPG123,&frameNum,&pAudioPtr,&audioBytesNum);
		if (err==MPG123_DONE)
		{
			return RETVAL_NOK;
		}
		err=mpg123_getformat((mpg123_handle*)pThis->pHandleMPG123,&rate,&channels,&encoding);
		pThis->audioFormat.channels=channels;
		pThis->audioFormat.rate=rate;
		pSongInfo->channels=channels;
		pSongInfo->samplerate=rate;
		if (decoder_mp3_convert_audioencoding(encoding,&pThis->audioFormat.encoding))
		{
			printf("UNKNOWN ENCODING %d. Please contact dettus@dettus.net\n",encoding);
			return RETVAL_NOK;
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

		pThis->opened=1;
	}
	return retval;
}
int decoder_mp3_jump(tHandleDecoderMp3 *pThis,tSongInfo* pSongInfo,int second)
{
	int retval;
	int err;

	retval=RETVAL_OK;
	if (pThis->opened)
	{
		err=mpg123_seek((mpg123_handle*)pThis->pHandleMPG123,second*pThis->audioFormat.rate,SEEK_SET);
		if (err)
		{
			retval=RETVAL_NOK;
		} else if (pSongInfo!=NULL) {
			pSongInfo->pos=second;
		}
	}
	return retval;
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
	struct mpg123_frameinfo frameInfo;
	retval=RETVAL_OK;
	
// start off with the pcm samples
	err=mpg123_decode_frame((mpg123_handle*)pThis->pHandleMPG123,&frameNum,&(pPcmSink->pAudioData),&tmp);
	pPcmSink->audio_bytes_num=(int)tmp;
	if (err==MPG123_DONE)
	{
		retval=RETVAL_DECODER_EOF;
	}

	mpg123_getformat((mpg123_handle*)pThis->pHandleMPG123,&rate,&pThis->audioFormat.channels,&encoding);
	pThis->audioFormat.rate=rate;
	if (decoder_mp3_convert_audioencoding(encoding,&pThis->audioFormat.encoding))
	{
		printf("UNKNOWN ENCODING %d. Please contact dettus@dettus.net\n",encoding);
		return RETVAL_NOK;
	}
	pPcmSink->audioFormat.rate=pThis->audioFormat.rate;
	pPcmSink->audioFormat.channels=pThis->audioFormat.channels;
	pPcmSink->audioFormat.encoding=pThis->audioFormat.encoding;

	audioBytesNum=mpg123_tell((mpg123_handle*)pThis->pHandleMPG123);
	pSongInfo->pos=(int)audioBytesNum/(int)pThis->audioFormat.rate;
	pSongInfo->len=pThis->songLen;

	mpg123_info((mpg123_handle*)pThis->pHandleMPG123,&frameInfo);
	pSongInfo->bitrate=frameInfo.bitrate;

	
	

	return retval;
}
int decoder_mp3_set_equalizer(tHandleDecoderMp3 *pThis,int band,int value,int preamp_value)
{
	double v;
	double dB;
	double preamp_dB;
	// okay, so... The THEME defined 14 dB values for the GUI.
	// the value is -100...0...100.
#if 0
#define	NUM_DB	14
	int mBval[NUM_DB]={0,142,428,570,714,857,1000,1143,1286,1429,1571,1714,1875,2000};	// those values are in milliBel. 
	int dbidx;


	if (value<-100)
	{
		value=-100;
	}
	if (value>100)
	{
		value=100;
	}

	if (value<0)
	{
		dbidx=-value;
	} else {
		dbidx=value;
	}


	if (preamp_value<-100)
	{
		preamp_value=-100;
	}
	if (preamp_value>100)
	{
		preamp_value=100;
	}
	dbidx=dbidx*(NUM_DB-1)/100;
	dB=mBval[dbidx]/100.0f;	// mB -> dB: divide by 100.
	if (value<0) 
	{
		dB=-dB;
	}
#else
	dB=value* 6.0f/100.0f;
#endif
	preamp_dB=preamp_value* 6.0f/100.0f;
	dB+=preamp_dB;
	
	v=pow(10,dB/10.0f);

	
	mpg123_eq((mpg123_handle*)pThis->pHandleMPG123,MPG123_LEFT|MPG123_RIGHT,band,v);
	return RETVAL_OK;
}
