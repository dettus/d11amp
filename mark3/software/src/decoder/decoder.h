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

#ifndef DECODER_H
#define	DECODER_H
#include "audiooutput.h"
#include "datastructures.h"
#include <pthread.h>

#include "decoder_mp3.h"
#include "decoder_shared.h"


typedef enum
{
	STATE_NONE=0,	
	STATE_STOP,
	STATE_PLAY,
	STATE_PAUSE,
	STATE_EOF
} eDecoderState;

typedef enum
{
	FILETYPE_NONE=0,
	FILETYPE_MP3
} eFileType;
typedef struct _tHandleDecoder
{
	tHandleAudioOutput *pHandleAudioOutput;
	tHandleDecoderMp3 handleDecoderMp3;

	eDecoderState state;
	eFileType fileType;
	

	tSongInfo songInfo;
	tPcmSink pcmSink;

	pthread_mutex_t	mutex;
	pthread_t	threadDecoder;
} tHandleDecoder;
int decoder_init(tHandleDecoder* pThis,tHandleAudioOutput* pHandleAudioOutput,tOptions *pCommandLineOptions);
int decoder_openfile(tHandleDecoder* pThis,char* filename);
int decoder_set_state(tHandleDecoder* pThis,eDecoderState nextState);
int decoder_get_state(tHandleDecoder* pThis,eDecoderState *pState);
int decoder_set_songPos(tHandleDecoder* pThis,int second);
int decoder_get_songInfo(tHandleDecoder* pThis,tSongInfo* pSongInfo);
void decoder_help();

#endif
