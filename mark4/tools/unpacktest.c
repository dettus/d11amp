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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct _tDefaultThemePackedDir
{
	char filename[13];
	int len;
	int start;
} tDefaultThemePackedDir;

#define	TOTAL_NUM	19

#include "default_theme.c"

#define	 MAX_FILE_SIZE (1<<20)
char buf[MAX_FILE_SIZE];


int main()
{
	FILE *f;
	int i;
	int l;
	int cnt;
	int j;

	printf("unpacking...\n");

	for (i=0;i<TOTAL_NUM;i++)
	{
		int iidx;
		int ridx;
		int oidx;
		int tag;

		int bytes;
		int offset;
		int taglen;
		cnt=8;
		bytes=defaultThemePackedDir[i].len;
		offset=defaultThemePackedDir[i].start;
		iidx=0;
		oidx=0;
		ridx=0;
		tag=0;
		printf("offset:%d -> ",offset);
		while (oidx<bytes && ((offset+ridx)<=sizeof(defaultThemePacked)))
		{
			if (cnt)
			{
				buf[oidx++]=tag?buf[iidx++]:defaultThemePacked[offset+ridx++];
				cnt--;
			} else {
				taglen=4;
				if (ridx>=256) taglen++;
				if (ridx>=65536) taglen++;
				tag=defaultThemePacked[offset+ridx++];
				
				if (tag)
				{
					cnt=0;
					cnt|=(unsigned int)defaultThemePacked[offset+ridx++]&0xff;cnt<<=8;
					cnt|=(unsigned int)defaultThemePacked[offset+ridx++]&0xff;

					iidx=0;
					if (taglen>=5) {iidx|=(unsigned int)defaultThemePacked[offset+ridx++]&0xff;iidx<<=8;}
					if (taglen>=4) {iidx|=(unsigned int)defaultThemePacked[offset+ridx++]&0xff;iidx<<=8;}
					iidx|=(unsigned int)defaultThemePacked[offset+ridx++]&0xff;
	
				} else {
					cnt=taglen+1;
				}
			}
		}
		printf("ridx: %d\n",ridx);
		f=fopen(defaultThemePackedDir[i].filename,"wb");
		fwrite(buf,sizeof(char),bytes,f);
		fclose(f);
	}
	printf("bytes:%d\n",sizeof(defaultThemePacked));
	
}

