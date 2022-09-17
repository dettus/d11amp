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

#include "theme_elements.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



void dumpbmps(char* directory)
{
	int col;
	int i;
	int j;
	int max;
	unsigned int *drawbuf;
	char filename[64];

	max=0;
	for (i=0;i<SOURCES_NUM;i++)
	{
		int size;
		size=sizeof(int)*cSources[i].width*cSources[i].height;

		if (size>max)
		{
			max=size;
		}
	}
	printf("max size:%d bytes\n",max);
	drawbuf=(unsigned int*)malloc(max);

	for (i=0;i<SOURCES_NUM;i++)
	{
		eElementSourceFile sourcefile;
		eElementID elementid;
		int width;
		int height;
		int cnt;
		GdkPixbuf *pixbuf;
		GError *err;
		sourcefile=(eElementSourceFile)i;

		cnt=0;
		width=cSources[i].width;
		height=cSources[i].height;
		for (j=0;j<width*height;j++)
		{
			drawbuf[j]=0xFF000000;	// alpha, rgb
		}
		for (j=0;j<ELEMENTS_NUM;j++)
		{
			if (cElementSources[j].sourcefile==sourcefile) 
			{
				int x1,x2;
				int y1,y2;
				int k,l;

				x1=ELEMENT_X(cElementSources[j].id);
				x2=x1+ELEMENT_WIDTH(cElementSources[j].id);
				y1=ELEMENT_Y(cElementSources[j].id);
				y2=y1+ELEMENT_HEIGHT(cElementSources[j].id);


				for (k=x1;k<x2;k++)
				{
					for (l=y1;l<y2;l++)
					{
						if (k>=0 && k<width && l>=0 && l<height)
						{
							drawbuf[k+l*width]=alphargbpalette[col%80];
						}
					}
				}
				col++;
				cnt++;
			}
		}
		err=NULL;
		pixbuf=gdk_pixbuf_new_from_data((unsigned char*)drawbuf,GDK_COLORSPACE_RGB,TRUE,8,width,height,4*width,NULL,NULL);
		snprintf(filename,64,"%s/%s",directory,cSources[i].filename);
		gdk_pixbuf_save(pixbuf,filename,"bmp",&err,NULL);
		g_object_unref(pixbuf);
		
		
		printf("%12s> %3d elements\n",cSources[i].filename,cnt);	
	}

	


	free(drawbuf);
}
void dumpviscolor(char* directory)
{
	char filename[64];
	FILE *f;
	char *descriptions[24]={"black","grey","top of spec","","","","","","","","","","","","","","","bottom of spec","osc 1","osc 2 (slightly dimmer)","osc 3","osc 4","osc 5","peak dots"};
	int i;

	snprintf(filename,64,"%s/VISCOLOR.TXT",directory);
	f=fopen(filename,"wb");
	
	for (i=0;i<24;i++)
	{
		int red;
		int green;
		int blue;

		red=(alphargbpalette[i]>>16)&0xff;
		green=(alphargbpalette[i]>>8)&0xff;
		blue=(alphargbpalette[i]>>0)&0xff;
		fprintf(f,"%d,%d,%d, // color %d = %s\r\n",red,green,blue,i,descriptions[i]);	// DOS style line endings
	}
	fclose(f);
}
void dumppledit(char* directory)
{
	char filename[64];
	FILE *f;
	char *keys[4]={"Normal","Current","NormalBG","SelectedBG"};
	int i;
	snprintf(filename,64,"%s/PLEDIT.TXT",directory);
	f=fopen(filename,"wb");
	fprintf(f,"[Text]\r\n");
	for (i=0;i<4;i++)
	{
		fprintf(f,"%s=#%06X\r\n",keys[i],alphargbpalette[i]&0xffffff);
	}	
	fprintf(f,"Font=Arial\r\n");

	fclose(f);
	
}
int main()
{
	// first, create the output directory
	struct stat st = {0};

	if (stat("output/", &st) == -1) {
		mkdir("output/", 0755);
	}

	// create the BMP files
	dumpbmps("output/");
	dumpviscolor("output/");
	dumppledit("output/");
	

}
