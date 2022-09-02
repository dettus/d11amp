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

#ifndef	DECODER_H
#define	DECODER_H
#include "datastructures.h"
#include "decoder_mp3.h"
#include <pthread.h>

typedef enum
{
	FILETYPE_NONE=0,
	FILETYPE_MP3
} eDecoderFileType;
typedef enum
{
	DECODER_NONE=0,
	DECODER_PAUSE,
	DECODER_PLAY,
	DECODER_EOF
} eDecoderState;
typedef struct _tHandleDecoder
{

	void *pControllerContext;
	tHandleDecoderMp3 handleDecoderMp3;
	tSongInfo songInfo;
	eDecoderState state;
	eDecoderFileType fileType;
	pthread_mutex_t	mutex;
	pthread_t thread;
} tHandleDecoder;

int decoder_init(tHandleDecoder* pThis,void* pControllerContext);
int decoder_open_file(tHandleDecoder* pThis,char* filename);
int decoder_play(tHandleDecoder* pThis);
int decoder_pause(tHandleDecoder* pThis);
int decoder_jump(tHandleDecoder* pThis,int newSongPos);
#endif

