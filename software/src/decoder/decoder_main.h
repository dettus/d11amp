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

#ifndef	DECODER_MAIN_H
#define	DECODER_MAIN_H
#include "datastructures.h"
#include "audioout.h"
#include "mp3decoder.h"
#include <pthread.h>
// the purpose of this file is create an abstraction layer from the file format.


typedef enum
{
	FILETYPE_NONE=0,
	FILETYPE_MP3=1
} eFileType;

typedef struct _tHandleDecoderMain
{
	eFileType	current_filetype;
	tHandleAudioOut	*pHandleAudioOut;
	tHandleMp3Decode handlemp3decoder;
	int done;

	int file_len_seconds;
	int file_pos_seconds;

	ePlayState	state;
        pthread_mutex_t mutex;
        pthread_t decoderthread;
	
	tPcmSink	pcmSink;
} tHandleDecoderMain;

int decode_init(tHandleDecoderMain* pThis,tHandleAudioOut* pHandleAudioOut);
int decode_fileopen(tHandleDecoderMain* pThis,char* filename);
int decode_fileplay(tHandleDecoderMain* pThis);
int decode_filestop(tHandleDecoderMain* pThis);
int decode_filepause(tHandleDecoderMain* pThis);
int decode_filejump(tHandleDecoderMain* pThis,int second);
int decode_getpos(tHandleDecoderMain* pThis,int* file_len_seconds,int* file_pos_seconds);
int decode_getdone(tHandleDecoderMain* pThis,int *done);

#define	DECODE_OK	0
#define	DECODE_NOK	-1

#endif

