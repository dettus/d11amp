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

#ifndef DECODER_MP3_H
#define	DECODER_MP3_H
#include "datastructures.h"
typedef struct _tHandleDecoderMp3
{
	void *pHandleMPG123;
	int statusOpen;
	tAudioFormat audioFormat;
	tSongInfo songInfo;	// TODO: IS IT REALLY NECESSARY??
} tHandleDecoderMp3;

int decoder_mp3_init(tHandleDecoderMp3 *pThis);
int decoder_mp3_process(tHandleDecoderMp3 *pThis,tSongInfo *pSongInfo,tPcmSink *pPcmSink);
int decoder_mp3_open(tHandleDecoderMp3 *pThis,char* filename,tSongInfo *pSongInfo);
int decoder_mp3_jump(tHandleDecoderMp3 *pThis,tSongInfo* pSongInfo,int second);
#endif

