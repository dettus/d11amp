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

#define	TOTAL_NUM	19
#define	MAX_FILE_SIZE	(1<<20)

char *filenames[TOTAL_NUM]={
	"AVS.BMP", "BALANCE.BMP", "CBUTTONS.BMP", "EQMAIN.BMP",
	"EQ_EX.BMP", "MAIN.BMP", "MB.BMP", "MONOSTER.BMP",
	"NUMBERS.BMP", "PLAYPAUS.BMP", "PLEDIT.BMP", "PLEDIT.TXT",
	"POSBAR.BMP", "SHUFREP.BMP", "TEXT.BMP", "TITLEBAR.BMP",
	"VISCOLOR.TXT", "VOLUME.BMP", "cc-0-10.txt"
};

typedef struct _tDefaultThemePackedDir
{
	char filename[13];
	int len;
	int start;
} tDefaultThemePackedDir;


unsigned char outbuf[MAX_FILE_SIZE];
unsigned char inbuf[MAX_FILE_SIZE];

// the data format is as followed:
// each file is packed individually as a block
// each block starts with 8 bytes of verbatim input data
// the next byte is the one of two tags: VERBATIM_TAG and REPEAT_TAG.
// The VERBATIM_TAG is denoted by bit7=0. bits 6..0 are the amount of
// bytes which follow.
// The REPEAT_TAG is denoted by bit7=1.
//     repeat_bytes 14..8 are bits 6..0
//     repeat_bytes  7..0 are bits 7..0 of the next byte
//     the next 1..3 bytes are a previous position within the output
//     buffer. note that the repeat buffer might be overlapping with
//     the edge. (implicit run-length encoding)



int packit(unsigned char* pOutBuf,unsigned char* pInBuf,int len)
{
	int oidx;	// write pointer into the output buffer
	int iidx;	// pointer into the input buffer

	int verbatim_tag_pos;	

	int cnt;

	int pos_byte_num;

	int i;
	int j;
	int k;
	
	int best_match;
	int best_pos;


	cnt=8;
	oidx=0;
	iidx=0;
	verbatim_tag_pos=0;
	
	while (iidx<len)
	{
		if (cnt)	// verbatim write
		{
			pOutBuf[oidx++]=pInBuf[iidx++];
			cnt--;


			if (verbatim_tag_pos!=0)			// update the VERBATIM TAG
			{
				pOutBuf[verbatim_tag_pos]++;	
				if (pOutBuf[verbatim_tag_pos]==127)	// if the tag is full
				{
					cnt=0;				// stop doing verbatim write
					verbatim_tag_pos=0;		// start the next verbatim tag
				}
			}
		} else {
			pos_byte_num=1;	// write the repeat_position as 1 byte
			if (iidx>=256)   pos_byte_num++;	// 1 byte is not enough, use 2
			if (iidx>=65536) pos_byte_num++;	// 2 bytes is not enough, use 3

			// first: check if it is worth using the REPEAT TAG, since it is rather long.
			best_pos=0;
			best_match=0;
			for (i=0;i<(iidx-1);i++)	// go through all the previous bytes
			{
				j=0;
				while ((j<32767) && (j+i)<len && pInBuf[(j+i)]==pInBuf[(j+iidx)])	// check how long the next bytes would match
				{
					j++;
				}
				if (j>best_match)	// if this was longer than before, remember it
				{
					best_match=j;
					best_pos=i;
				}
			}

			if (best_match>(2+pos_byte_num+1))	// the best repeat match is longer than the REPEAT TAG. --> worth it!
			{
				verbatim_tag_pos=0;		// close the verbatim tag.
				pOutBuf[oidx]=((best_match>>8)&0x7f);	// write bits 14..8 of the best match length
				pOutBuf[oidx]|=(1<<7);			// set bit 7.
				oidx++;
				pOutBuf[oidx]=best_match&0xff;		// write bits 7..0
				oidx++;
				if (pos_byte_num==3)	{pOutBuf[oidx]=(best_pos>>16)&0xff;oidx++;}
				if (pos_byte_num>=2)	{pOutBuf[oidx]=(best_pos>> 8)&0xff;oidx++;}
				pOutBuf[oidx]=(best_pos>> 0)&0xff;
				oidx++;
	
				iidx+=best_match;	// skip those bytes
	
			} else {
				if (verbatim_tag_pos==0)	
				{
					verbatim_tag_pos=oidx;
					oidx++;
					cnt=2+pos_byte_num+1;	// add at least more verbatim bytes that a REPEAT TAG
				} else {
					cnt=1;	// add one more byte, see what happens
				}
			}
		}
	}
	return oidx;
}

