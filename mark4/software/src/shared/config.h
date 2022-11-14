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

#ifndef	CONFIG_H
#define	CONFIG_H
#include "datastructures.h"
#define	MAXKEYS		32
#define	KEYLEN		32
#define	VALUELEN	160

typedef	struct _tHandleConfig
{
	void *pControllerContext;
	int keycnt;
	char configFileName[2048];
	char keys[MAXKEYS][KEYLEN];
	char values[MAXKEYS][VALUELEN];
} tHandleConfig;

int config_init(tHandleConfig* pThis,void* pControllerContext,char* filename);

int config_getint(tHandleConfig* pThis,char* key, int* pValue,int defValue);
int config_getstr(tHandleConfig* pThis,char* key, char* pValue,char* defValue);
int config_getbool(tHandleConfig* pThis,char* key, int* pValue,int defValue);

int config_setint(tHandleConfig* pThis,char* key,int value);
int config_setstr(tHandleConfig* pThis,char* key,char* value);
int config_setbool(tHandleConfig* pThis,char* key,int value);




#endif

