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

#include "decoder.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *decoder_thread(void* handle)
{
	tHandleDecoder* pThis=(tHandleDecoder*)handle;
	while (1)
	{	
		pthread_mutex_lock(&pThis->mutex);
		usleep(10);
		pthread_mutex_unlock(&pThis->mutex);
		usleep(100);
	}
}
int decoder_init(tHandleDecoder* pThis,void* pControllerContext)
{
	int retval;

	memset(pThis,0,sizeof(tHandleDecoder));
	pThis->pControllerContext=pControllerContext;
	retval=RETVAL_OK;

	pthread_mutex_init(&pThis->mutex,NULL);
	pthread_create(&pThis->thread,NULL,&decoder_thread,(void*)pThis);

	return retval;	
}
int decoder_open_file(tHandleDecoder* pThis,char* filename)
{

	int retval;

	retval=RETVAL_OK;
	pthread_mutex_lock(&pThis->mutex);
	usleep(10);
	pthread_mutex_unlock(&pThis->mutex);

	return retval;
}