int main(int argc,char** argv)
{
	FILE *f;
	int oidx;
	int len;
	int packedlen;
	int i;

	tDefaultThemePackedDir defaultThemePackedDir[TOTAL_NUM];	

	if (argc!=2)
	{
		printf("please run with %s THEMEDIR/\n",argv[0]);
		return -1;
	}

	oidx=0;
	for (i=0;i<TOTAL_NUM;i++)
	{
		char filename[1024];
		snprintf(filename,1024,"%s/%s",argv[1],filenames[i]);
		f=fopen(filename,"rb");
		len=fread(inbuf,sizeof(char),sizeof(inbuf),f);
		fclose(f);
		printf("read %d bytes\n",len);

		strncpy(defaultThemePackedDir[i].filename,filenames[i],16);
		defaultThemePackedDir[i].start=oidx;
		defaultThemePackedDir[i].len=len;

		packedlen=packit(&outbuf[oidx],inbuf,len);

		printf("%16s  read %6d bytes --> %4d bytes\n",filenames[i],len,packedlen);
		oidx+=packedlen;
	}
	printf("--> %d bytes\n",oidx);

		
	f=fopen("debug.packed.bin","wb");
	fwrite(defaultThemePackedDir,sizeof(tDefaultThemePackedDir),TOTAL_NUM,f);
	fwrite(outbuf,sizeof(char),oidx,f);
	fclose(f);



	f=fopen("default_theme.c","wb");
	fprintf(f,"/*\n");
	fprintf(f,"Copyright 2022, dettus@dettus.net\n");
	fprintf(f,"\n");
	fprintf(f,"Redistribution and use in source and binary forms, with or without modification,\n");
	fprintf(f,"are permitted provided that the following conditions are met:\n");
	fprintf(f,"\n");
	fprintf(f,"1. Redistributions of source code must retain the above copyright notice, this \n");
	fprintf(f,"   list of conditions and the following disclaimer.\n");
	fprintf(f,"\n");
	fprintf(f,"2. Redistributions in binary form must reproduce the above copyright notice, \n");
	fprintf(f,"   this list of conditions and the following disclaimer in the documentation \n");
	fprintf(f,"   and/or other materials provided with the distribution.\n");
	fprintf(f,"\n");
	fprintf(f,"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\n");
	fprintf(f,"ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED \n");
	fprintf(f,"WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE \n");
	fprintf(f,"DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE \n");
	fprintf(f,"FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL \n");
	fprintf(f,"DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR \n");
	fprintf(f,"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER \n");
	fprintf(f,"CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, \n");
	fprintf(f,"OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE \n");
	fprintf(f,"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n");
	fprintf(f,"*/\n");
	fprintf(f,"//\n");
	fprintf(f,"//\n");
	fprintf(f,"//\n");
	fprintf(f,"// THIS CODE WAS GENERATED BY THE PACK_DEFAULT_THEME TOOL //\n");
	fprintf(f,"//\n");
	fprintf(f,"//\n");
	fprintf(f,"//\n");
	fprintf(f,"#include \"default_theme.h\"\n");
	fprintf(f,"// i had a little fun with data structures here... \n");
	fprintf(f,"// each file is packed individually as a block\n");
	fprintf(f,"// each block starts with 8 bytes of verbatim input data\n");
	fprintf(f,"// the next byte is the one of two tags: VERBATIM_TAG and REPEAT_TAG.\n");
	fprintf(f,"// The VERBATIM_TAG is denoted by bit7=0. bits 6..0 are the amount of\n");
	fprintf(f,"// bytes which follow.\n");
	fprintf(f,"// The REPEAT_TAG is denoted by bit7=1.\n");
	fprintf(f,"//     repeat_bytes 14..8 are bits 6..0\n");
	fprintf(f,"//     repeat_bytes  7..0 are bits 7..0 of the next byte\n");
	fprintf(f,"//     the next 1..3 bytes are a previous position within the output\n");
	fprintf(f,"//     buffer. note that the repeat buffer might be overlapping with\n");
	fprintf(f,"//     the edge. (implicit run-length encoding)\n");


	fprintf(f,"const tDefaultThemePackedDir defaultThemePackedDir[TOTAL_NUM]={\n");
	for (i=0;i<TOTAL_NUM;i++)
	{
		unsigned char tmp[16];
		int j;
		for (j=0;j<14-strlen(defaultThemePackedDir[i].filename);j++)
		{
			tmp[j]=' ';
			tmp[j+1]=0;
		}
		fprintf(f,"\t{.filename=\"%s\"%s,.start=0x%08x,.len=%8d}",defaultThemePackedDir[i].filename,tmp,defaultThemePackedDir[i].start,defaultThemePackedDir[i].len);
		if (i!=(TOTAL_NUM-1)) fprintf(f,",");
		fprintf(f,"\n");
	}
	fprintf(f,"};\n");
	fprintf(f,"const unsigned char defaultThemePacked[%d]={",oidx);
	for (i=0;i<oidx;i++)
	{
		if ((i%16)==0) fprintf(f,"\n\t");
		fprintf(f,"0x%02x",outbuf[i]);
		if ((i+1)!=oidx) fprintf(f,",");
		if ((i%16)==15) fprintf(f,"\t// %08X",i-15);
	}
	fprintf(f,"\n};\n");
	fclose(f);

	return 0;
}


