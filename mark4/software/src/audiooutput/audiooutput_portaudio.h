/*

Copyright 2023, dettus@dettus.net

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

#ifndef	AUDIOOUTPUT_PORTAUDIO_H
#define	AUDIOOUTPUT_PORTAUDIO_H

#include <pthread.h>
#include "datastructures.h"

#define PINGPONG_BUFSIZE        8192
#define PINGPONG_BUFNUM         4
#define	PCMRINGBUF_SIZE		2048

#define	MAX_DEVICE_NUM	128
#define	MAX_DEVICE_LEN	128

typedef struct _tAudioDeviceList
{
	int devicenum;
	int defaultdevice;
	int idx[MAX_DEVICE_NUM];
	char name[MAX_DEVICE_NUM][MAX_DEVICE_LEN];
} tAudioDeviceList;
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

typedef struct _tHandleAudioOutputPortaudio
{
	void *paStream;
	void *paOutputParameters;
	int deviceIdx;
	tAudioBuffer audioBuffer;
	tAudioFormat audioFormat;
	int volume;		// 0..100
	int balance;		// -100.. 0 .. 100

	signed short pcmRingBuf[PCMRINGBUF_SIZE];
	int ringidx;
	int stop;

	int initialized;
} tHandleAudioOutputPortaudio;

int audiooutput_portaudio_init(tHandleAudioOutputPortaudio *pThis);
int audiooutput_portaudio_switch_device(tHandleAudioOutputPortaudio *pThis,int deviceIdx);
int audiooutput_portaudio_activate(tHandleAudioOutputPortaudio *pThis);
int audiooutput_portaudio_push(tHandleAudioOutputPortaudio *pThis,void* pAudioData,int audioBytesNum,tAudioFormat audioFormat);
int audiooutput_portaudio_stop(tHandleAudioOutputPortaudio *pThis);
int audiooutput_portaudio_setVolume(tHandleAudioOutputPortaudio *pThis,int volume);
int audiooutput_portaudio_setBalance(tHandleAudioOutputPortaudio *pThis,int balance);
int audiooutput_portaudio_getVolume(tHandleAudioOutputPortaudio *pThis,int* pVolume,int* pBalance);
int audiooutput_portaudio_getLastSamples(tHandleAudioOutputPortaudio *pThis,signed short *pPcm,int n);
int audiooutput_portaudio_get_devicelist(tAudioDeviceList *pList);
int audiooutput_portaudio_commandline_option(tHandleAudioOutputPortaudio* pThis,char* argument);
void audiooutput_portaudio_help();
int audiooutput_portaudio_get_current_deviceidx(tHandleAudioOutputPortaudio* pThis,int* pDeviceIdx);

#endif
