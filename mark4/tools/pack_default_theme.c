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


char buf[MAX_FILE_SIZE];
char packed[2<<21];

int packit(char* packed,int packedidx,char* buf,int bytes)
{
	// TODO: for now, just put a verbatim copy here

	memcpy(&packed[packedidx],buf,bytes);
	return bytes;	
}

int main(int argc,char** argv)
{
	FILE *f;
	FILE *g;
	int i;
	int j;
	int l;
	char filename[128];
	int bytes;
	int idx;
	int packedsize;
	tDefaultThemePackedDir defaultThemePackedDir[TOTAL_NUM];
	char tmp[1024];
	
	

	if (argc!=2)
	{
		fprintf(stderr,"please run with %s THEMEDIR/\n",argv[0]);
		return 0;
	}

	idx=0;
	for (i=0;i<TOTAL_NUM;i++)
	{
		snprintf(filename,128,"%s/%s",argv[1],filenames[i]);
		f=fopen(filename,"rb");
		bytes=fread(buf,sizeof(char),sizeof(buf),f);	
		printf("%s> read %d bytes\n",filename,bytes);
		fclose(f);

		strncpy(defaultThemePackedDir[i].filename,filenames[i],13);
		defaultThemePackedDir[i].len=bytes;
		defaultThemePackedDir[i].start=idx;
		idx+=packit(packed,idx,buf,bytes);
	}
	packedsize=idx;
	printf("total size:%d bytes\n",idx);

	printf("writing default_theme.c\n");

	g=fopen("default_theme.c","wb");
	fprintf(g,"/*\n");
	fprintf(g,"Copyright 2022, dettus@dettus.net\n");
	fprintf(g,"\n");
	fprintf(g,"Redistribution and use in source and binary forms, with or without modification,\n");
	fprintf(g,"are permitted provided that the following conditions are met:\n");
	fprintf(g,"\n");
	fprintf(g,"1. Redistributions of source code must retain the above copyright notice, this \n");
	fprintf(g,"   list of conditions and the following disclaimer.\n");
	fprintf(g,"\n");
	fprintf(g,"2. Redistributions in binary form must reproduce the above copyright notice, \n");
	fprintf(g,"   this list of conditions and the following disclaimer in the documentation \n");
	fprintf(g,"   and/or other materials provided with the distribution.\n");
	fprintf(g,"\n");
	fprintf(g,"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\n");
	fprintf(g,"ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED \n");
	fprintf(g,"WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE \n");
	fprintf(g,"DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE \n");
	fprintf(g,"FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL \n");
	fprintf(g,"DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR \n");
	fprintf(g,"SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER \n");
	fprintf(g,"CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, \n");
	fprintf(g,"OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE \n");
	fprintf(g,"OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n");
	fprintf(g,"*/\n");
	// substitute .c with .h
	fprintf(g,"#include \"default_theme.h\"\n",tmp);
	fprintf(g,"const tDefaultThemePackedDir defaultThemePackedDir[TOTAL_NUM]={\n");
	for (i=0;i<TOTAL_NUM;i++)
	{
		if (i) fprintf(g,",\n");
		l=16-strlen(defaultThemePackedDir[i].filename);
		for (j=0;j<l;j++)
		{
			tmp[j]=' ';
			tmp[j+1]=0;
		}
		fprintf(g,"\t{.filename=\"%s\"%s,.len=%8d,.start=%8d}",defaultThemePackedDir[i].filename,tmp,defaultThemePackedDir[i].len,defaultThemePackedDir[i].start);
	}
	fprintf(g,"\n};\n");
	fprintf(g,"const char defaultThemePacked[%d]={\n\t",packedsize);
	for (i=0;i<packedsize-1;i++)
	{
		fprintf(g,"0x%02x,",0xff&packed[i]);
		if ((i%16)==15)
		{
			fprintf(g,"\t// 0x%08X\n\t",i);
		}
	}
	fprintf(g,"0x%02x\n",packed[packedsize-1]);
	fprintf(g,"};\n");

	fclose(g);
	return 0;
}
