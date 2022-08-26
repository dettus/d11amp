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

#include "datastructures.h"
#include "theme_manager.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
typedef struct _tSources
{
	char* filename;
	int width;
	int height;
	eElementSourceFile sourceid;
} tSources;


const tSources cSources[SOURCES_NUM]=
{
	{.filename="AVS.BMP",     .width= 97,.height=188,.sourceid=AVS_BMP     },
	{.filename="BALANCE.BMP", .width= 68,.height=433,.sourceid=BALANCE_BMP },
	{.filename="CBUTTONS.BMP",.width=136,.height= 36,.sourceid=CBUTTONS_BMP},
	{.filename="EQ_EX.BMP",   .width=275,.height= 82,.sourceid=EQ_EX_BMP   },
	{.filename="EQMAIN.BMP",  .width=275,.height=315,.sourceid=EQMAIN_BMP  },
	{.filename="MAIN.BMP",    .width=275,.height=116,.sourceid=MAIN_BMP    },
	{.filename="MB.BMP",      .width=234,.height=119,.sourceid=MB_BMP      },
	{.filename="MONOSTER.BMP",.width= 58,.height= 24,.sourceid=MONOSTER_BMP},
	{.filename="NUMBERS.BMP", .width= 99,.height= 13,.sourceid=NUMBERS_BMP },
	{.filename="PLAYPAUS.BMP",.width= 42,.height=  9,.sourceid=PLAYPAUS_BMP},
	{.filename="PLEDIT.BMP",  .width=280,.height=186,.sourceid=PLEDIT_BMP  },
	{.filename="POSBAR.BMP",  .width=307,.height= 10,.sourceid=POSBAR_BMP  },
	{.filename="SHUFREP.BMP", .width= 92,.height= 85,.sourceid=SHUFREP_BMP },
	{.filename="TEXT.BMP",    .width=155,.height= 74,.sourceid=TEXT_BMP    },
	{.filename="TITLEBAR.BMP",.width=344,.height= 87,.sourceid=TITLEBAR_BMP},
	{.filename="VOLUME.BMP",  .width= 68,.height=433,.sourceid=VOLUME_BMP  }
};


int theme_manager_init(tHandleThemeManager* pThis)
{
	memset(pThis,0,sizeof(tHandleThemeManager));
	int i;

	eElementSourceFile sourceid[SOURCES_NUM]={AVS_BMP, BALANCE_BMP, CBUTTONS_BMP, EQ_EX_BMP, EQMAIN_BMP, MAIN_BMP, MB_BMP, MONOSTER_BMP, NUMBERS_BMP, PLAYPAUS_BMP, PLEDIT_BMP, POSBAR_BMP, SHUFREP_BMP, TEXT_BMP, TITLEBAR_BMP, VOLUME_BMP};
	int retval;
	memset(pThis,0,sizeof(tHandleThemeManager));
	for (i=0;i<SOURCES_NUM;i++)
	{
		int idx;
		idx=(int)sourceid[i];
		// create an empty pixbuf with the proper size
		pThis->loaded_bmp[idx]=gdk_pixbuf_new(GDK_COLORSPACE_RGB,FALSE,8,cSources[idx].width,cSources[idx].height);
	}
	retval=RETVAL_OK;

	return retval;
}
int theme_manager_compare(char* s1,char* s2)
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

