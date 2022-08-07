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

static int audioout_portaudio_paCallback(const void* inputBuffer, void* outputBuffer,
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
int audiooutput_portaudio_init(tHandleAudioOutputPortaudio *pThis)
{
	int i;
	memset(pThis,0,sizeof(tHandleAudioOutputPortaudio));
	pThis->audioBuffer.bytes_per_sample=4;
	pThis->paOutputParameters=malloc(sizeof(PaStreamParameters));
	memset(pThis->paOutputParameters,0,sizeof(PaStreamParameters));
// TODO: make this more configurable
	pThis->paOutputParamters.device=PaGetDefaultOutputDevice();
	pThis->paOutputParameters.channelCount=0;
	pThis->paOutputParamaters.sampleFormat=0;
	pThis->paOutputParameters->suggestedLatency=Pa_GetDeviceInfo(pThis->paOutputParameters->device)->defaultLowOutputLatency;
        pThis->paOutputParameters->hostApiSpecificStreamInfo=NULL;

	pThis->audioFormat.channels=0;
	pThis->audioFormat.rate=0;
	pThis->audioFormat.encoding=eAUDIO_ENCODING_NONE;

	pThis->paStream=NULL;

	for (i=0;i<PINGPONG_BUFNUM;i++)
	{
		pthread_rwlock_init(&pThis->audioBuffer.rwlock[i],NULL);
	}
	pthread_rwlock_rdlock(&pThis->audioBuffer.rwlock[pThis->audioBuffer.readbuf]);	// lock the first buffer for reading.

	return RETVAL_OK;
}
int audiooutput_portaudio_push(tHandleAudioOutputPortaudio *pThis,void* pAudioData,int audioBytesNum,tAudioFormat audioFormat)
{
	int retval;
	int total;
	char* rptr;
	char* wptr;

	retval=RETVAL_OK;
	if (audioFormat.rate!=pThis->audioFormat.rate || audioFormat.channels!=pThis->audioFormat.channels || audioFormat.encoding!=pThis->audioFormat.encoding)
	{
		if (pThis->paStream!=NULL)
		{
			Pa_CloseStream(pThis->paStream);
			pThis->paOutputParameters->channelCount=audioFormat.channels;
			switch (audioFormat.encoding)
			{
				case eAUDIO_ENCODING_S16LE:
					pThis->paOutputParameters->sampleFormat=paInt16;
					break;
				default:
					retval=RETVAL_NOK;
					break;
			}
			Pa_OpenStream(&pThis->paStream,NULL,pThis->paOutputParamters,pThis->audioFormat.rate,
				64,		// TODO
				paClipOff,
				audiooutput_portaudio_paCallback,(void*)&pThis->audioBuffer);
				PaStartStream(pThis->paStream);
		}	
		pThis->audioFormat=audioFormat;
	}
	total=audioBytesNum;
	rptr=(char*)pAudioData;
	while (total)
	{
		int bufidx;
		int widx;
		bufidx=pThis->audioBuffer.writebuf;
		writeidx=pThis->audioBuffer.writeidx[bufidx];
		if (writeidx==PINGPONG_BUFSIZE)
		{
			bufidx=(bufidx+1)%PINGPONG_BUFSIZE;
			writeidx=0;
		}
		wptr=(char*)&(pThis->audioBuffer.pingpong[bufidx][writeidx]);

		n=PINGPONG_BUFSIZE-writeidx;
		if (n>total)
		{
			n=total;
		}
		pthread_wrlock_wrlock(&pThis->audioBuffer.rwlock[bufidx]);
		memcpy(wptr,rptr,n);
		total-=n;
		writeidx+=n;
		rptr+=n;
		pThis->audioBuffer.writeidx[bufidx]=writeidx;
		pThis->audioBuffer.writebuf=bufidx;
		pthread_wrlock_wrunlock(&pThis->audioBuffer.rwlock[bufidx]);
	}

	return RETVAL_OK;
}

int audioout_portaudio_silence(tHandleAudioOutputPortaudio *pThis)
{
	int i;
	for (i=0;i<PINGPONG_BUFNUM;i++)
	{
		pthread_wrlock_wrlock(&pThis->audioBuffer.rwlock[bufidx]);
		pThis->audioBuffer.writeidx[i]=0;
		pthread_wrlock_wrunlock(&pThis->audioBuffer.rwlock[bufidx]);
	}
	return RETVAL_OK;
}
