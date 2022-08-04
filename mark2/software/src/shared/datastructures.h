/*
BSD 2-Clause License

Copyright (c) 2022, dettus
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef	DATASTRUCTURES_H
#define	DATASTRUCTURES_H

// different decoders and audio drivers use different ways of denoting this info.
// so i am creating my own
typedef enum
{
	eAUDIO_ENCODING_S16LE=0
} eAudioEncoding;


typedef struct _tAudioFormat
{
	int channels;
	int rate;
	eAudioEncoding audio_encoding;
} tAudioFormat;


typedef struct _tPcmSink
{
	void *pAudioBytes;
	int audio_bytes_num;
	tAudioFormat audioFormat;
} tPcmSink;

typedef struct _tSongInfo
{
	char filename[1024];
	unsigned char songinfo[256];
	int len;
	int pos;
	int channels;
	int samplerate;
	int bitrate;
} tSongInfo;


#define	RETVAL_OK		0
#define	RETVAL_NOK		-1
#define	RETVAL_INVALID_PTR	-2

#endif
