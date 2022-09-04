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
#include <string.h>
#include "datastructures.h"
#include "playlist.h"

int playlist_init(tHandlePlayList* pThis)
{
	memset(pThis,0,sizeof(tHandlePlayList));	
	return RETVAL_OK;
}
int playlist_load_m3u(tHandlePlayList* pThis,char* filename)
{
	FILE *f;
	int i;
	char c;
	char lc;
	

	f=fopen(filename,"rb");
	if (!f)
	{
		return RETVAL_NOK;
	}
	pThis->endPointer=fread(pThis->playListBuf,sizeof(char),PLAYLIST_BUF_SIZE,f);
	fclose(f);

	lc=0;
	
	pThis->numberOfEntries=0;
	pThis->currentEntry=0;
	for (i=0;i<pThis->endPointer && pThis->numberOfEntries<PLAYLIST_MAX_INDEX;i++)
	{
		c=pThis->playListBuf[i];
		if (c<' ') c=0;
		if (c && lc==0)	// new line, new entry
		{
			pThis->playListPointer[pThis->numberOfEntries]=i;
			pThis->playListMarked[pThis->numberOfEntries]=0;
			pThis->numberOfEntries++;
		}
		lc=c;
	}
	return RETVAL_OK;
}
int playlist_get_numbers(tHandlePlayList* pThis,int *pNumberOfEntries,int* pCurrentEntry)
{
	*pNumberOfEntries=pThis->numberOfEntries;
	*pCurrentEntry=pThis->currentEntry;
	return RETVAL_OK;
}
int playlist_set_current_entry(tHandlePlayList* pThis,int currentEntry)
{
	pThis->currentEntry=currentEntry;
	return RETVAL_OK;
}
int playlist_read_entry(tHandlePlayList* pThis,int index,tSongInfo *pSongInfo,char *pMarked)
{
	int l;
	int i;
	if (pSongInfo!=NULL)
	{
		memset(pSongInfo,0,sizeof(tSongInfo));
		if (index>pThis->numberOfEntries)
		{
			return RETVAL_NOK;
		}
		l=strlen(&(pThis->playListBuf[pThis->playListPointer[index]]));
		if (l>1023) l=1023;

		strncpy(pSongInfo->filename,&(pThis->playListBuf[pThis->playListPointer[index]]),l);
		for (i=0;i<l;i++)
		{
			if (pSongInfo->filename[i]<' ')	// zero-terminate at a line break;
			{
				pSongInfo->filename[i]=0;
			}
		}	
	}
	if (pMarked!=NULL)
	{
		*pMarked=pThis->playListMarked[index];
	}

	return RETVAL_OK;
}


