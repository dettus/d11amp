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

#define	MAGIC 0x4c4f4144		// "LOAD"
#include "pixbufloader.h"
#include "elements.h"
#include "dirent.h"
#include "ctype.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

typedef struct _tPixbufLoaderHandle
{
	unsigned int magic;
	GdkPixbuf* source[SOURCES_NUM];
	eElementSources sourceID[SOURCES_NUM];
} tPixbufLoaderHandle;

int pixbufloader_getsize(int* size)
{
	return sizeof(tPixbufLoaderHandle);
}

int pixbufloader_compare(char* s1,char* s2)
{
	int i;
	int l;

	l=strlen(s1);
	if (strlen(s2)!=l)
	{
		return 1;
	}
	for (i=0;i<l;i++)
	{
		if (tolower(s1[i])!=tolower(s2[i])) return 1;
	}
	return 0;
}

int pixbufloader_adapt_filename(char* directory,char* filename)
{
	DIR *dir;
	struct dirent *ent;
	printf("%s -> ",filename);
	if ((dir = opendir (directory)) != NULL)
	{
		while ((ent = readdir (dir)) != NULL) 
		{
			if (pixbufloader_compare(ent->d_name,filename)==0)
			{
				snprintf(filename,1024,"%s/%s",directory,ent->d_name);
				printf("%s\n",filename);
				return 1;
			}
		}
		closedir (dir);
	} else {
		return 0;
	}
	return 0;
}

int pixbufloader_open(void* handle,char* directory)
{
	tPixbufLoaderHandle* pThis=(tPixbufLoaderHandle*)handle;
	char filename[1024];
	char *filenames[]={ "AVS.BMP", "BALANCE.BMP", "CBUTTONS.BMP", "EQ.EX.BMP", "EQMAIN.BMP", "MAIN.BMP", "MB.BMP", "MONOSTER.BMP", "NUMBERS.BMP", "PLAYPAUS.BMP", "PLEDIT.BMP", "POSBAR.BMP", "SHUFREP.BMP", "TEXT.BMP", "TITLEBAR.BMP", "VOLUME.BMP" };
	eElementSources sourceids[]={ AVS_BMP, BALANCE_BMP, CBUTTONS_BMP, EQ_EX_BMP, EQMAIN_BMP, MAIN_BMP, MB_BMP, MONOSTER_BMP, NUMBERS_BMP, PLAYPAUS_BMP, PLEDIT_BMP, POSBAR_BMP, SHUFREP_BMP, TEXT_BMP, TITLEBAR_BMP, VOLUME_BMP };
	
	int i;
	pThis->magic=MAGIC;
	for (i=0;i<SOURCES_NUM;i++)
	{
		pThis->source[i]=NULL;
		pThis->sourceID[i]=sourceids[i];
		snprintf(filename,13,"%s",filenames[i]);
		if (pixbufloader_adapt_filename(directory,filename))
		{
			pThis->source[i]=gdk_pixbuf_new_from_file(filename,NULL);
		}
	}
	return 0;
}

int pixbufloader_addelement(void* handle,eElementID id,int x,int y,GdkPixbuf* drawbuf)
{
	int i;
	int sourceid;
	int elementid;
	tPixbufLoaderHandle* pThis=(tPixbufLoaderHandle*)handle;
	// todo: check the sourceID and the elementID

	elementid=cElements[id].id;
	printf("%d> ",id);
	if (elementid!=id)
	{
		printf("elementid does not match! %d ",elementid);
	}
	i=cElements[id].source;
	printf("source:%d ",i);
	sourceid=pThis->sourceID[i];
	if (sourceid!=i)
	{
		printf("sourceid does not match  %d ",sourceid);
	}
	printf("\n");
	fflush(stdout);
	
	
	
	
	gdk_pixbuf_copy_area(pThis->source[cElements[id].source],
		cElements[id].startx,
		cElements[id].starty,
		cElements[id].dimx,
		cElements[id].dimy,
		drawbuf,x,y);
	return 0;
}
int pixbufloader_initmainwindow(void* handle,GdkPixbuf** retpixbuf)
{
	tPixbufLoaderHandle* pThis=(tPixbufLoaderHandle*)handle;

	*retpixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB, 1, 8,275,116);
	gdk_pixbuf_copy_area(pThis->source[MAIN_BMP], 0,0,275,116,*retpixbuf,0,0);
	pixbufloader_addelement(handle,TITLEBAR_NORMAL_TITLEBAR_ACTIVE,0,0,*retpixbuf);
	pixbufloader_addelement(handle,TITLEBAR_CLUTTERBAR_SHOWN,10,22,*retpixbuf);
	pixbufloader_addelement(handle,NUMBERS_1,48,26,*retpixbuf);
	pixbufloader_addelement(handle,NUMBERS_3,60,26,*retpixbuf);
	pixbufloader_addelement(handle,NUMBERS_3,78,26,*retpixbuf);
	pixbufloader_addelement(handle,NUMBERS_7,90,26,*retpixbuf);
	pixbufloader_addelement(handle,PLAYPAUSE_PLAY_INDICATOR,24,18,*retpixbuf);
	pixbufloader_addelement(handle,MONOSTER_STEREO_ACTIVE,212,41,*retpixbuf);
	pixbufloader_addelement(handle,MONOSTER_MONO_INACTIVE,239,41,*retpixbuf);
	pixbufloader_addelement(handle,VOLUME_023_025,107,57,*retpixbuf);
	pixbufloader_addelement(handle,VOLUME_SLIDER_UNPRESSED,107+10,57,*retpixbuf);
	pixbufloader_addelement(handle,BALANCE_74LEFTORRIGHT,177,57,*retpixbuf);
	pixbufloader_addelement(handle,BALANCE_SLIDER_PRESSED,177+23,57,*retpixbuf);
	pixbufloader_addelement(handle,SHUFREP_EQUALIZER_UNPRESSED,219,58,*retpixbuf);
	pixbufloader_addelement(handle,SHUFREP_PLAYLIST_UNPRESSED,242,58,*retpixbuf);
	pixbufloader_addelement(handle,POSBAR_SONG_PROGRESS_BAR,16,72,*retpixbuf);
	pixbufloader_addelement(handle,POSBAR_SONG_SLIDER_UNPRESSED,76,72,*retpixbuf);
	pixbufloader_addelement(handle,SHUFREP_SHUFFLE_UNPRESSED,164,89,*retpixbuf);
	pixbufloader_addelement(handle,SHUFREP_REPEAT_UNPRESSED,211,89,*retpixbuf);
	pixbufloader_addelement(handle,CBUTTONS_PREV_BUTTON_UNPRESSED,16,88,*retpixbuf);
	pixbufloader_addelement(handle,CBUTTONS_PLAY_BUTTON_PRESSED,39,88,*retpixbuf);
	pixbufloader_addelement(handle,CBUTTONS_PAUSE_BUTTON_UNPRESSED,62,88,*retpixbuf);
	pixbufloader_addelement(handle,CBUTTONS_STOP_BUTTON_UNPRESSED,85,88,*retpixbuf);
	pixbufloader_addelement(handle,CBUTTONS_NEXT_BUTTON_UNPRESSED,108,88,*retpixbuf);
	pixbufloader_addelement(handle,CBUTTONS_OPEN_BUTTON_UNPRESSED,136,89,*retpixbuf);

	return 0;
	
}


