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
#define	MAX_FILE_SIZE	(1<<20)

unsigned char packbuf[MAX_FILE_SIZE];
unsigned char outbuf[MAX_FILE_SIZE];

int unpackit(unsigned char *pOutBuf,unsigned char *pPackBuf,int len)
{
	int oidx;	// write pointer into the output buffer
	int iidx;	// read pointer
	int ridx;	// read pointer into the output buffer (for REPEAT TAG)


	int verbatim_len;
	int repeat_len;
	unsigned char tag;	// 0=VERBATIM, 1=REPEAT

	oidx=0;
	iidx=0;
	ridx=0;

	verbatim_len=8;		// starts with 8 verbatim bytes
	tag=0;
	repeat_len=0;
	while (oidx<len)
	{
		if (tag==0 && verbatim_len)
		{
			pOutBuf[oidx]=pPackBuf[iidx];
			oidx++;
			iidx++;
			verbatim_len--;
		}
		else if (tag==1 && repeat_len)
		{
			pOutBuf[oidx]=pOutBuf[ridx];
			oidx++;
			ridx++;
			repeat_len--;
		} else {
			tag=pPackBuf[iidx];
			printf("%8d> ",oidx);
			iidx++;
			if ((tag&0x80)==0x00)
			{
				verbatim_len=tag;	
				printf("verbatim %d bytes\n",verbatim_len);
				tag=0;
			} else {
				int pos_byte_num;

				pos_byte_num=1;
				if (oidx>=256)	pos_byte_num++;
				if (oidx>=65536) pos_byte_num++;
	
				repeat_len=tag&0x7f;
				repeat_len<<=8;
				repeat_len|=(pPackBuf[iidx]);
				iidx++;

				ridx=pPackBuf[iidx];iidx++;
				if (pos_byte_num>=2)	{ridx<<=8;ridx|=pPackBuf[iidx];iidx++;}	
				if (pos_byte_num>=3)	{ridx<<=8;ridx|=pPackBuf[iidx];iidx++;}	
				printf("repeat %d bytes, from %08x\n",repeat_len,ridx);
				tag=1;
			}
		}
	}
	return oidx;
}

int main(int argc,char** argv)
{
	FILE *f;
	int bytes;
	int i;
	tDefaultThemePackedDir defaultThemePackedDir[TOTAL_NUM];

	if (argc!=2)
	{
		printf("please run with %s debug.packed.bin\n",argv[0]);
		return -1;
	}

	f=fopen(argv[1],"rb");
	fread(defaultThemePackedDir,sizeof(tDefaultThemePackedDir),TOTAL_NUM,f);
	bytes=fread(packbuf,sizeof(char),sizeof(packbuf),f);
	fclose(f);
	
	printf("read %d bytes\n",bytes);

	for (i=0;i<TOTAL_NUM;i++)
	{
		int len;
		int start;

		start=defaultThemePackedDir[i].start;
		len=defaultThemePackedDir[i].len;
		printf("%16s> %d bytes @%08X\n",defaultThemePackedDir[i].filename,len,start);
		len=unpackit(outbuf,&packbuf[start],len);

		f=fopen(defaultThemePackedDir[i].filename,"wb");
		fwrite(outbuf,sizeof(char),len,f);
		fclose(f);
	}
	return 0;
}

