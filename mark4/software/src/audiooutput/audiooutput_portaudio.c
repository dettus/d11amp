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
#include <portaudio.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "audiooutput_portaudio.h"

static int audiooutput_portaudio_paCallback(const void* inputBuffer, void* outputBuffer,
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
	if (readidx==PINGPONG_BUFSIZE)  // in case this ping pong buffer has been read until the end
	{
		pthread_rwlock_unlock(&pThis->rwlock[bufidx]);  // as long as the readlock has been held, the write lock was unable to access it
		bufidx=(bufidx+1)%PINGPONG_BUFNUM;      // switch to the next one
		pthread_rwlock_rdlock(&pThis->rwlock[bufidx]);  // this buffer is being read now. block it from being written to
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
int audiooutput_portaudio_init(tHandleAudioOutputPortaudio *pThis)	//,tOptions *pCommandLineOptions)
{
	int i;
	int device;
	PaStreamParameters* pPaStreamParameters;
	memset(pThis,0,sizeof(tHandleAudioOutputPortaudio));
	pThis->audioBuffer.bytes_per_sample=4;
	pThis->paOutputParameters=malloc(sizeof(PaStreamParameters));
	memset(pThis->paOutputParameters,0,sizeof(PaStreamParameters));

	

// TODO: make this more configurable
	i=Pa_Initialize();
	device=Pa_GetDefaultOutputDevice();

//	for (i=pCommandLineOptions->gtkargc;i<pCommandLineOptions->argc;i++)
//	{
//		int l;
//		l=strlen(pCommandLineOptions->argv[i]);
//		if (l>31 && strncmp("--audiooutput.portaudio.device=",pCommandLineOptions->argv[i],31)==0) 
//
//		{
//			device=atoi(&pCommandLineOptions->argv[i][31]);
//		}
//	}
	
	pPaStreamParameters=(PaStreamParameters*)pThis->paOutputParameters;	
	pPaStreamParameters->device=device;
	pPaStreamParameters->channelCount=0;
	pPaStreamParameters->sampleFormat=0;
	pPaStreamParameters->suggestedLatency=Pa_GetDeviceInfo(pPaStreamParameters->device)->defaultLowOutputLatency;
        pPaStreamParameters->hostApiSpecificStreamInfo=NULL;

	pThis->audioFormat.channels=0;
	pThis->audioFormat.rate=0;
	pThis->audioFormat.encoding=eAUDIO_ENCODING_NONE;

	pThis->volume=100;
	pThis->balance=0;

	pThis->paStream=NULL;

	for (i=0;i<PINGPONG_BUFNUM;i++)
	{
		pthread_rwlock_init(&pThis->audioBuffer.rwlock[i],NULL);
	}
	pthread_rwlock_rdlock(&pThis->audioBuffer.rwlock[pThis->audioBuffer.readbuf]);	// lock the first buffer for reading.
	pThis->stop=1;

	return RETVAL_OK;
}
int audiooutput_portaudio_push(tHandleAudioOutputPortaudio *pThis,void* pAudioData,int audioBytesNum,tAudioFormat audioFormat)
{
	int retval;
	int total;
	signed short* rptr;
	unsigned int* wptr;
	int bytes_per_sample;
	int mono;
	int ringidx;
	
	PaStreamParameters* pPaStreamParameters;
	pPaStreamParameters=(PaStreamParameters*)pThis->paOutputParameters;	

	retval=RETVAL_OK;
	pThis->stop=0;
	ringidx=0;
	if (audioFormat.rate!=pThis->audioFormat.rate || audioFormat.channels!=pThis->audioFormat.channels || audioFormat.encoding!=pThis->audioFormat.encoding)
	{	
		pThis->audioFormat=audioFormat;
		if (pThis->paStream!=NULL)
		{
			Pa_CloseStream(pThis->paStream);
		}
//		pPaStreamParameters->channelCount=audioFormat.channels;
		pPaStreamParameters->channelCount=2;
		pPaStreamParameters->sampleFormat=paInt16;
/*
		switch (audioFormat.encoding)
		{
			case eAUDIO_ENCODING_S16LE:
				pPaStreamParameters->sampleFormat=paInt16;
				break;
			default:
				retval=RETVAL_NOK;
				break;
		}
*/
		Pa_OpenStream(&pThis->paStream,NULL,pThis->paOutputParameters,pThis->audioFormat.rate,
				64,		// TODO
				paClipOff,
				audiooutput_portaudio_paCallback,(void*)&pThis->audioBuffer);

		Pa_StartStream(pThis->paStream);

	}

/////////////////////
	switch (audioFormat.encoding)
	{
		case eAUDIO_ENCODING_S16LE:
			bytes_per_sample=2;
			break;
		default:
			printf("UNKNOWN SAMPLE FORMAT. PLEASE CONTACT dettus@dettus.net\n");
			break;
	}

	if (audioFormat.channels==1)
	{
		mono=1;
	} else { 
		mono=0;
		bytes_per_sample*=2;
	}	
	total=audioBytesNum;
	rptr=(short*)pAudioData;
	
	while (total)
	{
		int bufidx;
		int writeidx;
		int n;	
		int m;
		int bal;
		int vol;
		int i;
		bufidx=pThis->audioBuffer.writebuf;
		writeidx=pThis->audioBuffer.writeidx[bufidx];
		if (writeidx==PINGPONG_BUFSIZE)
		{
			bufidx=(bufidx+1)%PINGPONG_BUFNUM;
			writeidx=0;
		}
		wptr=(unsigned int*)&(pThis->audioBuffer.pingpong[bufidx][writeidx]);

		n=PINGPONG_BUFSIZE-writeidx;
		if (n>total)
		{
			n=total;
		}
		pthread_rwlock_wrlock(&pThis->audioBuffer.rwlock[bufidx]);
		m=n/bytes_per_sample;
		vol=pThis->volume;
		bal=pThis->balance;
		for (i=0;i<m;i++)
		{
			signed int left;
			signed int right;

			signed int yl;
			signed int yr;
			unsigned int x;

	
			left=*rptr++;
			right=mono?left:(*rptr++);

			left*= vol;
			right*=vol;

			left/=100;
			right/=100;
					
			if (bal>0)	// balance 0..100 -> right speaker
			{
				yl=(left*(100-bal))/100;	
				yr=((right*100)+(left*bal))/(100+bal);

				left=yl;
				right=yr;
			}
			else if (bal)	// balance -100..0 -> left speaker
			{
				yl=((left*100)+(right*(100+bal)))/(100-bal);
				yr=(right*(100+bal))/100;

				left=yl;
				right=yr;
			}
			pThis->pcmRingBuf[(ringidx++)%PCMRINGBUF_SIZE]=left;			
			pThis->pcmRingBuf[(ringidx++)%PCMRINGBUF_SIZE]=right;
			
			x=((unsigned int)right)&0xffff;
			x<<=16;
			x|=((unsigned int)left)&0xffff;

			*wptr=x;
			wptr++;

		}	
		total-=n;
		writeidx+=n;
//		rptr+=m;
		pThis->audioBuffer.writeidx[bufidx]=writeidx;
		pThis->audioBuffer.writebuf=bufidx;
		pthread_rwlock_unlock(&pThis->audioBuffer.rwlock[bufidx]);
	}
	pThis->ringidx=ringidx;

	return retval;
}

int audiooutput_portaudio_stop(tHandleAudioOutputPortaudio *pThis)
{
	int i;
	for (i=0;i<PINGPONG_BUFNUM;i++)
	{
		pthread_rwlock_wrlock(&pThis->audioBuffer.rwlock[i]);
		pThis->audioBuffer.writeidx[i]=0;
		pthread_rwlock_unlock(&pThis->audioBuffer.rwlock[i]);
	}
	pThis->stop=1;
	return RETVAL_OK;
}
int audiooutput_portaudio_setVolume(tHandleAudioOutputPortaudio *pThis,int volume)
{
	pThis->volume=volume;
	return RETVAL_OK;
}
int audiooutput_portaudio_setBalance(tHandleAudioOutputPortaudio *pThis,int balance)
{
	pThis->balance=balance;
	return RETVAL_OK;
}
int audiooutput_portaudio_getVolume(tHandleAudioOutputPortaudio *pThis,int* pVolume,int* pBalance)
{
	*pVolume=pThis->volume;
	*pBalance=pThis->balance;
	return RETVAL_OK;
}
int audiooutput_portaudio_getLastSamples(tHandleAudioOutputPortaudio *pThis,signed short *pPcm,int n)
{
	int i;
	int ridx;
	ridx=pThis->ringidx-n;
	if (ridx<0) ridx+=PCMRINGBUF_SIZE;
	for (i=0;i<n;i++)
	{
		pPcm[i]=pThis->pcmRingBuf[(ridx++)%PCMRINGBUF_SIZE];
	}
	return RETVAL_OK;
}

void audiooutput_portaudio_help()
{
	printf(">> Port audio output options\n");
	printf("   --audiooutput.portaudio.device=NUMBER select one of the following devices:\n");
	fprintf(stderr,"                                     (It is save to ignore the ALSA warnings)\n");
	if (!Pa_Initialize())
	{
		int i;
		int devicenum;
		int defaultdevice;
		devicenum=Pa_GetDeviceCount();
		defaultdevice=Pa_GetDefaultOutputDevice();
		for (i=0;i<devicenum;i++)
		{
			const PaDeviceInfo *pDeviceInfo;
			pDeviceInfo=Pa_GetDeviceInfo(i);
			printf("   %3d> %s",i,pDeviceInfo->name);

			if (i==defaultdevice) printf(" (default device)");
			printf("\n");
		}
	} else {
		printf("Portaudio cannot be inialized.\n");
	}
}
