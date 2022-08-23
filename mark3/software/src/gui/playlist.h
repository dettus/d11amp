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

#ifndef	PLAYLIST_H
#define	PLAYLIST_H
#include "pthread.h"
#include "datastructures.h"

// the way the playlist is handled is by simply loading the full .m3u file into memory.
#define	PLAYLIST_BUFSIZE	(1<<21)	// 2MByte
typedef struct _tHandlePlaylist
{
// buffer for the loaded file
	char m3uBuf[PLAYLIST_BUFSIZE];
	int m3uSize;

	
// size of one page
	int lines_per_page;
	int firstLineNum;	// first line number in the page
// index of the beginning of the first line within the page
	int indexFirst;
// index of the end of the last line within the page
	int indexEnd;
// index of the line which the user has selected
	int indexCurrent;



	pthread_mutex_t mutex;
} tHandlePlaylist;

int playlist_init(tHandlePlaylist* pThis);
int playlist_load(tHandlePlaylist* pThis,char *filename);

// navigation
int playlist_resize(tHandlePlaylist* pThis,int lines_per_page);
int playlist_scroll_top(tHandlePlaylist* pThis);
int playlist_scroll_back(tHandlePlaylist* pThis,int linenum);
int playlist_scroll_forward(tHandlePlaylist* pThis,int linenum);
int playlist_scroll_end(tHandlePlaylist* pThis);
int playlist_scroll_relative(tHandlePlaylist* pThis,int y,int maxy);
int playlist_get_firstLineNum(tHandlePlaylist* pThis,int *pAbsolute_linenum);	// return the absolute line number

// selecting one file
int playlist_get_filename_by_line(tHandlePlaylist* pThis,char* pFilename,int size,char* pIsCurrent,int linenum); // get the filename in a line. pCurrent=1 if it is the current filename (so that it can be highlighted
int playlist_select_line(tHandlePlaylist *pThis,int linenum);
int playlist_shuffle(tHandlePlaylist* pThis,char* pFilename,int size,int setCurrent);	// returns a random filename
int playlist_prev(tHandlePlaylist* pThis,char* pFilename,int size,int setCurrent);	// returns the previous filename
int playlist_next(tHandlePlaylist* pThis,char* pFilename,int size,int setCurrent);	// returns the next filename
int playlist_jump_to_current(tHandlePlaylist* pThis);
int playlist_append(tHandlePlaylist *pThis,char* filename);
int playlist_get_position(tHandlePlaylist *pThis,int *pPosition,int multiplier);
#endif

