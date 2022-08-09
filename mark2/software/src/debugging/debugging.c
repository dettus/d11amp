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
#ifdef D11AMP_DEBUGGING

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "debugging.h"

static FILE *f_debug=NULL;
static unsigned long long now0;

typedef struct _tDebugVariables
{
	char* name;
	char symbol;
	char width;
} tDebugVariables;
#define	VARIABLES_NUM	4
const tDebugVariables debug_variables[VARIABLES_NUM]={
	{.name="audio_lock",.symbol='a',.width=1},
	{.name="initphase",.symbol='i',.width=1},
	{.name="decodermutex",.symbol='d',.width=1},
	{.name="pressed",.symbol='p',.width=5}
};
void gettimeofday(struct timeval*,void* ptr);
int debugging_start_vcd(char* filename)
{
	int i;
	struct timeval tv;
	if (f_debug!=NULL)
	{
		fclose(f_debug);	
	}
	f_debug=fopen(filename,"wb");
	fprintf(f_debug,"$date\n\tTue Aug  9 21:32:33 2022\n$end\n");
	fprintf(f_debug,"$version\n\tDebugging\n$end\n");
	fprintf(f_debug,"$timescale\n\t1us\n$end\n");
	fprintf(f_debug,"$scope module d11amp $end\n");
	for (i=0;i<VARIABLES_NUM;i++)
	{
		fprintf(f_debug,"$var wire %d %c %s $end\n",debug_variables[i].width,debug_variables[i].symbol,debug_variables[i].name);
	}
	fprintf(f_debug,"$upscope $end\n");
	fprintf(f_debug,"$enddefinitions $end\n");
	fflush(f_debug);
	gettimeofday(&tv,NULL);
	now0=tv.tv_sec;
	now0*=1000000;
	now0+=tv.tv_usec;	
	return 0;
}

int debugging_write_vcd(char* variable,unsigned long long value)
{
	unsigned long long now;
	struct timeval tv;
	int msb;
	int i;
	int found;
	int l1,l2;

	gettimeofday(&tv,NULL);
	now=tv.tv_sec;
	now*=1000000;
	now+=tv.tv_usec;	
	now-=now0;
	found=-1;

	l1=strlen(variable);
	for (i=0;i<VARIABLES_NUM && found==-1;i++)
	{
		l2=strlen(debug_variables[i].name);
		if (l1==l2 && strncmp(debug_variables[i].name,variable,l1)==0)
		{
			found=i;
		}
	}

	if (found>=0)
	{
		fprintf(f_debug,"#%lld\n",now);
		msb=debug_variables[found].width;
		
		fprintf(f_debug,"b");
		while (msb)
		{
			msb--;
			fprintf(f_debug,"%lld",(value>>msb)&1);
		}
		fprintf(f_debug," %c\n",debug_variables[found].symbol);
	}
	fflush(f_debug);
	return 0;	
}

#endif
