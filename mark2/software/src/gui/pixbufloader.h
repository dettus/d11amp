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


#ifndef	PIXBUFLOADER_H
#define	PIXBUFLOADER_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include "elements.h"
typedef struct _tHandlePixbufLoader
{
	GdkPixbuf* loaded_bmp[SOURCES_NUM];	
} tHandlePixbufLoader;
int pixbufloader_init(tHandlePixbufLoader* pThis);
int pixbufloader_load_from_directory(tHandlePixbufLoader* pThis,char* directory);
int pixbufloader_addelement(tHandlePixbufLoader* pThis,GdkPixbuf* destbuf,int x,int y,eElementID elementID);
int pixbufloader_textelement(tHandlePixbufLoader* pThis,unsigned char c,eElementID *pElementID,int songinfo0_time1_kbps2);

#define	PIXBUFLOADER_OK		0
#define	PIXBUFLOADER_NOK	-1
#endif
