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
#include "playlist.h"
#include <string.h>

int playlist_init(tHandlePlaylist* pThis)
{
	memset(pThis,0,sizeof(tHandlePlayList));
	return RETVAL_OK;
}
int playlist_find_end(tHandlePlaylist* pThis)
{
	int i;
	int idx;
	int linecnt;
	char c;

	idx=pThis->indexFirst;
	linecnt=pThis->lines_per_page;
	while (linecnt && idx<pThis->m3uSize)
	{
		c=pThis->m3uBuf[idx++];
		if (c==10) linecnt--;	// assuming here that each line ends with a LF.			
	}
	return idx;
}
int playlist_find_next_line(tHandlePlaylist* pThis,int idx)
{
	int i;
	int idx;
	int linecnt;
	char c;

	while (idx<pThis->m3uSize)
	{
		c=pThis->m3uBuf[idx++];
		if (c==10) return idx;	 // assuming here that each line ends with a LF.
	}
	return idx;
}
int playlist_find_prev_line(tHandlePlaylist* pThis,int idx)
{
	int i;
	int idx;
	int linecnt;
	char c;

	idx-=2;	// go back at least 2 characters, since the previous one will be a LF.
	if (idx>=0) 
	{
		while (idx>0)
		{
			c=pThis->m3uBuf[idx--];	
			if (c==10) return idx;	// assuming here that each line ends with a LF.
		}
		return idx;
	} else return 0;
}
int playlist_load(tHandlePlaylist* pThis,char* filename)
{
	int n;
	int retval;
	int idx;
	FILE *f;

	retval=RETVAL_OK;

	f=fopen(filename,"rb");
	if (f!=NULL) 
	{
		pThis->m3uSize=fread(pThis->m3uBuf,sizeof(char),sizeof(pThis->m3uBuf),f);
		fclose(f);

		pThis->indexFirst=0;
		playlist_find_end(pThis);
	} else {
		retval=RETVAL_NOK;
	}
	return retval;
}


