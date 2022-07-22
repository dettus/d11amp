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

#include "pixbufloader.h"
#include "dirent.h"
#include "ctype.h"


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
	if ((dir = opendir (directory)) != NULL)
	{
		while ((ent = readdir (dir)) != NULL)
		{
			if (pixbufloader_compare(ent->d_name,filename)==0)
			{
				snprintf(filename,1024,"%s/%s",directory,ent->d_name);
				return 1;
			}
		}
		closedir (dir);
	} else {
		return 0;
	}
	return 0;
}



int pixbufloader_init(tHandlePixbufLoader* pThis,char* directory)
{
	int i;

	eElementSourceFile sourceid[SOURCES_NUM]={AVS_BMP, BALANCE_BMP, CBUTTONS_BMP, EQ_EX_BMP, EQMAIN_BMP, MAIN_BMP, MB_BMP, MONOSTER_BMP, NUMBERS_BMP, PLAYPAUS_BMP, PLEDIT_BMP, POSBAR_BMP, SHUFREP_BMP, TEXT_BMP, TITLEBAR_BMP, VOLUME_BMP};
	char * filenames[SOURCES_NUM]={"AVS.BMP","BALANCE.BMP","CBUTTONS.BMP","EQ_EX.BMP","EQMAIN.BMP","MAIN.BMP","MB.BMP","MONOSTER.BMP","NUMBERS.BMP","PLAYPAUS.BMP","PLEDIT.BMP","POSBAR.BMP","SHUFREP.BMP","TEXT.BMP","TITLEBAR.BMP","VOLUME.BMP"};
	char filename[1024];
	
	for (i=0;i<SOURCES_NUM;i++)
	{
		int idx;
		idx=(int)sourceid[i];
		snprintf(filename,1024,"%s",filenames[i]);
		if (pixbufloader_adapt_filename(directory,filename))
		{
			pThis->loaded_bmp[idx]=gdk_pixbuf_new_from_file(filename,NULL);
		} else {
			pThis->loaded_bmp[idx]=NULL;
		}
	}
	return	PIXBUFLOADER_OK;
}
int pixbufloader_addelement(tHandlePixbufLoader* pThis,GdkPixbuf* destbuf,int x,int y,eElementID elementID)
{
	int idx;
	int check;
	int sourceid;

	idx=(int)elementID;
	check=cElementSources[idx].id;
	if (idx!=check)	// check if the compiler has rearranged the elements or something
	{
		printf("enums are not usable with this compiler. Please write to dettus@dettus.net\n");
		return PIXBUFLOADER_NOK;
	}
	sourceid=cElementSources[idx].sourcefile;
	if (pThis->loaded_bmp[sourceid]==NULL)	// check if the element was part of the skin
	{
		return	PIXBUFLOADER_NOK;
	}
	
	gdk_pixbuf_copy_area(pThis->loaded_bmp[sourceid],
			cElementSources[idx].startx,
			cElementSources[idx].starty,
			cElementSources[idx].dimx,
			cElementSources[idx].dimy,
			destbuf,x,y);
	return	PIXBUFLOADER_OK;
}
