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
#include "audioout.h"
#include <stdlib.h>
#include <portaudio.h>
#include <string.h>
#include <stdio.h>

static int audioout_paCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *pAudioBuffer)
{
	tAudioBuffer *pThis=(tAudioBuffer*)pAudioBuffer;
	int bufidx;
	int readidx;
	int i;
	int n;
	unsigned char* out=(unsigned char*)outputBuffer;
	unsigned char* ptr;

	bufidx=pThis->readbuf;
	readidx=pThis->readidx[bufidx];
	n=framesPerBuffer*pThis->bytes_per_sample;
	if (readidx==PINGPONG_BUFSIZE)	// in case this ping pong buffer has been read until the end
	{
		pthread_rwlock_unlock(&pThis->rwlock[bufidx]);	// as long as the readlock has been held, the write lock was unable to access it
		bufidx=(bufidx+1)%PINGPONG_BUFNUM;	// switch to the next one
		pthread_rwlock_rdlock(&pThis->rwlock[bufidx]);	// this buffer is being read now. block it from being written to
		readidx=0;
	}
	ptr=&(pThis->pingpong[bufidx][readidx]);
	if (pThis->writeidx[bufidx]==PINGPONG_BUFSIZE)
	{
		for (i=0;i<n;i++)
		{
			*out++=*ptr++;
		}
	} else {
		for (i=0;i<n;i++)
		{
			*out++=0;
		}
	}
	readidx+=n;

	pThis->readbuf=bufidx;
	pThis->readidx[bufidx]=readidx;
	return paContinue;
}

int audioout_init(tHandleAudioOut* pThis)
{
	int i;
	PaStreamParameters *pOutputParameters;
	memset(pThis,0,sizeof(tHandleAudioOut));
	
	pThis->audioBuffer.bytes_per_sample=4;
	pThis->paOutputParameters=malloc(sizeof(PaStreamParameters));
	pOutputParameters=(PaStreamParameters*)pThis->paOutputParameters;


	i=Pa_Initialize();
	// TODO: make this more configurable
	pOutputParameters->device=Pa_GetDefaultOutputDevice();
	pOutputParameters->channelCount=0;
	pOutputParameters->sampleFormat=0;

	printf("device:%d \n",pOutputParameters->device);	

	pOutputParameters->suggestedLatency=Pa_GetDeviceInfo(pOutputParameters->device)->defaultLowOutputLatency;
	pOutputParameters->hostApiSpecificStreamInfo=NULL;

	pThis->audioFormat.channels=0;
	pThis->audioFormat.encoding=0;
	pThis->audioFormat.rate=0;

	pThis->paStream=NULL;

	for (i=0;i<PINGPONG_BUFNUM;i++)
	{
		pthread_rwlock_init(&pThis->audioBuffer.rwlock[i],NULL);
	}
	pthread_rwlock_rdlock(&pThis->audioBuffer.rwlock[pThis->audioBuffer.readbuf]);	// lock the first buffer for reading.
	return 0;
}
int audioout_newPcm(tHandleAudioOut* pThis,tPcmSink *pPcmSink)
{
	PaStream *pPaStream=(PaStream*)pThis->paStream;
	PaStreamParameters *pOutputParameters=(PaStreamParameters*)pThis->paOutputParameters;
	int n;
	int bufidx;
	int total;
	int writeidx;
	char* rptr;
	char* wptr;

	if (pPcmSink->audioFormat.channels!=pThis->audioFormat.channels || pPcmSink->audioFormat.rate!=pThis->audioFormat.rate || pPcmSink->audioFormat.encoding!=pThis->audioFormat.encoding)
	{
		if (pPaStream!=NULL)
		{
			Pa_CloseStream(pPaStream);
		}
		pOutputParameters->channelCount=pPcmSink->audioFormat.channels;
		switch(pPcmSink->audioFormat.encoding)
		{
			case 208:	// signed integer 16 bit. TODO: WHY??
				pOutputParameters->sampleFormat=paInt16;
				pThis->audioBuffer.bytes_per_sample=2*pPcmSink->audioFormat.channels;
				break;
			default:
				printf("UNKNOWN AUDIO ENCODING %d %x\n",pPcmSink->audioFormat.encoding,pPcmSink->audioFormat.encoding);
				break;
		}
		pThis->audioFormat=pPcmSink->audioFormat;

		Pa_OpenStream(&pPaStream,NULL,pOutputParameters,pThis->audioFormat.rate,
		64,	// TODO
		paClipOff,
		audioout_paCallback,(void*)&pThis->audioBuffer);
		pThis->paStream=(void*)pPaStream;
        	Pa_StartStream(pThis->paStream);
	}


	// 
	total=pPcmSink->audio_bytes_num;
	rptr=(char*)pPcmSink->pcmSamples;
	while (total)
	{
		bufidx=pThis->audioBuffer.writebuf;
		writeidx=pThis->audioBuffer.writeidx[bufidx];
		if (writeidx==PINGPONG_BUFSIZE)
		{
			bufidx=(bufidx+1)%PINGPONG_BUFNUM;
			writeidx=0;
		}
		wptr=(char*)&(pThis->audioBuffer.pingpong[bufidx][writeidx]);


		n=PINGPONG_BUFSIZE-writeidx;
		if (n>total) n=total;
		pthread_rwlock_wrlock(&pThis->audioBuffer.rwlock[bufidx]);	// this will lock the buffer for reading. hopefully it is fast enough. (let's assume that it is). 
		memcpy(wptr,rptr,n);
		total-=n;
		writeidx+=n;
		rptr+=n;
		pThis->audioBuffer.writeidx[bufidx]=writeidx;	
		pthread_rwlock_unlock(&pThis->audioBuffer.rwlock[bufidx]);	// this will lock the buffer for reading. hopefully it is fast enough. (let's assume that it is). 
		
	}	
	pThis->audioBuffer.writeidx[bufidx]=writeidx;
	pThis->audioBuffer.writebuf=bufidx;
	return 0;
}
int audioout_silence(tHandleAudioOut* pThis)
{
	int i;
	for (i=0;i<PINGPONG_BUFNUM;i++)
	{
		pthread_rwlock_wrlock(&pThis->audioBuffer.rwlock[i]);
		pThis->audioBuffer.writeidx[i]=0;	// mark all audio buffers as empty
		pthread_rwlock_unlock(&pThis->audioBuffer.rwlock[i]);
	}
	return 0;
}

