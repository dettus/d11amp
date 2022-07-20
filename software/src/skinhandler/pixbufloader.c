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
#include "elements.h"
#include "dirent.h"
#include "ctype.h"

int strcmplowercase(char* s1,char* s2)
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

int adapt_filename(char* directory,char* filename)
{
	DIR *dir;
	struct dirent *ent;
	printf("%s -> ",filename);
	if ((dir = opendir (directory)) != NULL)
	{
		while ((ent = readdir (dir)) != NULL) 
		{
			if (strcmplowercase(ent->d_name,filename)==0)
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

int loadpixbuf(tPixbufs* pPixbuf,char* directory)
{
	char filename[1024];
	pPixbuf->avsbmp=NULL;
	pPixbuf->balancebmp=NULL;
	pPixbuf->cbuttonsbmp=NULL;
	pPixbuf->eq_exbmp=NULL;
	pPixbuf->eqmainbmp=NULL;
	pPixbuf->mainbmp=NULL;
	pPixbuf->mbbmp=NULL;
	pPixbuf->monosterbmp=NULL;
	pPixbuf->numbersbmp=NULL;
	pPixbuf->playpausbmp=NULL;
	pPixbuf->pleditbmp=NULL;
	pPixbuf->posbarbmp=NULL;
	pPixbuf->shufrepbmp=NULL;
	pPixbuf->textbmp=NULL;
	pPixbuf->titlebarbmp=NULL;
	pPixbuf->volumebmp=NULL;
	snprintf(filename,13,"avs.bmp"     );if (adapt_filename(directory,filename)) {pPixbuf->avsbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"balance.bmp" );if (adapt_filename(directory,filename)) {pPixbuf->balancebmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"cbuttons.bmp");if (adapt_filename(directory,filename)) {pPixbuf->cbuttonsbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"eq_ex.bmp"   );if (adapt_filename(directory,filename)) {pPixbuf->eq_exbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"eqmain.bmp"  );if (adapt_filename(directory,filename)) {pPixbuf->eqmainbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"main.bmp"    );if (adapt_filename(directory,filename)) {pPixbuf->mainbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"mb.bmp"      );if (adapt_filename(directory,filename)) {pPixbuf->mbbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"monoster.bmp");if (adapt_filename(directory,filename)) {pPixbuf->monosterbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"numbers.bmp" );if (adapt_filename(directory,filename)) {pPixbuf->numbersbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"playpaus.bmp");if (adapt_filename(directory,filename)) {pPixbuf->playpausbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"pledit.bmp"  );if (adapt_filename(directory,filename)) {pPixbuf->pleditbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"posbar.bmp"  );if (adapt_filename(directory,filename)) {pPixbuf->posbarbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"shufrep.bmp" );if (adapt_filename(directory,filename)) {pPixbuf->shufrepbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"posbar.bmp"  );if (adapt_filename(directory,filename)) {pPixbuf->posbarbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"text.bmp"    );if (adapt_filename(directory,filename)) {pPixbuf->textbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"titlebar.bmp");if (adapt_filename(directory,filename)) {pPixbuf->titlebarbmp=gdk_pixbuf_new_from_file(filename,NULL);}
	snprintf(filename,13,"volume.bmp"  );if (adapt_filename(directory,filename)) {pPixbuf->volumebmp=gdk_pixbuf_new_from_file(filename,NULL);}
}
