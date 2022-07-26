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
#include <dirent.h>
#include <ctype.h>
#include <string.h>


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

int theme_manager_parse_commandline(tHandleThemeManager* pThis,char* argument)
{
	int l;
	int retval;
	l=strlen(argument);
	if (l>4 && strncmp("dir=",argument,4)==0)
	{
		retval=theme_manager_load_from_directory(pThis,&argument[4]);
	} else {
		return RETVAL_NOK_COMMANDLINE;
	}
	return retval;
}

int theme_manager_init(tHandleThemeManager* pThis,tOptions *pCommandLineOptions)
{
	int i;

	eElementSourceFile sourceid[SOURCES_NUM]={AVS_BMP, BALANCE_BMP, CBUTTONS_BMP, EQ_EX_BMP, EQMAIN_BMP, MAIN_BMP, MB_BMP, MONOSTER_BMP, NUMBERS_BMP, PLAYPAUS_BMP, PLEDIT_BMP, POSBAR_BMP, SHUFREP_BMP, TEXT_BMP, TITLEBAR_BMP, VOLUME_BMP};
	int retval;
	memset(pThis,0,sizeof(tHandleThemeManager));
	for (i=0;i<SOURCES_NUM;i++)
	{
		int idx;
		idx=(int)sourceid[i];
		pThis->loaded_bmp[idx]=NULL;
	}
	retval=RETVAL_OK;
	for (i=pCommandLineOptions->gtkargc;i<pCommandLineOptions->argc;i++)
	{
		int l;
		l=strlen(pCommandLineOptions->argv[i]);
		if (l>12 && strncmp("--gui.theme.",pCommandLineOptions->argv[i],12)==0) 
		{
			int r;
			r=theme_manager_parse_commandline(pThis,&(pCommandLineOptions->argv[i][12]));
			if (r==RETVAL_NOK_COMMANDLINE) fprintf(stderr,"UNKNOWN ARGUMENT [%s] \n",pCommandLineOptions->argv[i]);
			retval|=r;
		}
	}
	return	retval;
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
	unsigned char *blackbuf;
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

	char filename[1024];
	blackbuf=malloc(344*433*4);		// TODO: find a better value.
	for (i=0;i<344*433*4;i+=4)
	{
		blackbuf[i+0]=0x00;
		blackbuf[i+1]=0x00;
		blackbuf[i+2]=0x00;
		blackbuf[i+3]=0xff;
	}
	
	for (i=0;i<SOURCES_NUM;i++)
	{
		int idx;
		int w2,h2;
		idx=(int)cSources[i].sourceid;
		w2=cSources[i].width;
		h2=cSources[i].height;
//		pThis->loaded_bmp[idx]=gdk_pixbuf_new_from_data(blackbuf,GDK_COLORSPACE_RGB,TRUE,8,w2,h2,w2*4,NULL,NULL);
		pThis->loaded_bmp[idx]=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,w2,h2);
		snprintf(filename,1024,"%s",cSources[i].filename);
		if (theme_manager_adapt_filename(directory,filename))
		{
			GdkPixbuf *pixbuf;
			int w1,h1;


			pixbuf=gdk_pixbuf_new_from_file(filename,NULL);
			w1=gdk_pixbuf_get_width(pixbuf);
			h1=gdk_pixbuf_get_height(pixbuf);

			if (w1>w2) w1=w2;
			if (h1>h2) h1=h2;

		        gdk_pixbuf_copy_area(pixbuf,0,0,w1,h1,pThis->loaded_bmp[idx],0,0);
			
			pThis->loaded_bmp[idx]=gdk_pixbuf_new_from_file(filename,NULL);
			g_object_unref(pixbuf);
		}
	}
	free(blackbuf);


// the visualizer
	for (i=0;i<VISCOLOR_NUM;i++)
	{
		pThis->visColors[i].red=0xff-(255*i)/VISCOLOR_NUM;
		pThis->visColors[i].green=(255*i)/VISCOLOR_NUM;
		pThis->visColors[i].blue=0;
	}
	snprintf(filename,1024,"VISCOLOR.TXT");
	if (theme_manager_adapt_filename(directory,filename))
	{
		theme_manager_parse_viscolor(pThis->visColors,filename);	
	}


// the playlist
	snprintf(pThis->playList.fontname,1023,"Arial");
	pThis->playList.color_normal.red=0xff;    pThis->playList.color_normal.green=0x40;    pThis->playList.color_normal.blue=0x00;
	pThis->playList.color_current.red=0xff;   pThis->playList.color_current.green=0xff;   pThis->playList.color_current.blue=0xff;
	pThis->playList.color_normalBG.red=0x00;  pThis->playList.color_normalBG.green=0x00;  pThis->playList.color_normalBG.blue=0x00;
	pThis->playList.color_selectedBG.red=0x00;pThis->playList.color_selectedBG.green=0x40;pThis->playList.color_selectedBG.blue=0xff;

	snprintf(filename,1024,"PLEDIT.TXT");
	if (theme_manager_adapt_filename(directory,filename))
	{
		theme_manager_parse_pledit(pThis,filename);
	}

	return	RETVAL_OK;
}
int theme_manager_addelement(tHandleThemeManager* pThis,GdkPixbuf* destbuf,int x,int y,eElementID elementID)
{
	int idx;
	int check;
	int sourceid;

	idx=(int)elementID;
	check=cElementSources[idx].id;
	if (idx!=check)	// check if the compiler has rearranged the elements or something
	{
		printf("enums are not usable with this compiler. Please write to dettus@dettus.net\n");
		return RETVAL_NOK;
	}
	sourceid=cElementSources[idx].sourcefile;
	if (pThis->loaded_bmp[sourceid]==NULL)	// check if the element was part of the skin
	{
		return	RETVAL_NOK;
	}
	
	gdk_pixbuf_copy_area(pThis->loaded_bmp[sourceid],
			cElementSources[idx].startx,
			cElementSources[idx].starty,
			cElementSources[idx].dimx,
			cElementSources[idx].dimy,
			destbuf,x,y);
	return	RETVAL_OK;
}

