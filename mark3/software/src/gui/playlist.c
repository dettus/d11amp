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
#include <stdio.h>
#include <time.h>

int playlist_init(tHandlePlaylist* pThis)
{
	memset(pThis,0,sizeof(tHandlePlaylist));
	srand(time(NULL));		// initalize the random generator
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

		playlist_scroll_top(pThis);
	} else {
		retval=RETVAL_NOK;
	}
	return retval;
}
int playlist_resize(tHandlePlaylist* pThis,int lines_per_page)
{
	pThis->lines_per_page=lines_per_page;
	pThis->indexEnd=playlist_find_end(pThis);	
	return RETVAL_OK;
}
int playlist_scroll_top(tHandlePlaylist* pThis)
{
	pThis->indexFirst=0;
	pThis->firstLineNum=0;
	pThis->indexEnd=playlist_find_end(pThis);
	return RETVAL_OK;
}
int playlist_scroll_back(tHandlePlaylist* pThis,int linenum)
{
	int i;
	if (pThis->firstLineNum>=linenum)
	{
		for (i=0;i<linenum;i++)
		{
			pThis->indexFirst=playlist_find_prev_line(pThis,pThis->indexFirst);
		}
		pThis->firstLineNum-=linenum;
		pThis->indexEnd=playlist_find_end(pThis);	
	} else {
		return playlist_scroll_top(pThis);
	}
	return RETVAL_OK;	
}
int playlist_scroll_forward(tHandlePlaylist* pThis,int linenum)
{
	int i;
	int idx;
	idx=pThis->indexFirst;
	for (i=0;i<linenum && idx!=pThis->m3uSize;i++)
	{
		idx=playlist_find_next_line(pThis,pThis->indexFirst);
		if (idx!=pThis->m3uSize)
		{
			pThis->indexFirst=idx;
			pThis->firstLineNum++;
		}
	}
	pThis->indexEnd=playlist_find_end(pThis);	
	return RETVAL_OK;
}
int playlist_calculate_first_linenum(tHandlePlaylist* pThis)
{
	int linecnt;
	int idx;
	char c;
		
	idx=0;
	linecnt=0;
	while (idx<pThis->indexFirst && idx<pThis->m3uSize)
	{
		char c;
		c=pThis->m3uBuf[idx++];	
		if (c==10) linecnt++;	// assuming that each line ends with LF.
	}
	pThis->firstLineNum=linecnt;
	return RETVAL_OK;
}
int playlist_scroll_end(tHandlePlaylist* pThis)
{
	int idx;
	idx=pThis->m3uSize;
	pThis->indexFirst=playlist_find_prev_line(pThis,idx);
	pThis->indexEnd=playlist_find_end(pThis);
	playlist_calculate_first_linenum(pThis);
	return RETVAL_OK;
}
int playlist_get_firstLineNum(tHandlePlaylist* pThis,int *pAbsolute_linenum)
{
	*pAbsolute_linenum=pThis->firstLineNum;
	return RETVAL_OK;
}
int playlist_get_filename_by_idx(tHandlePlaylist* pThis,char* pFilename,int size,char* pIsCurrent,int idx)
{
	int i;
	if (pIsCurrent!=NULL)
	{
		*pIsCurrent=(idx==pThis->indexCurrent);
	}
	pFilename[0]=0;
	for (i=0;i<(size-1) && idx<pThis->m3uSize;i++)
	{
		char c;	
		c=pThis->m3uBuf[idx++];
		if (c<' ') break;
		pFilename[i]=c;
		pFilename[i+1]=0;
	}
	return RETVAL_OK;

}
int playlist_get_filename_by_line(tHandlePlaylist* pThis,char* pFilename,int size,char* pIsCurrent,int linenum) // get the filename in a line. pCurrent=1 if it is the current filename (so that it can be highlighted
{
	int i;
	int idx;


	idx=pThis->indexFirst;
	idxnxt=idx;
	
	for (i=0;i<linenum;i++)
	{
		idx=pThis->playlist_find_next_line(pThis,idx);
	}
	return playlist_get_filename_by_idx(pThis,pFilename,size,pIsCurrent,idx);
}
int playlist_select_line(tHandlePlaylist *pThis,int linenum)
{
	int i;
	int idx;
	idx=pThis->indexFirst;
	for (i=0;i<linenum;i++)
	{
		idx=pThis->playlist_find_next_line(pThis,idx);	
	}
	pThis->indexCurrent=idx;
	return RETVAL_OK;
}
int playlist_shuffle(tHandlePlaylist* pThis,char* pFilename,int size,int setCurrent)	// returns a random filename
{
	int idx;
	// start with a random position in the file
	idx=rand()%pThis->m3uSize;
	idx=playlist_find_prev_line(pThis,idx);// find the beginning of this line

	if (setCurrent)
	{
		pThis->indexCurrent=idx;
	}

	return playlist_get_filename_by_idx(pThis,pFilename,size,NULL,idx);
}
int playlist_prev(tHandlePlaylist* pThis,char* pFilename,int size,int setCurrent)
{
	int idx;
	idx=pThis->indexCurrent;	
	idx=playlist_find_prev_line(pThis,idx);// find the beginning of this line

	if (setCurrent)
	{
		pThis->indexCurrent=idx;
	}

	return playlist_get_filename_by_idx(pThis,pFilename,size,NULL,idx);
}
int playlist_next(tHandlePlaylist* pThis,char* pFilename,int size,int setCurrent)
{
	int idx;
	idx=pThis->indexCurrent;	
	idx=playlist_find_next_line(pThis,idx);// find the beginning of this line

	if (setCurrent)
	{
		pThis->indexCurrent=idx;
	}

	return playlist_get_filename_by_idx(pThis,pFilename,size,NULL,idx);
}
int playlist_jump_to_current(tHandlePlaylist* pThis)
{
	pThis->indexFirst=pThis->indexCurrent;
	pThis->indexEnd=playlist_find_end(pThis);
	playlist_calculate_first_linenum(pThis);
		
	return RETVAL_OK;
}
