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

#ifndef	MP3DECODER_H
#define	MP3DECODER_H

#include "datastructures.h"


typedef struct _tHandleMp3Decode
{
	int 		done;
	tAudioFormat	audioFormat;
	void*		mpg123handle;
	int		file_len_seconds;
	int		file_pos_seconds;
	int		startpos;
	char		filename[1024];
} tHandleMp3Decode;


int mp3decode_init(tHandleMp3Decode* pThis);
int mp3decode_fileopen(tHandleMp3Decode* pThis,char* filename);
int mp3decode_process(tHandleMp3Decode* pThis,tPcmSink *pPcmSink,int* file_len_seconds,int* file_pos_seconds);
int mp3decode_jump(tHandleMp3Decode* pThis,int second);
int mp3decode_getsonginfo(tHandleMp3Decode* pThis,char** songinfo,int* bitrate,int* rate,int* channels);

#endif
