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
#include <portaudio.h>
#include <string.h>

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

	bufidx=pAudioBuffer->readbuf;
	readidx=pAudioBuffer->readidx[bufidx];
	n=framesPerBuffer*pAudioBuffer->bytes_per_sample;
	if (readidx==PINGPONG_BUFSIZE)	// in case this ping pong buffer has been read until the end
	{
		pthread_mutex_unlock(&pThis->mutex[bufidx]);
		bufidx=(bufidx+1)%PINGPONG_BUFNUM;	// switch to the next one
		pthread_mutex_lock(&pThis->mutex[bufidx]);	// TODO: hopefully, this is not a racing condition?
		readidx=0;
	}
	ptr=&(pAudioBuffer->pingpong[bufidx][readidx]);
	if (pAudioBuffer->writeidx[bufidx]==PINGPONG_BUFSIZE)
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

	pAudioBuffer.readbuf=bufidx;
	pAudioBuffer.readidx[bufidx]=readidx;
	return paContinue;
}

int audioout_init(tAudioBuffer* pThis)
{
	int i;
	PaStreamParameters *pOutputParameters;
	memset(pThis,0,sizeof(tAudioBuffer));
	
	pThis->bytes_per_sample=4;
	pThis->paOutputParameters=malloc(sizeof(PaStreamParameters));
	pOutputParameters=(PaStreamParameters*)pThis->paOutputParameters;


	// TODO: make this more configurable
	pOutputParameters->device=Pa_GetDefaultOutputDevice();
	pOutputParameters->channelCount=0;
	pOutputParameters->sampleFormat=0;
	

	pOutputParameters->suggestedLatency=Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	pOutputParameters->hostApiSpecificStreamInfo=NULL;

	pThis->audioFormat.channels=0;
	pThis->audioFormat.encoding=0;
	pThis->audioFormat.rate=0;

	pThis->paStream=NULL;

	for (i=0;i<PINGPONG_BUFNUM;i++)
	{
		pthread_mutex_init(&pThis->mutex[i],NULL);
	}
}
int audioout_newPcm(tAudioBuffer* pThis,tPcmSink *pPcmSink)
{
	PaStream *pPaStream=(PaStream*)pThis->pPaStream;
	PaStreamParameters *pOutputParameters=(PaStreamParameters*)pThis->paOutputParameters;
	int i;
	int n;
	int bufidx;
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

		Pa_OpenStream(&pPaStream,NULL,&pOutputParameters,pThis->audioFormat.rate,
		64,	// TODO
		paClipOff,
		audioout_paCallback,(void*)pThis);
		pThis->pPaStream=(void*)pPaStream;
	}



	// 
	n=audio_bytes_num;
	bufidx=pThis->audioBuffer.writebuf;
	writeidx=pThis->audioBuffer.writeidx[bufidx];
	if (writeidx==PINGPONG_BUFSIZE)
	{
		bufidx=(bufidx+1)%PINGPONG_BUFNUM;
		writeidx=0;
	}

	pthread_mutex_lock(&pThis->mutex[bufidx]);	// this will (hopefully) block until the playback is finished
	if ((PINGPONG_BUFSIZE-writeidx)>=n)
	{
		rptr=(char*)pPcmSink->pcmSamples;
		wptr=(char*)&(pThis->audioBuffer.pingpong[bufidx][writeidx]);
		for (i=0;i<n;i++)
		{
			*wptr++=*rptr++;
		}
		writeidx+=n;
	} else {	// unaligned write
		int m;
		m=PINGPONG_BUFSIZE-writeidx;
		rptr=(char*)pPcmSink->pcmSamples;
		wptr=(char*)&(pThis->audioBuffer.pingpong[bufidx][writeidx]);
		for (i=0;i<m;i++)
		{
			*wptr++=*rptr++;
		}
		n-=m;
		pThis->audioBuffer.writeidx[bufidx]=PINGPONG_BUFSIZE;
		pthread_mutex_unlock(&pThis->mutex[bufidx]);
		bufidx=(bufidx+1)%PINGPONG_BUFNUM;
		pthread_mutex_lock(&pThis->mutex[bufidx]);
		writeidx=0;
		wptr=(char*)&(pThis->audioBuffer.pingpong[bufidx][writeidx]);
		for (i=0;i<n;i++)
		{
			*wptr++=*rptr++;
		}
		writeidx+=n;
	}
	pthread_mutex_unlock(&pThis->mutex[bufidx]);
	pThis->audioBuffer.writeidx[bufidx]=writeidx;
	pThis->audioBuffer.writebuf=bufidx;
}

