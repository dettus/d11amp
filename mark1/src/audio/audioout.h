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

#ifndef	AUDIOOUT_H
#define	AUDIOOUT_H
#include <pthread.h>
#include "datastructures.h"

#define	PINGPONG_BUFSIZE	8192
#define	PINGPONG_BUFNUM		4
typedef struct _tAudioBuffer
{
	unsigned char pingpong[PINGPONG_BUFNUM][PINGPONG_BUFSIZE];
	int writeidx[PINGPONG_BUFNUM];	
	int readidx[PINGPONG_BUFNUM];
	int writebuf;
	int readbuf;
	int bytes_per_sample;
	pthread_rwlock_t rwlock[PINGPONG_BUFNUM];
} tAudioBuffer;


typedef struct _tHandleAudioOut
{
	void* paStream;
	void* paOutputParameters;
	tAudioBuffer audioBuffer;
	tAudioFormat audioFormat;
} tHandleAudioOut;

int audioout_init(tHandleAudioOut* pThis);
int audioout_newPcm(tHandleAudioOut* pThis,tPcmSink *pPcmSink);
int audioout_silence(tHandleAudioOut* pThis);
#endif
