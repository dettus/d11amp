/*

Copyright 2023, dettus@dettus.net

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

#define	NUM_MANDATORY_FILES	10	// TODO: find out, how many of the BMP files are mandatory

#include "controller.h"
#include "datastructures.h"
#include "theme_manager.h"
#include "default_theme.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zip.h>

int theme_manager_init(tHandleThemeManager* pThis,void* pControllerContext)
{
	int i;

	eElementSourceFile sourceid[SOURCES_NUM]={AVS_BMP, BALANCE_BMP, CBUTTONS_BMP, EQ_EX_BMP, EQMAIN_BMP, MAIN_BMP, MB_BMP, MONOSTER_BMP, NUMBERS_BMP, PLAYPAUS_BMP, PLEDIT_BMP, POSBAR_BMP, SHUFREP_BMP, TEXT_BMP, TITLEBAR_BMP, VOLUME_BMP};
	int retval;
	memset(pThis,0,sizeof(tHandleThemeManager));
	pThis->pControllerContext=pControllerContext;
	for (i=0;i<SOURCES_NUM;i++)
	{
		int idx;
		idx=(int)sourceid[i];
		// create an empty pixbuf with the proper size
		pThis->loaded_bmp[idx]=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,cSources[idx].width,cSources[idx].height);
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
				closedir (dir);
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
	if (!f)
	{
		fprintf(stderr,"9 unable to open file [%s] for reading\n",filename);
		return RETVAL_NOK;
	}
	while (!feof(f))
	{
		if (fgets(line,sizeof(line),f)!=NULL)
		{
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
		if (fgets(linebuf,sizeof(linebuf),f)!=NULL)
		{

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
			if (strncmp(linebuf,"NORMAL=",7)==0 && l>=7) {pThis->playListTheme.color_normal.red=(x>>16)&0xff;pThis->playListTheme.color_normal.green=(x>>8)&0xff;pThis->playListTheme.color_normal.blue=(x>>0)&0xff;}
			if (strncmp(linebuf,"CURRENT=",8)==0 && l>=8) {pThis->playListTheme.color_current.red=(x>>16)&0xff;pThis->playListTheme.color_current.green=(x>>8)&0xff;pThis->playListTheme.color_current.blue=(x>>0)&0xff;}
			if (strncmp(linebuf,"NORMALBG=",9)==0 && l>=9) {pThis->playListTheme.color_normalBG.red=(x>>16)&0xff;pThis->playListTheme.color_normalBG.green=(x>>8)&0xff;pThis->playListTheme.color_normalBG.blue=(x>>0)&0xff;}
			if (strncmp(linebuf,"SELECTEDBG=",11)==0 && l>=11) {pThis->playListTheme.color_selectedBG.red=(x>>16)&0xff;pThis->playListTheme.color_selectedBG.green=(x>>8)&0xff;pThis->playListTheme.color_selectedBG.blue=(x>>0)&0xff;}
			if (strncmp(linebuf,"FONT=",5)==0 && l>=5) {strncpy(pThis->playListTheme.fontname,&linebuf2[5],1023);}
		}
	}

	fclose(f);
	return RETVAL_OK;
}

int theme_manager_copy_into_directory(tHandleThemeManager* pThis,char* indir,char* outdir)
{
	int i;

	for (i=0;i<SOURCES_NUM+2;i++)
	{
		char infilename[1024];
		char outfilename[1024];
		if (i<SOURCES_NUM)
		{
			snprintf(infilename,1024,"%s",cSources[i].filename);
			snprintf(outfilename,1024,"%s/%s",outdir,cSources[i].filename);
		} else if (i==SOURCES_NUM) {
			snprintf(infilename,1024,"PLEDIT.TXT");
			snprintf(outfilename,1024,"%s/PLEDIT.TXT",outdir);
		} else if (i==SOURCES_NUM+1) {
			snprintf(infilename,1024,"VISCOLOR.TXT");
			snprintf(outfilename,1024,"%s/VISCOLOR.TXT",outdir);
		}
		// remove the double slashes from the filenames.
		{
			int j;
			int k;
			int l;
			char lastc;
			l=strlen(infilename);
			k=0;
			lastc=0;
			for (j=0;j<l+1;j++)
			{
				char c;
				c=infilename[j];
				infilename[k]=c;
				if (c!='/' || lastc!='/') k++;
				lastc=c;
			}
		}
		{
			int j;
			int k;
			int l;
			char lastc;
			l=strlen(outfilename);
			k=0;
			lastc=0;
			for (j=0;j<l+1;j++)
			{
				char c;
				c=outfilename[j];
				outfilename[k]=c;
				if (c!='/' || lastc!='/') k++;
				lastc=c;
			}
		}
		if (theme_manager_adapt_filename(indir,infilename))
		{
			FILE *f;
			FILE *g;
			int inbytes;
			int outbytes;
			int n;
			unsigned char buffer[1024];
			
			if (strcmp(infilename,outfilename))
			{
				f=fopen(infilename,"rb");
				if (!f)
				{
					fprintf(stderr,"unable to open file [%s] for reading\n",infilename);
					return RETVAL_NOK;
				}
				
				g=fopen(outfilename,"wb");
				if (!g)
				{
					fprintf(stderr,"0 Unable to open file [%s] for writing\n",outfilename);
					return RETVAL_NOK;
				}
				inbytes=0;
				outbytes=0;
				while (!feof(f))
				{
					n=fread(buffer,sizeof(char),sizeof(buffer),f);
					inbytes+=n;
					outbytes+=fwrite(buffer,sizeof(char),n,g);
				}
				fclose(f);	
				fclose(g);	
				if (inbytes!=outbytes)
				{
					fprintf(stderr,"%s -> %s failed. %d bytes in, %d bytes out\n",infilename,outfilename,inbytes,outbytes);
					return RETVAL_NOK;
				}	
			}
		}
	}
	return RETVAL_OK;
}
int theme_manager_write_default_unpack(unsigned char *pOutBuf,const unsigned char *pPackBuf,int len)
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
			iidx++;
			if ((tag&0x80)==0x00)
			{
				verbatim_len=tag;	
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
				tag=1;
			}
		}
	}
	return oidx;
}

int theme_manager_unpack(char* directory,char* filename)
{
	int i;
	unsigned char *outbuf;
	char outfilename[2048];

	mkdir(directory,S_IRWXU);	// create the directory

	for (i=0;i<TOTAL_NUM;i++)
	{
		if (strncmp(filename,defaultThemePackedDir[i].filename,16)==0)
		{
			int len;
			int start;
			FILE *f;
			start=defaultThemePackedDir[i].start;
			len=defaultThemePackedDir[i].len;
			snprintf(outfilename,2048,"%s/%s",directory,filename);
			outbuf=malloc(len+1024);	// add a couple of bytes at the end
			len=theme_manager_write_default_unpack(outbuf,&defaultThemePacked[start],len);
			f=fopen(outfilename,"wb");
			if (!f)
			{
				fprintf(stderr,"5 Unable to open file [%s] for writing\n",outfilename);
				free(outbuf);
				return RETVAL_NOK;
			}
			fwrite(outbuf,sizeof(char),len,f);
			fclose(f);
			free(outbuf);
		}
	}
	return RETVAL_OK;
}


int theme_manager_load_from_directory(tHandleThemeManager* pThis,char* directory)
{
	int i;
	int bmpwidth[SOURCES_NUM]={0};
	int bmpheight[SOURCES_NUM]={0};
	char filename[1024];
	char themedir[2048];
	char tmpname[3096];
	int retval;
	retval=RETVAL_OK;
	if (pThis->pControllerContext!=NULL)
	{
		controller_get_config_dir(pThis->pControllerContext,filename);
	} else {
		snprintf(filename,1024,".");
	}
	snprintf(themedir,2048,"%s/theme/",filename);

	theme_manager_copy_into_directory(pThis,directory,themedir);

	// step 0: check if the files are in the theme directory
	{
		for (i=0;i<SOURCES_NUM;i++)
		{
			snprintf(filename,1024,"%s",cSources[i].filename);
			if (!theme_manager_adapt_filename(themedir,filename))
			{
				theme_manager_unpack(themedir,cSources[i].filename);
			}
		}
		snprintf(filename,1024,"%s","VISCOLOR.TXT");
		if (!theme_manager_adapt_filename(themedir,filename))
		{
			theme_manager_unpack(themedir,"VISCOLOR.TXT");
		}
		snprintf(filename,1024,"%s","PLEDIT.TXT");
		if (!theme_manager_adapt_filename(themedir,filename))
		{
			theme_manager_unpack(themedir,"PLEDIT.TXT");
		}
	}
	// step 1: initialize the defaults
	{

		// the loaded bmp files
		// TODO: this one is inializing them as black. instead, create a DEFAULT THEME
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

		for (i=0;i<SOURCES_NUM;i++)
		{
			GdkPixbuf *pixbuf;
			int idx;
			idx=cSources[i].sourceid;	
			pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,cSources[i].width,cSources[i].height);
			gdk_pixbuf_fill(pixbuf,0x00000000);
			gdk_pixbuf_copy_area(pixbuf,0,0,cSources[i].width,cSources[i].height,pThis->loaded_bmp[idx],0,0);
			g_object_unref(pixbuf);
		}
		// the visualizer colors
		for (i=0;i<VISCOLOR_NUM;i++)
		{
			pThis->visColors[i].red=0xff-(255*i)/VISCOLOR_NUM;
			pThis->visColors[i].green=(255*i)/VISCOLOR_NUM;
			pThis->visColors[i].blue=0;
		}

		// the playlist
		snprintf(pThis->playListTheme.fontname,1023,"Arial");
		pThis->playListTheme.color_normal.red=0xff;    pThis->playListTheme.color_normal.green=0x40;    pThis->playListTheme.color_normal.blue=0x00;
		pThis->playListTheme.color_current.red=0xff;   pThis->playListTheme.color_current.green=0xff;   pThis->playListTheme.color_current.blue=0xff;
		pThis->playListTheme.color_normalBG.red=0x00;  pThis->playListTheme.color_normalBG.green=0x00;  pThis->playListTheme.color_normalBG.blue=0x00;
		pThis->playListTheme.color_selectedBG.red=0x00;pThis->playListTheme.color_selectedBG.green=0x40;pThis->playListTheme.color_selectedBG.blue=0xff;
	}
	// step 2: load the files
	{
		int okaycnt;
		// the images
		okaycnt=0;
		for (i=0;i<SOURCES_NUM;i++)
		{
			int minwidth;
			int minheight;
			GdkPixbuf *pixbuf;
			eElementID idx;
			idx=(int)cSources[i].sourceid;
			snprintf(tmpname,3096,"%s/%s",themedir,cSources[i].filename);
			pixbuf=gdk_pixbuf_new_from_file(tmpname,NULL);
			minwidth=cSources[i].width;	
			minheight=cSources[i].height;
			if (pixbuf)
			{
				bmpwidth[idx]=gdk_pixbuf_get_width(pixbuf);
				bmpheight[idx]=gdk_pixbuf_get_height(pixbuf);

				if (bmpwidth[idx]<minwidth)
				{
					minwidth=bmpwidth[idx];
				}
				if (bmpheight[idx]<minheight)
				{
					minheight=bmpheight[idx];
				}
				gdk_pixbuf_copy_area(pixbuf,0,0,minwidth,minheight,pThis->loaded_bmp[idx],0,0);
				g_object_unref(pixbuf);
				okaycnt++;
			}
		}
		// check the elements, if they were loaded with the theme
		for (i=0;i<ELEMENTS_NUM;i++)
		{
			int x1,x2;
			int y1,y2;
			eElementID eidx;
			eElementSourceFile sidx;

			eidx=cElementSources[i].id;
			sidx=cElementSources[i].sourcefile;

			x1=ELEMENT_X(eidx);
			x2=x1+ELEMENT_WIDTH(eidx);

			y1=ELEMENT_Y(eidx);
			y2=y1+ELEMENT_HEIGHT(eidx);

		
			if (x1>=0 && x2<=bmpwidth[sidx] && y1>=0 && y2<=bmpheight[sidx])
			{
				pThis->loaded[i]=1;
			} else {
				pThis->loaded[i]=0;
			}
		}
		
		// the visualizer
		snprintf(tmpname,3096,"%s/VISCOLOR.TXT",themedir);
		theme_manager_parse_viscolor(pThis->visColors,tmpname);	
		okaycnt++;

		// the playlist
		snprintf(tmpname,3096,"%s/PLEDIT.TXT",themedir);
		theme_manager_parse_pledit(pThis,tmpname);
		okaycnt++;

		if (okaycnt<NUM_MANDATORY_FILES)
		{
			retval=RETVAL_NOK;
		}
	}
	return retval;
}
int theme_manager_load_from_wsz(tHandleThemeManager* pThis,char* filename)
{
	struct zip *za;
	struct zip_file *zf;
	struct zip_stat sb;
	int err;
	int i,j;
	int entries;
	char buf[1024];
	char themedir[2048];
	char tmpname[3096];
	char sources[SOURCES_NUM+2][16];

	if (pThis->pControllerContext!=NULL)
	{
		controller_get_config_dir(pThis->pControllerContext,buf);
	} else {
		snprintf(buf,1024,".");
	}
	snprintf(themedir,2048,"%s/theme/",buf);

	for (i=0;i<SOURCES_NUM;i++)
	{
		snprintf(sources[i],16,"%s",cSources[i].filename);
	}
	snprintf(sources[SOURCES_NUM+0],16,"PLEDIT.TXT");
	snprintf(sources[SOURCES_NUM+1],16,"VISCOLOR.TXT");

	za=zip_open(filename,0,&err);
	if (za==NULL)
	{
		fprintf(stderr,"illegal skin file [%s]\n",filename);
		return RETVAL_NOK;
	}
	
	entries=zip_get_num_entries(za,0);	
	for (i=0;i<entries;i++)
	{
		char sourcename[32];
		int len;
		if (zip_stat_index(za,i,0,&sb)==0)
		{
			int k;
			int l1;
			int l2;
			int found;
			len=strlen(sb.name);
			k=0;
			// get just the last filename (without the leading path)
			for (j=0;j<len;j++)
			{
				char c;
				c=sb.name[j];
				if (c=='/' || c=='\\')
				{
					k=0;
					sourcename[k]=0;
				} else {
					if (c>='a' && c<='z')
					{
						c-=32;
					}
					if (k<31)
					{
						sourcename[k++]=c;
						sourcename[k]=0;
					}
				}
			}
			found=-1;
			l1=strlen(sourcename);
			for (j=0;j<SOURCES_NUM+2;j++)
			{
				l2=strlen(sources[j]);
				if (strncmp(sourcename,sources[j],13)==0 && l1==l2)
				{
					found=j;
				}
			}
			if (found!=-1)
			{
				signed long long bytes;
				int n;
				FILE *f;
				bytes=0;
				
				snprintf(tmpname,3096,"%s/%s",themedir,sourcename);
				zf=zip_fopen_index(za,i,0);
				f=fopen(tmpname,"wb");
				if (!f)
				{
					fprintf(stderr,"4 error opening file [%s]\n",tmpname);
					zip_fclose(zf);
					zip_close(za);
					return RETVAL_NOK;
				}

				while (bytes<sb.size)
				{
					n=zip_fread(zf,buf,100);
					if (n<0)
					{
						fprintf(stderr,"error reading WSZ file\n");
						fclose(f);
						zip_fclose(zf);
						zip_close(za);
						return RETVAL_NOK;
					}
					bytes+=fwrite(buf,sizeof(char),n,f);
				}
				fclose(f);
				zip_fclose(zf);
			}
		}
	}
	zip_close(za);
	return RETVAL_OK;
}
int theme_manager_draw_element(tHandleThemeManager* pThis,GdkPixbuf* destbuf,eElementID elementID)
{
	int idx;
	int destx;
	int desty;
	idx=(int)elementID;
	if (cElementSources[idx].id!=elementID)
	{
		fprintf(stderr,"enums are not usable with this compiler. please write to dettus@dettus.net\n");
		return RETVAL_NOK;
	}
	destx=cElementSources[idx].destx;
	desty=cElementSources[idx].desty;
	if (destx<0)
	{
		destx+=gdk_pixbuf_get_width(destbuf);
	}

	if (desty<0)
	{
		desty+=gdk_pixbuf_get_height(destbuf);
	}

	return theme_manager_draw_element_at(pThis,destbuf,elementID,destx,desty);
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
	if (pThis->loaded[idx])
	{
		gdk_pixbuf_copy_area(pThis->loaded_bmp[bmpid],
				cElementSources[idx].startx,
				cElementSources[idx].starty,
				cElementSources[idx].dimx,
				cElementSources[idx].dimy,
				destbuf,x,y);
	}
	return RETVAL_OK;
}

int theme_manager_draw_text(tHandleThemeManager* pThis,GdkPixbuf** pDestbuf,eElementID backGroundElement,char* text,int minwidth)
{
	int i;
	int l;
	int l2;
	int x;
	int retval;
	int width;
#define	CHAR_WIDTH	5
#define	CHAR_HEIGHT	6

	retval=RETVAL_OK;

	l=l2=strlen(text);
	// ... is a special character: the text contains ..., but it will be just one symbol in the end.
	for (i=0;i<l-2;i++)
	{
		if (text[i+0]=='.' && text[i+1]=='.' && text[i+2]=='.')
		{
			l2-=3;
			i+=3;
		}
	}

	if (*pDestbuf!=NULL)
	{
		width=gdk_pixbuf_get_width(*pDestbuf);
		if ((width!=l2*CHAR_WIDTH && width>minwidth) || width<minwidth)
		{
			g_object_unref(*pDestbuf);
			*pDestbuf=NULL;
		}
	}
	width=l2*CHAR_WIDTH;
	if (width<minwidth)
	{
		width=minwidth;
	}
	if (*pDestbuf==NULL)
	{
		*pDestbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,l2*CHAR_WIDTH,CHAR_HEIGHT);
	}

	x=0;
	for (i=0;i<l;i++)
	{
		unsigned char c;
		eElementID elementid;
		

		elementid=ELEMENT_NONE;	
		// ... is just one character
		if (i<l-2)
		{
			if (text[i+0]=='.' && text[i+1]=='.' && text[i+2]=='.')
			{
				elementid=TEXT_ELLIPSIS;
				i+=3;
			}
		}
		if (elementid==ELEMENT_NONE)
		{
			c=(unsigned char)text[i];
			switch(c)
			{
				case 'a': case 'A': elementid=(TEXT_A);break;
				case 'b': case 'B': elementid=(TEXT_B);break;
				case 'c': case 'C': elementid=(TEXT_C);break;
				case 'd': case 'D': elementid=(TEXT_D);break;
				case 'e': case 'E': elementid=(TEXT_E);break;
				case 'f': case 'F': elementid=(TEXT_F);break;
				case 'g': case 'G': elementid=(TEXT_G);break;
				case 'h': case 'H': elementid=(TEXT_H);break;
				case 'i': case 'I': elementid=(TEXT_I);break;
				case 'j': case 'J': elementid=(TEXT_J);break;
				case 'k': case 'K': elementid=(TEXT_K);break;
				case 'l': case 'L': elementid=(TEXT_L);break;
				case 'm': case 'M': elementid=(TEXT_M);break;
				case 'n': case 'N': elementid=(TEXT_N);break;
				case 'o': case 'O': elementid=(TEXT_O);break;
				case 'p': case 'P': elementid=(TEXT_P);break;
				case 'q': case 'Q': elementid=(TEXT_Q);break;
				case 'r': case 'R': elementid=(TEXT_R);break;
				case 's': case 'S': elementid=(TEXT_S);break;
				case 't': case 'T': elementid=(TEXT_T);break;
				case 'u': case 'U': elementid=(TEXT_U);break;
				case 'v': case 'V': elementid=(TEXT_V);break;
				case 'w': case 'W': elementid=(TEXT_W);break;
				case 'x': case 'X': elementid=(TEXT_X);break;
				case 'y': case 'Y': elementid=(TEXT_Y);break;
				case 'z': case 'Z': elementid=(TEXT_Z);break;
				case '"':           elementid=(TEXT_QUOTATION_MARK);break;
				case '@':           elementid=(TEXT_AT_SYMBOL);break;
				case '0':           elementid=(TEXT_0);break;
				case '1':           elementid=(TEXT_1);break;
				case '2':           elementid=(TEXT_2);break;
				case '3':           elementid=(TEXT_3);break;
				case '4':           elementid=(TEXT_4);break;
				case '5':           elementid=(TEXT_5);break;
				case '6':           elementid=(TEXT_6);break;
				case '7':           elementid=(TEXT_7);break;
				case '8':           elementid=(TEXT_8);break;
				case '9':           elementid=(TEXT_9);break;
						    // TEXT_ELLIPSIS= ...
				case '.':           elementid=(TEXT_PERIOD);break;
				case ':':           elementid=(TEXT_COLON);break;
				case '(':           elementid=(TEXT_LEFT_PARENTHESIS);break;
				case ')':           elementid=(TEXT_RIGHT_PARENTHESIS);break;
				case '-':           elementid=(TEXT_DASH);break;
				case '\'':          elementid=(TEXT_APOSTROPHE);break;
				case '!':           elementid=(TEXT_EXCLAMATION_MARK);break;
				case '_':           elementid=(TEXT_UNDERSCORE);break;
				case '+':           elementid=(TEXT_PLUS_SYMBOL);break;
				case '\\':          elementid=(TEXT_BACK_SLASH);break;
				case '/':           elementid=(TEXT_SLASH);break;
				case '[':           elementid=(TEXT_LEFT_BRACKET);break;
				case ']':           elementid=(TEXT_RIGHT_BRACKET);break;
				case '^':           elementid=(TEXT_CARROT);break;
				case '&':           elementid=(TEXT_AMPERSAND);break;
				case '%':           elementid=(TEXT_PERCENT);break;
				case ',':           elementid=(TEXT_COMMA);break;
				case '=':           elementid=(TEXT_EQUAL);break;
				case '$':           elementid=(TEXT_DOLLAR);break;
				case '#':           elementid=(TEXT_POUND);break;
				case 226: case 194: elementid=(TEXT_A_CIRCUMFLEX);break;
				case 246: case 214: elementid=(TEXT_O_UMLAUT);break;
				case 228: case 196: elementid=(TEXT_A_UMLAUT);break;
				case '?':           elementid=(TEXT_QUESTION_MARK);break;
				case '*':           elementid=(TEXT_ASTERISK);break;
				default:            elementid=backGroundElement;
					 //(TEXT_TITLE_DISPLAY_SPACE, TEXT_TIME_DISPLAY_BACKGROUND, TEXT_KBPS_DISPLAY_SPACE
					 break;
			}
		}
		retval|=theme_manager_draw_element_at(pThis,*pDestbuf,elementid,x,0);
		x+=CHAR_WIDTH;	
	}
	return retval;
}

int theme_manager_get_viscolors(tHandleThemeManager* pThis,tVisColor** pVisColors)
{
	int retval;
	retval=RETVAL_OK;
	*pVisColors=pThis->visColors;	
	return retval;
}

int theme_manager_get_playListTheme(tHandleThemeManager* pThis,tPlayListTheme** pPlayListTheme)
{
	int retval;
	retval=RETVAL_OK;
	*pPlayListTheme=&(pThis->playListTheme);
	return retval;
}



// this is a helper tool for theme designers: it writes the necessay .bmp files and the .txt files
int theme_manager_write_template(char* directory)
{
	// just a collection of RGB values
	// TODO: make sure they distinct enough
	unsigned int alphargbpalette[]={

		0xFF696969, 0xFFa9a9a9, 0xFFdcdcdc, 0xFF2f4f4f, 0xFF556b2f, 0xFF6b8e23, 0xFFa0522d, 0xFF2e8b57,
		0xFF800000, 0xFF191970, 0xFF006400, 0xFF808000, 0xFF483d8b, 0xFFb22222, 0xFF5f9ea0, 0xFF778899,
		0xFF008000, 0xFF3cb371, 0xFFbc8f8f, 0xFF663399, 0xFF008080, 0xFFbdb76b, 0xFFcd853f, 0xFF4682b4,
		0xFFd2691e, 0xFF9acd32, 0xFF20b2aa, 0xFFcd5c5c, 0xFF00008b, 0xFF4b0082, 0xFF32cd32, 0xFFdaa520,

		0xFF7f007f, 0xFF8fbc8f, 0xFFb03060, 0xFF66cdaa, 0xFF9932cc, 0xFFff4500, 0xFFff8c00, 0xFFffa500,
		0xFFffd700, 0xFFffff00, 0xFFc71585, 0xFF0000cd, 0xFFdeb887, 0xFF40e0d0, 0xFF7fff00, 0xFF00ff00,
		0xFFba55d3, 0xFF00fa9a, 0xFF8a2be2, 0xFF00ff7f, 0xFF4169e1, 0xFFdc143c, 0xFF00ffff, 0xFF00bfff,
		0xFF9370db, 0xFF0000ff, 0xFFadff2f, 0xFFda70d6, 0xFFd8bfd8, 0xFFb0c4de, 0xFFff7f50, 0xFFff00ff,

		0xFFdb7093, 0xFFfa8072, 0xFFeee8aa, 0xFFffff54, 0xFF6495ed, 0xFFdda0dd, 0xFF90ee90, 0xFF87ceeb,
		0xFFff1493, 0xFF7b68ee, 0xFFffa07a, 0xFFafeeee, 0xFF7fffd4, 0xFFffdab9, 0xFFff69b4, 0xFFffc0cb
	};
	int i;
	int j;
	int max;
	char filename[64];
	FILE *f;
	{
		// first, create the output directory
		struct stat st = {0};

		if (stat(directory, &st) == -1) {
			mkdir(directory, 0755);
		}
	}

// first: write the PLEDIT.TXT file.
	{
		char *keys[4]={"Normal","Current","NormalBG","SelectedBG"};
		snprintf(filename,64,"%s/PLEDIT.TXT",directory);
		f=fopen(filename,"wb");
		if (f==NULL)
		{
			printf("unable to open [%s] for writing.\n",filename);
			return RETVAL_NOK;
		}
		fprintf(f,"[Text]\r\n");
		for (i=0;i<4;i++)
		{
			fprintf(f,"%s=#%06X\r\n",keys[i],alphargbpalette[i]&0xffffff);
		}	
		fprintf(f,"Font=Arial\r\n");
		fclose(f);
	}

	// second: write the VISCOLOR.TXT file.
	{
		char *descriptions[24]={"black","grey","top of spec","","","","","","","","","","","","","","","bottom of spec","osc 1","osc 2 (slightly dimmer)","osc 3","osc 4","osc 5","peak dots"};
		snprintf(filename,64,"%s/VISCOLOR.TXT",directory);
		f=fopen(filename,"wb");
		if (f==NULL)
		{
			printf("1 unable to open [%s] for writing.\n",filename);
			return RETVAL_NOK;
		}

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


	// third: write the .BMP files
	{	
		int col;
		unsigned int *drawbuf;
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
		drawbuf=(unsigned int*)malloc(max);

		col=0;
		for (i=0;i<SOURCES_NUM;i++)
		{
			eElementSourceFile sourcefile;
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
	return RETVAL_OK;
}
int theme_manager_write_default(char *directory)
{
	unsigned char *outbuf;	
	int i;
	int max_file_size;

	max_file_size=0;
	for (i=0;i<TOTAL_NUM;i++)
	{
		if (defaultThemePackedDir[i].len>max_file_size)
		{
			max_file_size=defaultThemePackedDir[i].len;
		}
	}
	outbuf=malloc(max_file_size+1024);	// add a couple of bytes at the end.
	
	for (i=0;i<TOTAL_NUM;i++)
	{
		int len;
		int start;
		FILE *f;
		char filename[1024];

		start=defaultThemePackedDir[i].start;
		len=defaultThemePackedDir[i].len;

		len=theme_manager_write_default_unpack(outbuf,&defaultThemePacked[start],len);
		snprintf(filename,1024,"%s/%s",directory,defaultThemePackedDir[i].filename);
		f=fopen(filename,"wb");
		if (!f)
		{
			fprintf(stderr,"2 Unable to open file [%s] for writing\n",filename);
			free(outbuf);
			return RETVAL_NOK;
		}
		fwrite(outbuf,sizeof(char),len,f);
		fclose(f);
	}

	free(outbuf);
	return RETVAL_OK;
}