int theme_manager_adapt_filename(char* directory,char* filename)
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (directory)) != NULL)
	{
		while ((ent = readdir (dir)) != NULL)
		{
			if (theme_manager_compare(ent->d_name,filename)==0)
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
int theme_manager_parse_viscolor(tVisColor* pVisColor,char* filename)
{
	char line[1024];
	char tmp[16];
	int idx;
	int kommacnt;
	int l;
	int i;
	int j;
	int red,green,blue;
	FILE *f;

	idx=0;
	f=fopen(filename,"rb");
	while (!feof(f))
	{
		fgets(line,sizeof(line),f);
		l=strlen(line);
		j=0;
		kommacnt=0;
		for (i=0;i<l;i++)
		{
			char c;
			c=line[i];
			if (c>='0' && c<='9')
			{
				if (j<15) 
				{
					tmp[j++]=c;
					tmp[j]=0;
				}
			} else if (j!=0) {
				j=0;
				switch (kommacnt)
				{
					case 0:
						kommacnt=1;
						red=atoi(tmp);
						break;
					case 1:
						kommacnt=2;
						green=atoi(tmp);
						break;
					case 2:
						kommacnt=3;
						blue=atoi(tmp);
						break;
					default:
						break;
				}
			}
			if (c=='/') i=l;
		}
		if (kommacnt==3)
		{
			pVisColor[idx].red=red;
			pVisColor[idx].green=green;
			pVisColor[idx].blue=blue;
			idx++;
			
		}
	}
	fclose(f);
	if (idx==VISCOLOR_NUM)
	{
		return RETVAL_OK;
	}
	return RETVAL_NOK;
	
}
int theme_manager_parse_pledit(tHandleThemeManager* pThis,char *filename)
{
	int i;
	int l;
	FILE *f;
	int equal;
	unsigned long long x;
	char linebuf[1024];
	char linebuf2[1024];

	f=fopen(filename,"rb");
	if (!f) return RETVAL_NOK;
	while (!feof(f))
	{
		fgets(linebuf,sizeof(linebuf),f);
		strncpy(linebuf2,linebuf,1024);
		l=strlen(linebuf);
		equal=0;
		x=0;
		for (i=0;i<l;i++)	// first: upper case
		{
			char c;
			c=linebuf[i];
			if (c<' ') c=0;	// terminate the string at the newline
			if (c>='a' && c<='z') c^=32;
			if (c=='=') equal=i;
			if (equal)
			{
				
				if (c>='0' && c<='9') {x<<=4;x|=(c-'0');}
				if (c>='A' && c<='F') {x<<=4;x|=(c-'A'+10);}
			}
			linebuf[i]=c;
		}
		if (strncmp(linebuf,"NORMAL=",7)==0 && l>=7) {pThis->playList.color_normal.red=(x>>16)&0xff;pThis->playList.color_normal.green=(x>>8)&0xff;pThis->playList.color_normal.blue=(x>>0)&0xff;}
		if (strncmp(linebuf,"CURRENT=",8)==0 && l>=8) {pThis->playList.color_current.red=(x>>16)&0xff;pThis->playList.color_current.green=(x>>8)&0xff;pThis->playList.color_current.blue=(x>>0)&0xff;}
		if (strncmp(linebuf,"NORMALBG=",9)==0 && l>=9) {pThis->playList.color_normalBG.red=(x>>16)&0xff;pThis->playList.color_normalBG.green=(x>>8)&0xff;pThis->playList.color_normalBG.blue=(x>>0)&0xff;}
		if (strncmp(linebuf,"SELECTEDBG=",11)==0 && l>=11) {pThis->playList.color_selectedBG.red=(x>>16)&0xff;pThis->playList.color_selectedBG.green=(x>>8)&0xff;pThis->playList.color_selectedBG.blue=(x>>0)&0xff;}
		if (strncmp(linebuf,"FONT=",5)==0 && l>=5) {strncpy(pThis->playList.fontname,&linebuf2[5],1023);}
	}

	fclose(f);
	return RETVAL_OK;
}

int theme_manager_load_from_directory(tHandleThemeManager* pThis,char* directory)
{
	int i;
	char filename[1024];

	// step 1: initialize the defaults
	{

		// the loaded bmp files
		// TODO: this one is inializing them as black. instead, create a DEFAULT THEME
		unsigned char *blackbuf;
		int max;

		max=0;	// find the size for the blackbuf
		for (i=0;i<SOURCES_NUM;i++)
		{
			int size;
			size=cSources[i].width*cSources[i].height;
			if (size>max)
			{
				max=size;
			}
		}
		blackbuf=malloc(max*4);
		memset(blackbuf,0,max*4);	// all zero=black

		for (i=0;i<SOURCES_NUM;i++)
		{
			GdkPixbuf *pixbuf;
			int idx;
			idx=cSources[i].sourceid;	
			pixbuf=gdk_pixbuf_new_from_data(blackbuf,GDK_COLORSPACE_RGB,8,FALSE,cSources[i].width,cSources[i].height,cSources[i].width*3,NULL,NULL);
			gdk_pixbuf_copy_area(pixbuf,0,0,cSources[i].width,cSources[i].height,pThis->loaded_bmp[idx],0,0);
			g_object_unref(pixbuf);
		}
		free(blackbuf);
		// the visualizer colors
		for (i=0;i<VISCOLOR_NUM;i++)
		{
			pThis->visColors[i].red=0xff-(255*i)/VISCOLOR_NUM;
			pThis->visColors[i].green=(255*i)/VISCOLOR_NUM;
			pThis->visColors[i].blue=0;
		}

		// the playlist
		snprintf(pThis->playList.fontname,1023,"Arial");
		pThis->playList.color_normal.red=0xff;    pThis->playList.color_normal.green=0x40;    pThis->playList.color_normal.blue=0x00;
		pThis->playList.color_current.red=0xff;   pThis->playList.color_current.green=0xff;   pThis->playList.color_current.blue=0xff;
		pThis->playList.color_normalBG.red=0x00;  pThis->playList.color_normalBG.green=0x00;  pThis->playList.color_normalBG.blue=0x00;
		pThis->playList.color_selectedBG.red=0x00;pThis->playList.color_selectedBG.green=0x40;pThis->playList.color_selectedBG.blue=0xff;
	}
	// step 2: load the files
	{
		// the images

		for (i=0;i<SOURCES_NUM;i++)
		{
			int bmpwidth;
			int bmpheight;
			int minwidth;
			int minheight;
			eElementID idx;
			idx=(int)cSources[i].sourceid;
			snprintf(filename,1024,"%s",cSources[i].filename);
			if (theme_manager_adapt_filename(directory,filename))
			{
				GdkPixbuf *pixbuf;
				pixbuf=gdk_pixbuf_new_from_file(filename,NULL);
				bmpwidth=gdk_pixbuf_get_width(pixbuf);
				bmpheight=gdk_pixbuf_get_height(pixbuf);
				minwidth=cSources[i].width;	
				minheight=cSources[i].height;

				if (bmpwidth<minwidth)
				{
					minwidth=bmpwidth;
				}
				if (bmpheight<minheight)
				{
					minheight=bmpheight;
				}
				gdk_pixbuf_copy_area(pixbuf,0,0,minwidth,minheight,pThis->loaded_bmp[idx],0,0);
				g_object_unref(pixbuf);
			}
		}
		
		// the visualizer
		snprintf(filename,1024,"VISCOLOR.TXT");
		if (theme_manager_adapt_filename(directory,filename))
		{
			theme_manager_parse_viscolor(pThis->visColors,filename);	
		}

		// the playlist
		snprintf(filename,1024,"PLEDIT.TXT");
		if (theme_manager_adapt_filename(directory,filename))
		{
			theme_manager_parse_pledit(pThis,filename);
		}
	}
	return RETVAL_OK;
}
int theme_manager_draw_element(tHandleThemeManager* pThis,GdkPixbuf* destbuf,eElementID elementID)
{
	int idx;
	idx=(int)elementID;

	if (cElementSources[idx].id!=elementID)
	{
		fprintf(stderr,"enums are not usable with this compiler. please write to dettus@dettus.net\n");
		return RETVAL_NOK;
	}
	return theme_manager_draw_element_at(pThis,destbuf,elementID,cElementSources[idx].destx,cElementSources[idx].desty);
}
int theme_manager_draw_element_at(tHandleThemeManager* pThis,GdkPixbuf* destbuf,eElementID elementID,int x,int y)
{
	int idx;
	int bmpid;
	idx=(int)elementID;

	if (cElementSources[idx].id!=elementID)
	{
		fprintf(stderr,"enums are not usable with this compiler. please write to dettus@dettus.net\n");
		return RETVAL_NOK;
	}
	bmpid=cElementSources[idx].sourcefile;
	gdk_pixbuf_copy_area(pThis->loaded_bmp[bmpid],
			cElementSources[idx].startx,
			cElementSources[idx].starty,
			cElementSources[idx].dimx,
			cElementSources[idx].dimy,
			destbuf,x,y);
	return RETVAL_OK;
}

int theme_manager_draw_text(tHandleThemeManager* pThis,GdkPixbuf** pDestbuf,eElementID backGroundElement,char* text)
{
	int i;
	int l;
	int l2;
#define	CHAR_WIDTH	5
#define	CHAR_HEIGHT	6
	
	l=l2=strlen(text);

	// ... is a special character: the text contains ..., but it will be just one symbol/letter in the end.
	for (i=0;i<l-2;i++)
	{
		if (text[i+0]=='.' && text[i+1]=='.' && text[i+2]=='.')
		{
			l2-=3;
			i+=3;
		}
	}

	*pDestbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,FALSE,8,l2*5,5);


	return RETVAL_OK;	
}



