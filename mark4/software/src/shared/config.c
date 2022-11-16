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

#include "config.h"
#include "controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int config_read_file(tHandleConfig* pThis)
{
	FILE *f;
	char line[1024];

	pThis->keycnt=0;
	f=fopen(pThis->configFileName,"rb");
	if (f)
	{
		int i;
		int j;
		int equal;
		fgets(line,sizeof(line),f);
		while (!feof(f) && pThis->keycnt<MAXKEYS)
		{
			int l;
			equal=0;
			l=strlen(line);
			j=0;
			for (i=0;i<l;i++)
			{
				char c;
				c=line[i];
				if (c=='=' && equal==0)
				{
					equal=1;
					j=0;
				} else if (c>=' ') {
					if (equal==0)
					{
						if (j<KEYLEN)
						{
							pThis->keys[pThis->keycnt][j++]=c;
							pThis->keys[pThis->keycnt][j]=0;
						}
					} else {
						if (j<VALUELEN)
						{
							pThis->values[pThis->keycnt][j++]=c;
							pThis->values[pThis->keycnt][j]=0;
						}
					}
				}
			}
			if (equal && pThis->keycnt<MAXKEYS) 
			{
				pThis->keycnt++;
	// TODO: include an error message for " too many keys "
			}
			fgets(line,sizeof(line),f);
		}
		fclose(f);
	} else {
		printf("TODO: CREATE DIRECTORY AND FILE %s\n",pThis->configFileName);
	}
	return RETVAL_OK;
}



int config_init(tHandleConfig* pThis,void* pControllerContext,char* filename)
{
	int retval;
	char dirname[1024];

	memset(pThis,0,sizeof(tHandleConfig));
	pThis->pControllerContext=pControllerContext;

	controller_get_config_dir(pControllerContext,dirname);	
	snprintf(pThis->configFileName,2048,"%s/%s",dirname,filename);

	retval=config_read_file(pThis);
	
	return retval;
}


int config_write_file(tHandleConfig* pThis)
{
	FILE *f;
	int i;
	f=fopen(pThis->configFileName,"wb");
	if (f)
	{
		for (i=0;i<pThis->keycnt;i++)
		{
			fprintf(f,"%s=%s\n",pThis->keys[i],pThis->values[i]);
		}
		fclose(f);
	} else {
		printf("TODO: CREATE DIRECTORY AND FILE %s\n",pThis->configFileName);
	}
	return RETVAL_OK;
}
int config_findkey(tHandleConfig* pThis,char* key,char* defValue)
{
	int i;
	int l1,l2;
	int found;
	l1=strlen(key);
	found=-1;
	for (i=0;i<pThis->keycnt;i++)
	{
		l2=strlen(pThis->keys[i]);
		if (l1==l2)
		{
			if (strncmp(key,pThis->keys[i],l1)==0)
			{
				found=i;
			}
		}
	}
	if (found==-1 && pThis->keycnt<MAXKEYS)
	{
		strncpy(pThis->keys[pThis->keycnt],key,l1);
		strncpy(pThis->values[pThis->keycnt],defValue,strlen(defValue));
		found=pThis->keycnt;
		pThis->keycnt++;
		config_write_file(pThis);
	}
	// TODO: include an error message for " too many keys "
	return found;
}
int config_getint(tHandleConfig* pThis,char* key, int* pValue,int defValue)
{
	int idx;
	char tmp[64];
	snprintf(tmp,64,"%d",defValue);
	*pValue=defValue;
	idx=config_findkey(pThis,key,tmp);
	if (idx==-1)
	{
		return RETVAL_NOK;
	}
	*pValue=atoi(pThis->values[idx]);
	return RETVAL_OK;
}
int config_getstr(tHandleConfig* pThis,char* key, char* pValue,char* defValue)
{
	int idx;
	strncpy(pValue,defValue,VALUELEN);
	idx=config_findkey(pThis,key,defValue);
	if (idx==-1)
	{
		return RETVAL_NOK;
	}
	strncpy(pValue,pThis->values[idx],VALUELEN);
	return RETVAL_OK;
}
int config_getbool(tHandleConfig* pThis,char* key, int* pValue,int defValue)
{
	int idx;
	*pValue=defValue;
	idx=config_findkey(pThis,key,defValue?"TRUE":"FALSE");
	if (idx==-1)
	{
		return RETVAL_NOK;
	}
	
	switch(pThis->values[idx][0])
	{
		case 'f':	// false
		case 'F':	// False/FALSE
			*pValue=0;
			break;
		case 't':	// true
		case 'T':	// True/TRUE
			*pValue=1;
			break;
		default:
			return RETVAL_NOK;	// key error
	}
	return RETVAL_OK;
}

int config_getonoff(tHandleConfig* pThis,char* key, eOnOff* pValue,eOnOff defValue)
{
	int idx;
	*pValue=defValue;
	idx=config_findkey(pThis,key,(defValue==eONOFF_ON)?"ON":"OFF");
	if (idx==-1)
	{
		return RETVAL_NOK;	// key error
	}
	
	switch(pThis->values[idx][1])
	{
		case 'f':	// off
		case 'F':	// oFF/OFF
			*pValue=eONOFF_OFF;
			break;
		case 'n':	// on
		case 'N':	// oN/ON
			*pValue=eONOFF_ON;;
			break;
		default:
			return RETVAL_NOK;	// value error
	}
	return RETVAL_OK;
}

int config_setint(tHandleConfig* pThis,char* key,int value)
{
	char tmp[64];
	int idx;
	snprintf(tmp,64,"%d",value);
	idx=config_findkey(pThis,key,tmp);
	if (idx==-1)
	{
		return RETVAL_NOK;
	}
	strncpy(pThis->values[idx],tmp,VALUELEN);
	return config_write_file(pThis);
	
}
int config_setstr(tHandleConfig* pThis,char* key,char* value)
{
	int idx;
	idx=config_findkey(pThis,key,value);
	if (idx==-1)
	{
		return RETVAL_NOK;
	}
	strncpy(pThis->values[idx],value,VALUELEN);
	return config_write_file(pThis);

}
int config_setbool(tHandleConfig* pThis,char* key,int value)
{
	int idx;
	idx=config_findkey(pThis,key,value?"TRUE":"FALSE");
	if (idx==-1)
	{
		return RETVAL_NOK;
	}
	snprintf(pThis->values[idx],VALUELEN,"%s",value?"TRUE":"FALSE");
	return config_write_file(pThis);
}

int config_setonoff(tHandleConfig* pThis,char* key,eOnOff value)
{
	int idx;
	idx=config_findkey(pThis,key,(value==eONOFF_ON)?"ON":"OFF");
	if (idx==-1)
	{
		return RETVAL_NOK;
	}
	snprintf(pThis->values[idx],VALUELEN,"%s",value?"ON":"OFF");
	return config_write_file(pThis);
}