int theme_manager_textelement(tHandleThemeManager* pThis,unsigned char c,eElementID *pElementID,eElementID background_element)
{
	switch(c)
	{
		case 'a': case 'A': *pElementID=(TEXT_A);break;
		case 'b': case 'B': *pElementID=(TEXT_B);break;
		case 'c': case 'C': *pElementID=(TEXT_C);break;
		case 'd': case 'D': *pElementID=(TEXT_D);break;
		case 'e': case 'E': *pElementID=(TEXT_E);break;
		case 'f': case 'F': *pElementID=(TEXT_F);break;
		case 'g': case 'G': *pElementID=(TEXT_G);break;
		case 'h': case 'H': *pElementID=(TEXT_H);break;
		case 'i': case 'I': *pElementID=(TEXT_I);break;
		case 'j': case 'J': *pElementID=(TEXT_J);break;
		case 'k': case 'K': *pElementID=(TEXT_K);break;
		case 'l': case 'L': *pElementID=(TEXT_L);break;
		case 'm': case 'M': *pElementID=(TEXT_M);break;
		case 'n': case 'N': *pElementID=(TEXT_N);break;
		case 'o': case 'O': *pElementID=(TEXT_O);break;
		case 'p': case 'P': *pElementID=(TEXT_P);break;
		case 'q': case 'Q': *pElementID=(TEXT_Q);break;
		case 'r': case 'R': *pElementID=(TEXT_R);break;
		case 's': case 'S': *pElementID=(TEXT_S);break;
		case 't': case 'T': *pElementID=(TEXT_T);break;
		case 'u': case 'U': *pElementID=(TEXT_U);break;
		case 'v': case 'V': *pElementID=(TEXT_V);break;
		case 'w': case 'W': *pElementID=(TEXT_W);break;
		case 'x': case 'X': *pElementID=(TEXT_X);break;
		case 'y': case 'Y': *pElementID=(TEXT_Y);break;
		case 'z': case 'Z': *pElementID=(TEXT_Z);break;
		case '"':           *pElementID=(TEXT_QUOTATION_MARK);break;
		case '@':           *pElementID=(TEXT_AT_SYMBOL);break;
		case '0':           *pElementID=(TEXT_0);break;
		case '1':           *pElementID=(TEXT_1);break;
		case '2':           *pElementID=(TEXT_2);break;
		case '3':           *pElementID=(TEXT_3);break;
		case '4':           *pElementID=(TEXT_4);break;
		case '5':           *pElementID=(TEXT_5);break;
		case '6':           *pElementID=(TEXT_6);break;
		case '7':           *pElementID=(TEXT_7);break;
		case '8':           *pElementID=(TEXT_8);break;
		case '9':           *pElementID=(TEXT_9);break;
				    // TEXT_ELLIPSIS= ...
		case '.':           *pElementID=(TEXT_PERIOD);break;
		case ':':           *pElementID=(TEXT_COLON);break;
		case '(':           *pElementID=(TEXT_LEFT_PARENTHESIS);break;
		case ')':           *pElementID=(TEXT_RIGHT_PARENTHESIS);break;
		case '-':           *pElementID=(TEXT_DASH);break;
		case '\'':          *pElementID=(TEXT_APOSTROPHE);break;
		case '!':           *pElementID=(TEXT_EXCLAMATION_MARK);break;
		case '_':           *pElementID=(TEXT_UNDERSCORE);break;
		case '+':           *pElementID=(TEXT_PLUS_SYMBOL);break;
		case '\\':          *pElementID=(TEXT_BACK_SLASH);break;
		case '/':           *pElementID=(TEXT_SLASH);break;
		case '[':           *pElementID=(TEXT_LEFT_BRACKET);break;
		case ']':           *pElementID=(TEXT_RIGHT_BRACKET);break;
		case '^':           *pElementID=(TEXT_CARROT);break;
		case '&':           *pElementID=(TEXT_AMPERSAND);break;
		case '%':           *pElementID=(TEXT_PERCENT);break;
		case ',':           *pElementID=(TEXT_COMMA);break;
		case '=':           *pElementID=(TEXT_EQUAL);break;
		case '$':           *pElementID=(TEXT_DOLLAR);break;
		case '#':           *pElementID=(TEXT_POUND);break;
		case 226: case 194: *pElementID=(TEXT_A_CIRCUMFLEX);break;
		case 246: case 214: *pElementID=(TEXT_O_UMLAUT);break;
		case 228: case 196: *pElementID=(TEXT_A_UMLAUT);break;
		case '?':           *pElementID=(TEXT_QUESTION_MARK);break;
		case '*':           *pElementID=(TEXT_ASTERISK);break;
		default: *pElementID=background_element;
	//(TEXT_TITLE_DISPLAY_SPACE, TEXT_TIME_DISPLAY_BACKGROUND, TEXT_KBPS_DISPLAY_SPACE
			 break;
	}
	return RETVAL_OK;
}
int theme_manager_get_viscolors(tHandleThemeManager* pThis,tVisColor **pVisColor)
{
	*pVisColor=pThis->visColors;
	return RETVAL_OK;
}
int theme_manager_get_playlistTheme(tHandleThemeManager* pThis,tPlayListTheme **pPlayList)
{
	*pPlayList=&(pThis->playList);
	return RETVAL_OK;
}

